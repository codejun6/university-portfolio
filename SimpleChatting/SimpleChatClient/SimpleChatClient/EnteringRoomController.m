//
//  EnteringRoomController.m
//  SimpleChatClient
//
//  Created by MS MacBook on 2015. 6. 12..
//  Copyright (c) 2015년 MS MacBook. All rights reserved.
//

#import "EnteringRoomController.h"
#import "RoomItem.h"

@implementation EnteringRoomController

@synthesize roomNo;

// 초기화 메소드
- (id)init
{
    self = [super init];
    if(self)
        roomList = [[NSMutableArray alloc] init];
    return self;
}

// 객체를 초기화하고 반환하는 팩토리 메소드
+ (EnteringRoomController *)instance
{
    EnteringRoomController *enteringRoomController = [[EnteringRoomController alloc] initWithWindowNibName: @"EnteringRoomWindow"];
    enteringRoomController.roomNo = 0;
    
    return enteringRoomController;
}

// 현재 테이블 뷰의 행의 수를 반환하는 메소드
- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    return roomList.count;
}

// 테이블 뷰의 특정 항목에 해당하는 객체의 특정 값을 반환하는 메소드
- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    NSString *identifier = [tableColumn identifier];
    RoomItem *roomItem = [roomList objectAtIndex: row];
    return [roomItem valueForKey: identifier];
}

// 버튼을 눌렀을 경우, 창을 닫는 메소드
- (IBAction)clickButton:(NSButton *)sender
{
    // 누른 버튼의 태그값에 따라 처리를 한다.
    // 들어가기 버튼을 눌렀다면, 텍스트 필드에 설정한 값으로 IP주소와 포트번호를 설정한다.
    if(sender.tag) {
    }
    
    // 누른 버튼의 태그값을 반환하며 Modal모드를 중지하고, 창을 닫는다.
    // 들어가기 버튼: 1, 취소 버튼: 0
    [NSApp stopModalWithCode: sender.tag];
    [self.window close];
}

// 방 목록 배열에 항목을 추가하는 메소드
- (void)addItemWithNo:(NSInteger)no andTitle:(NSString *)title andMaxUsers:(int)max
{
    if(!roomList)
        roomList = [[NSMutableArray alloc] init];
    [roomList addObject: [RoomItem initWithNo: no andTitle: title andMaxUsers: max]];
    //[roomListTable reloadData];
}

// 방 목록 배열에 항목을 추가하는 메소드
- (void)addItemWithNo:(NSInteger)no andTitle:(NSString *)title andMaxUsers:(int)max andCurrUsers:(int)curr
{
    if(!roomList)
        roomList = [[NSMutableArray alloc] init];
    [roomList addObject: [RoomItem initWithNo: no andTitle: title andMaxUsers: max andCurrUsers: curr]];
    //[roomListTable reloadData];
}

// 방 목록 테이블 뷰에서 선택된 행의 방 번호를 반환하는 메소드
- (RoomItem *)getSelectedRoomItem
{
    NSInteger row = [roomListTable selectedRow];
    if(row < 0 || row > roomList.count)
        return nil;
    
    return [roomList objectAtIndex: row];
}

@end
