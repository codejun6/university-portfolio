#ifndef _MSQ_H_
#define _MSQ_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 msq_node
 
 문자열 노드 구조체
 
 문자열을 보관하는 노드
 문자열은 동적할당하여 사용해야 한다.
 
 멤버:
 - data: 문자열 데이터
 - next: 다음 노드 포인터
 */
typedef struct _msq_node {
    char *data;
    struct _msq_node *next;
} msq_node;

/*
 msq_queue
 
 문자열 큐 구조체
 
 문자열 노드로 이루어진 큐
 
 멤버:
 - head: Head 노드 포인터
 - tail: Tail 노드 포인터
 - size: 큐의 크기
 */
typedef struct _msq_queue {
    msq_node *head;
    msq_node *tail;
    int size;
} msq_queue;

/* 큐를 생성하는 함수 */
msq_queue *msq_create();

/* 큐에 데이터를 넣는 함수 */
int msq_enqueue(msq_queue *q, char *data);

/* 큐에서 데이터를 꺼내는 함수 */
int msq_dequeue(msq_queue *q, char *retbuf);

/* 큐를 제거하는 함수 */
int msq_destroy(msq_queue *q);

/* 큐가 비어있는지 확인하는 함수 */
int msq_isempty(msq_queue *q);

#endif
