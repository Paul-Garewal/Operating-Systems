#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>


#define MAX_LINE_LEN 100
#define MAX_CUSTOMERS 265
#define NCLERKS 5

typedef struct Customer{
    int ID; // unique customer id
    int class; // 0 economy, 1 business
    int arrival_time; // measured in tenths of a second
    int service_time; // measured in tenths of a second

}Customer;

typedef struct node {
    Customer value;
    struct node* prev;
} node;

typedef struct queue {
    node *head;
    node *tail;
    int count;
} queue;

node* create_node(int value);
queue* queue_init(int value);

queue* businessQueue = NULL;
queue* economyQueue = NULL;

void queue_destory(node* head);
node* enqueue(queue* self, int value);
int dequeue(queue* self);
int queue_count(queue* self);
int queue_next_value(queue* self);

pthread_mutex_t mutex; //mutex declaration
pthread_mutex_t mutexTime; // mutex variable for waiting time.

//CONVARs
pthread_cond_t queueConvar[2] = {PTHREAD_COND_INITIALIZER};
pthread_cond_t clerkConvar[NCLERKS] = {PTHREAD_COND_INITIALIZER};


pthread_mutex_t mutexTypeQueue[2];
pthread_mutex_t mutexClerk[NCLERKS];

Customer totalCustomers[MAX_CUSTOMERS]; // store all customers in an array of customer structs
queue* economyQueue;
queue* businessQueue;

/* global variables */

struct timeval init_time; // use this variable to record the simulation start time; No need to use mutex_lock when reading this variable since the value would not be changed by thread once the initial time was set.
double overall_waiting_time; //A global variable to add up the overall waiting time for all customers, every customer add their own waiting time to this variable, mutex_lock is necessary.
double business_waiting_time;
double economy_waiting_time;
double total_business_waiting_time;
double total_economy_waiting_time;


int queue_length[2];// variable stores the real-time queue length information; mutex_lock needed
int num_customers; // initalize the number of customers we have to deal with
int queueStatus[2] ={0, 0};
bool selectCustomer[2] = {false};

queue* readCustomerFile(char *filename);
void* customer_entry(void* customer_info);
void* clerk_entry(void* clerk_info);
double getCurTime();
//void createThreads(int numCustomers, queue* customer_queue);

int main(int argc, char*argv[]) {

    gettimeofday(&init_time, NULL); // gets the time we start

    printf("%d", init_time);
    // read the file

    char fileName[MAX_LINE_LEN]; // hold filename argument passed 

    char* filePointer = NULL; // create pointer to file, to be referenced after sscanf

    sscanf(argv[1], "%s", fileName);

    filePointer = fileName;

    int numCustomers;

    queue* customer_queue;

    economyQueue = queue_init(num_customers);
    businessQueue = queue_init(num_customers);

    //create clerk threads 
    pthread_t clerkId[NCLERKS];
    int clerkInfo[NCLERKS] = {1, 2, 3, 4, 5};
    for(int i = 0; i < NCLERKS; i++){
        pthread_create(&clerkId[i], NULL, clerk_entry, (void *)&clerkInfo[i]);
    }

    // create customer threads
    pthread_t customerId[num_customers];
    
    for(int i = 0; i < num_customers; i++){
        pthread_create(&customerId[i], NULL, customer_entry, (void*)&totalCustomers[i]);
    }

    // join threads
    for(int i = 0; i < NCLERKS ; i++){
        pthread_join(clerkId[i], NULL);
    }

    for(int i = 0; i < num_customers; i++){
        pthread_join(customerId[i], NULL);
    }


    exit(0);
}

void *clerk_entry(void * clerk_info){

    int* clerk_id = (int*) clerk_info;
    int class_queue = 0;


    for(;;){

        //TODO
        if (queue_length[1] == 1){
            class_queue = 1;
        }else if(queue_length[0] == 0) {
            class_queue = 0;
        } else
            continue;
            //Queue Mutex Locked
		pthread_mutex_lock(&mutexTypeQueue[class_queue]);

		//If the selected queue is being served, continue waiting. Else continue function
		if(queueStatus[class_queue] != 0){
			pthread_mutex_unlock(&mutexTypeQueue[class_queue]);
			usleep(10);
			continue;
		}
		else{
			queueStatus[class_queue] = *clerk_id;			
			pthread_mutex_unlock(&mutexTypeQueue[class_queue]);
		}
		//Queue Mutex Unlocked

            pthread_mutex_lock(&mutexTypeQueue[class_queue]);

            pthread_cond_broadcast(&queueConvar[class_queue]);

            pthread_mutex_unlock(&mutexTypeQueue[class_queue]);


            pthread_mutex_lock(&mutexClerk[*clerk_id - 1]);

            pthread_cond_wait(&clerkConvar[*clerk_id-1], &mutexClerk[*clerk_id-1]);

            pthread_mutex_unlock(&mutexClerk[*clerk_id - 1]);
        
    }

    pthread_exit(NULL);

    return NULL;
}

