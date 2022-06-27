// ------------------------------------------------------------
// Queue.c
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
#include "engine.h"
#include "queue.h"
//#include "dnw.h"


#define USE_CIRCULAR_QUEUE 			1

#if (USE_CIRCULAR_QUEUE)

//------------------------------
// Queue 
//------------------------------
ctQueue dcu2meu;



// --------------------------------------------------------
// Intialized Circular Queue
// --------------------------------------------------------
void InitCQueue(ctQueue *pArr, int Size)
{
	int idx = 0;

	if( NULL==pArr )
	{
		EB_Printf("[dnw] InitCQueue is NULL +++ Size(%d)  \r\n", Size);
		return;
	}

	pArr->qsiz = Size; 
	pArr->head = pArr->tail = 0;

	for(idx=0; idx<Size; idx++)
	{
		pArr->CQdata[idx] = (TCHAR*)malloc( MAX_QUEUE_DATA_BUF_SIZE*sizeof(TCHAR) );
		memset( pArr->CQdata[idx], 0x00, sizeof( MAX_QUEUE_DATA_BUF_SIZE*sizeof(TCHAR) ) ); 
	}
}


// --------------------------------------------------------
// Destoty Circular Queue
// --------------------------------------------------------
void UnCQueue(ctQueue *pArr, int Size)
{
	int idx = 0;

	if( NULL==pArr )
	{
		//printf("UnCQueue is NULL +++ Size(%d), pArr->qsiz(%d) \r\n", Size, pArr->qsiz );
		return;
	}

	pArr->head = pArr->tail = 0;


	for(idx=0; idx<Size; idx++)
		free(pArr->CQdata[idx]);
}


// --------------------------------------------------------
// Check full Circular Queue
// --------------------------------------------------------
int FullCQueue(ctQueue *pArr)
{
	return ( (pArr->tail+1)%pArr->qsiz == pArr->head );
}


// --------------------------------------------------------
// Circular EnQueue
// --------------------------------------------------------
int EnCQueue(ctQueue *pArr, TCHAR *indata, int length)
{

	int iLength = 0;

	if( NULL==pArr || NULL==indata || FullCQueue(pArr) )
	{
		EB_Printf("[dnw] EnQueue Err 1 \r\n");
		return -1;
	}

	if( length > MAX_QUEUE_DATA_BUF_SIZE )
	{
		EB_Printf("[dnw] EnQueue Err 3+++length(%d) \r\n", length);
		iLength = length;
	}
	else
	{

		pArr->tail = (pArr->tail+1) % pArr->qsiz;

		//pArr->CQdata[pArr->tail] = indata;
		memset( (void*)pArr->CQdata[pArr->tail], 0x00, MAX_QUEUE_DATA_BUF_SIZE );

		//memcpy( (void*)pArr->CQdata[pArr->tail], (void*)indata, length);
		lstrcpy( pArr->CQdata[pArr->tail], indata );
		iLength = length;

	}
	// Inserted Count
	// return pArr->CQdata[pArr->tail];
	return iLength; // EnQueued string length

}


// --------------------------------------------------------
// Check empty Circular Queue
// --------------------------------------------------------
int EmptyCQueue(ctQueue *pArr)
{
	if( NULL==pArr )
	{
//		printf("EmptyCQueue pArr NULL +++ \r\n");
		return -1;
	}
	
	// empty Queue when head is same to tail
	return (pArr->head == pArr->tail);
}


// --------------------------------------------------------
// Circular DeQueue
// --------------------------------------------------------
TCHAR* DeCQueue(ctQueue *pArr)
{
	//TCHAR *outData = NULL;

	if( NULL==pArr || EmptyCQueue(pArr) )
	{
//		printf("DeCQueue Empty Queue +++ pArr->qsiz (%d) \r\n", pArr->qsiz );
		return NULL;
	}

	pArr->head = (pArr->head+1) % pArr->qsiz;

#if 1
	return pArr->CQdata[pArr->head];
#else
	outData = pArr->CQdata[pArr->head]; 
	return outData;
#endif
}


// --------------------------------------------------------
// Circular Peek Queue
// --------------------------------------------------------
TCHAR* PeekCQueue(ctQueue *pArr)
{
	if( NULL==pArr || EmptyCQueue(pArr) )
	{
//		printf("PeekCQueue Empty Queue +++ \r\n");
		return NULL;
	}

	return pArr->CQdata[ (pArr->head+1)%pArr->qsiz ];
}
#endif // USE_CIRCULAR_QUEUE




#if USE_LINKED_LIST_QUEUE 

// Create Linked List Queue
void CreateQueue(tllQUEUE** Queue)
{
	(*Queue) = (tllQUEUE *)malloc ( sizeof(tllQUEUE) );

	(*Queue)->head = NULL;
	(*Queue)->tail = NULL;
	(*Queue)->Qcount = 0;
}



// Create Queue Node
tllNODE* CreateNode( BYTE inData[] )
{
	tllNODE* newNode = (tllNODE *)malloc( sizeof(tllNODE) );

	newNode->Qsize = strlen( (char *)inData );
	strcpy( (char *)(newNode->Qdata), (char *)inData );

	newNode->nextNode = NULL;

	return newNode;
}



int isEmptyQueue(tllQUEUE* Queue)
{
	// If head is NULL, empty
	return( NULL==Queue->head );
}


int isFullQueue(tllQUEUE* Queue)
{
	if( NULL==Queue )
		return -1;

	return 1;
}


