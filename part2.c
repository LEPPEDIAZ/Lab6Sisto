#include <stdio.h>      
#include <unistd.h>     
#include <pthread.h>    
#include <sys/sem.h>
#define PERMS 0660
pthread_cond_t  cond_var = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t id_mutex = PTHREAD_MUTEX_INITIALIZER;

int counter = 0;
int semId;

int initSem(int semId, int semNum, int initValue){
	return semctl(semId, semNum, SETVAL, initValue);
}
    

#define LIMIT 10   


int getId() {
    static int id = 0;
    pthread_mutex_lock(&id_mutex);      
    id++;
    pthread_mutex_unlock(&id_mutex);   
    return id;
}

int P(int semId, int semNum) {
   
    struct sembuf operationList[1];
    operationList[0].sem_num = semNum;
    operationList[0].sem_op = -1;
    operationList[0].sem_flg = 0;
    return semop(semId, operationList, 1);
}

int V(int semId, int semNum) {
   
    struct sembuf operationList[1];
    operationList[0].sem_num = semNum;
    operationList[0].sem_op = 1;
    operationList[0].sem_flg = 0;
    
    return semop(semId, operationList, 1);
}

void* funcA(void* nothing) {
    printf("Semaforo abierto con exito\n");
    printf("Iniciando iteracion\n");
    printf("Recurso tomado\n");
    P(semId, 0);        
    printf("Buenos dias! Recurso usado con 1 .\n");
    
    usleep(50*1000);  
    
    printf("Buenos dias!Recurso usado con 2 \n");
    P(semId, 1);       
    printf("Recurso devuelto.\n");

    V(semId, 0);        
    V(semId, 1);        
    return NULL;
}

void* funcB(void* nothing) {
    printf("Semaforo abierto con exito\n");
    printf("Iniciando iteracion\n");
    printf("Recurso tomado\n");
    P(semId, 1);       
    printf("Buenos dias! Recurso usado con 2 .\n");
    
    usleep(5*1000);  
    
    printf("Buenos dias!Recurso usado con 1 \n");
    P(semId, 0);       
    printf("Recurso devuelto.\n");

    V(semId, 0);        
    V(semId, 1);        
    return NULL;
}
void* increment(void* nothing) {
    int	exit = 0;  
    int shortId = getId();   

    while(!exit) {   	
    
        pthread_mutex_lock(&mutex);         

        if (counter >= LIMIT) {     
            printf("[MUTEX ADQUIRIDO] %d : ENTRANDO EN EL MONITOR\n", shortId);
            pthread_cond_signal(&cond_var);	
            exit = 1;    
        } else {
            
            printf("[MUTEX ADQUIRIDO] %d : MONITOR DE '%d' A '%d'\n", 
            shortId, counter, counter + 1);

            counter = counter + 1;      
        }

        pthread_mutex_unlock(&mutex);    

      
        usleep(1000); 
    }

   
    printf("\tRecursos actuales [MUTEX ADQUIRIDO] %d\n", shortId);
    return NULL;
}

void* isReached(void* nothing) {

    int shortId = getId();

    pthread_mutex_lock(&mutex);         

    
    if (counter < LIMIT) { 
        printf("[ISREACHED] %d : WAITING SIGNAL\n", shortId);
        pthread_cond_wait(&cond_var, &mutex);
    }
 
    printf("[MUTEX ENCONTRADO] %d : senal recibida\n", shortId);
    printf("[MUTEX ENCONTRADO] %d : thread %d\n", shortId, counter);
	
    pthread_mutex_unlock(&mutex);    


    printf("\tRecursos actuales [MUTEX ADQUIRIDO] %d\n", shortId);
    return NULL;
}


int main(int argc, char* argv[]) {
    int num;
    FILE *fptr;
    FILE *fptropen;
    fptropen = fopen("new1.txt", "r");
    fptr = fopen("new.txt", "w");
    printf("Creando threads... \n");
    int i;          
    semId = semget(ftok(argv[0], 'A'), 2, IPC_CREAT | PERMS);
    initSem(semId, 0, 1);
   
    initSem(semId, 1, 1);
   
    
    pthread_t thread[10];

    
    int iret[5];

    
    iret[0] = pthread_create(&thread[0], NULL, increment, NULL);
    iret[1] = pthread_create(&thread[1], NULL, increment, NULL);
    iret[2] = pthread_create(&thread[2], NULL, isReached, NULL);
    iret[0]= pthread_create(&thread[0], NULL, funcA, NULL);
    printf("Iniciando Thread 1 \n");
    iret[3]=pthread_create(&thread[3], NULL, funcB, NULL);
    printf("Iniciando Thread 2 \n");
    iret[4]=pthread_create(&thread[4], NULL, funcA, NULL);
    printf("Iniciando Thread 3 \n");
    iret[5]=pthread_create(&thread[5], NULL, funcB, NULL);
    printf("Iniciando Thread 4 \n");
   
   
    for (i = 0 ; i < 3 ; i++)
        pthread_join(thread[i], NULL);

    
    for (i = 0 ; i < 3 ; i++)
        printf("Thread %d returns: %d\n", i, iret[i]);
    for (i = 0 ; i < 2 ; i++) {
        pthread_join(thread[i], NULL);
  
    } 

    for (i = 0 ; i < 2 ; i++) {
        printf("thread %d retorna: %d\n", i, iret[i]);
        
    } 
    semctl(semId, 0, IPC_RMID, 0);
    semctl(semId, 1, IPC_RMID, 0);
    return 0;
}
