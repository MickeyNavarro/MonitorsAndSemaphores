#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

//Almicke Navarro
//CST-221
//September 29, 2019
//This is my own work.

//This is the same file for the Producer and Consumer problem, but I have implemented a Monitor here

/*Solution: Create a monitor.
 The monitor will have the variables of mutex, full, and empty
 The mutex will only be unlocked when performing the two processes;
    it will only be unlocked once in the producer process to allow both processes to occur
    it will only be locked once in the consumer process when both processes are finished
 The full variable will keep track of if the buffer is full;
    it will be start with the value of 0 as bueffer is not full
 The empty variable will keep track of if the buffer is empty;
    it will be start with the value of 1 as buffer is empty
 
*/


//Resources: https://www.cs.columbia.edu/~junfeng/13fa-w4118/lectures/l10-semaphore-monitor.pdf
//           http://denninginstitute.com/modules/ipc/purple/prodmon.html
//           https://github.com/jriley15/CST-221/blob/master/Threads/Monitors%20and%20Semaphores/m.c
//           https://www.tutorialspoint.com/monitors-vs-semaphores



//define buffer
#define MAX 10
int buffer[MAX];

//define a monitor using struct
typedef struct Monitor {
    
    //mutex variable (0 = locked, 1 = unlocked)
    int mutex;
    
    //full variable (0 = false, 1 = true)
    int full;
    
    //empty variable (0 = false, 1 = true)
    int empty;
    
} Monitor;

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


//method to simply increment the product, hence creating a new product
int produce(){
    return theProduct+1;
}

//method to output that the product has been consumed
void consume(int i){
    printf("Consumed: %i\n", i);
}

//method to put the product in the buffer
void *producer(Monitor *monitor) {
    printf("/***Producing***/\n");
    
    int i;
    
    //unlock the mutex until the producer and consumer functions are finished
    monitor->mutex = 1;
    
    printf("Mutex has been unlocked.\n");
    
    //loop while monitor is NOT deemed as full
    while(monitor->full == 0){
        
        //check if the buffer is full
        if(length == counter){
            
            //if yes,
            //output
            printf("Buffer is full. No new products may be added.\n");
            
            //set monitor to full & not empty
            monitor->full = 1;
            monitor->empty = 0;
            
            //put the producer to sleep
            printf("Producer will now been put to sleep.\n");
            sleep(20); //process will temperarily stop here
            
            //call the consumer process
            consumer(monitor);
        }
        else {
            //if no,
            //output
            printf("Buffer is not full. You may add new products.\n");
            
            //produce the new product
            i = produce();
            
            //put the product into the buffer
            put(i);
            
            //increase counter
            counter++;
        }
        
    }
    return 0;
}

//method to consumer the product from the buffer
void *consumer(Monitor *monitor) {
    printf("/***Consuming***/\n");
    
    int i;
    //loop while monitor is NOT deemed as empty
    while(monitor->empty == 0){
        
        //check if buffer is full
        if (counter <= 10 && counter > 0){
            //if yes,
            //output
            printf("Buffer is full. Products may now be consumed.\n");
            
            //get the most recent product from the buffer
            i = get();
            
            //consume the product
            consume(i);
            
            //decrease the counter
            counter--;
            
            //check if the buffer is empty
            if (counter ==0){
                //if yes,
                //output
                printf("Buffer is empty. Products may NOT be consumed.\n");
                
                //set monitor to not full & empty
                monitor->full = 0;
                monitor->empty = 1;
                
                //lock the mutex and block until it becomes available
                    //the consumer function is done so the mutex must be locked
                monitor->mutex = 0;
                
                //put consumer to sleep
                printf("Consumer will now been put to sleep.\n");
                sleep(20);
                
            }
        }
    }
    return 0;
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


/*
 
 unable to figure out how to code correctly; got compiler errors; unsure about syntax
 
Monitor ProducerConsumerMonitor
{
    //mutex variable (0 = locked, 1 = unlocked)
    int mutex;
    
    //full variable
    int full;
    
    //empty variable
    int empty;
    
    Procedure p() {
        producer();
    }
    
    Procedure c(){
        consumer();
    }
    
    
}
*/


int main(int argc, char *argv[]) {
    
    //create monitor struct
    Monitor monitor = {
        //unlock the mutex by setting to 1 (true)
        .mutex = 1,
        //set the full attribute to 0 (false)
        .full = 0,
        //set the empty attribute to 1 (true)
        .empty = 1
    };
    
    
    //call the producer function first and pass the monitor
    producer(&monitor);
    
    return 0;
}
