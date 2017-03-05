//
//  ViewController.m
//  SimpleChatClient
//
//  Created by MS MacBook on 2015. 5. 11..
//  Copyright (c) 2015년 MS MacBook. All rights reserved.
//

#import "ViewController.h"
#import "MakingRoomController.h"
#import "EnteringRoomController.h"
#import "SettingController.h"
#import "ChatUser.h"
#import "RoomItem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "msjc_io.h"

#define MAX_LINE 1023  // 최대 버퍼 길이
#define MAX_ID 8      // 최대 ID 길이
#define MAX_TITLE 95  // 최대 방 제목 길이

// 메시지 종류를 저장하기 위한 열거형
typedef enum _msg_kind {
    MSG_NONE = 0,     MSG_TEXT,
    MSG_USER_LOGOUT,  MSG_USER_CONNECT, MSG_USER_DISCONN,
    MSG_ROOM_MAKE,    MSG_ROOM_LIST,    MSG_ROOM_ENTER,   MSG_ROOM_EXIT,
    MSG_ROOM_CONNECT, MSG_ROOM_DISCONN
} msg_kind;

// 메시지를 문자열로 변환해주는 함수
const char *msg2str(msg_kind msg)
{
    switch(msg) {
    case MSG_NONE:         return "MSG_NONE";
    case MSG_TEXT:         return "MSG_TEXT";
    case MSG_USER_LOGOUT:  return "MSG_USER_LOGOUT";
    case MSG_USER_CONNECT: return "MSG_USER_CONNECT";
    case MSG_USER_DISCONN: return "MSG_USER_DISCONN";
    case MSG_ROOM_MAKE:    return "MSG_ROOM_MAKE";
    case MSG_ROOM_LIST:    return "MSG_ROOM_LIST";
    case MSG_ROOM_ENTER:   return "MSG_ROOM_ENTER";
    case MSG_ROOM_EXIT:    return "MSG_ROOM_EXIT";
    case MSG_ROOM_CONNECT: return "MSG_ROOM_CONNECT";
    case MSG_ROOM_DISCONN: return "MSG_ROOM_DISCONN";
    default: break;
    }
    return NULL;
    
}

// 메시지로부터 메시지의 종류를 읽는 함수
msg_kind get_msgkind(char **msg_p)
{
    msg_kind kind = MSG_NONE;
    *msg_p = readintbuf(*msg_p, NULL, (int *)&kind);
    return kind;
}

/* 방 정보 */
typedef struct _room_info {
    int no;                   // 방 번호
    char title[MAX_TITLE+1];  // 방 제목
    int max_users;            // 최대 인원
    int curr_users;           // 현재 인원
} room_info;



@implementation ViewController
{
    IBOutlet NSTableView *userListTable;
    NSMutableArray *friendList;
    EnteringRoomController *enteringRoomController;
    
    char servIp[16];   // 서버 IP주소
    int servPort;      // 서버 포트번호
    int recvSock;      // 수신소켓
    int sendSock;      // 송신소켓
    BOOL isLogin;      // 로그인 여부 플래그
    int friendCount;   // 친구 수
    room_info roomInfo;
    
    NSString *userId;  // 사용자ID
}

// 뷰가 열리고 호출되는 메소드
- (void)viewDidLoad {
    [super viewDidLoad];
    
    // 서버 IP주소 및 포트번호를 초기화한다.
    memset(servIp, 0, 16);
    strncpy(servIp, "127.0.0.1", 15);
    servIp[15] = 0;
    servPort = 50000;
    
    // 프로그램을 초기화한다.
    [self initProgram];
    
    printf("LOG: 프로그램 초기화 완료\n");
}

// 뷰를 갱신하는 메소드
- (void)setRepresentedObject:(id)representedObject
{
    [super setRepresentedObject:representedObject];
}

// 뷰가 사라지기 전에 호출되는 메소드
- (void)viewWillDisappear
{
    // 수신소켓 및 송신소켓 해제
    if(recvSock > 0) close(recvSock);
    if(sendSock > 0) close(sendSock);
}

// 현재 테이블 뷰의 행의 수를 반환하는 메소드
- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    return friendList.count;
}

// 테이블 뷰의 특정 항목에 해당하는 객체의 특정 값을 반환하는 메소드
- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    id returnValue = nil;
    NSString *identifier = [tableColumn identifier];
    ChatUser *chatUser = [friendList objectAtIndex: row];
    
    if([identifier isEqualToString: @"userId"]) {
        returnValue = chatUser.userId;
    }
    
    return returnValue;
}

