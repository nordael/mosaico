
#include <stdlib.h>

typedef struct node{
	void *item;
	struct node * next;
	struct node * prev;
} TNode;

typedef struct list {

	TNode * begin;
	TNode * end;
	unsigned int length;
} TList;

/* returns a pointer to a list */
TList * newList();

/*inserts an item to the list of specified size 
	returns the address of the item inserted
*/
void* addItem( TList *, void *, size_t );

/*remove a specified item from the list using a comparison function defined by the user */
int removeItem ( TList *, void*, int (*fptr)( void*, void* ) );

/* removes an item of a specified size from the beginig of the list */
int _dequeue ( TList *, void* , size_t );

/* add an item of a specified size in end of the list 
	
*/
void* _enqueue ( TList *, void* , size_t );

/*inserts an item to the list of specified size in the end of the list 
	returns the address of the item inserted
*/
void* _push ( TList *, void* , size_t );

/*removes an item of specified size from end of the list  */
int _pop ( TList *, void* , size_t );

/*tests if empty list */
int isEmpty( TList * );

/* returns the length of the list */
int length( TList * );

/* erase all data from the list */
void deleteList( TList * );

/*perform a search into the list to find an item using a comparison function defined by user */
void* findItem( TList *, void *, int (*fptr)( void*, void* ) );
