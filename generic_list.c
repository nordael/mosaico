
#include "generic_list.h"
#include <string.h>


TList * newList(){
	TList *aNewList;

	aNewList = (TList *) malloc( sizeof(TList) );
	aNewList->begin = NULL;
	aNewList->end = NULL;
	aNewList->length = 0;

	return aNewList;
}


void* addItem( TList *thisList, void * anItem, size_t size ){
	TNode *newNode;

	newNode = malloc( sizeof(TNode) );
	
	if ( newNode == NULL )
		return NULL; //0

	newNode->next = NULL;
	newNode->item = malloc( size ); 
	
	if (! memmove( newNode->item, anItem, size ) )
			return NULL;;

	if( thisList->begin == NULL ) {
		newNode->prev = NULL;
		thisList->begin = newNode;
	}else{
		newNode->prev = thisList->end;
		thisList->end->next = newNode;
	}

	thisList->end = newNode;
	thisList->length++;
	
	return newNode->item;
}

int _dequeue ( TList * thisList, void* anItem, size_t size ){

	TNode *oldNode;
	
	if( isEmpty(thisList) )
		return 0;

	oldNode = thisList->begin;
	if ( !memcpy( anItem, oldNode->item, size ) )
		return 0;

	thisList->begin = oldNode->next;
	if ( thisList->begin )	
		thisList->begin->prev = NULL;
	
	free(oldNode->item);
	free(oldNode);
	thisList->length--;
    return 1;
}

void* _enqueue ( TList * thisList, void* anItem, size_t size ){
	
	TNode * newNode;
	newNode = malloc( sizeof(TNode) );

	if ( newNode == NULL )
		return NULL; //0

	newNode->item = malloc( size );

	newNode->next = NULL;
	newNode->prev = thisList->end;

	if( !memcpy( newNode->item, anItem, size ) )
		return NULL; //0
	
	if( thisList->begin == NULL ){
		thisList->begin = newNode;
	}else{
		thisList->end->next = newNode;
	}

	thisList->end = newNode;
	thisList->length++;

	return newNode->item;

}

void* _push ( TList *thisList, void* anItem , size_t size ){
	
	TNode *newNode;

	newNode = malloc( sizeof(TNode) );
	
	if ( newNode == NULL )
		return NULL; //0

	newNode->item = malloc(size);

	newNode->next = NULL;
	newNode->prev = thisList->end;

	if( !memcpy( newNode->item, anItem, size ))
		return NULL; //0

	if ( thisList->begin == NULL ){
		thisList->begin = newNode;
	}else{
		thisList->end->next = newNode;
	}
	
	thisList->end = newNode;
	thisList->length++;

	return newNode->item;

}

int _pop ( TList *thisList, void* anItem , size_t size ){

	TNode *oldNode;

	if( isEmpty( thisList ) )
		return 0;

	oldNode = thisList->end;
	
	if( !memcpy( anItem, oldNode->item, size ))
		return 0;
	
	thisList->end = oldNode->prev;
	
	if( thisList->end != NULL )
		thisList->end->next = NULL;
	
	thisList->length--;

	free(oldNode->item);
	free(oldNode);	

	return 1;
}

int isEmpty( TList * thisList ){
	return thisList->length == 0;
}

int length( TList *thisList ){
	return thisList->length;
}

void deleteList( TList *thisList ){
	TNode *old;

	old = thisList->begin;
	while( old ){
		thisList->begin = old->next;
		free(old->item);
		free(old);
		old = thisList->begin;
	}
	thisList->end = NULL;
	thisList->length = 0;
	free( thisList );
}


void* findItem( TList *thisList, void *item, int (*fptr)( void*, void* ) ){
	TNode *_this;
	_this = thisList->begin;

	int found = 0;
	while( _this && !found ){
		found = (*fptr)( item, _this->item );
		
		if( found )
			break;

		_this = _this->next;
	}

	return _this->item;
}

static TNode* findNode( TList *thisList, void * item, int (*fptr)( void*, void* ) ){
	TNode *_this;
	_this = thisList->begin;

	int found = 0;
	while( _this && !found ){
		found = (*fptr)( item, _this->item );
		
		if( found )
			break;

		_this = _this->next;
	}

	return _this;
}

int removeItem( TList *thisList, void* item, int(*fptr)( void*, void*) ){
	
	TNode *_old, *_prev, *_next;
	
	_old = findNode( thisList, item, fptr );

	if( !_old )
		return 0;
	
	_prev = _old->prev;
	_next = _old->next;

	if( _prev )
		_old->prev->next = _old->next;
	else
		thisList->begin = _old->next;

	if( _next )
		_old->next->prev = _old->prev;

	free(_old->item );
	free(_old );

	return 1;
}