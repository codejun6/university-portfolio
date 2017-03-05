#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include "msjc_io.h"
#include "msq.h"

#define MAX_LINE 1023  // 최대 버퍼 길이
#define MAX_ID 8      // 최대 ID 길이
#define MAX_TITLE 95  // 최대 방 제목 길이

#define MAX_USER 50  // 최대 사용자 수

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

/* 사용자 정보 */
typedef struct _user_info {
    char id[MAX_ID+1];  // 사용자 ID
    int recvsock;       // 수신소켓
    int sendsock;       // 송신소켓
    int room_no;        // 소속 방 번호
    int is_exit;        // 방을 나갔는지 여부
} user_info;

/* 방 정보 */
typedef struct _room_info {
    int no;                   // 방 번호
    char title[MAX_TITLE+1];  // 방 제목
    int max_users;            // 최대 인원
    int curr_users;           // 현재 인원
} room_info;

static int g_listen_sock;

static msq_queue *g_text_queue[MAX_USER+1] = {0,};      // 텍스트 큐 배열(방 하나당 큐 하나 할당)
static pthread_mutex_t g_text_lock[MAX_USER+1] = {0,};  // 텍스트 큐 접근에 대한 뮤텍스 배열

static user_info g_userlist[MAX_USER] = {0,};  // 사용자 목록
static int g_user_cnt = 0;                     // 사용자 수
static int g_login_cnt = 0;                    // 접속 중인 사용자 수

static room_info g_roomlist[MAX_USER+1] = {0,};  // 방 목록
static int g_room_cnt = 0;                       // 생성된 방의 수
static int g_destroy_room_no = -1;               // 스레드를 종료할 방 번호

void *broadcast_thread(void *room_no);         // 텍스트를 뿌리는 스레드
void *receive_thread(void *index);             // 메시지 수신 스레드
void enqueue_text(int idx, const char *text);  // 텍스트를 텍스트 큐에 넣는 함수

void init_server(int port);           // 서버 초기화 함수
void init_userlist();                 // 사용자 목록 초기화 함수
msg_kind get_msgkind(char **msg_p);   // 메시지 종류 추출 함수
int search_userid(const char *id);    // 사용자 ID 검색 함수
int check_connection(int idx);        // 사용자 접속 여부 검사 함수
void chk_destroy_room(int room_no);   // 방 파괴 함수

pthread_t tid;

