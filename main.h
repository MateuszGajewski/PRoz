#ifndef GLOBALH
#define GLOBALH


#define ROOT 0
#define INFORMACJE_O_EKIPIE 1
#define MSG_TAG 2

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

struct QNode {
        int key;
        int active;
        int ts;
        struct QNode* next;
};

// The queue, front stores the front node of LL and rear stores the
// last node of LL
extern struct Queue {
        struct QNode *front, *rear;
};
extern int send_num;
extern int X, Y, Z; //liczba mistrzow
extern int rank;
extern int size;
extern int END;
extern char master_type;
typedef enum {Rest, Wait, Insection, NotMyTurn, Paired, Taking, Updaiting_queue, Updated_queue} state_t;
extern state_t stan;
typedef struct {
    int id;         // id procesu
    int ts;       // timestamp (zegar lamporta)  
    int typ;      // REQX-1, REQY- 2, ACKX- 3, ACKY-4, RELX- 5, RELY- 6, IN- 7, OUT- 8, REQE- 9, ACKE- 10, RELE- 11, Reset_pair- 12, RELZ- 13, NUMBER- 14, Stop- 15, start- 16, inactive- 17
    int num;
    } packet_t;
extern MPI_Datatype MPI_PAKIET_T;

void zwiekszLamporta(int value);
void zmienStan(state_t);

extern int releaseCount;
    extern int lamportValue; // wartość lamporta
    extern int *clocks; // nazwa jest znacząca, tablica przechowuje indeksy procesów od których otrzymano zgody
    extern packet_t *msg;
    extern int proba;
    extern struct  Queue* WaitQueueX;
extern struct Queue* WaitQueueZ;
extern struct Queue* WaitQueueE;
extern struct Queue* WaitQueueY;
extern int EnergiaZ;
extern int Pair;
extern state_t prevstate;
extern state_t prevprevstate;
extern int lamportValue; // wartość lamporta
extern int *clocks; 
extern int *place;
extern packet_t *msg;
extern int proba;
extern int *priorities;
extern int curr_pair;
extern int count_rel_e;
extern int pair_in;
extern int ans_count;
extern int deleter;
extern int how_many_del;
extern int cleared1;


    #define REQX 1
    #define REQY 2
    #define ACKX 3
    #define ACKY 4
    #define RELX 5
    #define RELY 6
    #define IN 7
    #define OUT 8
    #define REQE 9
    #define ACKE 10
    #define RELE 11
    #define RESET_PAIR 12
    #define RELZ 13
    #define NUMBER 14
    #define STOP 15
    #define START 16
    #define INACTIVE 17


#endif