// 테이블 뷰의 특정 항목에 해당하는 뷰를 만드는 메소드
// 항목 뷰의 이미지 뷰와 텍스트 뷰를 설정해 항목 뷰를 완성한다.
- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    NSTableCellView *result = [tableView makeViewWithIdentifier: @"ChatUserTableCellView" owner:self];
    
    if(result == nil) {
        result = [[NSTableCellView alloc] init];
        result.identifier = @"ChatUserTableCellView";
    }
    
    result.textField.stringValue = [[friendList objectAtIndex: row] userId];
    if([[friendList objectAtIndex: row] login] == YES)
        result.imageView.image = [NSImage imageNamed: @"user_on.png"];
    else
        result.imageView.image = [NSImage imageNamed: @"user_off.png"];
    
    return result;
}

// 테이블 뷰의 행의 높이를 결정하는 메소드
// 이 메소드에서 반환하는 값에 따라 실제 보여지는 행의 높이가 결정된다.
- (CGFloat)tableView:(NSTableView *)tableView heightOfRow:(NSInteger)row
{
    return 18;
}

// 서버에 로그인을 요청하는 메소드
// (아이디입력 필드, 로그인 버튼의 액션)
- (IBAction)clickLogin:(id)sender
{
    @autoreleasepool {
        // ID를 입력하지 않았다면, 로그인을 할 수 없다.
        if([_idField.stringValue isEqualToString: @""]) {
            NSAlert *alert = [[NSAlert alloc] init];
            [alert setAlertStyle: NSWarningAlertStyle];
            [alert setMessageText: @"아이디를 입력해주세요."];
            [alert addButtonWithTitle: @"확인"];
            [alert runModal];
            
            printf("LOG: 아이디입력 필드가 비었습니다.\n");
            return;
        }
        // ID의 길이가 8글자를 넘는다면, 로그인을 할 수 없다.
        if([_idField.stringValue length] > 8) {
            printf("LOG: 아이디의 길이는 8글자를 넘길 수 없습니다.\n");
            return;
        }
        
        // 로그인 버튼을 누른 경우
        if([_loginBtn.title isEqualToString: @"로그인"]) {
            userId = [_idField stringValue];  // ID를 설정한다.
            
            // 소켓을 생성하고 서버에 연결한다.
            [self initSocketAndConnect];
            
            // 로그인되지 않았다면, 아무 처리도 하지 않는다.
            if(!isLogin) {
                printf("LOG: 로그인 실패: %s\n", [userId UTF8String]);
                return;
            }
            printf("LOG: 로그인 성공: %s\n", [userId UTF8String]);
            
            // 로그인에 성공한 경우
            [_idField setEditable: NO];       // ID를 변경하지 못하게 한다.
            [_loginBtn setTitle: @"로그아웃"];  // 로그아웃 버튼으로 변경한다.
            [_inputField setEditable: YES];   // 텍스트를 입력할 수 있다.
            [_sendBtn setEnabled: YES];       // 텍스트를 전송할 수 있다.
            [_makeBtn setEnabled: YES];       // 방을 만들 수 있다.
            [_enterBtn setEnabled: YES];      // 방에 들어갈 수 있다.
            
            // 사용자 목록 테이블 뷰에 친구 목록을 초기화한다.
            for(int i = 0; i < friendCount; i++) {
                [friendList addObject: [ChatUser initWithUserId: [NSString stringWithFormat: @"User%d", i]]];
            }
            // 사용자 목록의 ID를 사전식 오름차순으로 정렬한다.
            [friendList sortUsingComparator:^NSComparisonResult(id obj1, id obj2) {
                NSString *str1 = [(ChatUser *)obj1 userId];
                NSString *str2 = [(ChatUser *)obj2 userId];
                return [str1 compare: str2];
            }];
            
            // 사용자 목록 테이블 뷰를 갱신한다.
            [userListTable performSelectorOnMainThread: @selector(reloadData)
                                            withObject: nil
                                         waitUntilDone: YES];
            
            // 수신스레드를 시작시켜 서버로부터의 메시지를 받을 준비를 한다.
            [NSThread detachNewThreadSelector:@selector(receiveThread) toTarget:self withObject:nil];
            
            // 텍스트입력 필드로 포커스를 옮긴다.
            [_inputField becomeFirstResponder];
        }
        // 로그아웃 버튼을 누른 경우
        else {
            // 서버에 로그아웃 메시지를 전송한다.
            [self sendLogoutRequest];
        }
    }
}

// 서버에 텍스트를 전송하는 메소드
// (텍스트입력 필드, 보내기 버튼의 액션)
- (IBAction)clickSend:(id)sender
{
    @autoreleasepool {
        // ID를 입력하지 않았다면 텍스트를 보내지 않는다.
        if([[_idField stringValue] isEqualToString: @""]) {
            printf("LOG: 아이디입력 필드가 비었습니다.\n");
            return;
        }
        // 전송할 텍스트가 없다면 텍스트를 보내지 않는다.
        if([[_inputField stringValue] isEqualToString: @""]) {
            printf("LOG: 텍스트입력 필드가 비었습니다.\n");
            return;
        }

        // 서버에 텍스트를 전송한다.
        [self sendText];
    }
}

