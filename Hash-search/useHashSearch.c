#include <stdbool.h>

void useHashSearch()
{
    insert_customer();
   	inithashtable();
   	insert_hashtable();

    while (true)
    {
        menu();
        keydown();
    }
}
