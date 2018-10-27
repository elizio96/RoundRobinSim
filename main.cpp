#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <atomic>
#include <fstream>
#include "List1.h"

#define NUM_THREADS 4
using namespace std;

struct mutexCounter{
	pthread_mutex_t lock;
	atomic<int> count;
};
void FiletoList(string n);
void *incrementClock(void *parms);
void *waitingProcesses(void *parms);
void *readyTOPCB(void *parms);
void *finishedProcesses(void *parms);
bool isRunning = true;
mutexCounter countingClock;
List newList, readyList, pcbElement, FinishList;
int timeMax;
int main(int argc, char *argv[]) {
	countingClock.count = 0;
	pthread_mutex_init(&countingClock.lock, NULL);
	initializeList(&newList);
	initializeList(&readyList);
	initializeList(&pcbElement);
	initializeList(&FinishList);
	
	int num;
	cout << "Enter 0 for data0 or Enter 1 for data1: " << endl;
	cin >> num;
	if(num == 1){
		FiletoList("Simulation-data-1.txt");
	}
	if(num == 0){
		FiletoList("Simulation-data-0.txt");
	}
	
	cout << "What is the max time you want to run for each process: " << endl;
	cin >> timeMax;
	

	pthread_t threads[NUM_THREADS];
	int rc;
	void *status;
	
	
	rc = pthread_create(&threads[0], NULL, incrementClock, (void *)&countingClock);
	if(rc){
		cout << "ERROR, Clock not operating." << endl;
		exit(-1);
	}
	
	while(isRunning){
		rc = pthread_create(&threads[1], NULL, waitingProcesses, (void *)&countingClock);
		//cout << "Process 1" << endl;
		if(rc){
			cout << "ERROR in waiting" << endl;
			exit(-1);
		}
		rc = pthread_create(&threads[2], NULL, readyTOPCB, (void *)&countingClock);
		//cout << "Process 2" << endl;
		if(rc){
			cout << "ERROR in tranfering to PCB" << endl;
			exit(-1);
		}
		rc = pthread_create(&threads[3], NULL, finishedProcesses, (void *)&countingClock);
		//cout << "Process 3" << endl;
		if(rc){
			cout << "ERROR in Finishing a processes" << endl;
			exit(-1);
		}
	}
	
	pthread_join(threads[0], &status);
	pthread_join(threads[1], &status);			
	pthread_join(threads[2], &status);
	pthread_join(threads[3], &status);
	
	
	pthread_mutex_destroy(&countingClock.lock);	
	
	ofstream readDoc;
	readDoc.open("finishedProcesses.txt");
	cout << "Completed Processes:\n" << endl;
	readDoc << "Completed Processes:\n" << endl;
	int sum = 0;
	int sizeOfList = size(FinishList);
	for(int i = 0; i < sizeOfList; i++){		
		readDoc << "Process " << getFront(FinishList).pid << "\nFinish time: " << getFront(FinishList).timeOnFinish << " Time spent on ready: " << (getFront(FinishList).timeOnFinish-getFront(FinishList).timeOnWaiting)<< endl;
		
		cout << "Process " << getFront(FinishList).pid << "\nFinish time: " << getFront(FinishList).timeOnFinish << " Time spent on ready: " << (getFront(FinishList).timeOnFinish-getFront(FinishList).timeOnWaiting)<< endl;
		sum += (getFront(FinishList).timeOnFinish-getFront(FinishList).timeOnWaiting);
		popFront(FinishList);
	}
	cout << "\nAverage time spent on ready: "<< (double)sum/sizeOfList << endl;
	readDoc << "\nAverage time spent on ready: "<< (double)sum/sizeOfList << endl;
	cout << "Ending clock time: " << countingClock.count << endl;
	readDoc << "Ending clock time: " << countingClock.count << endl;
	readDoc.close();
}