void EnQueue( tllQUEUE *Queue, BYTE inData[] )
{
	
	//if( NULL==Queue->head )
	if( isEmptyQueue(Queue) )
	{
		// if empty, create new queue. 
		Queue->head = CreateNode(inData);
		Queue->tail = Queue->head;
	}
	else
	{
		// if no empty, tail index increased. 
		Queue->tail->nextNode = CreateNode(inData);
		Queue->tail = Queue->tail->nextNode;
	}
	Queue->Qcount++;

}



tllNODE* DeQueue(tllQUEUE* Queue)
{
	tllNODE* outNode = Queue->head;

	if( NULL==outNode || NULL==Queue )
		return NULL;

	
	if( NULL==Queue->head->nextNode )
	{
		Queue->head = NULL;
		Queue->tail = NULL;
	}
	else
	{
		Queue->head = Queue->head->nextNode;
	}

	Queue->Qcount--;

	// head index
	return outNode;
}


BYTE *PeekQueue(tllQUEUE* Queue)
{
	if( NULL==Queue )
	{
		printf("PeekQueue: Queue is NULL +++++ \n");
		return NULL;
	}
	
	if( isEmptyQueue(Queue) )
	{
		printf("PeekQueue: empry queue ++++++++ \n");
		return NULL;
	}
	else
	{
		return Queue->head->Qdata;
	}
}

void printQueue(tllNODE* outNode)
{
	printf("Linked DeQ data: [%s] size(%d) \n", outNode->Qdata, outNode->Qsize );
	free(outNode);
}

#endif // USE_LINKED_LIST_QUEUE 




#if SELF_TEST

//------------------------------
// Queue 
//------------------------------
ctQueue dcu2meu;


int main()
{
	BYTE buf[MAX_QUEUE_DATA_BUF_SIZE]={0,};
	int len;
	int i=0;
	

	tllQUEUE* Queue=NULL;
	tllNODE*  outNode=malloc( sizeof(tllNODE) );

	//LLQueueType llqq;
	int initem;


	// -----------------------------
	// --- Circular Queue Test
	// -----------------------------

	memset( buf, 0x00, sizeof(buf) );

	// intialized Circular Queue for dcu2meu
	InitCQueue(&dcu2meu, MAX_QUEUE_SIZE );

	printf("sizeof : %d, %d  \n", sizeof(dcu2meu.CQdata), sizeof(dcu2meu));
	while(1)
	{
		printf("------------------------------------------------------\n");
		printf("Circle Queue:: [1:enQ], [2:DeQ], [3:peek], [4:UnQ] >> ");
		scanf("%d",&i);
		switch(i)
		{
		case 1:
			// EnQueue 하기 전에 Queue full 확인 ---
			if( FullCQueue(&dcu2meu) )
			{
				printf("EnQ full +++++++++ \n");
			}
			else
			{
				printf("EnQ data: ");
				scanf("%s", buf);
				len = strlen(buf);
				printf("EnQ length: [%d]\n", EnCQueue(&dcu2meu, buf, len ));
				printf("EnQ head = %d, tail = %d\n",dcu2meu.head, dcu2meu.tail);
			}
			break;
		case 2:
			// DeQueue 하기 전에 Queue empty 확인 ---
			if( EmptyCQueue(&dcu2meu) )
			{
				printf("DeQ Empty Queue +++++++++ \n");
			}
			else
			{
				printf("DeQ data: [%s] \n",DeCQueue(&dcu2meu));
				printf("DeQ head = %d, tail = %d\n",dcu2meu.head, dcu2meu.tail);
			}
			break;

		case 3:
			printf("peek data: [%s]\n",PeekCQueue(&dcu2meu));
			printf("peek head = %d, tail = %d\n",dcu2meu.head, dcu2meu.tail);
			break;

		case 4:
			// Destory --
			UnCQueue(&dcu2meu, MAX_QUEUE_SIZE);
			printf("Destory Circle Queue \n" );
			// return 0;
			break;
		}
		if( i==4 ) break;
	}



	// -----------------------------
	// --- Linked List Queue Test
	// -----------------------------

	memset( buf, 0x00, sizeof(buf) );


	// ------------------------------------
	// -- Iniialized Linked-List Queue
	// ------------------------------------
	CreateQueue(&Queue);

	//printf("Inital LLQueue >> head : %d, tail %d\n\n",llqq.head, llqq.tail);
	while(1)
	{
		printf("------------------------------------------------------\n");
		printf("Linked Queue:: [1:enQ], [2:DeQ], [3:peek], [4:UnQ] >> ");
		scanf("%d",&i);
		switch(i)
		{
		case 1:
			printf("Linked EnQ data: ");
			scanf("%s", buf);
			len = strlen(buf);
			EnQueue(Queue, buf );
			printf("stored data: [%s] length: [%d], Qcount(%d) \n", buf, len, Queue->Qcount );
			break;
		
		case 2:
			if( isEmptyQueue(Queue) )
			{
				printf("Linked DeQ data: empty (Qcount:%d) +++++ \n", Queue->Qcount);
			}
			else
			{
				outNode = DeQueue(Queue);
				printf("Linked DeQ data: [%s] size(%d), Qcount(%d) \n", outNode->Qdata, outNode->Qsize, Queue->Qcount );
				if(outNode) free(outNode);
			}
			break;

		case 3:
			printf("Linked peek data: [%s] Qcount(%d) \n", PeekQueue(Queue), Queue->Qcount );
			break;

		case 4:
			printf("Destory Linked List Queue \n" );
			break;

		default:
			printf("wrong+++ \n");
			break;
		}

		if( i==4 ) break;
	}


	return 0;
}
#endif




