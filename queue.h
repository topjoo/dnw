// ------------------------------------------------------------
// Queue.h
//------------------------------------------------------------
// Revision History
// 1) 2015.11.16 : Circular Queue by top.joo
// 2) 2015.11.19 : Linked List Queue by top.joo
//
//
//
//
//
//------------------------------------------------------------
//
//


#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "resource.h"

#include <windows.h>

#include <string.h>
#include <stdlib.h>
#include <tchar.h>
#include <stdio.h> //for _vsntprintf()
#include <malloc.h>




#define FALSE 	0
#define TRUE 	1

#define USE_CIRCULAR_QUEUE 				1 // 2015.11.14
#define USE_LINKED_LIST_QUEUE 			0 // 2015.11.14
#define SELF_TEST 						0 // 2015.11.14

#if USE_CIRCULAR_QUEUE

#define MAX_QUEUE_DATA_BUF_SIZE 		(STRING_LEN+4096) // Queue Buffer size
#define MAX_QUEUE_SIZE 					(50) // Circular Queue Size 
// ----------------------------------------------------
// Circular Queue structure
// ----------------------------------------------------
// typedef unsigned char BYTE;
typedef struct {
	TCHAR *CQdata[MAX_QUEUE_SIZE]; // [MAX_QUEUE_DATA_BUF_SIZE];
	int qsiz; // -- Queue size
	int head; // -- Dequeue-
	int tail; // -- Enqueue-
} ctQueue;


#ifdef __cplusplus
extern "C" {
#endif

extern void InitCQueue(ctQueue *pArr, int Size);
extern void UnCQueue(ctQueue *pArr, int Size);
extern int FullCQueue(ctQueue *pArr);
extern int EnCQueue(ctQueue *pArr, TCHAR *indata, int length);
extern int EmptyCQueue(ctQueue *pArr);
extern TCHAR* DeCQueue(ctQueue *pArr);
extern TCHAR* PeekCQueue(ctQueue *pArr);

#ifdef __cplusplus
}
#endif

#endif // USE_CIRCULAR_QUEUE





#if USE_LINKED_LIST_QUEUE 

//typedef unsigned char 	BYTE;

typedef struct tagNode {
	BYTE 	Qdata[MAX_QUEUE_DATA_BUF_SIZE];
	int 	Qsize;
	struct 	tagNode* nextNode;
} tllNODE;


typedef struct tagQueue {
	tllNODE 		*head; // DeQueue
	tllNODE 		*tail;   // EnQueue
	unsigned int 	Qcount;
} tllQUEUE;

#endif // USE_LINKED_LIST_QUEUE 






