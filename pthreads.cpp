/*
Alec Lindsey
COSC 3360
arlindse
1915596


*/
#include <iostream>
#include <pthread.h>
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <stdio.h>

using namespace std;

//global variables
static int nStudents = 0; //total number of students
static int nFreeTAs = 2; //number of TAs available
static int nFreeChairs = 2; //number of chairs available outside of TA office. arriving students will leave if value is 0
static int nStudentsWhoLeft = 0; //tracks number of students who left w/o getting help b/c the chairs were full and TAs were busy
static int nStudentsWhoWaited = 0; //tracks number of students who had to wait in a chair outside for help
static int nStudentsWhoGotHelp = 0; //tracks number of students who got help from TAs w/o waiting

//mutex declarations
static pthread_mutex_t alone;

//condition variables
pthread_cond_t FreeTA = PTHREAD_COND_INITIALIZER; //signals when a TA has become available

//structure used to store data of each student read from the input
struct pData{
    string name;
    int arrivalTime;
    int helpTime;

};

//child function
void *students(void *arg) {
    
    //store data in local variables to avoid 
    struct pData *argptr;
    char myName[32];
    int myhelpTime;
    int myArrivalTime;
    argptr = (struct pData *) arg;
    // required
    strcpy(myName, argptr->name.c_str()); 
    myhelpTime = argptr->helpTime;
    myArrivalTime = argptr->arrivalTime;

    //wait for student to arrive as given arrival time
    sleep(myArrivalTime);
    pthread_mutex_lock(&alone);
    cout << myName << " has arrived at the TA office." << endl;
    pthread_mutex_unlock(&alone);
    
    if(nFreeTAs > 0){
        //Student walks into TA office if there is a free TA
        pthread_mutex_lock(&alone);
        nFreeTAs--;
        cout << myName << " gets help." << endl;
        pthread_mutex_unlock(&alone);

        //waits until student has finished their help time w/ the TA
        sleep(myhelpTime);

        //student leaves TA office which frees up a TA
        pthread_mutex_lock(&alone);
        cout << myName << " leaves the TA office." << endl;
        nFreeTAs++;
        nStudentsWhoGotHelp++;
        pthread_cond_signal(&FreeTA); 
        pthread_mutex_unlock(&alone);
        pthread_exit((void*) 0); //terminates pthread
    }
    else if(nFreeChairs > 0){
        //if there are no free TAs, but a chair is free then the student will wait outside the TA office in a chair
        //until a TA is free
        pthread_mutex_lock(&alone); 
        nFreeChairs--;
        cout << myName << " has to wait." << endl;
        nStudentsWhoWaited++;
        while (nFreeTAs == 0) {
            pthread_cond_wait(&FreeTA, &alone); 
        } 
        nFreeChairs++;
        pthread_mutex_unlock(&alone);

        //Student walks into TA office
        pthread_mutex_lock(&alone);
        nFreeTAs--;
        cout << myName << " gets help." << endl;
        pthread_mutex_unlock(&alone);
        //waits until student has finished their help time w/ the TA
        sleep(myhelpTime);
        //student leaves TA office
        pthread_mutex_lock(&alone);
        cout << myName << " leaves the TA office." << endl;
        nFreeTAs++;
        pthread_cond_signal(&FreeTA); 
        pthread_mutex_unlock(&alone);
        pthread_exit((void*) 0); //terminates pthread
    }
    else{
        //if there are no free TAs or chairs then the student will leave the TA office w/o getting help
        pthread_mutex_lock(&alone);
        cout << myName << " has left without getting help." << endl;
        nStudentsWhoLeft++;
        pthread_mutex_unlock(&alone);
        pthread_exit((void*) 0); //terminates pthread
    }

    pthread_exit((void*) 0); //terminates pthread
}

//Function declarations
void readInput(vector<pData> &data); //reads specified input file
void outputData(vector<pData> data); //outputs all data in pData format
void summaryReport(); //outputs the gathered data about TA office activity

int main(){
    vector<pData> data; //stores all input data
    int i = 0; //index value used for indexing tid
    void *status;

    readInput(data);

    //creates thread id array that counts from 0 to nStudents-1
    pthread_t tid[nStudents];

    //initializes mutex
    pthread_mutex_init(&alone, NULL);

    //loop generates threads
    for(i = 0; i < nStudents; i++){
        pthread_create(&tid[i], NULL, students, (void *) &data[i]);
    }

    //waits for all threads to finish
    for(i = 0; i < nStudents; i++) {
        pthread_join(tid[i], &status);
    }

    //output summary
    cout << endl << endl;
    summaryReport();

    //clean up any remaining threads/mutexes before ending program
    pthread_mutex_destroy(&alone);
    pthread_exit((void*) 0); //terminates pthread
    return 0;
}


//Function definitions
void readInput(vector<pData> &data){
    string tempName;
    int tempArrivalTime;
    int tempHelpTime;
    


    
    //read each line of file until there is no more input
    //file is formatted as {name, arrivalTime, helpTime}
    while(cin >> tempName){
        string fileName;
        cout << "Enter input file name: ";
        cin >> fileName;
        ifstream inFile(fileName);
        inFile >> tempArrivalTime;
        inFile >> tempHelpTime;
        
        //create a temporary pData
        pData temp;
        temp.name = tempName;
        temp.arrivalTime = tempArrivalTime;
        temp.helpTime = tempHelpTime;
        //append temp to end of data vector
        data.push_back(temp);
        //tally student total
        nStudents++;
    }
}

void outputData(vector<pData> data){
    for(auto a : data){
        cout << a.name << " " << a.arrivalTime << " " << a.helpTime << endl;
    }
    cout << "Number of Students: " << nStudents << endl;
}

void summaryReport(){
    cout << "SUMMARY REPORT" << endl;
    cout << nStudents << " student(s) went to the TA office." << endl;
    cout << nStudentsWhoLeft << " student(s) left without waiting." << endl;
    cout << nStudentsWhoWaited << " student(s) had to wait before getting help." << endl;
    cout << nStudentsWhoGotHelp << " student(s) did not have to wait." << endl;
}