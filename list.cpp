#include <iostream>
#include "List1.h"
using namespace std;

void initializeList(List *listPtr){
	NodeType* newNode = (struct NodeType*)malloc(sizeof(struct NodeType));
	
	newNode->next = newNode;
	newNode->prev = newNode;
	*listPtr = newNode;
}

bool isEmpty(const List list){
	if((list->next == list) && (list->prev = list)){
		return true;
	}
	return false;
}

void clearList(List *listPtr){
	List list = *listPtr;
	while(!isEmpty(list)){
		popFront(list);	
	}
	cout << "Emptied list!" << endl;
}

void pushFront(const List list, ListInfo value){	
	struct NodeType* newItem = (struct NodeType*)malloc(sizeof(NodeType));
	
	newItem->info = value;
	newItem->next = list->next;
	newItem->prev = list;
	list->next->prev = newItem;
	list->next = newItem;
}

void pushBack(const List list, ListInfo value){
	struct NodeType* newItem = (struct NodeType*)malloc(sizeof(NodeType));
	
	newItem->info = value;
	newItem->prev = list->prev;
	newItem->next = list;
	list->prev->next = newItem;
	list->prev = newItem;
}

ListInfo popFront(const List list){
	List NoValue = NULL;
	if(!isEmpty(list)){
		NodeType *ptr = list ->next;
		ListInfo value = ptr-> info;
		list -> next = list -> next -> next;
		list -> next -> prev = list;
		free(ptr);
		return value;
	}
	return NoValue->info;
}

ListInfo popBack(const List list){	
	List NoValue = NULL;
	if(!isEmpty(list)){
		NodeType *ptr = list->prev;
		ListInfo value = ptr->info;
		list->prev = list->prev->prev;
		list->prev->next = list;
		free(ptr);
		return value;
	}
	return NoValue->info;
}

bool findInList(const List list, ListInfo value){
	List newList = list->next;
	while (newList != list){
		if(newList->info.pid != value.pid) {
			return true;
		}
		newList = newList->next;	
	}
	return false;
}

ListInfo getFront(const List list){
	List nullPtr = NULL;
		
	if(!isEmpty(list))
		return list->next->info;
	return nullPtr->info;
}

ListInfo getBack(const List list){
	List nullPtr = NULL;

	if(!isEmpty(list))
		return list->prev->info;
	return nullPtr->info;
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