// 메인 함수
int main(int argc, char *argv[])
{
    struct sockaddr_in cliaddr;
    unsigned int addrlen = sizeof(cliaddr);
    int accp_sock;
    int i;
    
    if(argc != 2) {
        printf("사용법: %s port\n", argv[0]);
        exit(0);
    }
    
    // 서버소켓을 준비하고, 서버를 초기화한다.
    init_server(atoi(argv[1]));
    
    // 사용자들에게 텍스트를 뿌리는 스레드를 수행한다.
    pthread_create(&tid, NULL, broadcast_thread, NULL);
    
    // 사용자의 로그인을 처리한다.
    puts("LOG: 서버 시작");
    while(1) {
        // 메시지 전송을 위한 임시 데이터
        char msgbuf[MAX_LINE+1] = {0,};
        char *msgbuf_p = NULL;
        int msglen = 0;
        int nbyte;
        char *tok;
        char id[MAX_ID+1] = {0,};
        int useridx;
        
        // 사용자가 접속할 때까지 대기한다.
        puts("LOG: 사용자 접속 대기 중");
        if((accp_sock = accept(g_listen_sock, (struct sockaddr *)&cliaddr, &addrlen)) < 0) {
            perror("ERROR: Accept 실패");
            break;
        }
        
        // 메시지를 수신한다.
        if((nbyte = read(accp_sock, msgbuf, MAX_LINE)) < 0) {
            perror("ERROR: 로그인 요청 수신 실패");
            close(accp_sock);
            break;
        }
        msgbuf[nbyte] = 0;

        tok = strtok(msgbuf, ";");
        strcpy(id, tok);
        tok = strtok(NULL, ";");
        printf("LOG: [%s][%s] 연결 요청\n", id, tok);
        
        // 접속한 사용자가 수신소켓 연결을 요청한 경우
        if(strcmp(tok, "recv") == 0) {
            int isloginfail = 0;
            
            // 해당 ID의 사용자가 없다면, 로그인 실패이다.
            if((useridx = search_userid(id)) < 0) {
                printf("LOG: 해당 사용자가 존재하지 않습니다: %s\n", id);
                sprintf(msgbuf, "loginfail1");
                isloginfail = 1;
            }
            // 해당 사용자의 수신소켓이 이미 설정되어 있다면, 이미 접속 중이므로 로그인 실패이다.
            else if(g_userlist[useridx].recvsock > 0) {
                printf("LOG: 해당 사용자가 이미 접속 중입니다: %s\n", id);
                sprintf(msgbuf, "loginfail2");
                isloginfail = 1;
            }
            
            // 로그인 실패인 경우, 로그인 실패 메시지를 전송한다.
            if(isloginfail) {
                if(write(accp_sock, msgbuf, strlen(msgbuf)) < 0) {
                    perror("ERROR: 로그인 실패 메시지 전송 실패");
                    close(accp_sock);
                    exit(0);
                }
                
                close(accp_sock);
                continue;
            }
            
            // 로그인 성공 메시지를 전송한다.
            sprintf(msgbuf, "loginsuccess");
            if(write(accp_sock, msgbuf, strlen(msgbuf)) < 0) {
                perror("ERROR: 로그인 성공 메시지 전송 실패");
                close(accp_sock);
                exit(0);
            }

            // 수신소켓을 설정한다.
            g_userlist[useridx].recvsock = accp_sock;
        }
        // 접속한 사용자가 송신소켓 연결을 요청한 경우
        else if(strcmp(tok, "send") == 0) {
            int count = 0;
            int i;
            
            // 일치하는 ID가 없는 경우, 에러를 출력하고, 소켓을 닫는다.
            if((useridx = search_userid(id)) < 0) {
                perror("ERROR: 잘못된 연결입니다.");
                close(accp_sock);
                continue;
            }
            
            // 송신소켓을 설정하고, 방 번호를 로비 번호(0)로 초기화하고, 메시지 수신 스레드를 시작시킨다.
            g_userlist[useridx].sendsock = accp_sock;
            g_userlist[useridx].room_no = 0;
            pthread_create(&tid, NULL, receive_thread, &useridx);
            
            // 접속 중인 다른 사용자들에게 로그인을 알린다.
            memset(msgbuf, 0, MAX_LINE+1);
            msglen = 0;
            msgbuf_p = writeintbuf(msgbuf, &msglen, MSG_USER_CONNECT);  // 메시지의 종류를 MSG_USER_CONNECT로 설정한다.
            msgbuf_p = writeintbuf(msgbuf_p, &msglen, 1);               // 메시지에 로그인한 사용자에 해당하는 1을 넣는다.
            msgbuf_p = writestrbuf(msgbuf_p, &msglen, id, MAX_ID);      // 메시지에 로그인한 사용자의 ID를 넣는다.
            *msgbuf_p = '\0'; msglen++;
            for(i = 0; i < g_user_cnt; i++) {
                if(i == useridx)
                    continue;
                
                // 해당 인덱스의 사용자가 접속 중이면, 메시지를 전송한다.
                if(check_connection(i)) {
                    if(write(g_userlist[i].recvsock, msgbuf, msglen) < 0) {
                        perror("ERROR: 로그인 알림 메시지 전송 실패");
                        exit(0);
                    }

                    printf("LOG: [%s]에게 [%s]의 로그인을 알렸습니다.\n", g_userlist[i].id, id);
                }
            }
            
            // 로그인한 사용자에게 접속 중인 사용자 목록을 전송한다.
            memset(msgbuf, 0, MAX_LINE+1);
            msglen = 0;
            msgbuf_p = writeintbuf(msgbuf, &msglen, MSG_USER_CONNECT);   // 메시지의 종류를 MSG_USER_CONNECT로 설정한다.
            msgbuf_p = writeintbuf(msgbuf_p, &msglen, g_login_cnt + 1);  // 메시지에 접속 중인 사용자 수를 넣는다.
            for(i = 0; i < g_user_cnt; i++) {
                // 로그인한 사용자는 마지막에 넣기 위해 무시한다.
                if(i == useridx)
                    continue;
                
                // 해당 인덱스의 사용자가 접속 중이면, 메시지에 해당 ID를 넣는다.
                if(check_connection(i))
                    msgbuf_p = writestrbuf(msgbuf_p, &msglen, g_userlist[i].id, MAX_ID);
            }
            msgbuf_p = writestrbuf(msgbuf_p, &msglen, id, MAX_ID);  // 로그인한 사용자의 ID는 마지막에 넣는다.
            *msgbuf_p = '\0'; msglen++;
            if(write(g_userlist[useridx].recvsock, msgbuf, msglen) < 0) {
                perror("ERROR: 로그인 사용자 목록 전송 실패");
                exit(0);
            }
            
            // 접속 중인 사용자 수를 증가시킨다.
            g_login_cnt++;
        }
        // 알려지지 않은 접속이면, 무시한다.
        else {
            perror("LOG: 알려지지 않은 접속입니다.");
        }
    }
    
    for(i = 0; i < MAX_USER + 1; i++) {
        if(g_text_queue[i] != NULL)
            msq_destroy(g_text_queue[i]);
        if(pthread_mutex_destroy(&g_text_lock[i]) != 0)
            perror("ERROR: 뮤텍스 메모리 해제 실패");
    }
    close(g_listen_sock);
    return 0;
}

