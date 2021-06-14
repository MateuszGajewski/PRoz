#include "funkcje.h"
#include "main.h"
#include "WatekKom.h"
//#include "queue.h"
#include <pthread.h>

#define MAX(a, b) ((a)>(b) ? (a) : (b))



pthread_t threadKom;
pthread_mutex_t stateMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lamportMut = PTHREAD_MUTEX_INITIALIZER;
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
int *place;
packet_t *msg;
int proba;
struct Queue* WaitQueueE;
struct Queue* WaitQueueX;
struct Queue* WaitQueueZ;
struct Queue* WaitQueueY;
int EnergiaZ = 0;
int Pair;
state_t prevstate, prevprevstate;
int lamport;
char master_type;
int *priorities;
int send_num = 0;
int curr_pair = -1;
int pair_in = 0;
int cleared = 0;
int cleared1 = 0;
int send_stop = 0;
int ans_count = 0;
int deleter =-1;
int how_many_del = 0;
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
    int blocklengths[4] = {1,1,1,1};
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



void zmien_pair_in(int nowy)
{
    pthread_mutex_lock( &stateMut );
    pair_in = nowy;
    pthread_mutex_unlock( &stateMut );
}

void zmien_place(int id, int plac)
{
    pthread_mutex_lock( &stateMut );
    place[id] = plac;
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
if(tmp->key == rank && tmp->active == 1){return -1;}
else if(tmp->key == key && tmp->active == 1){ return 1;}
tmp = tmp->next;
}
return -1;
}

int check_if_in_queue(struct Queue* q, int key){
struct QNode* tmp = q->front;
while (tmp != NULL){
if(tmp->key == rank && tmp->active == 1){return -1;}
else if(tmp->key == key && tmp->active == 1){ return 1;}
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
		int active  = check_active(WaitQueueE, i);
printf("rank- %d, i- %d, act- %d\n",rank, i, active);
	if(i != rank && active == -1){
	if((clocks[i]< get_queue_ts(WaitQueueE, rank)) || (clocks[i]== get_queue_ts(WaitQueueE, rank) && i< rank )){return -1;}
	}
	}

int num = get_queue_num(WaitQueueE, rank);
printf("rank- %d, numm-  %d\n",rank, num);

return num;
}

int what_to_del(){

struct QNode* tmp = WaitQueueX->front;
int num = 0;
while (tmp != NULL){
if(tmp->active == 1){num++;}
else if(tmp->active == 1){ return num;}
tmp = tmp->next;
}
return num;


}

