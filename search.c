#include "search.h"

void useHashSearch()
{
  insert_customer();
  inithashtable();
  insert_hashtable();
  int flag = 1;
  while (flag)
  {
    menu();
    flag = keydown();
  }
}

int useBPtreeSearch()
{
  struct BPTree *tree = bptree_init();
  use_bptree_search(tree);
  bptree_destroy(tree);
  return 0;
}

int useTicketSystem()
{
  int choice;
  int break_flag = 0;
  while (1)
  {
    printf("\n****************************************\n\n");
    printf("欢迎使用航班客票查询系统\n");
    printf("我们提供以下查询方式：\n");
    printf("1. 根据航班号和飞行时间查询航班信息\n");
    printf("2. 根据始发站和终点站查询航班信息\n");
    printf("3. 查询客户信息\n");
    printf("4. 退出查询程序\n");
    printf("请输入对应序号使用对应功能：");
    scanf("%d", &choice);
    printf("\n\n****************************************\n\n");

    switch (choice)
    {
    case 1:
      useBPtreeSearch();
      break;
    case 2:
      useBtreeSearch();
      break;
    case 3:
      useHashSearch();
      break;
    case 4:
      break_flag = 1;
      break;
    default:
      break;
    }

    if (break_flag)
    {
      return 0;
    }
  }
}

int main()
{
  useTicketSystem();
  return 0;
}