#include "funkcje.h"
#include "main.h"
#include "WatekKom.h"
//#include "queue.h"
#include <pthread.h>

#define MAX(a, b) ((a)>(b) ? (a) : (b))



pthread_t threadKom;
pthread_mutex_t stateMut = PTHREAD_MUTEX_INITIALIZER;
pthreadgmutex_t lamportMut = PTHREAD_MUTEX_INITIALIZER;
int END = 1;
MPI_Datatype MPI_PAKIET_T;
int X, Y, Z; //liczba mistrzow
int rank;
int size;
int END;
state_t stan = Rest;
int releaseCount;
int lamportValue; // wartość lamporta
int *clocks; // nazwa jest znacząca, tablica przechowuje indeksy procesów od których otrzymano zgody
packet_t *msg;
int proba;
struct Queue* WaitQueueE;
struct Queue* WaitQueueX;
struct Queue* WaitQueueZ;
struct Queue* WaitQueueY;
int EnergiaZ;
int Pair;
state_t prevstate;
int lamport;
char master_type;
int *priorities;
int send_num = 0;
int curr_pair = -1;
int pair_in = 0;
struct Queue* createQueue()
{
        struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
        q->front = q->rear = NULL;
        return q;
}
void check_thread_support(int provided)
{
    printf("THREAD SUPPORT: %d\n", provided);
    switch (provided) {
        case MPI_THREAD_SINGLE:
            printf("Brak wsparcia dla wątków, kończę\n");
                fprintf(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
                MPI_Finalize();
                exit(-1);
                break;

        case MPI_THREAD_FUNNELED:
            printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
                break;
        case MPI_THREAD_SERIALIZED:
            printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
                break;
        case MPI_THREAD_MULTIPLE: printf("Pełne wsparcie dla wątków\n");
                break;
        default: printf("Nikt nic nie wie\n");
    }
}

void finalizuj()
{
    pthread_mutex_destroy( &stateMut);
    printf("[%d] - Czekam na wątek w celu zakonczenia.\n", rank );
    pthread_join(threadKom,NULL);
    printf("[%d] - [*] KONCZE SWOJ ZYWOT. FAREWELL. [*]\n", rank );
    MPI_Type_free(&MPI_PAKIET_T);
    MPI_Finalize();
}
void makeSimpleInit(int *argc, char ***argv){
    int provided;
    MPI_Init_thread(argc, argv,MPI_THREAD_MULTIPLE, &provided);
    check_thread_support(provided);

    //clocks =  malloc((X+Y+Z+1) * sizeof(int));  
    priorities =  malloc((X+Y+Z+1) * sizeof(int)) ;
    const int nitems=4;
    int blocklengths[4] = {1,1,1, 1};
    MPI_Datatype typy[4] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};

    WaitQueueX = createQueue();
    WaitQueueY = createQueue();
    WaitQueueZ = createQueue();
    WaitQueueE = createQueue();
    MPI_Aint offsets[4];
    offsets[0] = offsetof(packet_t, id);
    offsets[1] = offsetof(packet_t, ts);
    offsets[2] = offsetof(packet_t, typ);
    offsets[3] = offsetof(packet_t, num);



    MPI_Type_create_struct(nitems, blocklengths, offsets, typy, &MPI_PAKIET_T);
    MPI_Type_commit(&MPI_PAKIET_T);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    msg = malloc( sizeof(packet_t) * (X+Y+Z) );
    for(int i=0; i<(X+Y+Z); i++){
        msg[i].id = -1;
        msg[i].ts = -1;
        msg[i].typ = -1;
    }

    pthread_create( &threadKom, NULL, startKomWatek , 0); // Utworzenie wątku komunikacyjnego
if(rank < X){
master_type = 'X';
}
else if(rank<X+Y){
master_type = 'Y';}
else if(master_type < X+Y+Z){
master_type = 'Z';
stan = NotMyTurn;
}
}
void zmienStan(state_t nowy)
{
    pthread_mutex_lock( &stateMut );
    stan = nowy;
    pthread_mutex_unlock( &stateMut );
}

void zmienStan(int new)
{
    pthread_mutex_lock( &stateMut );
    pair_in = new;
    pthread_mutex_unlock( &stateMut );
}


void zmien_pair_in(state_t nowy)
{
    pthread_mutex_lock( &stateMut );
    stan = nowy;
    pthread_mutex_unlock( &stateMut );
}


