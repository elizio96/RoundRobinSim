#ifndef PCB_H_INCLUDED
#define PCB_H_INCLUDED

struct PCB{
	int pid;
	int processTime;
	int proccessEnd;
	
	int timeOnWaiting;
	int timeOnFinish;
};
 

#endif // PCB_H_INCLUDED
