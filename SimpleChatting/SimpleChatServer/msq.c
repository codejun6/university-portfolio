#include "msq.h"

/*
 큐를 생성하는 함수
 
 큐를 동적할당하고 초기화해 그 포인터를 반환한다.
 
 반환값:
 - 생성한 큐의 포인터
 */
msq_queue *msq_create()
{
    // 큐를 메모리에 할당한다.
    msq_queue *q = (msq_queue *)malloc(sizeof(msq_queue));

    // 큐를 초기화한다.
    q->head = q->tail = NULL;
    q->size = 0;
    
    return q;
}

/*
 큐에 데이터를 넣는 함수
 
 큐의 Tail 노드의 뒤에 노드를 추가한다.
 
 반환값:
 - 데이터를 넣은 후, 큐의 크기
 - 에러코드 -1: 할당되지 않은 큐를 전달받은 경우
 */
int msq_enqueue(msq_queue *q, char *data)
{
    msq_node *newnode = NULL;
    
    // 큐가 할당되어있지 않다면, 에러 코드 -1을 반환한다.
    if(q == NULL)
        return -1;
    
    // 새 노드를 메모리에 할당한다.
    newnode = (msq_node *)malloc(sizeof(msq_node));
    // 새 노드의 데이터를 data로 초기화한다.
    newnode->data = (char *)malloc(strlen(data) + 1);
    strncpy(newnode->data, data, strlen(data));
    newnode->data[strlen(data)] = '\0';
    // 새 노드의 다음 노드를 NULL로 초기화한다.
    newnode->next = NULL;
    
    // 큐가 비어있다면, 새 노드가 첫 노드이므로 Head 노드가 새 노드를 가리킨다.
    if(q->size == 0) {
        q->head = newnode;
    }
    // 큐에 노드가 있다면, Tail 노드의 다음 노드를 새 노드로 설정한다.
    else {
        q->tail->next = newnode;
    }
    // 새 노드를 Tail 노드로 설정한다.
    q->tail = newnode;
    
    // 큐의 크기를 증가시킨다.
    q->size++;
    
    return q->size;
}

/*
 큐에서 데이터를 꺼내는 함수
 
 큐의 Head 노드를 꺼내고 다음 노드로 이동한다.
 데이터는 전달받은 반환버퍼에 복사한다.
 
 반환값:
 - 데이터를 꺼낸 후, 큐의 크기
 - 에러코드 -1: 할당되지 않은 큐나 반환버퍼를 전달받은 경우
 - 에러코드 -2: 큐가 비어있는 경우
 */
int msq_dequeue(msq_queue *q, char *retbuf)
{
    msq_node *delnode = NULL;
    
    // 큐나 반환버퍼가 할당되어있지 않다면, 에러 코드 -1을 반환한다.
    if(q == NULL || retbuf == NULL)
        return -1;
    // 큐의 크기가 0이라면, 반환버퍼를 빈 문자열로 만들고 에러 코드 -2를 반환한다.
    if(q->size == 0) {
        retbuf[0] = '\0';
        return -2;
    }
    
    // 삭제 노드를 큐의 Head 노드로 설정한다.
    delnode = q->head;
    
    // 삭제 노드의 데이터를 반환버퍼에 복사한다.
    // (단, 반환버퍼의 크기는 고려하지 않는다.)
    // (반환버퍼는 메모리에 할당되어있는 배열이어야 한다.)
    strncpy(retbuf, delnode->data, strlen(delnode->data));
    retbuf[strlen(delnode->data)] = '\0';
    
    // 큐에 한 개의 노드만 있다면, 큐의 Head 노드와 Tail 노드를 NULL로 초기화한다.
    if(q->size == 1) {
        q->head = q->tail = NULL;
    }
    // 큐에 여러 개의 노드가 있다면, 큐의 Head 노드를 다음 노드로 설정한다.
    else {
        q->head = q->head->next;
    }
    
    // 메모리에 할당한 삭제 노드의 데이터(문자열)와 삭제 노드를 해제한다.
    free(delnode->data);
    free(delnode);
    
    // 큐의 크기를 감소시킨다.
    q->size--;
    
    return q->size;
}

/*
 큐를 제거하는 함수
 
 큐의 모든 노드와 큐를 메모리에서 해제한다.
 
 반환값:
 - 1: 정상적으로 큐를 제거한 경우
 - 에러코드 -1: 할당되지 않은 큐를 전달받은 경우
 */
int msq_destroy(msq_queue *q)
{
    msq_node *delnode = NULL;
    
    // 큐가 할당되어있지 않다면, 에러 코드 -1을 반환한다.
    if(q == NULL)
        return -1;
    
    // 큐에 노드가 있다면, 큐를 순회하며 모두 제거한다.
    while(q->head != NULL) {
        delnode = q->head;
        q->head = q->head->next;
        
        free(delnode->data);
        free(delnode);
    }
    
    // 메모리에 할당한 큐를 해제한다.
    free(q);
    
    return 1;
}

/*
 큐가 비어있는지 확인하는 함수
 
 큐가 비어있다면 1을, 비어있지 않다면 0을 반환한다.
 
 반환값:
 - 1: 큐가 비어있는 경우
 - 0: 큐가 비어있지 않은 경우
 - 에러코드 -1: 할당되지 않은 큐를 전달받은 경우
 */
int msq_isempty(msq_queue *q)
{
    // 큐가 할당되어있지 않다면, 에러 코드 -1을 반환한다.
    if(q == NULL)
        return -1;
    
    // 큐의 크기를 이용해 판단
    return q->size == 0;
}
