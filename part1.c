#include <stdio.h>      
#include <unistd.h>     
#include <pthread.h>    
#include <sys/sem.h>    

#define PERMS 0660  

int semId;

int initSem(int semId, int semNum, int initValue) {
    return semctl(semId, semNum, SETVAL, initValue);
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
void* funcD(void* nothing) {
    printf("Semaforo abierto con exito\n");
    printf("Iniciando iteracion\n");
    printf("Recurso tomado\n");
    P(semId, 0);        
    printf("Buenos dias! Recurso usado con 3 .\n");
    
    usleep(50*1000);  
    
    printf("Buenos dias!Recurso usado con 4 \n");
    P(semId, 1);       
    printf("Recurso devuelto.\n");

    V(semId, 0);        
    V(semId, 1);        
    return NULL;
}

void* funcE(void* nothing) {
    printf("Semaforo abierto con exito\n");
    printf("Iniciando iteracion\n");
    printf("Recurso tomado\n");
    P(semId, 1);       
    printf("Buenos dias! Recurso usado con 3 .\n");
    
    usleep(5*1000);  
    
    printf("Buenos dias!Recurso usado con 4 \n");
    P(semId, 0);       
    printf("Recurso devuelto.\n");

    V(semId, 0);        
    V(semId, 1);        
    return NULL;
}


int main(int argc, char* argv[]) {
    int num;
    FILE *fptr;
    FILE *fptropen;
    fptropen = fopen("new.txt", "r");
    fptr = fopen("new.txt", "w");
    printf("Creando threads... \n");
    fprintf(stderr, "Creando threads... \n");
    int i;          
    semId = semget(ftok(argv[0], 'A'), 2, IPC_CREAT | PERMS);
    initSem(semId, 0, 1);
   
    initSem(semId, 1, 1);
    
    pthread_t thread[10];    
    int iret[5];  
    iret[0]= pthread_create(&thread[0], NULL, funcA, NULL);
    printf("Iniciando Thread 1 \n");
    iret[1]=pthread_create(&thread[1], NULL, funcB, NULL);
    printf("Iniciando Thread 2 \n");
    iret[2]=pthread_create(&thread[2], NULL, funcD, NULL);
    printf("Iniciando Thread 3 \n");
    iret[3]=pthread_create(&thread[4], NULL, funcE, NULL);
    printf("Iniciando Thread 4 \n");
   
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