void zwiekszLamporta(int value)
{
    pthread_mutex_lock( &lamportMut );
    if(value == -1)
        lamportValue ++;
    else
        lamportValue = value;
    pthread_mutex_unlock( &lamportMut );
}
int check_active(struct Queue* q, int key){
struct QNode* tmp = q->front;
while (tmp != NULL){
if(tmp->key == rank){return -1;}
else if(tmp->key == key && tmp->active == 1){ return 1;}
tmp = tmp->next;
}
return -1;
}

int check_if_in_queue(struct Queue* q, int key){
struct QNode* tmp = q->front;
while (tmp != NULL){
if(tmp->key == rank){return -1;}
else if(tmp->key == key){ return 1;}
tmp = tmp->next;
}
return -1;

}

int check_place_in_queue_x(){
for(int i = 0; i < X; i++){
int active  = check_active(WaitQueueX, i);
//printf("i- %d, act- %d\n", i, active);
if(i != rank && active == -1){
if((clocks[i]< get_queue_ts(WaitQueueX, rank)) || (clocks[i]== get_queue_ts(WaitQueueX, rank) && i< rank )){return -1;}
}
}

int num = get_queue_num(WaitQueueX, rank);

return num;}

int check_place_in_queue_y(){
for(int i = X; i < X+Y; i++){
int active  = check_active(WaitQueueY, i);
//printf("i- %d, act- %d\n", i, active);
if(i != rank && active == -1){
if((clocks[i]< get_queue_ts(WaitQueueY, rank)) || (clocks[i]== get_queue_ts(WaitQueueY, rank) && i< rank )){return -1;}
}
}

int num = get_queue_num(WaitQueueY, rank);

return num;}
int can_get_to_crit_section(){

        for(int i = 0; i < X; i++){
                int active  = check_if_in_queue(WaitQueueE, i);
//printf("i- %d, act- %d\n", i, active);
        if(i != rank && active == -1){
        if((clocks[i]< get_queue_ts(WaitQueueE, rank)) || (clocks[i]== get_queue_ts(WaitQueueE, rank) && i< rank )){return -1;}
        }
        }

int num = get_queue_num(WaitQueueE, rank);

return num;
}


