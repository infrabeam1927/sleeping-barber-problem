#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_STUDENTS 5

pthread_t ta_thread;
pthread_t student_threads[NUM_STUDENTS];

//global variables
int num_waiting = 0;   
int chairs_available = 3; 
pthread_mutex_t mutex;  // for the shared variables
sem_t sem_students;     // for TA when a student arrives
sem_t sem_ta;           // for students when the TA is ready


typedef struct {
    int help_time;     
} ta_t;

typedef struct {
    int id;       
    
    int programming_time; 
    int help_time;      
} student_t;


void* student_function(void* arg) {
    student_t* student = (student_t*)arg; //assiging a variable

    while (1) {
        printf("Student %d is programming\n", student->id);
        sleep(student->programming_time);
        
        pthread_mutex_lock(&mutex);// critical section
        
        if (num_waiting < chairs_available) {
            num_waiting++;
            pthread_mutex_unlock(&mutex);
            printf("Student %d is waiting for help\n", student->id);
            sem_wait(&sem_students); // Wait for the TA to be available
            printf("Student %d is receiving help\n", student->id);
            sem_wait(&sem_ta); // Wait for the TA to finish helping
        }
        
        else {
            pthread_mutex_unlock(&mutex);// end of critical section
            printf("Student %d is coming back later\n", student->id);
        }
    
        
    }
    
    return NULL;
}
void* ta_function(void* arg) {
    
    ta_t* ta = (ta_t*)arg;
    
    while (1) {
        pthread_mutex_lock(&mutex);//critical section
        // Check if there are students waiting
        if (num_waiting<=0){ // no students waiting
            printf("TA is sleeping\n");// The TA is sleeping
        }
        else {// There are students waiting
            sem_post(&sem_students);
            num_waiting--;
        }
        pthread_mutex_unlock(&mutex);// end of critical section
        printf("TA is helping a student\n");
        sleep(ta->help_time);
        sem_post(&sem_ta); // Signal to the student that TA is done helping
    }
    return NULL;
}
int main() {
    int a,b,c,imp;
    char cd;
    
    pthread_mutex_init(&mutex, NULL); // Initialize the mutex and semaphores below it
    sem_init(&sem_students, 0, 0);
    sem_init(&sem_ta, 0, 0);
    ta_t ta = {10};
    student_t students[NUM_STUDENTS]; 
    // makes students
    for (int i = 0; i < NUM_STUDENTS; i++) {
        students[i].id = i+1;
        students[i].programming_time = rand() % 20;
        students[i].help_time = rand() % 3;
    }
    
    int aadi,rumael;
    
    pthread_create(&ta_thread, NULL, ta_function, &ta);
    for (int i = 0; i < NUM_STUDENTS; i++) {
        pthread_create(&student_threads[i], NULL, student_function, &students[i]);
    }
    pthread_join(ta_thread, NULL);
   
    for (int i = 0; i < NUM_STUDENTS; i++) {
        pthread_join(student_threads[i], NULL);
    }
    
    pthread_mutex_destroy(&mutex);
    
    sem_destroy(&sem_students);
    sem_destroy(&sem_ta);

    return 0;
}

