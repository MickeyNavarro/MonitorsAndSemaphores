#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <dispatch/dispatch.h> //due to Mac OS not supporting certain semaphore calls, this will be used in place of them

//Almicke Navarro
//CST-221
//September 24, 2019
//This is my own work.

//This is the same file for the Producer and Consumer problem, but I have implemented Semaphores here

/*Solution: Create a mutex and a semaphore.
 The mutex will only be unlocked when performing the two processes;
    it will only be unlocked once in the producer process to allow both processes to occur
    it will only be locked once in the consumer process when both processes are finished
 The semaphore will keep track of how many spots have been filled in the buffer;
    it will be start with the value of 0 as there will be no filled spots upon run
    it will be incremented after a new product has been added to the buffer
    it will be decermented after a product has been taken from the buffer
 */

//Resources: https://heldercorreia.com/semaphores-in-mac-os-x-fd7a7418e13b
//           https://stackoverflow.com/questions/36755003/initialise-semaphores-using-sem-open
//           https://www.geeksforgeeks.org/producer-consumer-problem-using-semaphores-set-1/
//           https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/sem_post.2.html#//apple_ref/doc/man/2/sem_post
//           https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/pthread_exit.3.html



//define buffer
#define MAX 10
int buffer[MAX];

//define 2 threads through the use of an array
pthread_t threads[2];

//define the mutex and semaphores
pthread_mutex_t mutex;
sem_t *empty, *full;
    //empty semaphore is used to keep track of how many empty spots there is left in the buffer
    //full semaphore is used to keep track if all the buffer spots are filled

//dispatch_semaphore_t semaphore;
//semaphore = dispatch_semaphore_create(1);

//define a counter to determine how many products are produced/yet to be consumed
int counter = 0;

//define the size of buffer
int length = sizeof(buffer)/sizeof(int);

//define methods
int produce();
void put();
void *producer();
void *consumer();
void consume();
int get();

//code given by instructor
int theProduct;

// The Child PID if the Parent else the Parent PID if the Child
pid_t otherPid;

//method to simply increment the product, hence creating a new product
int produce(){
    return theProduct+1;
}

//method to output that the product has been consumed
void consume(int i){
    printf("Consumed: %i\n", i);
}

//method to put the product in the buffer
void *producer() {
    printf("/***Producing***/\n");
    
    int i;
    
    //unlock the mutex until the producer and consumer functions are finished
    if (pthread_mutex_unlock(&mutex)){
        printf("\n ERROR unlocking the mutex");
        exit(1);
    }
    printf("Mutex has been unlocked.\n");
    
    //loop for forever
    while(1){
        
        //check if the buffer is full
        if(length == counter){
            
            //if yes,
            //output
            printf("Buffer is full. No new products may be added.\n");
            
            //put the producer to sleep
            printf("Producer will now been put to sleep.\n");
            sleep(20); //process will temperarily stop here
            
            //exit cleanly from the producer
            pthread_exit(&threads[0]);
            
        }
        else {
            //if no,
            //output
            printf("Buffer is not full. You may add new products.\n");
            
            //produce the new product
            i = produce();
            
            //put the product into the buffer
            put(i);
            
            //increments the # of full spots in the buffer
            sem_post(full);
            
            printf("Now there is one more filled spot in the buffer.\n");

            //increase counter
            counter++;
        }
        
    }
}

//method to consumer the product from the buffer
void *consumer() {
    printf("/***Consuming***/\n");
    
    int i;
    //loop for forever
    while(1){
        
        
        //check if buffer is full
        if (counter <= 10 && counter > 0){
            //if yes,
            //output
            printf("Buffer is full. Products may now be consumed.\n");
            
            //get the most recent product from the buffer
            i = get();
            
            //consume the product
            consume(i);
            
            //decrements the # of full spots in the buffer
            sem_wait(full);
            
            printf("Now there is one less filled spot in the buffer.\n");
            
            //decrease the counter
            counter--;
            
            //check if the buffer is empty
            if (counter ==0){
                //if yes,
                //output
                printf("Buffer is empty. Products may NOT be consumed.\n");
                
                //lock the mutex and block until it becomes available
                    //the consumer function is done so the mutex must be locked
                pthread_mutex_lock(&mutex);
                printf("Mutex has been locked.\n");
                
                //put consumer to sleep
                printf("Consumer will now been put to sleep.\n");
                sleep(20);
                
                //exit cleanly from the consumer
                pthread_exit(&threads[1]);
                
            }
        }
    }
}

//method to receive what the producer creates
void put(int i) {
    //set the product to the given int
    theProduct = i;
    
    //output what has been produced
    printf("Produced: %i\n", i);
    
    //put into buffer
    buffer[counter] = i;
    
    return;
}

//method to get the product from the buffer
int get() {
    //get the most recent product produced from the buffer
    int productToBeConsumed = buffer[counter];
    return productToBeConsumed;
}

int main(int argc, char *argv[]) {
    //initiate the mutex
    pthread_mutex_init(&mutex, NULL);
    
    /*
    //create an empty semaphores & check if it was successful
        //empty semaphore is set to 10 empty spots in the buffer
    empty = sem_open("/empty_sem", O_CREAT, 0644, MAX);
    if (empty == SEM_FAILED) {
        perror("Failed to open semphore for empty");
        exit(-1);
    }
     */
    
    //create a full semaphore & check if it was successful
        //full semaphore is set to 0 full spots in the buffer
    full = sem_open("/full_sem", O_CREAT, 0644, 0);
        //sem_open will return SEM_FAILED when it fails
    if (full == SEM_FAILED) {
        sem_close(empty);
        perror("Failed to open semphore for full");
        exit(-1);
    }
     
    //create thread for the producer
    if (pthread_create(&threads[0], 0, producer, 0)) {
        printf("\n ERROR creating producer thread");
        exit(1);
    }
    
    //wait for thread termination
    if (pthread_join(threads[0],0)) {
        printf("\n ERROR producer thread is not joinable");
        exit(1);
    }
    
    //create thread for the consumer
    if (pthread_create(&threads[1], 0, consumer, 0)){
        printf("\n ERROR creating consumer thread");
        exit(1);
    }
    //wait for thread termination
    if (pthread_join(threads[1],0)){
        printf("\n ERROR consumer thread is not joinable");
        exit(1);
    }
    
    // Thread creation cleanup
    pthread_exit(NULL);
    return 0;
}