// 텍스트를 뿌리는 스레드: 각 방마다 하나씩 할당된다.
void *broadcast_thread(void *room_no)
{
    int room_idx = (room_no)? *((int *)room_no) : 0;
    
    // 메시지 전송을 위한 임시 데이터
    char stdbuf[MAX_LINE+1] = {0,};
    char qmsgbuf[MAX_LINE+1] = {0,};
    char msgbuf[MAX_LINE+1] = {0,};
    char *msgbuf_p = NULL;
    int msglen = 0;
    int i = 0;
    
    // 해당 방 번호의 텍스트 큐에 대해 반복 처리한다.
    while(1) {
        // 해당 스레드를 종료한다.
        if(g_destroy_room_no == room_idx) {
            g_destroy_room_no = -1;
            break;
        }
        
        // 텍스트 큐가 비어있지 않다면, 텍스트를 꺼내서 사용자들에게 뿌린다.
        if(msq_isempty(g_text_queue[room_idx]) == 0) {
            // 텍스트 큐에서 텍스트를 꺼낸다.
            pthread_mutex_lock(&g_text_lock[room_idx]);
            msq_dequeue(g_text_queue[room_idx], qmsgbuf);
            pthread_mutex_unlock(&g_text_lock[room_idx]);

            // 접속 중인 사용자들에게 메시지를 전송한다.
            for(i = 0; i < g_user_cnt; i++) {
                // 해당 사용자가 접속 중이고 같은 방에 속한다면, 메시지를 전송한다.
                if(check_connection(i) && (g_userlist[i].room_no == room_idx)) {
                    msgbuf_p = NULL;
                    msglen = 0;
                    
                    msgbuf_p = writeintbuf(msgbuf, &msglen, MSG_TEXT);             // 메시지의 종류를 MSG_TEXT로 설정한다.
                    writestrbuf(msgbuf_p, &msglen, qmsgbuf, strlen(qmsgbuf) + 1);  // 메시지 큐에서 꺼낸 텍스트를 메시지에 넣는다.
                    if(write(g_userlist[i].recvsock, msgbuf, msglen) < 0) {
                        perror("ERROR: 텍스트 메시지 전송 실패");
                        close(g_userlist[i].recvsock);
                        exit(0);
                    }

                    sprintf(stdbuf, "LOG: [%s]에게 다음 메시지를 전송했습니다: %s\n", g_userlist[i].id, qmsgbuf);
                    write(STDIN_FILENO, stdbuf, strlen(stdbuf));
                }
            }
        }
    }
    
    return NULL;
}

