#include <stdbool.h>

void useHashSearch()
{
    insert_customer();
   	inithashtable();
   	insert_hashtable();

    int flag = 1;
    while (flag != 0)
    {
        menu();
        flag = keydown();
    }
}
