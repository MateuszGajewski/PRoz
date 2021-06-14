#include "main.h"
#include "WatekKom.h"
#include "funkcje.h"
//#include "queue.h"


struct QNode* newNode(int k, int a, int ts)
{
        struct QNode* temp = (struct QNode*)malloc(sizeof(struct QNode));
        temp->key = k;
        temp->next = NULL;
	temp->active = a;
	temp->ts = ts;
        return temp;
}



void deQueue(struct Queue* q)
{
        // If queue is empty, return NULL.
        if (q->front == NULL)
                return;

        // Store previous front and move front one node ahead
        struct QNode* temp = q->front;

        q->front = q->front->next;

        // If front becomes NULL, then change rear also as NULL
        if (q->front == NULL)
                q->rear = NULL;

        free(temp);
}

void delFromQueue(struct Queue* q, int element)
{
struct QNode* tmp = q->front;
struct QNode* prev = NULL;
while (tmp != NULL){
	if(tmp->key == element){
		if(prev == NULL){deQueue(q); return;}
		else if (tmp->next == NULL){
		q->rear = prev;
		prev->next = NULL;
		free(tmp);
		return;
		}
		else{
		prev->next = tmp->next;
		free(tmp);
		return;
		}
	

	
	}
	prev = tmp;
	tmp = tmp->next;
	}
}
void insertToq(struct Queue* q, struct QNode* in){
//printf("debug-1 %d\n", rank);

struct QNode* tmp = q->front;
struct QNode* prev = NULL;
//printf("debug0 %d\n", in->key);

if(q->rear == NULL && q->front == NULL){
q->rear = in;
q->front = in;
return;
}
while (tmp != NULL){
	//printf("debug %d\n", in->key);
	if(tmp->ts > in->ts ||(tmp->ts == in->ts && tmp->key > in->key ) ){
		if(prev == NULL){q->front = in;  in->next = tmp; return;}
		else{
		prev->next = in;
		in->next = tmp;
		return;
		}
	

	
	}
	prev = tmp;
	tmp = tmp->next;
	}
tmp = q->rear;
q->rear = in;
tmp->next = in;
in->next = NULL;
return;
}

int get_queue_ts(struct Queue* q, int rank){
int num = 0;
struct QNode* tmp = q->front;
while (tmp != NULL){
if(tmp->key == rank && tmp->active == 1){return tmp->ts;}
tmp = tmp->next;
num+=1;
}
return -1;
}


int get_queue_num(struct Queue* q, int rank){
int num = 0;
struct QNode* tmp = q->front;
while (tmp != NULL){
if(tmp->key == rank && tmp->active == 1){return num;}
tmp = tmp->next;
num+=1;
}
return -1;
}



void set_inactive(struct Queue* q, int rank){
struct QNode* tmp = q->front;
int prev_in =0;
while (tmp != NULL){
if(tmp->key == rank && tmp->active == 1){tmp->active = 0; return;}
tmp = tmp->next;

}
return;
}

int topProces(int id) //Funkcja zwracajaca id procesu bedacego na szczycie lokalnej kolejki zadan
{
 
    return id;
}

int czyWejde(int tunel) // Funkcja ma na celu sprawdzenie czy w lokalnej kolejce zadan nie istnieje proces o wyzszym priorytecie, ktory chce przejsc w dru
{
    	return tunel;
}

void wyczysc(int proces) //Funkcja usuwajaca zadanie z lokalnej kolejki zadan
{
   int i;
}

int sumaOdKtorychNieOtrzymal(){ //Funkcja zwracajaca sume pojemnosci ekip, od ktorych zgody nie otrzymal
    int sum = 0;
    return sum;
}

int zgodyPunkt5(int id) // Przydzielanie zgod na podstawie punktu 5 algorytmu
{
    return 0; // w każdym innym wypadku
}

int zgodyPunkt6(int id) //Przydzielanie zgod na podstawie punktu 6 algorytmu
{
  
    return 0; // w każdym innym wypadku
}