int check_if_pair_exists(int num){

	if (master_type == 'X'){
	
	for(int i = X; i< X+Y; i++){
	if(place[i] == num){return i;}
	}
	
	}
        if (master_type == 'Y'){

        for(int i = 0; i< X; i++){
        if(place[i] == num){return i;}
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
    place = malloc( sizeof (int) * (X+Y+Z));
    memset(place, -1, sizeof(int) * (X+Y+Z));
    //for (int i = 0; i< X+Y+Z; i++){priorities[i]=-1;} 

    makeSimpleInit(&argc, &argv); // inicjacja pewnych wartośći

    packet_t *pkt = malloc( sizeof(packet_t)); // Instancja struktury przesyłanej wiadomości

    int proba = 80; 
    int proba1 = 80;
    srand(rank );

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

	if(num !=-1){ curr_pair = check_if_pair_exists(num);}
	if (curr_pair != -1){ zmienStan(Paired);
	printf("TYP -[%c] id-[%d] lamp-{%d} - moja para: %d, miejsce w kolejce: %d\n", master_type, rank, lamportValue, curr_pair, num);
	send_num = 0;
	                zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->id = rank;
                pkt->typ = RELX;
                sendPacketToAll(pkt,MSG_TAG);

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
                sendPacket(pkt, curr_pair,MSG_TAG);

	}
	else if(num == Z){
	        printf("TYP -[%c] id-[%d] lampi-{%d} - jestem w stanie paired, brak jednostek energii\n", master_type, rank, lamportValue);
		if(releaseCount == Z){
                zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->id = rank;
                pkt->typ = OUT;
                sendPacketToAll(pkt, MSG_TAG);
		prevstate = Paired;
		zmienStan(NotMyTurn);
		}
	}
	else{
	                printf("TYP -[%c] id-[%d] lamp-{%d} - jestem w stanie paired, brak jednostek energii \n", master_type, rank, lamportValue);

	}
	
        }
	else if(stan == Taking){
	if(pair_in == 1){
		printf("TYP -[%c] id-[%d] lamp-{%d} - jestem w stanie taking, moja para tez, czerpie moc\n", master_type, rank, lamportValue);
		int los = rand()%100;
		if(los>proba){
			printf("TYP -[%c] id-[%d] lamp-{%d} - jestem w stanie taking, moja para tez, koncze czerpanie mocy \n", master_type, rank, lamportValue);
			zwiekszLamporta(-1);
                	pkt->ts = lamportValue;
                	pkt->id = rank;
                	pkt->typ = IN;
                	sendPacket(pkt, curr_pair,MSG_TAG);
			priorities[rank] = -1;
			clocks[curr_pair] = -1;
			
			curr_pair = -1;
			pair_in = 0;
		zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->id = rank;
                pkt->typ = RELE;
                sendPacketToAll(pkt,MSG_TAG);
		send_num = 0;		
		if(place[rank] == -3){prevprevstate = Rest,  zmienStan(Updaiting_queue);}		
		zmienStan(Rest);
		}

	}
	

	}
	else if(stan == NotMyTurn){
	if(cleared == 0){
	for(int i = 0; i < Z; i++){
	deQueue(WaitQueueE);
	cleared = 1;
	}
	
	}
            printf("TYP -[%c] id-[%d] lamp-{%d} - NotMyTurn\n", master_type, rank, lamportValue);
		if(EnergiaZ==Z){
		
		zmienStan(prevstate);
		EnergiaZ = 0;
		cleared = 0;
		releaseCount -=Z;
		}
	}
	else if(stan == Updated_queue){
	
                printf("TYP -[%c] id-[%d] lamp-{%d} - jestem w stanie Udated_queue, usuwam z kolejki, deleter = %d\n", master_type, rank, lamportValue, how_many_del);
		                if(cleared1 == 0){
                 for(int i = 0; i < how_many_del; i++){
                        deQueue(WaitQueueX);
                        }
                pkt->id = deleter;
                zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->typ = STOP;
                sendPacket( pkt, deleter, MSG_TAG );
                cleared = 1;
		send_num = 0;
		for (int i = 0 ; i<X+Y+Z; i++){place[i]=-1;}
                }

	
	}
	else if(stan == Updaiting_queue){
	                printf("TYP -[%c] id-[%d] lamp-{%d} - jestem w stanie Udating_queue, usuwam z kolejki\n", master_type, rank, lamportValue);

		if(send_stop == 0){
			int num = what_to_del();
			zwiekszLamporta(-1);
                        pkt->ts = lamportValue;
                        pkt->id = rank;
                        pkt->typ = STOP;
			pkt->num = num;
                        sendPacketToAll(pkt,MSG_TAG);
			send_stop = 1;
			for(int i = 0; i < num; i++){
			deQueue(WaitQueueX);
			}
			for (int i = 0 ; i<X+Y+Z; i++){place[i]=-1;}
	}
	if(ans_count == X+Y){send_stop = 0;
	zwiekszLamporta(-1);
                        pkt->ts = lamportValue;
                        pkt->id = rank;
                        pkt->typ = START;
                        sendPacketToAll(pkt,MSG_TAG);
			ans_count = 0;
			zmienStan(prevprevstate);
	
	}

	
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
        
	if(num != -1){curr_pair = check_if_pair_exists(num);}
	if (curr_pair != -1){ zmienStan(Paired);
        printf("TYP -[%c] id-[%d] lamp-{%d} - moja para: %d, miejsce w kolejce: %d\n", master_type, rank, lamportValue, curr_pair, num);
        send_num = 0;
	        zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->id = rank;
                pkt->typ = RELY;
                sendPacketToAll(pkt,MSG_TAG);

	}

    }
	else if(stan == Paired){
	                printf("TYP -[%c] id-[%d] lamp-{%d} - jestem w stanie paired, czekam na IN\n", master_type, rank, lamportValue);

	
	}
	else if(stan == Taking){

	if(send_num == 0){
		        zwiekszLamporta(-1);
                        pkt->ts = lamportValue;
                        pkt->id = rank;
                        pkt->typ = IN;
                        sendPacket(pkt, curr_pair,MSG_TAG);
                        send_num == 1;

	}


	printf("TYP -[%c] id-[%d] lamp-{%d} - jestem w stanie taking, czerpie energie\n", master_type, rank, lamportValue);
	if(pair_in == 1){

	int los = rand()%100;
                if(los>proba){
		printf("TYP -[%c] id-[%d] lamp-{%d} - jestem w stanie paired, chce zakonczyc\n", master_type, rank, lamportValue);
		        
                        zmienStan(Rest);
			priorities[rank] = -1;
			curr_pair  =-1;


		}

	}}
	else if(stan == NotMyTurn){
	if(cleared == 0){
        for(int i = 0; i < Z; i++){
        deQueue(WaitQueueE);
        }
	cleared = 1;
	}
            printf("TYP -[%c] id-[%d] lamp-{%d} - NotMyTurn\n", master_type, rank, lamportValue);
                if(EnergiaZ==Z){

                zmienStan(prevstate);
                EnergiaZ = 0;
		cleared = 0;
                }
        
	}
	        else if(stan == Updated_queue){
                printf("TYP -[%c] id-[%d] lamp-{%d} - jestem w stanie Udated_queue, usuwam z kolejki\n", master_type, rank, lamportValue);
		if(cleared1 == 0){
		 for(int i = 0; i < how_many_del; i++){
                        deQueue(WaitQueueY);
                        }
                pkt->id = deleter;
                zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->typ = STOP;
                sendPacket( pkt, deleter, MSG_TAG );
		cleared1 = 1;
		send_num =0;
		for (int i = 0 ; i<X+Y+Z; i++){place[i]=-1;}
		}

        }
	
        sleep(1); // zamrożenie stanu
    
}}
    if(master_type == 'Z'){
    while(1){
	if(stan == NotMyTurn){
	
	         printf("TYP -[%c] id-[%d] lamp-{%d} - NotMyTurn, czekam\n", master_type, rank, lamportValue);

}
        if(stan == Wait){
        int los = rand()%100;
                if(los>proba){zmienStan(Insection);
		printf("TYP -[%c] id-[%d] lamp-{%d} - Wchodze do sekcji\n", master_type, rank, lamportValue);

		}
		else{
		                 printf("TYP -[%c] id-[%d] lamp-{%d} - Nic nie robie\n", master_type, rank, lamportValue);

}}
        if(stan == Insection){
		                 printf("TYP -[%c] id-[%d] lamp-{%d} - Uzupelniam energie\n", master_type, rank, lamportValue);

				        int los = rand()%100;
                if(los>proba){zmienStan(NotMyTurn);
                printf("TYP -[%c] id-[%d] lamp-{%d} - Skonczylem uzupelniac\n", master_type, rank, lamportValue);
		                zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->id = rank;
                pkt->typ = RELZ;
                sendPacketToAll(pkt,MSG_TAG);

                }


}



	    sleep(1); // zamrożenie stanu

    }
    }


    zmienStan(Rest);
    END = 0;
    //free(args);
    finalizuj();
    free(clocks);
    free(msg);
    return 0;
    
}
