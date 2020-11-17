#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> 
#include<sys/shm.h>

sem_t coronaPatient;
sem_t fluPatient;
sem_t Stop;


int virusType(){
	return rand() & 1;
}

void * Admit(void * param){

int key=shmget(12329, 1024,IPC_CREAT | IPC_EXCL | 0666);
int potentialCPatients = (int) shmat(key, NULL, 0);

 sem_wait(&Stop); 
 potentialCPatients++;
 sem_post(&Stop); 

   if( virusType() == 1){	
	 sem_wait(&Stop); 
	potentialCPatients--; 
	sem_post(&Stop); 

	printf("Patient has corona\n");
	sem_post(&coronaPatient); 

	while(1){
		if( virusType() == 0 && virusType() == 0 ){
		 sem_wait(&coronaPatient); 
		 printf("Patient Recovered\n");
		 break;
		}
	}

 }
   else{
	sem_wait(&Stop); 
	potentialCPatients--;
	sem_post(&Stop); 

 	printf("Patient has Flu\n");
	printf("Patient not admitted\n");	
	sem_post(&fluPatient); 
	}
	
}


int main()
{
srand(time(0)); 

	int N;
	int i;
	printf("Input the number of patients"); 
	scanf("%d", &N);  

	pthread_t * id =  (pthread_t*) malloc(N* sizeof(pthread_t));

		sem_init(&coronaPatient, 0, 1); 
		sem_init(&fluPatient, 0, 1); 
		sem_init(&Stop, 0, 1); 

		int key=shmget(12329, 1024,0);
		int ptr= (int) shmat(key, NULL, 0);

			for( i=0 ; i < N; i++){
				if (pthread_create( &id[i] , NULL,  &Admit , NULL)== -1){
					printf("Thread Creation Failed!");
					return 1;
				}
			}

			for( i=0 ; i < N; i++){
				pthread_join( id[i] , NULL  );
			}

		sem_destroy(&coronaPatient); 
		sem_destroy(&fluPatient); 
		shmdt(ptr);
		shmctl(key, IPC_RMID, NULL);
		return 0;
}