int check_if_pair_exists(int num){

        if (master_type == 'X'){

        for(int i = X; i< X+Y; i++){
        if(priorities[i] == num){return i;}
        }

        }
        if (master_type == 'Y'){

        for(int i = 0; i< X; i++){
        if(priorities[i] == num){return i;}
        }

        }

return -1;
}
int main(int argc,char **argv)
{
    MPI_Status status;


    //Na wstępie sprawdzam czy poprawnie zainicjowano program, czy podano odpowiednią ilość parametrów
    if(argc != 4){
        printf("Blednie zainicjowano program (Podaj odpowiendia ilosc parametrow) \n");
        return 0;
    }

    X = atoi( argv[1] );
    Y = atoi( argv[2] );
    Z = atoi(argv[3]);

    lamportValue = 0; // ustawienie wartośći zegaru lamporta na 0

    clocks = malloc( sizeof (int) * (X+Y+Z));
    memset(clocks, 0, sizeof(int) * (X+Y+Z));
    priorities = malloc(sizeof (int)*(X+Y+Z));
    memset(priorities, 0, sizeof(int) * (X+Y+Z));

    makeSimpleInit(&argc, &argv); // inicjacja pewnych wartośći

    packet_t *pkt = malloc( sizeof(packet_t)); // Instancja struktury przesyłanej wiadomości

    int proba = 80;

    srand( time(NULL) + rank );

    printf("TYP -[%c] id-[%d] lamp-{%d}- Zaczniemy za 3 sekundy ... \n\n", master_type, rank, lamportValue);
    sleep(3);
    printf("TYP -[%c] id-[%d] lamp-{%d}- START! \n", master_type,  rank, lamportValue);
    if(master_type == 'X'){
    while(1)
    {
        if(stan == Rest){
        int los = rand()%100;
        if(los > proba){
            printf("TYP -[%c] id-[%d] lamp-{%d} - bede ubiegal sie o sekcje\n", master_type, rank, lamportValue);
                zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->id = rank;
                pkt->typ = REQX;
                //pkt->active = 1;

                printf("k\n");
                sendPacketToAll(pkt,MSG_TAG);
                zmienStan(Wait);
            }
        else{printf("TYP- [%c][%d]{%d} - pozostaje w stanie Rest  i nic nie robie\n", master_type, rank, lamportValue);}
        }
        else if(stan == Wait){
        printf("TYP -[%c] id-[%d] lamp-{%d} - ubiegam sie o sekcje\n", master_type, rank, lamportValue);
        int num = check_place_in_queue_x();
        if (num != -1 && send_num == 0){
                priorities[rank] = num;
                zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->id = rank;
                pkt->typ = NUMBER;
                pkt->num = priorities[rank];
                sendPacketToAll(pkt,MSG_TAG);
                send_num = 1;
        }

        curr_pair = check_if_pair_exists(num);
        if (curr_pair != -1){ zmienStan(Paired);
        printf("TYP -[%c] id-[%d] lamp-{%d} - moja para: %d, miejsce w kolejce: %d\n", master_type, rank, lamportValue, curr_pair, num);
        send_num = 0;
        }

    }
                else if(stan == Paired){
        printf("TYP -[%c] id-[%d] lamp-{%d} - jestem w stanie paired\n", master_type, rank, lamportValue);
        if (send_num == 0){
                zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->id = rank;
                pkt->typ = REQE;
                sendPacketToAll(pkt,MSG_TAG);
                send_num = 1;
                printf("TYP -[%c] id-[%d] lamp-{%d} - jestem w stanie paired, wysylam REQE\n", master_type, rank, lamportValue);

        }
        int num = can_get_to_crit_section();
        printf("TYP -[%c] id-[%d] lamp-{%d} - jestem w stanie paired, miejsce w kolejce E: %d\n", master_type, rank, lamportValue, num);
        if(num < Z && num != -1){
                printf("TYP -[%c] id-[%d] lamp-{%d} - jestem w stanie paired, bede wchodzil czerpac moc %d\n", master_type, rank, lamportValue);

                zmienStan(Taking);
                zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->id = rank;
                pkt->typ = IN;
                sendPacketl(pkt, curr_pair,MSG_TAG);

        }
        else if(num == Z){
                printf("TYP -[%c] id-[%d] lamp-{%d} - jestem w stanie paired, brak jednostek energii %d\n", master_type, rank, lamportValue);

        }
        else{
                        printf("TYP -[%c] id-[%d] lamp-{%d} - jestem w stanie paired, brak jednostek energii %d\n", master_type, rank, lamportValue);

        }

        }
        else if(stan == Taking){

        }


        sleep(1); // zamrożenie stanu

}}
    if(master_type == 'Y'){
    while(1)
    {
        if(stan == Rest){
        int los = rand()%100;
        if(los > proba){
            printf("TYP -[%c] id-[%d] lamp-{%d} - bede ubiegal sie o sekcje\n", master_type, rank, lamportValue);
                zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->id = rank;
                pkt->typ = REQY;
                //pkt->active = 1;

                printf("k\n");
                sendPacketToAll(pkt,MSG_TAG);
                zmienStan(Wait);
            }
        else{printf("TYP- [%c][%d]{%d} - pozostaje w stanie Rest  i nic nie robie\n", master_type, rank, lamportValue);}
        }
        else if(stan == Wait){
        printf("TYP -[%c] id-[%d] lamp-{%d} - ubiegam sie o sekcje\n", master_type, rank, lamportValue);
        int num = check_place_in_queue_y();
        if (num != -1 && send_num == 0){
                        priorities[rank] = num;
                zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->id = rank;
                pkt->typ = NUMBER;
                pkt->num = priorities[rank];
                sendPacketToAll(pkt,MSG_TAG);
                send_num = 1;
        }
        curr_pair = check_if_pair_exists(num);
        if (curr_pair != -1){ zmienStan(Paired);
        printf("TYP -[%c] id-[%d] lamp-{%d} - moja para: %d, miejsce w kolejce: %d\n", master_type, rank, lamportValue, curr_pair, num);
        send_num = 0;
        }

    }
        else if(stan == Paired){
        printf("TYP -[%c] id-[%d] lamp-{%d} - jestem w stanie paired\n", master_type, rank, lamportValue);
        }
        sleep(1); // zamrożenie stanu

}}

    zmienStan(Rest);
    END = 0;
    //free(args);
    finalizuj();
    free(clocks);
    free(msg);
    return 0;

}

