#include <stdio.h>
#include <stdlib.h>

#include "bp-search.h"

typedef struct PlaneInfo
{
  char plane_num[MAX_LEN];
  char fly_num[MAX_LEN];
  char setoff_time[MAX_LEN];
  char start_station[MAX_LEN];
  char terminal_station[MAX_LEN];
  char fly_duration[MAX_LEN];
  int passenger_limit;
} PlaneInfo;

void __print__bptree(struct BPTreeNode *root);

struct BPTree
{
  int level;
  struct BPTreeNode *root;
};

extern void __print__bptree(struct BPTreeNode *root);

void main()
{
  use_bptree_search();
}
