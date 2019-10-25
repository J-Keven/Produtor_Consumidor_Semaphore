#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h> 

#define Tam_BUFFER 4
#define N 2// Thr_Produtor
#define M 3// Thr_Cliente

sem_t ocupado; // Define a quantidade de posiçoes no buffer esta oculpado.
sem_t vazio; // Define a quantidade de posiçoes no buffer esta vazia.
sem_t mutua; // garantir que padronize entre um produtor e um consumidor. 

void* produtor(void *id);
void* consumidor(void *id);

static int inicio = 0, fim = 0;
static int QTD_producer = N * Tam_BUFFER;
static int QTD_consumer = M * Tam_BUFFER;

int buffer[Tam_BUFFER];
pthread_t  Thr_Produtor[N], Thr_Cliente[M];

int main()
{
    srand(time(NULL));
    sem_init(&ocupado,0,0);
    sem_init(&vazio, 0, Tam_BUFFER);
    sem_init(&mutua, 0, 1);

    long i;
    for(i = 0; i < N; i++){
        pthread_create(&Thr_Produtor[i], NULL, produtor, (void *) &i);
    }
    for(i = 0; i < M; i++){
        pthread_create(&Thr_Cliente[i], NULL, consumidor, (void *) &i);
    }
    for(i = 0; i < N;i++){
        pthread_join(Thr_Produtor[i], NULL);
    }
    for(i = 0; i < M; i++)
    {
        pthread_join(Thr_Cliente[i], NULL);
    }

    printf("Estoque: ");
    for(i = 0; i < Tam_BUFFER; i++)
        printf("%d ", buffer[i]);

    printf("\nSupermecado Fechado hahahahaha!!!\n");
    sem_destroy(&vazio);
    sem_destroy(&ocupado);
    pthread_exit(NULL);
    return 0;
}

void* produtor(void *id)  
{
    long id2 = *(long *) id;
    for(int i = 0; i < Tam_BUFFER; i++){
        if(QTD_consumer > 0){
            sem_wait(&vazio);
            sem_wait(&mutua);
            sleep(3);
            if(QTD_consumer > 0){
                QTD_producer--;
                fim =(fim + 1) % Tam_BUFFER;
                buffer[fim] = 1 + rand() % 9;
                printf("PRODUTOR %ld esta inserido o produto %d na posicao %d.\n\n", id2, buffer[fim], fim);
            }
            sem_post(&ocupado);

        }
        if(QTD_consumer <= 0){
            for(int j = 0; j < N; j++)
                pthread_cancel(Thr_Produtor[j]); 
        }
    }
    return NULL;
}

void* consumidor(void *id)
{   
    long id2 = *(long *) id;
    for(int i = 0; i < Tam_BUFFER; i++) {
        if(QTD_producer > 0){
            sem_wait(&ocupado);
            sleep(3);
            QTD_consumer--;
            inicio = (inicio + 1) % Tam_BUFFER;
            printf("CONSUMIDOR %ld esta retirando o produto %d na posicao %d.\n\n", id2, buffer[inicio], inicio);
            buffer[inicio] = 0;
            sem_post(&mutua);
            sem_post(&vazio);
        }
        if(QTD_producer <= 0)
        {
            for(int j = 0; j < M; j++)
                pthread_cancel(Thr_Cliente[j]); 
        }
    }
    return NULL;
    
}