void rozpatrzNaNowo(int what)  // Funkcja, która na nowo rozpatrzy kolejkę po zmiania która nastąpiła o wyjściu z tunelu
{
    int czyWyslacZgode;

}

void *startKomWatek(void *ptr)
{
    packet_t *pkt = malloc( sizeof(packet_t)); // Pomocnicza
    MPI_Status status;
    packet_t pakiet;
    int maxLamportRec = 0;
    struct QNode* tmp;
if (master_type =='X'){
while(END){
	MPI_Recv(&pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MSG_TAG, MPI_COMM_WORLD, &status);
if(pakiet.ts > lamportValue){
                    zwiekszLamporta(pakiet.ts);}	
printf("TYP -[%c] id-[%d] lamp-{%d} - Odbieralem wiadomosc od {%d}, typ %d.\n", master_type, rank, lamportValue, pakiet.id, pakiet.typ);

if(stan == Rest){
switch ( pakiet.typ ) {
	    case REQX:
		    pkt->id = rank;
		 tmp = newNode(pakiet.id, 1, pakiet.ts);
		 insertToq(WaitQueueX, tmp);	
                 zwiekszLamporta(-1);
                 pkt->ts = lamportValue;
		 pkt->typ = ACKX;
		 if(pakiet.id != rank) {sendPacket( pkt, pakiet.id, MSG_TAG);}
		 clocks[pakiet.id] = pakiet.ts;
		 break;
            case REQE:
                 pkt->id = rank;
                 tmp = newNode(pakiet.id, 1, pakiet.ts);
                 insertToq(WaitQueueE, tmp);

                 zwiekszLamporta(-1);
                 pkt->ts = lamportValue;
                 pkt->typ = ACKE;
		 clocks[pakiet.id] = pakiet.ts;
		                 sendPacket( pkt, pakiet.id, MSG_TAG );

		 break;
            case RELX:
                set_inactive(WaitQueueX, pakiet.id);
                clocks[pakiet.id] = pakiet.ts;
		break;
            case RESET_PAIR:
                Pair = -1;
		break;
            case OUT:
		prevstate = stan;
                zmienStan(NotMyTurn);
		break;
           case NUMBER:
		printf("num %d\n", pakiet.num);
                 place[pakiet.id] = pakiet.num;
		 break;
           case RELE:
		set_inactive(WaitQueueE, pakiet.id);
                releaseCount++;
		clocks[pakiet.id] = pakiet.ts;

                break;
	   case ACKE:
                clocks[pakiet.id] = pakiet.ts;
                break;
        case STOP:
                prevprevstate = stan;
                zmienStan(Updated_queue);
		deleter = pakiet.id;
                how_many_del = pakiet.num;

                break;
}
}
else if(stan == Wait){
switch ( pakiet.typ ) {

	case REQX:
		tmp = newNode(pakiet.id, 1, pakiet.ts);

		insertToq(WaitQueueX, tmp);
		//clocks[pakiet.id] = pakiet.ts;
		if((pakiet.ts < get_queue_ts(WaitQueueX, rank))|| (pakiet.ts == get_queue_ts(WaitQueueX, rank) && pakiet.id< rank )){
		pkt->id = rank;
		zwiekszLamporta(-1);
		pkt->ts = lamportValue;
                pkt->typ = ACKX;
		sendPacket( pkt, pakiet.id, MSG_TAG );
		}
	       break;	
	case RELX:
                set_inactive(WaitQueueX, pakiet.id);
                clocks[pakiet.id] = pakiet.ts;
		break;
	case REQE:
                 pkt->id = rank;
                 tmp = newNode(pakiet.id, 1, pakiet.ts);
                 insertToq(WaitQueueE, tmp);

                 zwiekszLamporta(-1);
                 pkt->ts = lamportValue;
                 pkt->typ = ACKE;
		                 clocks[pakiet.id] = pakiet.ts;

                 sendPacket( pkt, pakiet.id, MSG_TAG );
		break;
	case RELE:
		                set_inactive(WaitQueueE, pakiet.id);
			                 clocks[pakiet.id] = pakiet.ts;

		releaseCount++;
		break;
        case RESET_PAIR:
                Pair = -1;
                break;
        case OUT:
		prevstate = stan;
                zmienStan(NotMyTurn);
                break;

        case NUMBER:
		 printf("num %d\n", pakiet.num);
                 place[pakiet.id] = pakiet.num;
                 break;
	case ACKX:
		 clocks[pakiet.id] = pakiet.ts;
		 break;
	case ACKE:
                clocks[pakiet.id] = pakiet.ts;
                break;
        case STOP:
                prevprevstate = stan;
                zmienStan(Updated_queue);
		                deleter = pakiet.id;
                how_many_del = pakiet.num;

                break;

}

}
else if(stan == Paired){

switch ( pakiet.typ ) {

        case REQX:
                 pkt->id = rank;
                 tmp = newNode(pakiet.id, 1, pakiet.ts);
                 insertToq(WaitQueueX, tmp);
                 zwiekszLamporta(-1);
                 pkt->ts = lamportValue;
                 pkt->typ = ACKX;
                 if(pakiet.id != rank) {sendPacket( pkt, pakiet.id, MSG_TAG);}
               break;
	case RELX:
                set_inactive(WaitQueueX, pakiet.id);
                clocks[pakiet.id] = pakiet.ts;
                break;
	case RELE:
		                 clocks[pakiet.id] = pakiet.ts;

		set_inactive(WaitQueueE, pakiet.id);

                releaseCount++;
                break;
	case REQE:
		tmp = newNode(pakiet.id, 1, pakiet.ts);
		insertToq(WaitQueueE, tmp);
                //clocks[pakiet.id] = pakiet.ts;
                if((pakiet.ts < get_queue_ts(WaitQueueE, rank))|| (pakiet.ts == get_queue_ts(WaitQueueE, rank) && pakiet.id< rank )){
                pkt->id = rank;
                zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->typ = ACKE;
		                 //clocks[pakiet.id] = pakiet.ts;
		
                if(pakiet.id != rank) {sendPacket( pkt, pakiet.id, MSG_TAG );}
                }
               break;
	case NUMBER:
                 printf("num %d\n", pakiet.num);
                 place[pakiet.id] = pakiet.num;
                 break;
	case ACKX:
                 clocks[pakiet.id] = pakiet.ts;
                 break;
        case OUT:
		 if(pakiet.id != rank){
		 prevstate = stan;
                zmienStan(NotMyTurn);}
                break;
	case ACKE:
		clocks[pakiet.id] = pakiet.ts;
		break;
        case STOP:
		                deleter = pakiet.id;
                how_many_del = pakiet.num;

                prevprevstate = stan;
                zmienStan(Updated_queue);
                break;
}

}
else if(stan ==Taking){
switch (pakiet.typ){
	case REQX:
                 pkt->id = rank;
                 tmp = newNode(pakiet.id, 1, pakiet.ts);
                 insertToq(WaitQueueX, tmp);
                 zwiekszLamporta(-1);
                 pkt->ts = lamportValue;
                 pkt->typ = ACKX;
                 if(pakiet.id != rank) {sendPacket( pkt, pakiet.id, MSG_TAG);}
               break;
        case RELX:
                set_inactive(WaitQueueX, pakiet.id);
                clocks[pakiet.id] = pakiet.ts;
                break;
        case RELE:
                set_inactive(WaitQueueE, pakiet.id);
                 clocks[pakiet.id] = pakiet.ts;

                releaseCount++;
                break;
        case REQE:
                tmp = newNode(pakiet.id, 1, pakiet.ts);
                insertToq(WaitQueueE, tmp);
                //clocks[pakiet.id] = pakiet.ts;
               
                pkt->id = rank;
                zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->typ = ACKE;
                 clocks[pakiet.id] = pakiet.ts;

		if(pakiet.id != rank) {sendPacket( pkt, pakiet.id, MSG_TAG );}
               
               break;
        case NUMBER:
                 printf("num %d\n", pakiet.num);
                 place[pakiet.id] = pakiet.num;
                 break;
        case ACKX:
                 clocks[pakiet.id] = pakiet.ts;
                 break;
        case OUT:
                 prevstate = stan;
                zmienStan(NotMyTurn);
                break;
        case ACKE:
                clocks[pakiet.id] = pakiet.ts;
                break;

	case IN:
		zmien_pair_in(1);
		break;
        case STOP:
                prevprevstate = stan;
                zmienStan(Updated_queue);
		                deleter = pakiet.id;
                how_many_del = pakiet.num;

                break;


}
}
else if (stan == NotMyTurn){

switch(pakiet.typ){
        case REQX:
                 pkt->id = rank;
                 tmp = newNode(pakiet.id, 1, pakiet.ts);
                 insertToq(WaitQueueX, tmp);
                 zwiekszLamporta(-1);
                 pkt->ts = lamportValue;
                 pkt->typ = ACKX;
                 if(pakiet.id != rank) {sendPacket( pkt, pakiet.id, MSG_TAG);}
               break;
        case RELX:
                set_inactive(WaitQueueX, pakiet.id);
                clocks[pakiet.id] = pakiet.ts;
                break;

	case RELZ:
		EnergiaZ++;
		break;
        case STOP:
		                deleter = pakiet.id;
                how_many_del = pakiet.num;

                prevprevstate = stan;
                zmienStan(Updated_queue);
                break;
        case RELE:
                set_inactive(WaitQueueE, pakiet.id);
                 clocks[pakiet.id] = pakiet.ts;

                releaseCount++;
                break;
        case REQE:
                tmp = newNode(pakiet.id, 1, pakiet.ts);
                insertToq(WaitQueueE, tmp);
                //clocks[pakiet.id] = pakiet.ts;
                if((pakiet.ts < get_queue_ts(WaitQueueE, rank))|| (pakiet.ts == get_queue_ts(WaitQueueE, rank) && pakiet.id< rank )){
                pkt->id = rank;
                zwiekszLamporta(-1);
                                 clocks[pakiet.id] = pakiet.ts;

		pkt->ts = lamportValue;
                pkt->typ = ACKE;
                if(pakiet.id != rank) {sendPacket( pkt, pakiet.id, MSG_TAG );}
                }
               break;
        case NUMBER:
                 printf("num %d\n", pakiet.num);
                 place[pakiet.id] = pakiet.num;
                 break;
        case ACKX:
                 clocks[pakiet.id] = pakiet.ts;
                 break;
        case ACKE:
                clocks[pakiet.id] = pakiet.ts;
                break;

}

}
else if(stan == Updated_queue){
switch(pakiet.typ){
	case STOP:
		for(int i = 0; i < pakiet.num; i++){
                        deQueue(WaitQueueX);
                        }
		pkt->id = rank;
                zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->typ = STOP;
                sendPacket( pkt, pakiet.id, MSG_TAG );
	case START:
		cleared1 = 0;
		zmienStan(prevprevstate);
		break;
        case RELZ:
                EnergiaZ++;
                break;
case REQX:
                 pkt->id = rank;
                 tmp = newNode(pakiet.id, 1, pakiet.ts);
                 insertToq(WaitQueueX, tmp);
                 zwiekszLamporta(-1);
                 pkt->ts = lamportValue;
                 pkt->typ = ACKX;
                 if(pakiet.id != rank) {sendPacket( pkt, pakiet.id, MSG_TAG);}
               break;
        case RELX:
                set_inactive(WaitQueueX, pakiet.id);
                clocks[pakiet.id] = pakiet.ts;
                break;
        case RELE:
                set_inactive(WaitQueueE, pakiet.id);
                 clocks[pakiet.id] = pakiet.ts;

                releaseCount++;
                break;
        case REQE:
                tmp = newNode(pakiet.id, 1, pakiet.ts);
                insertToq(WaitQueueE, tmp);
                //clocks[pakiet.id] = pakiet.ts;
                if((pakiet.ts < get_queue_ts(WaitQueueE, rank))|| (pakiet.ts == get_queue_ts(WaitQueueE, rank) && pakiet.id< rank )){
                pkt->id = rank;
                zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                 clocks[pakiet.id] = pakiet.ts;

		pkt->typ = ACKE;
                if(pakiet.id != rank) {sendPacket( pkt, pakiet.id, MSG_TAG );}
                }
               break;
        case NUMBER:
                 printf("num %d\n", pakiet.num);
                 place[pakiet.id] = pakiet.num;
                 break;
        case ACKX:
                 clocks[pakiet.id] = pakiet.ts;
                 break;
        case ACKE:
                clocks[pakiet.id] = pakiet.ts;
                break;

}

}
else if(stan == Updaiting_queue){
switch(pakiet.typ){
	case STOP:
		ans_count++;
		break;
        case RELZ:
                EnergiaZ++;
                break;

case REQX:
                 pkt->id = rank;
                 tmp = newNode(pakiet.id, 1, pakiet.ts);
                 insertToq(WaitQueueX, tmp);
                 zwiekszLamporta(-1);
                 pkt->ts = lamportValue;
                 pkt->typ = ACKX;
                 if(pakiet.id != rank) {sendPacket( pkt, pakiet.id, MSG_TAG);}
               break;
        case RELX:
                set_inactive(WaitQueueX, pakiet.id);
                clocks[pakiet.id] = pakiet.ts;
                break;


        case RELE:
                set_inactive(WaitQueueE, pakiet.id);
                 clocks[pakiet.id] = pakiet.ts;

                releaseCount++;
                break;
        case REQE:
                tmp = newNode(pakiet.id, 1, pakiet.ts);
                insertToq(WaitQueueE, tmp);
                //clocks[pakiet.id] = pakiet.ts;
                if((pakiet.ts < get_queue_ts(WaitQueueE, rank))|| (pakiet.ts == get_queue_ts(WaitQueueE, rank) && pakiet.id< rank )){
                pkt->id = rank;
                zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->typ = ACKE;
                 clocks[pakiet.id] = pakiet.ts;

		if(pakiet.id != rank) {sendPacket( pkt, pakiet.id, MSG_TAG );}
                }
               break;
        case NUMBER:
                 printf("num %d\n", pakiet.num);
                 place[pakiet.id] = pakiet.num;
                 break;
        case ACKX:
                 clocks[pakiet.id] = pakiet.ts;
                 break;
        case ACKE:
                clocks[pakiet.id] = pakiet.ts;
                break;

}

}

}
}
else if (master_type == 'Y'){

while(END){
        MPI_Recv(&pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MSG_TAG, MPI_COMM_WORLD, &status);
if(pakiet.ts > lamportValue){
                    zwiekszLamporta(pakiet.ts);}
printf("TYP -[%c] id-[%d] lamp-{%d} - Odbieralem wiadomosc od {%d}, typ %d.\n", master_type, rank, lamportValue, pakiet.id, pakiet.typ);

if(stan == Rest){
switch ( pakiet.typ ) {
            case REQY:
                    pkt->id = rank;
                 tmp = newNode(pakiet.id, 1, pakiet.ts);
                 insertToq(WaitQueueY, tmp);
                 zwiekszLamporta(-1);
                 pkt->ts = lamportValue;
                 pkt->typ = ACKY;
                 if(pakiet.id != rank) {sendPacket( pkt, pakiet.id, MSG_TAG);}
                 //clocks[pakiet.id] = pakiet.ts;
                 break;
            /*case REQE:
                 pkt->id = rank;
                 tmp = newNode(pakiet.id, 1, pakiet.ts);
                 insertToq(WaitQueueE, tmp);

                 zwiekszLamporta(-1);
                 pkt->ts = lamportValue;
                 pkt->typ = ACKE;

                 sendPacket( pkt, pakiet.id, MSG_TAG );
                 break;*/
            case RELX:
                set_inactive(WaitQueueY, pakiet.id);
                clocks[pakiet.id] = pakiet.ts;
                break;
            case RESET_PAIR:
                Pair = -1;
                break;
            case OUT:
		prevstate = stan;
                zmienStan(NotMyTurn);
                break;
           case NUMBER:
                 place[pakiet.id] = pakiet.num;
                 break;
           case RELE:
                releaseCount++;
                break;
        case STOP:
                deleter = pakiet.id;
                how_many_del = pakiet.num;

                prevprevstate = stan;
                zmienStan(Updated_queue);
                break;
}
}
else if(stan == Wait){
switch ( pakiet.typ ) {

        case REQY:
                tmp = newNode(pakiet.id, 1, pakiet.ts);

                insertToq(WaitQueueY, tmp);
                //clocks[pakiet.id] = pakiet.ts;
                if((pakiet.ts < get_queue_ts(WaitQueueY, rank))|| (pakiet.ts == get_queue_ts(WaitQueueY, rank) && pakiet.id< rank )){
                pkt->id = rank;
                zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->typ = ACKY;
                sendPacket( pkt, pakiet.id, MSG_TAG );
                }
               break;
        case RELY:
                set_inactive(WaitQueueY, pakiet.id);
                clocks[pakiet.id] = pakiet.ts;
                break;
        /*case REQE:
                 pkt->id = rank;
                 tmp = newNode(pakiet.id, 1, pakiet.ts);
                 insertToq(WaitQueueE, tmp);

                 zwiekszLamporta(-1);
                 pkt->ts = lamportValue;
                 pkt->typ = ACKE;

                 sendPacket( pkt, pakiet.id, MSG_TAG );
                break;*/
        case RELE:
                releaseCount++;
                break;
        case RESET_PAIR:
                Pair = -1;
                break;
        case OUT:
		prevstate = stan;
                zmienStan(NotMyTurn);
                break;

        case NUMBER:
                 place[pakiet.id] = pakiet.num;
                 break;
        case ACKY:
                 clocks[pakiet.id] = pakiet.ts;
                 break;
        case STOP:
                deleter = pakiet.id;
                how_many_del = pakiet.num;

                prevprevstate = stan;
                zmienStan(Updated_queue);
                break;

}

}
else if(stan == Paired){
switch ( pakiet.typ ) {

        case REQY:
		 pkt->id = rank;
                 tmp = newNode(pakiet.id, 1, pakiet.ts);
                 insertToq(WaitQueueY, tmp);
                 zwiekszLamporta(-1);
                 pkt->ts = lamportValue;
                 pkt->typ = ACKY;
                 if(pakiet.id != rank) {sendPacket( pkt, pakiet.id, MSG_TAG);}
                 //clocks[pakiet.id] = pakiet.ts;
                 break;
        case OUT:
		 prevstate = stan;
                zmienStan(NotMyTurn);
                break;

        case NUMBER:
                 place[pakiet.id] = pakiet.num;
                 break;
        case ACKY:
                 clocks[pakiet.id] = pakiet.ts;
		 break;
        case RELY:
                set_inactive(WaitQueueY, pakiet.id);
                clocks[pakiet.id] = pakiet.ts;
                break;
	case IN:
		zmienStan(Taking);
		break;
        case STOP:
                deleter = pakiet.id;
                how_many_del = pakiet.num;

                prevprevstate = stan;
                zmienStan(Updated_queue);
                break;
}


}

else if(stan == Taking){
switch(pakiet.typ){
 	case REQY:
                 pkt->id = rank;
                 tmp = newNode(pakiet.id, 1, pakiet.ts);
                 insertToq(WaitQueueY, tmp);
                 zwiekszLamporta(-1);
                 pkt->ts = lamportValue;
                 pkt->typ = ACKY;
                 if(pakiet.id != rank) {sendPacket( pkt, pakiet.id, MSG_TAG);}
                 //clocks[pakiet.id] = pakiet.ts;
                 break;
        case OUT:
		 prevstate = stan;
                zmienStan(NotMyTurn);
                break;

        case NUMBER:
                 place[pakiet.id] = pakiet.num;
                 break;
        case ACKY:
                 clocks[pakiet.id] = pakiet.ts;
                 break;
        case RELY:
                set_inactive(WaitQueueY, pakiet.id);
                clocks[pakiet.id] = pakiet.ts;
                break;
	
	case IN:
                zmien_pair_in(1);
                break;
	case STOP:
		                deleter = pakiet.id;
                how_many_del = pakiet.num;

                prevprevstate = stan;
                zmienStan(Updated_queue);
                break;

}
}
else if(stan == NotMyTurn){
switch(pakiet.typ){
        case REQY:
                 pkt->id = rank;
                 tmp = newNode(pakiet.id, 1, pakiet.ts);
                 insertToq(WaitQueueY, tmp);
                 zwiekszLamporta(-1);
                 pkt->ts = lamportValue;
                 pkt->typ = ACKY;
                 if(pakiet.id != rank) {sendPacket( pkt, pakiet.id, MSG_TAG);}
                 //clocks[pakiet.id] = pakiet.ts;
                 break;
        case ACKY:
                 clocks[pakiet.id] = pakiet.ts;
                 break;
        case RELY:
                set_inactive(WaitQueueY, pakiet.id);
                clocks[pakiet.id] = pakiet.ts;
                break;
	case RELZ:
		EnergiaZ++;
		break;
	case NUMBER:
                 place[pakiet.id] = pakiet.num;
                 break;


	case STOP:
                deleter = pakiet.id;
                how_many_del = pakiet.num;

		prevprevstate = stan;
		zmienStan(Updated_queue);
		break;

}
}
else if(stan == Updated_queue){
switch(pakiet.typ){
	        case STOP:
                for(int i = 0; i < pakiet.num; i++){
                        deQueue(WaitQueueY);
                        }
		pkt->id = rank;
                zwiekszLamporta(-1);
                pkt->ts = lamportValue;
                pkt->typ = STOP;
                sendPacket( pkt, pakiet.id, MSG_TAG );
        case START:

                zmienStan(prevprevstate);
		cleared1 = 0;
                break;
	case RELZ:
                EnergiaZ++;
                break;
        case REQY:
                 pkt->id = rank;
                 tmp = newNode(pakiet.id, 1, pakiet.ts);
                 insertToq(WaitQueueY, tmp);
                 zwiekszLamporta(-1);
                 pkt->ts = lamportValue;
                 pkt->typ = ACKY;
                 if(pakiet.id != rank) {sendPacket( pkt, pakiet.id, MSG_TAG);}
                 //clocks[pakiet.id] = pakiet.ts;
                 break;
        case ACKY:
                 clocks[pakiet.id] = pakiet.ts;
                 break;
        case RELY:
                set_inactive(WaitQueueY, pakiet.id);
                clocks[pakiet.id] = pakiet.ts;
                break;
        case NUMBER:
                 place[pakiet.id] = pakiet.num;
                 break;

}

}

}
}
else if (master_type == 'Z'){
while(END){
MPI_Recv(&pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MSG_TAG, MPI_COMM_WORLD, &status);
if(pakiet.ts > lamportValue){
                    zwiekszLamporta(pakiet.ts);}
printf("TYP -[%c] id-[%d] lamp-{%d} - Odbieralem wiadomosc od {%d}, typ %d.\n", master_type, rank, lamportValue, pakiet.id, pakiet.typ);

if(stan == NotMyTurn){
switch(pakiet.typ){
	case OUT:
		zmienStan(Wait);
		break;
}
}

}
}
}