// 메시지를 수신 스레드: 각 사용자마다 하나씩 할당된다.
void *receive_thread(void *index)
{
    // 이 수신 스레드와 연결된 사용자의 정보
    int this_idx = *((int *)index);
    char this_id[MAX_ID+1] = {0,};
    int this_sendsock = g_userlist[this_idx].sendsock;
    int this_recvsock = g_userlist[this_idx].recvsock;
    
    // 메시지 전송을 위한 임시 데이터
    char stdbuf[MAX_LINE+1];
    char msgbuf[MAX_LINE+1];
    char *msgbuf_p = NULL;
    int msglen = 0;
    int nbyte;
    msg_kind msgkind = MSG_NONE;
    int i;

    strncpy(this_id, g_userlist[this_idx].id, MAX_ID);
    this_id[MAX_ID] = 0;

    // 사용자의 접속이 끊기거나 로그아웃을 할 때까지, 반복적으로 메시지를 수신하고 적합한 처리를 한다.
    while(1) {
        // 사용자가 전송한 메시지를 받는다.
        if((nbyte = read(this_sendsock, msgbuf, MAX_LINE)) < 0) {
            perror("ERROR: Receive 스레드 수신 실패");
            break;
        }
        
        // 사용자의 접속이 끊겼다면 스레드를 종료한다.
        if(nbyte == 0) {
            // 해당 사용자가 방에 접속 중이었다면, 방에서 나간 것으로 처리한다.
            if(g_userlist[this_idx].room_no > 0) {
                // 해당 방의 현재 인원을 줄이고, 필요 시 방을 없앤다.
                chk_destroy_room(g_userlist[this_idx].room_no);
            }
            
            // 스레드를 종료한다.
            printf("LOG: 해당 사용자의 접속이 끊겼습니다: %s\n", this_id);
            break;
        }

        // 수신한 메시지의 종류를 파악한다.
        // (메시지 구조에서 []는 의미 단위이고, ()는 차지하는 바이트 수이다.)
        msgbuf_p = msgbuf;
        msgkind = get_msgkind(&msgbuf_p);
        sprintf(stdbuf, "LOG: 수신한 메시지 종류: %s\n", msg2str(msgkind));
        write(STDOUT_FILENO, stdbuf, strlen(stdbuf));

        // 로그아웃 메시지인 경우: "[메시지 종류(4)]"
        if(msgkind == MSG_USER_LOGOUT) {
            // 로그아웃 됐음을 알린다.
            memset(msgbuf, 0, MAX_LINE+1);
            msglen = 0;
            writeintbuf(msgbuf, &msglen, MSG_USER_LOGOUT);  // 메시지의 종류를 MSG_USER_LOGOUT으로 설정한다.
            if(write(this_recvsock, msgbuf, msglen) < 0) {
                perror("ERROR: 로그아웃 메시지 전송 실패");
                exit(0);
            }
            
            // 해당 사용자가 방에 접속 중이었다면, 방에서 나간 것으로 처리한다.
            if(g_userlist[this_idx].room_no > 0) {
                // 해당 방의 현재 인원을 줄이고, 필요 시 방을 없앤다.
                chk_destroy_room(g_userlist[this_idx].room_no);
            }
            
            // 스레드를 종료한다.
            printf("LOG: 해당 사용자가 로그아웃 했습니다: %s\n", this_id);
            break;
        }
        // 텍스트 메시지인 경우: "[메시지 종류(4)][텍스트(?)]"
        else if(msgkind == MSG_TEXT) {
            // 텍스트를 메시지 큐에 넣는다.
            enqueue_text(g_userlist[this_idx].room_no, msgbuf_p);
        }
        // 방 만들기 메시지인 경우: "[메시지 종류(4)][방 제목(MAX_TITLE)][최대 인원(4)]"
        else if(msgkind == MSG_ROOM_MAKE) {
            // 방 수가 초과되었다면, 방 번호로 -1을 전송해 방 만들기 실패를 알린다.
            if(g_room_cnt + 1 > MAX_USER) {
                memset(msgbuf, 0, MAX_LINE+1);
                msglen = 0;
                msgbuf_p = writeintbuf(msgbuf, &msglen, MSG_ROOM_MAKE);  // 메시지의 종류를 MSG_USER_LOGOUT으로 설정한다.
                writeintbuf(msgbuf_p, &msglen, -1);                      // 메시지에 방 만들기 실패를 알리는 방 번호 -1을 넣는다.
                if(write(this_recvsock, msgbuf, msglen) < 0) {
                    perror("ERROR: 방 만들기 실패 메시지 전송 실패");
                    exit(0);
                }
                
                printf("LOG: 방 수가 초과하여 방을 만들 수 없습니다: %d개\n", g_room_cnt);
                continue;
            }
            
            // 로비(0번) 이후부터 빈 방의 인덱스를 찾는다.
            for(i = 1; i <= MAX_USER; i++) {
                // 방 번호가 0이면 빈 방으로 판정한다.
                if(g_roomlist[i].no == 0) {
                    // 빈 방의 인덱스를 이용해 방 정보와 해당 사용자의 방 번호를 설정한다.
                    g_roomlist[i].no = g_userlist[this_idx].room_no = i;
                    g_roomlist[i].curr_users = 1;
                    msgbuf_p = readstrbuf(msgbuf_p, NULL, g_roomlist[i].title, MAX_TITLE);
                    g_roomlist[i].title[MAX_TITLE] = 0;
                    readintbuf(msgbuf_p, NULL, &g_roomlist[i].max_users);
                    g_room_cnt++;
                    
                    // 해당 방에 대한 텍스트 큐를 생성하고, 텍스트를 뿌리는 스레드를 실행시킨다.
                    g_text_queue[i] = msq_create();
                    pthread_create(&tid, NULL, broadcast_thread, &g_userlist[this_idx].room_no);
                    break;
                }
            }
            
            // 개설한 방의 번호를 메시지에 넣어 전송한다.
            memset(msgbuf, 0, MAX_LINE+1);
            msglen = 0;
            msgbuf_p = writeintbuf(msgbuf, &msglen, MSG_ROOM_MAKE);        // 메시지의 종류를 MSG_USER_LOGOUT으로 설정한다.
            writeintbuf(msgbuf_p, &msglen, g_userlist[this_idx].room_no);  // 메시지에 개설한 방의 번호를 넣는다.
            if(write(this_recvsock, msgbuf, msglen) < 0) {
                perror("ERROR: 방 만들기 메시지 전송 실패");
                exit(0);
            }
            
            printf("LOG: 방을 성공적으로 만들었습니다: %d번방\n", g_userlist[this_idx].room_no);
        }
        // 방 목록 요청 메시지인 경우: "[메시지 종류(4)]"
        else if(msgkind == MSG_ROOM_LIST) {
            memset(msgbuf, 0, MAX_LINE+1);
            msglen = 0;
            
            msgbuf_p = writeintbuf(msgbuf, &msglen, MSG_ROOM_LIST);  // 메시지의 종류를 MSG_ROOM_LIST로 설정한다.
            msgbuf_p = writeintbuf(msgbuf_p, &msglen, g_room_cnt);   // 메시지에 개설된 방 수를 넣는다.
            
            // 로비(0번) 이후부터 개설된 방을 메시지에 넣는다.
            for(i = 1; i <= MAX_USER; i++) {
                // 방 번호가 0보다 크면, 개설된 방으로 판정한다.
                if(g_roomlist[i].no > 0) {
                    msgbuf_p = writeintbuf(msgbuf_p, &msglen, g_roomlist[i].no);
                    msgbuf_p = writestrbuf(msgbuf_p, &msglen, g_roomlist[i].title, MAX_TITLE);
                    msgbuf_p = writeintbuf(msgbuf_p, &msglen, g_roomlist[i].max_users);
                    msgbuf_p = writeintbuf(msgbuf_p, &msglen, g_roomlist[i].curr_users);
                }
            }
            
            // 방 목록 메시지를 전송한다.
            if(write(this_recvsock, msgbuf, msglen) < 0) {
                perror("ERROR: 방 목록 메시지 전송 실패");
                exit(0);
            }
        }
        // 방 들어가기 메시지인 경우: "[메시지 종류(4)][방 번호(4)]"
        else if(msgkind == MSG_ROOM_ENTER) {
            // 방 번호를 읽는다.
            int room_no = -1;
            readintbuf(msgbuf_p, NULL, &room_no);
            
            // 해당 방의 현재 인원을 늘리고, 사용자가 해당 방에 소속되게 한다.
            g_roomlist[room_no].curr_users++;
            g_userlist[this_idx].room_no = room_no;
            
            // 방에 들어간 사용자에게 접속 중인 사용자 목록을 전송한다.
            memset(msgbuf, 0, MAX_LINE+1);
            msglen = 0;
            msgbuf_p = writeintbuf(msgbuf, &msglen, MSG_ROOM_ENTER);                    // 메시지의 종류를 MSG_ROOM_ENTER로 설정한다.
            msgbuf_p = writeintbuf(msgbuf_p, &msglen, g_roomlist[room_no].curr_users);  // 메시지에 현재 인원을 넣는다.
            for(i = 0; i < g_user_cnt; i++) {
                // 방에 들어간 사용자는 무시한다.
                if(i == this_idx)
                    continue;
                
                // 해당 인덱스의 사용자가 접속 중이고 같은 방에 속하면, 메시지에 해당 ID를 넣는다.
                if(check_connection(i) && room_no == g_userlist[i].room_no)
                    msgbuf_p = writestrbuf(msgbuf_p, &msglen, g_userlist[i].id, MAX_ID);
            }
            msgbuf_p = writestrbuf(msgbuf_p, &msglen, this_id, MAX_ID);  // 방에 들어간 사용자의 ID는 마지막에 넣는다.
            *msgbuf_p = '\0'; msglen++;
            if(write(this_recvsock, msgbuf, msglen) < 0) {
                perror("ERROR: 방 접속 사용자 목록 전송 실패");
                exit(0);
            }
            
            // 같은 방에 있는 사용자들에게 해당 사용자가 방에 들어왔음을 알린다.
            memset(msgbuf, 0, MAX_LINE+1);
            msglen = 0;
            msgbuf_p = writeintbuf(msgbuf, &msglen, MSG_ROOM_CONNECT);  // 메시지의 종류를 MSG_ROOM_CONNECT로 설정한다.
            msgbuf_p = writeintbuf(msgbuf_p, &msglen, 1);               // 메시지에 접속한 사용자에 해당하는 1을 넣는다.
            writestrbuf(msgbuf_p, &msglen, this_id, MAX_ID);            // 메시지에 방에 들어온 사용자의 ID를 넣는다.
            for(i = 0; i < g_user_cnt; i++) {
                // 현재 사용자에게는 메시지를 전송하지 않는다.
                if(i == this_idx)
                    continue;
                
                // 해당 인덱스의 사용자가 접속 중이고 같은 방에 속하면, 메시지를 전송한다.
                if(check_connection(i) && room_no == g_userlist[i].room_no) {
                    if(write(g_userlist[i].recvsock, msgbuf, msglen) < 0) {
                        perror("ERROR: 방 접속 메시지 전송 실패");
                        exit(0);
                    }
                    
                    printf("LOG: [%s]에게 [%s]의 방 접속을 알렸습니다.\n", g_userlist[i].id, this_id);
                }
            }
        }
        // 방 나가기 메시지인 경우: "[메시지 종류(4)][방 번호(4)]"
        else if(msgkind == MSG_ROOM_EXIT) {
            // 방 번호를 읽는다.
            int room_no = -1;
            readintbuf(msgbuf_p, NULL, &room_no);
            
            // 해당 사용자는 로비 번호(0)로 초기화해 로비로 돌아간다.
            g_userlist[this_idx].room_no = 0;
            
            // 정상적으로 방을 나갔음을 알린다.
            memset(msgbuf, 0, MAX_LINE+1);
            msglen = 0;
            writeintbuf(msgbuf, &msglen, MSG_ROOM_EXIT);
            if(write(this_recvsock, msgbuf, msglen) < 0) {
                perror("ERROR: 방 나가기 메시지 전송 실패");
                exit(0);
            }
            
            // 같은 방에 있던 사용자들에게 해당 사용자가 방을 나갔음을 알린다.
            memset(msgbuf, 0, MAX_LINE+1);
            msglen = 0;
            msgbuf_p = writeintbuf(msgbuf, &msglen, MSG_ROOM_DISCONN);  // 메시지의 종류를 MSG_ROOM_DISCONN으로 설정한다.
            writestrbuf(msgbuf_p, &msglen, this_id, MAX_ID);            // 메시지에 방을 나가는 사용자의 ID를 넣는다.
            for(i = 0; i < g_user_cnt; i++) {
                // 현재 사용자에게는 메시지를 전송하지 않는다.
                if(i == this_idx)
                    continue;
                
                // 해당 인덱스의 사용자가 접속 중이고 같은 방에 속하면, 메시지를 전송한다.
                if(check_connection(i) && room_no == g_userlist[i].room_no) {
                    if(write(g_userlist[i].recvsock, msgbuf, msglen) < 0) {
                        perror("ERROR: 방 접속끊김 메시지 전송 실패");
                        exit(0);
                    }
                    
                    printf("LOG: [%s]에게 [%s]의 방 나감을 알렸습니다.\n", g_userlist[i].id, this_id);
                }
            }
            
            // 해당 방의 현재 인원을 줄이고, 필요 시 방을 없앤다.
            chk_destroy_room(room_no);
        }
    }
    
    // 접속 중인 사용자들에게 로그아웃을 알린다.
    memset(msgbuf, 0, MAX_LINE+1);
    msglen = 0;
    msgbuf_p = writeintbuf(msgbuf, &msglen, MSG_USER_DISCONN);  // 메시지의 종류를 MSG_USER_DISCONN으로 설정한다.
    writestrbuf(msgbuf_p, &msglen, this_id, MAX_ID);            // 메시지에 로그아웃한 사용자의 ID를 넣는다.
    for(i = 0; i < g_user_cnt; i++) {
        // 로그아웃한 사용자에게는 메시지를 전송하지 않는다.
        if(i == this_idx)
            continue;
        
        // 해당 인덱스의 사용자가 접속 중이면, 메시지를 전송한다.
        if(check_connection(i)) {
            if(write(g_userlist[i].recvsock, msgbuf, msglen) < 0) {
                perror("ERROR: 연결끊김 메시지 전송 실패");
                exit(0);
            }
            
            printf("LOG: [%s]에게 [%s]의 로그아웃을 알렸습니다.\n", g_userlist[i].id, this_id);
        }
    }
    
    // 종료한 사용자의 소켓을 초기화한다.
    close(g_userlist[this_idx].recvsock);
    close(g_userlist[this_idx].sendsock);
    g_userlist[this_idx].recvsock = g_userlist[this_idx].sendsock = -1;
    g_userlist[this_idx].room_no = -1;
    
    // 접속 중인 사용자 수를 감소시킨다.
    g_login_cnt--;
    return NULL;
}