void *incrementClock(void *parms){
	while(isRunning){
		if(!isEmpty(newList)|| !isEmpty(readyList) || !isEmpty(pcbElement)){ 
			pthread_mutex_lock(&(countingClock.lock));
			countingClock.count++;
			pthread_mutex_unlock(&(countingClock.lock));
			usleep(500);
		}else {
			isRunning = false;
		}
	}
	return EXIT_SUCCESS;
}

void FiletoList(string n){
	ifstream readDoc;
	readDoc.open(n);
	int data1, data2, data3;
	PCB a;
	while(readDoc >> data1 >> data2 >> data3){	
		a.pid = data1;
		a.processTime = data2;
		a.proccessEnd = data3;
		pushBack(newList, a);
	}
	readDoc.close();
}

void *waitingProcesses(void *parms){
	pthread_mutex_lock(&(countingClock.lock));
	int ctime = countingClock.count;
	pthread_mutex_unlock(&(countingClock.lock));
	PCB a;	
	if(!isEmpty(newList)){
		if((ctime >= getFront(newList).processTime) && (timeMax <= (getFront(newList).proccessEnd+ctime))){
			pthread_mutex_lock(&(countingClock.lock));
			a = popFront(newList);
			a.timeOnWaiting = ctime;
			pushBack(readyList, a);
			pthread_mutex_unlock(&(countingClock.lock));
		}
		if((getFront(newList).proccessEnd-ctime) < timeMax){
			pthread_mutex_lock(&(countingClock.lock));
			pushBack(newList, popFront(newList));
			pthread_mutex_unlock(&(countingClock.lock));
		}
	}
	return 0;
}

void *readyTOPCB(void *parms){	
	pthread_mutex_lock(&(countingClock.lock));
	int ctime = countingClock.count;
	pthread_mutex_unlock(&(countingClock.lock));
	if(!isEmpty(readyList)){  //pcbElement
		pthread_mutex_lock(&(countingClock.lock));
		pushBack(pcbElement, popFront(readyList));
		pthread_mutex_unlock(&(countingClock.lock));
	}
	return 0;
}

void *finishedProcesses(void *parms){
	pthread_mutex_lock(&(countingClock.lock));
	int ctime = countingClock.count;
	pthread_mutex_unlock(&(countingClock.lock));
	PCB a;
	if(size(pcbElement) == 1){
		pthread_mutex_lock(&(countingClock.lock));
		a = popFront(pcbElement);
		a.timeOnFinish = ctime;
		pushBack(FinishList, a);
		//pushBack(FinishList, popFront(pcbElement));
		pthread_mutex_unlock(&(countingClock.lock));
		int id = getBack(FinishList).pid;
		cout << id << ": process finished" << endl;
	}
	return 0;
}


//List.cpp
ListInfo getBack(const List list){
	return list->prev->info;
}

void initializeList(List *listPtr){
	NodeType* newNode = (struct NodeType*)malloc(sizeof(struct NodeType));
	
	newNode->next = newNode;
	newNode->prev = newNode;
	*listPtr = newNode;
}

void pushBack(const List list, ListInfo value){
	struct NodeType* newItem = (struct NodeType*)malloc(sizeof(NodeType));
	
	newItem->info = value;
	newItem->prev = list->prev;
	newItem->next = list;
	list->prev->next = newItem;
	list->prev = newItem;
}

bool isEmpty(const List list){
	if((list->next == list) && (list->prev = list)){
		return true;
	}
	return false;
}

ListInfo getFront(const List list){
	return list->next->info;
}

ListInfo popFront(const List list){
	NodeType *ptr = list->next;
	ListInfo value = ptr->info;
	list->next = list->next->next;
	list->next->prev = list;
	free(ptr);
	return value;
}

void pushFront(const List list, ListInfo value){	
	struct NodeType* newItem = (struct NodeType*)malloc(sizeof(NodeType));
	
	newItem->info = value;
	newItem->next = list->next;
	newItem->prev = list;
	list->next->prev = newItem;
	list->next = newItem;
}

int size(const List list){
	List newList = list->next;
	int count = 0;
	while(newList != list){
		count++;
		newList = newList->next;
	}
	return count;
}