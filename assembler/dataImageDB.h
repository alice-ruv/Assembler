#include "utils.h"
#include "assemblerDefinitions.h"

#ifndef DATA_IMAGE_INCLUDED
#define DATA_IMAGE_INCLUDED

typedef struct DataImageDB DataImageDB;
typedef struct DataImageIterator DataImageIterator;

DataImageDB * create_data_image_db();
bool add_string_to_db(DataImageDB * dataImageDB, const char * string, int size);
bool add_int_to_db(DataImageDB * dataImageDB, int number);
DataImageIterator * init_data_image_iterator(DataImageDB * dataImageDB);
DataWord * get_next_data_word(DataImageIterator * dataImageIterator);

void free_data_image_db(DataImageDB * dataImageDB);
void free_data_image_iteraror(DataImageIterator * dataImageIterator);
#endif