// 방 만들기 창을 여는 방 만들기 버튼의 액션 메소드
- (IBAction)clickMakeRoom:(id)sender
{
    MakingRoomController *makingRoomController = [MakingRoomController instance];
    
    // 방 만들기 창에서 만들기 버튼을 누른 경우, 서버에 방 만들기 요청을 전송한다.
    if([NSApp runModalForWindow: makingRoomController.window]) {
        NSString *title = makingRoomController.title;
        int maxNum = makingRoomController.maxNum;
        
        // 입력한 값으로 방 정보를 초기화한다.
        roomInfo.max_users = maxNum;
        roomInfo.curr_users = 1;
        strncpy(roomInfo.title, [title UTF8String], MAX_TITLE);
        roomInfo.title[MAX_TITLE] = 0;
        
        // 메시지 전송을 위한 임시 데이터
        char msgbuf[MAX_LINE+1] = {0,};
        char *msgbuf_p = NULL;
        int msglen = 0;
        
        msgbuf_p = writeintbuf(msgbuf, &msglen, MSG_ROOM_MAKE);                // 메시지의 종류를 MSG_ROOM_MAKE로 설정한다.
        msgbuf_p = writestrbuf(msgbuf_p, &msglen, roomInfo.title, MAX_TITLE);  // 메시지에 방 제목을 넣는다.
        writeintbuf(msgbuf_p, &msglen, roomInfo.max_users);                    // 메시지에 최대 인원을 넣는다.
        if(write(sendSock, msgbuf, msglen) < 0) {
            printf("ERROR: 방 만들기 메시지 전송 실패\n");
            exit(0);
        }
        
        printf("LOG: 방 만들기 요청: %s, %d\n", [title UTF8String], maxNum);
    }
}

// 방 들어가기 창을 여는 방 들어가기 버튼의 액션 메소드
- (IBAction)clickEnterRoom:(id)sender
{
    enteringRoomController = [EnteringRoomController instance];
    
    // 메시지 전송을 위한 임시 데이터
    char msgbuf[MAX_LINE+1] = {0,};
    char *msgbuf_p = NULL;
    int msglen = 0;
    
    // 서버에게 방 목록을 요청한다.
    writeintbuf(msgbuf, &msglen, MSG_ROOM_LIST);  // 메시지의 종류를 MSG_ROOM_LIST로 설정한다.
    if(write(sendSock, msgbuf, msglen) < 0) {
        printf("ERROR: 방 목록 요청 메시지 전송 실패\n");
        exit(0);
    }
    printf("LOG: 방 목록 요청\n");
    
    // 방 들어가기 창을 띄우고, 요청한 목록이 갱신되면 방을 선택해 서버에 방 들어가기 요청을 한다.
    if([NSApp runModalForWindow: enteringRoomController.window]) {
        RoomItem *selRoom = [enteringRoomController getSelectedRoomItem];
        
        NSLog(@"item: %d, %@, %d, %d", (int)selRoom.roomNo, selRoom.title, selRoom.maxUsers, selRoom.currUsers);
        
        // 방 번호가 0이면, 선택한 행이 없다.
        if(selRoom.roomNo == 0) {
            printf("LOG: 선택한 방이 없습니다.\n");
            return;
        }
        // 현재 인원과 최대 인원이 같으면, 들어갈 수 없다.
        if(selRoom.currUsers == selRoom.maxUsers) {
            NSAlert *alert = [[NSAlert alloc] init];
            [alert setAlertStyle: NSWarningAlertStyle];
            [alert setMessageText: @"방이 꽉 찼습니다."];
            [alert addButtonWithTitle: @"확인"];
            [alert runModal];
            
            printf("LOG: 해당 방에는 들어갈 수 없습니다.\n");
            return;
        }
        
        // 선택한 행의 데이터로 방 정보를 초기화한다.
        roomInfo.no = (int)selRoom.roomNo;
        strncpy(roomInfo.title, [selRoom.title UTF8String], MAX_TITLE);
        roomInfo.title[MAX_TITLE] = 0;
        roomInfo.max_users = selRoom.maxUsers;
        roomInfo.curr_users = selRoom.currUsers;
        
        // 메시지에 선택한 방 번호를 넣어 서버에 전송한다.
        memset(msgbuf, 0, MAX_LINE+1);
        msglen = 0;
        msgbuf_p = writeintbuf(msgbuf, &msglen, MSG_ROOM_ENTER);  // 메시지의 종류를 MSG_ROOM_ENTER로 설정한다.
        writeintbuf(msgbuf_p, &msglen, roomInfo.no);              // 메시지에 들어갈 방 번호를 넣는다.
        if(write(sendSock, msgbuf, msglen) < 0) {
            printf("ERROR: 방 들어가기 메시지 전송 실패\n");
            exit(0);
        }
        
        printf("LOG: 방 들어가기 요청: %d번방\n", (int)selRoom.roomNo);
    }
}

