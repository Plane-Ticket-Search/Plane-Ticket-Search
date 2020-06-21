#include "search.h"

int useHashSearch()
{
  int choice;
  int j = 0;
  insert_customer();
  inithashtable();
  insert_hashtable();
  while (true)
  {
    printf("\n1.查询客户信息\n");
    printf("2.退出系统\n");
    printf("请输入你要的操作:\n");
    scanf("%d", &choice);
    switch (choice)
    {
    case 1:
      search_cnumber();
      break;
    case 2:
      printf("谢谢使用!\n");
      break;
    default:
      printf("输入错误，请重输:\n");
    }
  }
  return 0;
}

int useBPtreeSearch()
{
  struct BPTree *tree = bptree_init();
  use_bptree_search(tree);
  bptree_destroy(tree);
  return 0;
}

int useBtreeSearch()
{

  btree_t *bt;

  bt = btree_creat(4); //创建一个M为4的B树

  FILE *fp;

  PlaneType B;

  errno_t err;

  //fopen_s(&fp, "bookinfo.dat", "r");

  if ((err = fopen_s(&fp, "./B-search/bookinfo.dat", "r")) != 0) //从dat文件读取航班信息

  {

    printf("不能打开航班信息文件,请确认%s文件已放到盘根目录...", bookinfomationfile);

    getchar();

    return 1;
  }
  int ha = 0;
  while (!feof(fp) && ha < 17) //17是文件中航班信息的数量，写在这里停止循环防止最后一项乱码
  {
    B = (PlaneType)malloc(sizeof(PlaneNode)); // 申请航班空间

    fscanf(fp, "%d %s %s %d %d %d %s", &B->FTnumber, B->From, B->To, &B->FlightNumber,

           &B->CrewNumber, &B->FlightDate, &B->Planenumber); // 读入航班数据

    printf("插入航班%s", B->Planenumber);

    btree_insert(bt, B->FTnumber, B);

    Inorder(bt->root, 0);

    ++ha;
  }

  fclose(fp); //读取完成

  for (;;)
  {

    printf("请输入始发地：\n");

    char fchar[20];

    char tchar[20];

    int tn, fn, num = 0;

    scanf("%s", &fchar);

    fn = ToCode(fchar);

    printf("请输入目的地：\n");

    scanf("%s", &tchar);

    tn = ToCode(tchar);

    num = 100 * fn + tn;

    search(bt, num);

  } //这一部分是搜索的循环部分，ToCode函数把汉字转换为对应的数字，再通过数字拼接得到一个独一无二的标识码，用于搜索

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