void *customer_entry(void * customer_info){
    Customer* p_myInfo = (Customer*) customer_info;

    int arrival_time = p_myInfo->arrival_time;
    int service_time = p_myInfo->service_time;
    int user_id = p_myInfo->ID;
    int class = p_myInfo->class;

    usleep(arrival_time*100000);

	fprintf(stdout, "A customer arrives: customer ID %2d. \n", user_id);

    // need mutexes
    pthread_mutex_lock(&mutexTypeQueue[class]);
    // change this 5 at some point
    fprintf(stdout, "A customer enters a queue: the queue ID %1d, and length of the queue %2d.\n", class, queue_length[class]);

    if(class == 1){
        // go into business class
        enqueue(businessQueue, user_id);

    } else {
        // go into economy class
        enqueue(economyQueue, user_id);
    }

    queue_length[class]++;
    double queueEntryTime = getCurTime();

    for(;;){

        pthread_cond_wait(&queueConvar[class], &mutexTypeQueue[class]);

		if((user_id == dequeue(economyQueue) || user_id == dequeue(businessQueue)) && !selectCustomer[class]){
			if(class == 0){
				dequeue(economyQueue);
			}
			else{
				dequeue(businessQueue);
			}
			queue_length[class]--;
			selectCustomer[class] = true;
			break;
		}
    }
    pthread_mutex_unlock(&mutexTypeQueue[class]);
	//Queue Mutex Locked

	usleep(10);

	//Queue Mutex Locked
	pthread_mutex_lock(&mutexTypeQueue[class]);

	//Determine clerk that selected the customer
	int selectedClerk = queueStatus[class];
	queueStatus[class] = 0;

	pthread_mutex_unlock(&mutexTypeQueue[class]);
	//Queue Mutex Locked

	double serviceBegin = getCurTime();
	fprintf(stdout, "A clerk starts serving a customer: start time %.2f, the customer ID %2d, the clerk ID %1d.\n", serviceBegin, user_id, selectedClerk);

	//A customer is serviced
	usleep(service_time * 100000);

	double serviceEnd = getCurTime();
	fprintf(stdout, "A clerk finishes serving a customer: end time %.2f, the customer ID %2d, the clerk ID %1d.\n", serviceEnd, user_id, selectedClerk);

	//Time Mutex Locked
	pthread_mutex_lock(&mutexTime);
	//Add waiting times to totals
	if(class == 0){
		economy_waiting_time += (serviceBegin - queueEntryTime);
		total_economy_waiting_time;

	}
	else{
		business_waiting_time += (serviceBegin - queueEntryTime);
		total_business_waiting_time++;
	}
	pthread_mutex_unlock(&mutexTime);
	//Time Mutex Unlocked

	//Signal clerk that customer done
	pthread_cond_signal(&clerkConvar[selectedClerk - 1]);


    pthread_exit(NULL);

    return NULL;
}

/**
 * @brief Function: readCustomerFile
 * 
 *        Description: Reads the txt file passed in as an argument and parses the customer information.
 * 
 * @param filename - name of file, specififed in terminal when first running the program.
 * @return int - the number of customers???
 */
queue* readCustomerFile(char *filename){

    FILE* file = fopen(filename, "r");
    /* If file is null, we return an error message and a 0 int to stop the file.*/

    if(file == NULL){
        printf("%s", "File is null.");
        return 0;
    }
    char singleLine[MAX_LINE_LEN]; // initialize single line to hold one line of customer information in file

    Customer currentCustomer; // create customer struct to store customer values

    int count = 0;

    // remember to do error handling, could there be missing values?

    while(fgets(singleLine, MAX_LINE_LEN, file) != NULL){

        sscanf(singleLine, "%i", &num_customers);

        sscanf(singleLine, "%i:%i,%i,%i", &currentCustomer.ID, &currentCustomer.class, &currentCustomer.arrival_time, &currentCustomer.service_time);

        totalCustomers[count] = currentCustomer;

        count++;
    }

    queue* customer_info = queue_init(totalCustomers[1].ID);

    for(int i=2; i <9; i++){

        enqueue(customer_info, totalCustomers[i].ID);

    }
   
    return customer_info;

}
//getCurTime: Gets the current simulation time of the program.
double getCurTime(){
	struct timeval curTime;
	double curSec, initialSec;

	pthread_mutex_lock(&mutexTime);
	initialSec = (init_time.tv_sec + (double) init_time.tv_usec / 1000000);
	pthread_mutex_unlock(&mutexTime);

	gettimeofday(&curTime, NULL);
	curSec = (curTime.tv_sec + (double) curTime.tv_usec / 1000000);

	return curSec - initialSec;
}