// 방에 들어가 있을 때 방을 나가는 방 나가기 버튼의 액션 메소드
- (IBAction)clickExitRoom:(id)sender
{
    // 메시지 전송을 위한 임시 데이터
    char msgbuf[MAX_LINE+1] = {0,};
    char *msgbuf_p = NULL;
    int msglen = 0;
    
    msgbuf_p = writeintbuf(msgbuf, &msglen, MSG_ROOM_EXIT);  // 메시지의 종류를 MSG_ROOM_EXIT로 설정한다.
    writeintbuf(msgbuf_p, &msglen, roomInfo.no);             // 메시지에 방 번호를 넣는다.
    if(write(sendSock, msgbuf, msglen) < 0) {
        printf("ERROR: 방 나가기 메시지 전송 실패\n");
        exit(0);
    }
    
    printf("LOG: 방 나가기 요청: %d번방\n", roomInfo.no);
}

// 설정 창을 여는 설정 버튼의 액션 메소드
- (IBAction)clickSetting:(id)sender
{
    SettingController *settingController = [SettingController instanceWithIpAddr: [NSString stringWithUTF8String: servIp]
                                                                         andPort: servPort];
    
    // 로그인 상태라면, 설정 창의 IP주소 및 포트번호를 변경하지 못하게 한다.
    if(isLogin) {
        [settingController.ipAddrField setEditable: NO];
        [settingController.portField setEditable: NO];
    }
    else {
        [settingController.ipAddrField setEditable: YES];
        [settingController.portField setEditable: YES];
    }
    
    // 설정 창에서 확인 버튼을 누른 경우, 서버의 IP주소 및 포트번호를 입력한 값으로 설정한다.
    if([NSApp runModalForWindow: settingController.window]) {
        memset(servIp, 0, 16);
        strncpy(servIp, [settingController.ipAddr UTF8String], 15);
        servIp[15] = 0;
        servPort = settingController.port;
        
        printf("LOG: 설정을 변경했습니다: %s, %d\n", servIp, servPort);
    }
}

// 프로그램을 종료하는 종료 버튼의 액션 메소드
- (IBAction)clickClose:(id)sender
{
    // 로그인 상태라면, 로그아웃을 하고 종료한다.
    if(isLogin)
        [self sendLogoutRequest];
    
    // 프로그램을 초기화하고 종료한다.
    [self initProgram];

    printf("LOG: 프로그램을 종료합니다.\n");
    exit(0);
}

//
//  private 메소드 정의
//

// 프로그램을 초기화하는 메소드
// (필히 메인 스레드에서 호출)
- (void)initProgram
{
    // 수신소켓 및 송신소켓을 해제한다.
    if(recvSock > 0) close(recvSock);
    if(sendSock > 0) close(sendSock);

    // 초기화 작업
    recvSock = sendSock = 0;
    isLogin = NO;
    friendCount = 10;
    roomInfo.no = roomInfo.max_users = roomInfo.curr_users = 0;
    memset(roomInfo.title, 0, MAX_TITLE+1);
    
    // 친구 목록 작성
    if(!friendList)
        friendList = [[NSMutableArray alloc] init];
    else
        [friendList removeAllObjects];
    
    // 사용자 목록 테이블 뷰를 갱신한다.
    [userListTable reloadData];
    
    // 뷰를 모두 비운다.
    [_chatArea setEditable: YES];
    [_chatArea setString: @""];
    [_chatArea setEditable: NO];
    [_inputField setStringValue: @""];
    
    // 로그인을 하기 전에는 텍스트를 전송하지 못하게 한다.
    [_inputField setEditable: NO];
    [_sendBtn setEnabled: NO];
    
    // 다시 로그인할 수 있도록 설정한다.
    [_titleLabel setStringValue: @"로비"];
    [_idField setEditable: YES];
    [_loginBtn setTitle: @"로그인"];
    [_makeBtn setEnabled: NO];
    [_enterBtn setEnabled: NO];
    [_exitBtn setEnabled: NO];
}

