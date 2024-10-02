#include "utils.h"

typedef struct LinkedListDB LinkedListDB;
typedef struct LinkedListIterator LinkedListIterator;

/*pointer to function for releasing stored data in LinkedListDB*/
typedef void (* linkedListfreeData)(void *);

LinkedListDB * create_linked_list_db();
bool add_data_to_linked_list(LinkedListDB * linkedListDB, void * data);
LinkedListIterator * init_linked_list_iteraror(LinkedListDB * linkedListDB);
void * get_next_data(LinkedListIterator * iterator);
void free_linked_list_db(LinkedListDB * linkedListDB, linkedListfreeData freeData);
void free_linked_list_iteraror(LinkedListIterator * linkedListIterator);