// 텍스트를 텍스트 큐에 넣는 함수
void enqueue_text(int idx, const char *text)
{
    char stdbuf[MAX_LINE+1] = {0,};
    char buf[MAX_LINE+1] = {0,};  // 전체를 0으로 초기화함으로써 널문자 종료 처리가 된다.
    readstrbuf((char *)text, NULL, buf, strlen(text));
    
    // 텍스트 큐에 텍스트를 넣는다.
    pthread_mutex_lock(&g_text_lock[idx]);
    msq_enqueue(g_text_queue[idx], buf);
    sprintf(stdbuf, "LOG: 텍스트 큐 삽입 성공: %s\n", buf);
    write(STDOUT_FILENO, stdbuf, strlen(stdbuf));
    pthread_mutex_unlock(&g_text_lock[idx]);
}

// 서버 초기화 함수
void init_server(int port)
{
    struct sockaddr_in servaddr;
    int i = 0;
    
    // 서버주소 정보를 설정한다.
    bzero((char *)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    
    // 서버소켓을 생성한다.
    if((g_listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("ERROR: 서버소켓 생성 실패");
        exit(0);
    }
    
    // 서버주소 정보로 서버소켓을 연결한다.
    if(bind(g_listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("ERROR: Bind 실패");
        exit(0);
    }
    
    // 수신 대기 상태를 활성화한다.
    if(listen(g_listen_sock, 5) < 0) {
        perror("ERROR: Listen 실패");
        exit(0);
    }
    
    // 로비에 대한 텍스트 큐 생성
    g_text_queue[0] = msq_create();
    
    // 텍스트 큐 뮤텍스 배열 초기화
    for(i = 0; i < MAX_USER + 1; i++) {
        if(pthread_mutex_init(&g_text_lock[i], NULL) != 0) {
            perror("ERROR: 뮤텍스 메모리 해제 실패");
            exit(1);
        }
    }
    
    // 사용자 목록을 초기화한다.
    // User0 ~ User9를 정적으로 추가한다.
    init_userlist();
}

// 사용자 목록 초기화 함수
void init_userlist()
{
    char buf[MAX_ID+1];
    int i;
    
    // 사용자 목록에 User0 ~ User9를 정적으로 추가한다.
    // 이 때, 사용자의 소켓은 아직 설정되지 않았으므로 -1로 초기화한다.
    for(i = 0; i < 10; i++) {
        sprintf(buf, "User%d", i);
        strncpy(g_userlist[i].id, buf, strlen(buf)+1);
        g_userlist[i].recvsock = g_userlist[i].sendsock = -1;
        g_userlist[i].room_no = -1;
    }
    g_user_cnt = i;   // 사용자 수를 10으로 초기화한다.
    g_login_cnt = 0;  // 접속 중인 사용자 수를 0으로 초기화한다.
}

// 메시지 종류 추출 함수: 주어진 메시지 버퍼로부터 메시지 종류를 추출
msg_kind get_msgkind(char **msg_p)
{
    msg_kind kind = MSG_NONE;
    *msg_p = readintbuf(*msg_p, NULL, (int *)&kind);
    return kind;
}

// 사용자 ID 검색 함수: 주어진 ID의 사용자를 검색해 인덱스를 반환
int search_userid(const char *id)
{
    int i;

    // 사용자 목록을 검색해 일치하는 ID가 있는지 검사한다.
    for(i = 0; i < g_user_cnt; i++) {
        // 일치하는 ID가 있는 경우, 해당 인덱스를 반환한다.
        if(strcmp(g_userlist[i].id, id) == 0)
            return i;
    }
    
    return -1;  // 일치하는 ID가 없는 경우, -1을 반환한다.
}

// 사용자 접속 여부 검사 함수: 주어진 인덱스의 사용자가 접속 중이면 1을, 아니면 0을 반환
int check_connection(int idx)
{
    // 일치하는 ID가 없는 경우, 0을 반환한다.
    if(idx == -1)
        return 0;
    
    // 수신소켓과 송신소켓 모두 설정되어 있는 경우, 접속 중이므로 1을 반환한다.
    if(g_userlist[idx].recvsock > 0 && g_userlist[idx].sendsock > 0)
        return 1;
    
    return 0;  // 소켓이 하나라도 설정되어 있지 않는 경우, 접속 중이 아니므로 0을 반환한다.
}

// 방 파괴 함수: 해당 방의 현재 인원을 줄이고, 0이면 방을 파괴
void chk_destroy_room(int room_no)
{
    // 해당 방의 현재 인원을 줄인다.
    g_roomlist[room_no].curr_users--;
    
    // 해당 방의 현재 인원이 0이라면, 방을 없앤다.
    if(g_roomlist[room_no].curr_users == 0) {
        // 해당 방 정보를 초기화한다.
        memset(g_roomlist[room_no].title, 0, MAX_TITLE+1);
        g_roomlist[room_no].no = g_roomlist[room_no].max_users = g_roomlist[room_no].curr_users = 0;
        g_room_cnt--;
        
        // 해당 방의 스레드와 텍스트 큐를 해제한다.
        g_destroy_room_no = room_no;
        msq_destroy(g_text_queue[room_no]);
        
        printf("LOG: 방을 성공적으로 없앴습니다: %d번방\n", room_no);
    }
}