// 서버에 연결하여 수신소켓과 송신소켓을 설정하는 메소드
- (void)initSocketAndConnect
{
    @autoreleasepool {
        // 수신소켓 및 송신소켓 생성
        if((recvSock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
            printf("ERROR: 수신소켓 생성 실패\n");
            exit(0);
        }
        if((sendSock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
            printf("ERROR: 송신소켓 생성 실패\n");
            exit(0);
        }
        
        // 서버에 대한 소켓정보 초기화
        struct sockaddr_in servaddr;
        bzero((char *)&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        inet_pton(AF_INET, servIp, &servaddr.sin_addr);
        servaddr.sin_port = htons(servPort);
        
        char msgbuf[MAX_LINE+1] = {0,};
        unsigned long msglen;
        const char *idStr = [userId UTF8String];
        
        // 수신소켓을 서버에 연결
        if(connect(recvSock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
            printf("ERROR: 수신소켓 연결 실패\n");
            return;
        }
        
        // 서버에 아이디와 소켓의 역할(수신)을 전송
        strcpy(msgbuf, idStr);
        strcat(msgbuf, ";recv");
        msglen = strlen(msgbuf);
        if(write(recvSock, msgbuf, msglen) < 0) {
            printf("ERROR: 수신소켓 연결 요청 메시지 전송 실패\n");
            exit(0);
        }
        
        // 서버로부터 로그인 관련 메시지를 받는다.
        if((msglen = read(recvSock, msgbuf, MAX_LINE)) <= 0) {
            printf("ERROR: 로그인 관련 메시지 수신 실패\n");
            exit(0);
        }
        msgbuf[msglen] = 0;
        
        // 로그인 실패 메시지를 받는 경우(ID가 없는 경우)
        if(strcmp(msgbuf, "loginfail1") == 0) {
            NSAlert *alert = [[NSAlert alloc] init];
            [alert setAlertStyle: NSWarningAlertStyle];
            [alert setMessageText: @"해당 아이디는 존재하지 않습니다."];
            [alert addButtonWithTitle: @"확인"];
            [alert runModal];
            
            close(recvSock);
            return;
        }
        // 로그인 실패 메시지를 받는 경우(ID가 중복된 경우)
        else if(strcmp(msgbuf, "loginfail2") == 0) {
            NSAlert *alert = [[NSAlert alloc] init];
            [alert setAlertStyle: NSWarningAlertStyle];
            [alert setMessageText: @"해당 아이디는 이미 로그인 중입니다."];
            [alert addButtonWithTitle: @"확인"];
            [alert runModal];
            
            close(recvSock);
            return;
        }
        
        // 송신소켓을 서버에 연결
        [NSThread sleepForTimeInterval: 0.1];
        if(connect(sendSock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
            printf("ERROR: 송신소켓 연결 실패\n");
            exit(0);
        }
        
        // 서버에 아이디와 소켓의 역할(송신)을 전송
        memset(msgbuf, 0, MAX_LINE+1);
        strcpy(msgbuf, idStr);
        strcat(msgbuf, ";send");
        msglen = strlen(msgbuf);
        if(write(sendSock, msgbuf, msglen) < 0) {
            printf("ERROR: 송신소켓 연결 요청 메시지 전송 실패\n");
            exit(0);
        }
        
        // 로그인에 성공했다.
        isLogin = YES;
    }
}

// 메시지 수신 스레드
- (void)receiveThread
{
    @autoreleasepool {
        char msgbuf[MAX_LINE+1] = {0,};
        char *msgbuf_p = NULL;
        long nbyte;
        msg_kind msgkind = MSG_NONE;
        
        while(1) {
            // 서버로부터 메시지를 받는다.
            if((nbyte = read(recvSock, msgbuf, MAX_LINE)) < 0) {
                printf("ERROR: 메시지 수신 실패\n");
                break;
            }
            // 메시지가 비어있다면 다시 받는다.
            else if(nbyte == 0) {
                printf("ERROR: 서버와의 연결이 끊겼습니다.\n");
                
                // 프로그램을 초기화한다.
                [self performSelectorOnMainThread: @selector(initProgram)
                                       withObject: nil
                                    waitUntilDone: YES];
                break;
            }
            msgbuf[nbyte] = 0;
            
            // 수신한 메시지의 종류를 파악한다.
            msgbuf_p = msgbuf;
            msgkind = get_msgkind(&msgbuf_p);
            printf("LOG: 수신한 메시지 종류: %s\n", msg2str(msgkind));

            // 텍스트 메시지인 경우: "[메시지 종류(4)][텍스트(?)]"
            if(msgkind == MSG_TEXT) {
                printf("LOG: 텍스트 수신 성공\n");
                
                // 채팅화면에 수신한 텍스트 출력
                [self performSelectorOnMainThread: @selector(displayTextOnChatArea:)
                                       withObject: [NSString stringWithUTF8String: msgbuf_p]
                                    waitUntilDone: YES];
            }
            // 로그아웃 메시지인 경우: "[메시지 종류(4)]"
            else if(msgkind == MSG_USER_LOGOUT) {
                // 프로그램을 초기화한다.
                [self performSelectorOnMainThread: @selector(initProgram)
                                       withObject: nil
                                    waitUntilDone: YES];
                
                printf("LOG: 로그아웃 성공: %s\n", [userId UTF8String]);
                
                // 접속이 끊겼으므로 스레드를 종료한다.
                break;
            }
            // 사용자 접속 메시지인 경우: "[메시지 종류(4)][사용자 수(4)][사용자 ID(MAX_ID)]..."
            else if(msgkind == MSG_USER_CONNECT) {
                // 현재 접속 중인 사용자 수를 읽는다.
                int count = 0;
                msgbuf_p = readintbuf(msgbuf_p, NULL, &count);

                // 접속 중인 사용자 수에 따라 친구 목록을 갱신한다.
                for(; count > 0; count--) {
                    // 다음 사용자 ID를 읽는다.
                    char idbuf[MAX_ID+1] = {0,};
                    msgbuf_p = readstrbuf(msgbuf_p, NULL, idbuf, MAX_ID);
                    
                    // 친구 목록 갱신
                    for(int i = 0; i < friendCount; i++) {
                        // 수신한 사용자 ID와 일치하는 ID를 로그인 상태로 설정한다.
                        ChatUser *tmpUser = [friendList objectAtIndex: i];
                        if([tmpUser.userId isEqualToString: [NSString stringWithUTF8String: idbuf]]) {
                            tmpUser.login = YES;
                            
                            // 사용자 목록 테이블 뷰를 갱신한다.
                            [userListTable performSelectorOnMainThread: @selector(reloadData)
                                                            withObject: nil
                                                         waitUntilDone: YES];
                            break;
                        }
                    }
                    
                    // 채팅화면에 로그인한 사용자를 알린다.
                    // 마지막에 로그인한 사용자 ID를 넣었으므로 해당 사용자는 채팅화면에 출력한다.
                    if(count == 1) {
                        [self performSelectorOnMainThread: @selector(displayTextOnChatArea:)
                                               withObject: [NSString stringWithFormat: @"[%s]이(가) 접속했습니다.", idbuf]
                                            waitUntilDone: YES];
                    }
                }
            }
            // 사용자 접속 끊기 메시지인 경우: "[메시지 종류(4)][사용자 ID(MAX_ID)]"
            else if(msgkind == MSG_USER_DISCONN) {
                // 접속을 끊은 사용자 ID를 읽는다.
                char idbuf[MAX_ID+1] = {0,};
                readstrbuf(msgbuf_p, NULL, idbuf, MAX_ID);
                
                // 친구 목록 갱신
                for(int i = 0; i < friendCount; i++) {
                    // 수신한 사용자 ID와 일치하는 ID를 로그아웃 상태로 설정한다.
                    ChatUser *tmpUser = [friendList objectAtIndex: i];
                    if([tmpUser.userId isEqualToString: [NSString stringWithUTF8String: idbuf]]) {
                        tmpUser.login = NO;
                        
                        // 사용자 목록 테이블 뷰를 갱신한다.
                        [userListTable performSelectorOnMainThread: @selector(reloadData)
                                                        withObject: nil
                                                     waitUntilDone: YES];
                        
                        // 채팅화면에 로그아웃한 사용자를 알린다.
                        [self performSelectorOnMainThread: @selector(displayTextOnChatArea:)
                                               withObject: [NSString stringWithFormat: @"[%s]이(가) 접속을 끊었습니다.", idbuf]
                                            waitUntilDone: YES];
                        break;
                    }
                }
            }
            // 방 만들기 메시지인 경우: "[메시지 종류(4)][방 번호(4)]"
            else if(msgkind == MSG_ROOM_MAKE) {
                // 방 번호를 읽는다.
                readintbuf(msgbuf_p, NULL, &roomInfo.no);
                
                // 수신한 방 번호가 -1이면, 방 수가 초과되었으므로 만들 수가 없다.
                if(roomInfo.no == -1) {
                    // 방 정보를 초기화한다.
                    memset(roomInfo.title, 0, MAX_TITLE+1);
                    roomInfo.no = roomInfo.max_users = roomInfo.curr_users = 0;
                    
                    // 방 만들기 실패를 알리는 창을 띄운다.
                    NSAlert *alert = [[NSAlert alloc] init];
                    [alert setAlertStyle: NSWarningAlertStyle];
                    [alert setMessageText: @"더 이상 방을 만들 수 없습니다."];
                    [alert addButtonWithTitle: @"확인"];
                    [alert runModal];
                    
                    printf("LOG: 방 만들기 실패\n");
                    continue;
                }
                
                // 방 제목을 설정한다.
                NSString *title = [NSString stringWithFormat: @"[%d번방] ", roomInfo.no];
                title = [title stringByAppendingString: [NSString stringWithUTF8String: roomInfo.title]];
                [_titleLabel setEditable: YES];
                [_titleLabel setStringValue: title];
                [_titleLabel setEditable: NO];
                
                // 채팅화면을 비우고, 방 입장을 알린다.
                [_chatArea setEditable: YES];
                [_chatArea setString: @""];
                [_chatArea setEditable: NO];
                [self performSelectorOnMainThread: @selector(displayTextOnChatArea:)
                                       withObject: [NSString stringWithFormat: @"[%@]이(가) 방에 들어왔습니다.", userId]
                                    waitUntilDone: YES];
                
                // 방에 들어가면, 방 나가기 버튼만 활성화된다.
                [_makeBtn setEnabled: NO];
                [_enterBtn setEnabled: NO];
                [_exitBtn setEnabled: YES];
                
                printf("LOG: 방 만들기 성공: %d번 방\n", roomInfo.no);
            }
            // 방 목록 요청 메시지인 경우: "[메시지 종류(4)][개설된 방 수(4)][방 정보(4+MAX_TITLE+4+4)]..."
            else if(msgkind == MSG_ROOM_LIST) {
                // 방 정보 임시 데이터
                room_info tempRoom;
                
                // 개설된 방 수를 읽는다.
                int count = 0;
                msgbuf_p = readintbuf(msgbuf_p, NULL, &count);
                
                // 방 정보를 읽어 방 목록 배열에 넣는다.
                for(; count > 0; count--) {
                    msgbuf_p = readintbuf(msgbuf_p, NULL, &tempRoom.no);
                    msgbuf_p = readstrbuf(msgbuf_p, NULL, tempRoom.title, MAX_TITLE);
                    tempRoom.title[MAX_TITLE] = 0;
                    msgbuf_p = readintbuf(msgbuf_p, NULL, &tempRoom.max_users);
                    msgbuf_p = readintbuf(msgbuf_p, NULL, &tempRoom.curr_users);
                    
                    [enteringRoomController addItemWithNo: tempRoom.no
                                                 andTitle: [NSString stringWithUTF8String: tempRoom.title]
                                              andMaxUsers: tempRoom.max_users
                                             andCurrUsers: tempRoom.curr_users];
                }
            }
            // 방 들어가기 메시지인 경우: "[메시지 종류(4)]"
            else if(msgkind == MSG_ROOM_ENTER) {
                // 방 제목을 설정한다.
                NSString *title = [NSString stringWithFormat: @"[%d번방] ", roomInfo.no];
                title = [title stringByAppendingString: [NSString stringWithUTF8String: roomInfo.title]];
                [_titleLabel setEditable: YES];
                [_titleLabel setStringValue: title];
                [_titleLabel setEditable: NO];
                
                // 채팅화면을 비우고, 방 입장을 알린다.
                [_chatArea setEditable: YES];
                [_chatArea setString: @""];
                [_chatArea setEditable: NO];
                
                // 현재 방에 접속 중인 사용자 수를 읽는다.
                int count = 0;
                msgbuf_p = readintbuf(msgbuf_p, NULL, &count);
                
                // 접속 중인 사용자 수에 따라 방 접속을 알린다.
                for(; count > 0; count--) {
                    // 다음 사용자 ID를 읽는다.
                    char idbuf[MAX_ID+1] = {0,};
                    msgbuf_p = readstrbuf(msgbuf_p, NULL, idbuf, MAX_ID);
                    
                    // 방 접속 알림
                    for(int i = 0; i < friendCount; i++) {
                        // 수신한 사용자 ID와 ID가 일치하는 경우
                        ChatUser *tmpUser = [friendList objectAtIndex: i];
                        if([tmpUser.userId isEqualToString: [NSString stringWithUTF8String: idbuf]]) {
                            // 채팅화면에 방에 들어온 사용자를 알린다.
                            [self performSelectorOnMainThread: @selector(displayTextOnChatArea:)
                                                   withObject: [NSString stringWithFormat: @"[%s]이(가) 방에 들어왔습니다.", idbuf]
                                                waitUntilDone: YES];
                            break;
                        }
                    }
                }
                
                /*
                [self performSelectorOnMainThread: @selector(displayTextOnChatArea:)
                                       withObject: [NSString stringWithFormat: @"[%@]이(가) 방에 들어왔습니다.", userId]
                                    waitUntilDone: YES];*/
                
                // 방에 들어가면, 방 나가기 버튼만 활성화된다.
                [_makeBtn setEnabled: NO];
                [_enterBtn setEnabled: NO];
                [_exitBtn setEnabled: YES];
            }
            // 방 나가기 메시지인 경우: "[메시지 종류(4)]"
            else if(msgkind == MSG_ROOM_EXIT) {
                // 방 제목을 설정한다.
                [_titleLabel setEditable: YES];
                [_titleLabel setStringValue: @"로비"];
                [_titleLabel setEditable: NO];
                
                // 채팅화면을 비우고, 로비로 돌아왔음을 알린다.
                [_chatArea setEditable: YES];
                [_chatArea setString: @""];
                [_chatArea setEditable: NO];
                [self performSelectorOnMainThread: @selector(displayTextOnChatArea:)
                                       withObject: [NSString stringWithFormat: @"로비로 돌아왔습니다."]
                                    waitUntilDone: YES];
                
                // 방을 나오면, 방 나가기 버튼은 비활성화되고 나머지 버튼은 활성화된다.
                [_makeBtn setEnabled: YES];
                [_enterBtn setEnabled: YES];
                [_exitBtn setEnabled: NO];
            }
            // 방 접속 메시지인 경우: "[메시지 종류(4)][사용자 수(4)][사용자 ID(MAX_ID)]..."
            else if(msgkind == MSG_ROOM_CONNECT) {
                // 현재 방에 접속 중인 사용자 수를 읽는다.
                int count = 0;
                msgbuf_p = readintbuf(msgbuf_p, NULL, &count);
                
                // 접속 중인 사용자 수에 따라 방 접속을 알린다.
                for(; count > 0; count--) {
                    // 다음 사용자 ID를 읽는다.
                    char idbuf[MAX_ID+1] = {0,};
                    msgbuf_p = readstrbuf(msgbuf_p, NULL, idbuf, MAX_ID);
                    
                    // 방 접속 알림
                    for(int i = 0; i < friendCount; i++) {
                        // 수신한 사용자 ID와 ID가 일치하는 경우
                        ChatUser *tmpUser = [friendList objectAtIndex: i];
                        if([tmpUser.userId isEqualToString: [NSString stringWithUTF8String: idbuf]]) {
                            // 채팅화면에 방에 들어온 사용자를 알린다.
                            [self performSelectorOnMainThread: @selector(displayTextOnChatArea:)
                                                   withObject: [NSString stringWithFormat: @"[%s]이(가) 방에 들어왔습니다.", idbuf]
                                                waitUntilDone: YES];
                            break;
                        }
                    }
                }
            }
            // 방 접속 끊기 메시지인 경우: "[메시지 종류(4)][사용자 ID(MAX_ID)]"
            else if(msgkind == MSG_ROOM_DISCONN) {
                // 방을 나간 사용자 ID를 읽는다.
                char idbuf[MAX_ID+1] = {0,};
                readstrbuf(msgbuf_p, NULL, idbuf, MAX_ID);
                
                // 방 나감 알림
                for(int i = 0; i < friendCount; i++) {
                    // 수신한 사용자 ID와 ID가 일치하는 경우
                    ChatUser *tmpUser = [friendList objectAtIndex: i];
                    if([tmpUser.userId isEqualToString: [NSString stringWithUTF8String: idbuf]]) {
                        // 채팅화면에 방을 나간 사용자를 알린다.
                        [self performSelectorOnMainThread: @selector(displayTextOnChatArea:)
                                               withObject: [NSString stringWithFormat: @"[%s]이(가) 방을 나갔습니다.", idbuf]
                                            waitUntilDone: YES];
                        break;
                    }
                }
            }
        }
    }
}

// 서버에 텍스트를 전송하는 메소드
- (void)sendText
{
    // 로그인 상태가 아니라면, 텍스트를 전송할 수 없다.
    if(!isLogin)
        return;
    
    @autoreleasepool {
        // 메시지 전송을 위한 임시 데이터
        char msgbuf[MAX_LINE+1] = {0,};
        char *msgbuf_p = NULL;
        int msglen = 0;
        const char *text;
        
        // 메시지의 종류를 MSG_TEXT로 설정한다.
        msgbuf_p = writeintbuf(msgbuf, &msglen, MSG_TEXT);
        
        // 입력받은 텍스트를 메시지에 넣는다.
        NSString *inputStr = [userId stringByAppendingString: @": "];
        inputStr = [inputStr stringByAppendingString: _inputField.stringValue];
        text = [inputStr UTF8String];
        writestrbuf(msgbuf_p, &msglen, text, (int)strlen(text) + 1);
        
        // 서버에 메시지를 전송한다.
        if(write(sendSock, msgbuf, msglen) < 0) {
            printf("ERROR: 텍스트 전송 실패\n");
            exit(0);
        }
        printf("LOG: 텍스트 전송 성공: %s\n", text);
        
        // 텍스트입력필드를 비운다.
        [_inputField setStringValue: @""];
    }
}

// 서버에 로그아웃 메시지를 전송하는 메소드
- (void)sendLogoutRequest
{
    // 로그인 상태가 아니라면, 로그아웃을 할 수 없다.
    if(!isLogin)
        return;
    
    // 메시지 전송을 위한 임시 데이터
    char msgbuf[MAX_LINE+1] = {0,};
    char *msgbuf_p = NULL;
    int msglen = 0;
    
    // 메시지의 종류를 MSG_USER_LOGOUT으로 설정한다.
    msgbuf_p = writeintbuf(msgbuf, &msglen, MSG_USER_LOGOUT);
    
    // 서버에 메시지를 전송한다.
    if(write(sendSock, msgbuf, msglen) < 0) {
        printf("ERROR: 로그아웃 메시지 전송 실패\n");
        exit(0);
    }
}

// 채팅화면에 텍스트를 출력하는 메소드
// (필히 메인 스레드에서 호출)
- (void)displayTextOnChatArea:(NSString *)displayString
{
    [_chatArea setEditable: YES];
    [_chatArea moveToEndOfDocument: nil];
    [_chatArea insertText: [displayString stringByAppendingString: @"\n"]];
    [_chatArea setEditable: NO];
}

@end
