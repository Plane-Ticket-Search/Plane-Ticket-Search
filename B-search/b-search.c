
#include <stdio.h>

#include <string.h>

#include <stdlib.h>

#include	<windows.h>

#include	<stdio.h>

#include	<conio.h>

#include	<malloc.h>

#include	<time.h>

#include    <string.h>

#include    <errno.h>

#define MAX_NAME_LEN 20//姓名长度

#define MAX_BKNAME_LEN 50

#define	bookinfomationfile "bookinfo.dat"

#define _CRT_SECURE_NO_WARNINGS

#define _CRT_SECURE_NO_DEPRECATE




typedef struct _btree_node_t

{

    int num;                      /*关键字个数*/

    int* key;                     /* 关键字：所占空间为(max+1) - 多出来的1个空间用于交换空间使用 */

    struct _btree_node_t** child;  /* 子结点：所占空间为（max+2）- 多出来的1个空间用于交换空间使用 */

    struct _btree_node_t* parent;  /* 父结点 */

    struct PlaneNode** Plane;

}btree_node_t;

typedef struct result {

    btree_node_t* pt;				  //指向找到的结点

    int i;					  //1<=i<=m,在结点中的关键字位序

    int tag;				  //1:查找成功，0:查找失败

}result;


typedef struct PlaneNode {

    int FTnumber;   //路径号

    char From[MAX_BKNAME_LEN];  //起始地点

    char To[MAX_NAME_LEN];      //终点

    char FlightNumber[MAX_NAME_LEN];                    //航班号

    char CrewNumber[MAX_NAME_LEN];						//总库存

    char FlightDate[MAX_NAME_LEN];				//出版年份

    char Planenumber[MAX_NAME_LEN];					//飞机号



}PlaneNode, * PlaneType;				//图书类型


typedef struct

{

    int max;                /* 单个结点最大关键字个数 - 阶m=max+1 */

    int min;                /* 单个结点最小关键字个数 min=（m/2）向上取整-1*/

    int sidx;               /* 分裂索引 = (max+1)/2 */

    btree_node_t* root;     /* B树根结点地址 */

}btree_t;



static int btree_merge(btree_t* btree, btree_node_t* node);

static int _btree_merge(btree_t* btree, btree_node_t* left, btree_node_t* right, int mid);

//节点创建

static btree_node_t* btree_creat_node(btree_t* btree)

{

    btree_node_t* node = NULL;

    node = (btree_node_t*)calloc(1, sizeof(btree_node_t));

    if (NULL == node) {

        fprintf(stderr, "[%s][%d] errmsg:[%d] %s\n", __FILE__, __LINE__, errno, strerror(errno));

        return NULL;

    }



    node->num = 0;  //结点关键字个数初始化为0



    /* 申请max+1个空间，当关键字个数到达max+1时执行分裂操作 */

    node->key = (int*)calloc(btree->max + 1, sizeof(int));

    if (NULL == node->key) {

        free(node), node = NULL;

        fprintf(stderr, "[%s][%d] errmsg:[%d] %s\n", __FILE__, __LINE__, errno, strerror(errno));

        return NULL;

    }



    /* 申请max+2个子节点空间，因为子节点每分裂一次，父节点多一个关键字，所以当父节点需要分裂时，正好是子节点个数为max+2个 */

    node->child = (btree_node_t**)calloc(btree->max + 2, sizeof(btree_node_t*));

    if (NULL == node->child) {

        free(node->key);

        free(node), node = NULL;

        fprintf(stderr, "[%s][%d] errmsg:[%d] %s\n", __FILE__, __LINE__, errno, strerror(errno));

        return NULL;

    }

    node->Plane = (PlaneNode**)calloc(btree->max + 1, sizeof(PlaneNode*));

    return node;

}





btree_t* btree_creat(int m)

{

    btree_t* btree = NULL;



    if (m < 3) { //阶数不能小于3阶

        fprintf(stderr, "[%s][%d] Parameter 'max' must geater than 2.\n", __FILE__, __LINE__);

        return NULL;

    }



    btree = (btree_t*)calloc(1, sizeof(btree_t));

    if (NULL == btree) {

        fprintf(stderr, "[%s][%d] errmsg:[%d] %s!\n", __FILE__, __LINE__, errno, strerror(errno));

        return NULL;

    }



    btree->max = m - 1;

    btree->min = m / 2;

    if (0 != m % 2) {

        btree->min++;

    }

    btree->min--;  //min=（m/2）向上取整-1

    btree->sidx = m / 2;  //分裂索引

    btree->root = NULL;



    return btree;

}



//分裂结点

static int btree_split(btree_t* btree, btree_node_t* node)

{

    int idx = 0, total = 0, sidx = btree->sidx;

    btree_node_t* parent = NULL, * newNode = NULL;





    while (node->num > btree->max) {

        total = node->num;



        newNode = btree_creat_node(btree);  //创建一个新结点用来放索引sidx+1开始的关键字

        if (NULL == newNode) {

            fprintf(stderr, "[%s][%d] Create node failed!\n", __FILE__, __LINE__);

            return -1;

        }





        memcpy(newNode->key, node->key + sidx + 1, (total - sidx - 1) * sizeof(int));  //把sidx之后的关键字放入新结点中

        memcpy(newNode->child, node->child + sidx + 1, (total - sidx) * sizeof(btree_node_t*));  //把child[sidx]之后的子结点放到newNode->child中

        memcpy(newNode->Plane, node->Plane + sidx + 1, (total - sidx - 1) * sizeof(PlaneNode*));





        newNode->num = (total - sidx - 1);  //计算新结点的关键字个数

        newNode->parent = node->parent;    //将node和newNode作为原node父节点的子结点



        node->num = sidx;



        parent = node->parent;

        if (NULL == parent) {   //如果node没有父节点



            parent = btree_creat_node(btree);  //新建一个结点作为父节点

            if (NULL == parent) {

                fprintf(stderr, "[%s][%d] Create root failed!", __FILE__, __LINE__);

                return -1;

            }



            btree->root = parent;   //更新根结点

            //把node,newNode作为新建父节点的子结点

            parent->child[0] = node;

            parent->child[1] = newNode;

            node->parent = parent;

            newNode->parent = parent;



            parent->key[0] = node->key[sidx];  //把分裂索引所在的关键字放到该父节点中

            parent->Plane[0] = node->Plane[sidx];

            parent->num++;

        }

        else {

            //如果node原本就有父节点，就直接把key[sidx]插入到父节点中

            for (idx = parent->num; idx > 0; idx--) {      //找到父节点中比key[sidx]大的最小关键字，把key[sidx]插到该关键字的前面

                if (node->key[sidx] < parent->key[idx - 1]) {   //在parent->key中每次找到一个比key[sidx]大的就再比较前一个

                    parent->key[idx] = parent->key[idx - 1];  //每找到一个，就把该关键字的值，和该关键字下方的右孩子结点向后移一位

                    parent->child[idx + 1] = parent->child[idx];

                    parent->Plane[idx] = parent->Plane[idx - 1];

                    continue;  //继续进行for循环

                }

                break;

                //直到找到比key[sidx]大的最小parent->key索引，

                //此时的parent->key[idx]表示node->key[sidx]要插入的地方,parent->child[idx+1]是newNode要插入的地方

            }



            parent->key[idx] = node->key[sidx];  //插入分裂关键字

            parent->child[idx + 1] = newNode;      //插入新结点

            parent->Plane[idx] = node->Plane[sidx];

            newNode->parent = parent;

            parent->num++;

        }



        //  memset(node->key+sidx, 0, (total - sidx) * sizeof(int));

        //  memset(node->child+sidx+1, 0, (total - sidx) * sizeof(btree_node_t *));



          /* Change node2's child->parent */

        //  for(idx = 0; idx <= newNode->num; idx++) {

        //      if(NULL != newNode->child[idx]) {

        //          newNode->child[idx]->parent = newNode;

        //      }

        //  }

        memset(node->key + sidx, 0, (total - sidx) * sizeof(int));

        memset(node->child + sidx + 1, 0, (total - sidx) * sizeof(btree_node_t*));

        memset(node->Plane + sidx, 0, (total - sidx) * sizeof(PlaneNode*));

        //将复制给newNode的子结点中的parent指向newNode

        for (idx = 0; idx <= newNode->num; idx++) {

            if (NULL != newNode->child[idx]) {

                newNode->child[idx]->parent = newNode;

            }

        }

        node = parent;

    }



    return 0;

}



static int _btree_insert(btree_t* btree, btree_node_t* node, int key, int idx,PlaneNode*plane)

//传入要插入关键字的结点，idx表示比自己大的结点的位置，也就是key要插入的位置

{

    int i = 0;





    for (i = node->num; i > idx; i--) {

        node->key[i] = node->key[i - 1];  //插入关键字
        node->Plane[i] = node->Plane[i - 1];

    }



    node->key[idx] = key;
    node->Plane[idx] = plane;

    node->num++;



    if (node->num > btree->max) {  //当该结点关键字个数大于最大关键字个数时，执行分裂操作

        return btree_split(btree, node);

    }



    return 0;

}





//插入操作

int btree_insert(btree_t* btree, int key,PlaneNode* plane)

{

    int idx = 0;  //key索引

    btree_node_t* node = btree->root;



    if (NULL == node) {  //如果为空树，就创建第一个结点

        node = btree_creat_node(btree);

        if (NULL == node) {

            fprintf(stderr, "[%s][%d] Create node failed!\n", __FILE__, __LINE__);

            return -1;

        }

        //第一个结点创建成果后，插入key

        node->num = 1;

        node->key[0] = key;

        node->parent = NULL;

        node->Plane[0] = plane;



        btree->root = node;

        return 0;

    }





    while (NULL != node) {

        for (idx = 0; idx < node->num; idx++) {

            if (key == node->key[idx]) {  //如果要插入的数据已经存在就不需要插入

               break;

            }

            else if (key < node->key[idx]) {  //找到第一个比自己大的关键字,没有的话就继续idx++

                break;

            }

        }

        //如果该结点存在子结点，此时的idx也表示子结点的索引

        //子结点索引从0开始

        //child[idx]<key[idx]<child[idx+1]

        if (NULL != node->child[idx]) {  //如果这个关键字有对应的子节点，就进入这个子节点，在这个子节点中执行上述操作

            node = node->child[idx];

        }

        else {

            break;  //直到找到一个比自己大的且没有子结点的关键字，执行插入操作，插到这个关键字的前面

        }

    }





    return _btree_insert(btree, node, key, idx,plane);  //执行插入操作


}

void search(btree_t* btree, int key)
{
    int idx = 0;

    int sign = 0;

    PlaneNode* B;

    btree_node_t* node = btree->root;
    while (NULL != node) {
        for (idx = 0; idx < node->num; idx++) {

            if (key == node->key[idx]) {  //如果要插入的数据已经存在就不需要插入
                sign = 1;

                B = node->Plane[idx];

                printf("|*************************************************航班基本信息*************************************************|\n");

                printf("|  FT号  |      始发地        |     目的地    |     航班号    |   乘员定额    |   飞行时间    |    飞机号     |\n");


                printf("|--------|--------------------|---------------|---------------|---------------|---------------|---------------|\n");

                printf("|  %-4d  |%-20s|%-15s|", B->FTnumber, B->From, B->To);

                printf("%-15s|%-15s|%-15s|%-15s|\n", B->FlightNumber, B->CrewNumber, B->FlightDate, B->Planenumber);



            }

            else if (key < node->key[idx]) {  //找到第一个比自己大的关键字,没有的话就继续idx++

                break;

            }

        }


        if (NULL != node->child[idx]) {  //如果这个关键字有对应的子节点，就进入这个子节点，在这个子节点中执行上述操作

            node = node->child[idx];

        }
        else if (sign == 0) {
            printf("...........................尚无此航班............................\n");
            printf("查询完成\n");

            break;
        }

        else {
            break;
        }



    }

}

static int _btree_merge(btree_t* btree, btree_node_t* left, btree_node_t* right, int mid)

{

    int m = 0;

    btree_node_t* parent = left->parent;



    left->key[left->num++] = parent->key[mid];



    memcpy(left->key + left->num, right->key, right->num * sizeof(int));

    memcpy(left->child + left->num, right->child, (right->num + 1) * sizeof(btree_node_t*));

    for (m = 0; m <= right->num; m++) {

        if (NULL != right->child[m]) {

            right->child[m]->parent = left;

        }

    }

    left->num += right->num;



    for (m = mid; m < parent->num - 1; m++) {

        parent->key[m] = parent->key[m + 1];

        parent->child[m + 1] = parent->child[m + 2];

    }



    parent->key[m] = 0;

    parent->child[m + 1] = NULL;

    parent->num--;

    free(right);



    /* Check */

    if (parent->num < btree->min) {

        return btree_merge(btree, parent);

    }



    return 0;

}



static int btree_merge(btree_t* btree, btree_node_t* node)

{

    int idx = 0, m = 0, mid = 0;

    btree_node_t* parent = node->parent, * right = NULL, * left = NULL;



    if (NULL == parent) {

        if (0 == node->num) {

            if (NULL != node->child[0]) {

                btree->root = node->child[0];

                node->child[0]->parent = NULL;

            }

            else {

                btree->root = NULL;

            }

            free(node);

        }

        return 0;

    }



    for (idx = 0; idx <= parent->num; idx++) {

        if (parent->child[idx] == node) {

            break;

        }

    }



    if (idx > parent->num) {

        fprintf(stderr, "[%s][%d] Didn't find node in parent's children array!\n", __FILE__, __LINE__);

        return -1;

    }



    else if (idx == parent->num) {

        mid = idx - 1;

        left = parent->child[mid];



        if ((node->num + left->num + 1) <= btree->max) {

            return _btree_merge(btree, left, node, mid);

        }





        for (m = node->num; m > 0; m--) {

            node->key[m] = node->key[m - 1];

            node->child[m + 1] = node->child[m];

        }

        node->child[1] = node->child[0];



        node->key[0] = parent->key[mid];

        node->num++;

        node->child[0] = left->child[left->num];

        if (NULL != left->child[left->num]) {

            left->child[left->num]->parent = node;

        }



        parent->key[mid] = left->key[left->num - 1];

        left->key[left->num - 1] = 0;

        left->child[left->num] = NULL;

        left->num--;

        return 0;

    }





    mid = idx;

    right = parent->child[mid + 1];



    if ((node->num + right->num + 1) <= btree->max) {

        return _btree_merge(btree, node, right, mid);

    }



    node->key[node->num++] = parent->key[mid];

    node->child[node->num] = right->child[0];

    if (NULL != right->child[0]) {

        right->child[0]->parent = node;

    }



    parent->key[mid] = right->key[0];

    for (m = 0; m < right->num; m++) {

        right->key[m] = right->key[m + 1];

        right->child[m] = right->child[m + 1];

    }

    right->child[m] = NULL;

    right->num--;

    return 0;

}



static int _btree_delete(btree_t* btree, btree_node_t* node, int idx)

{

    btree_node_t* orig = node, * child = node->child[idx];



    while (NULL != child) {

        node = child;

        child = node->child[child->num];

    }



    orig->key[idx] = node->key[node->num - 1];





    node->key[--node->num] = 0;

    if (node->num < btree->min) {

        return btree_merge(btree, node);

    }



    return 0;

}



int btree_delete(btree_t* btree, int key)

{

    int idx = 0;

    btree_node_t* node = btree->root;





    while (NULL != node) {

        for (idx = 0; idx < node->num; idx++) {

            if (key == node->key[idx]) {

                return _btree_delete(btree, node, idx);

            }

            else if (key < node->key[idx]) {

                break;

            }

        }



        node = node->child[idx];

    }



    return 0;

}



void Inorder(btree_node_t* root, int deep) {

    int i, j, k, a = 1;

    if (root != NULL)

    {

        if (deep) {

            printf("\n");

        }

        for (j = 0; j < deep; j++) {

            printf("---");

        }

        for (i = 0; i <= root->num; i++) {

            if (a) {

                printf("< %d | ", root->num);

                for (k = 0; k < root->num; k++) {

                    printf("%d ", root->key[k]);


                }

                a--;

                printf(">");

            }

            Inorder(root->child[i], deep + 1);

        }

        printf("\n");

    }

}

int ToCode(char fchar[20])
{

    int fn = 0;



    if (strcmp(fchar, "Beijing") == 0)
        fn = 10;
    else if (strcmp(fchar, "Shanghai") == 0)
        fn = 11;
    else if (strcmp(fchar, "Chengdu") == 0)
        fn = 12;
    else if (strcmp(fchar, "Shenzhen") == 0)
        fn = 13;
    else if (strcmp(fchar, "Xiamen") == 0)
        fn = 14;
    else if (strcmp(fchar, "Xian") == 0)
        fn = 15;
    else if (strcmp(fchar, "Guangzhou") == 0)
        fn = 16;
    else if (strcmp(fchar, "Chongqing") == 0)
        fn = 17;
    else if (strcmp(fchar, "Tianjin") == 0)
        fn = 18;
    else if (strcmp(fchar, "Taibei") == 0)
        fn = 19;
    else if (strcmp(fchar, "Tokyo") == 0)
        fn = 20;
    else if (strcmp(fchar, "Osaka") == 0)
        fn = 21;
    else if (strcmp(fchar, "Yokohama") == 0)
        fn = 23;
    else if (strcmp(fchar, "Sapporo") == 0)
        fn = 22;
    else if (strcmp(fchar, "Nagoya") == 0)
        fn = 24;
    else if (strcmp(fchar, "Kobe") == 0)
        fn = 25;
    else if (strcmp(fchar, "Newyork") == 0)
        fn = 30;
    else if (strcmp(fchar, "Washington") == 0)
        fn = 31;

    else fn = -1;



    return fn;
}

int useBtreeSearch()
{



  int choice1=0;

printf("请选择功能：1查询 2.插入");

  scanf("%d",&choice1);

  switch(choice1)

  {
  case 1:

      {
          btree_t *bt;

  bt = btree_creat(500); //创建一个M为100的B树

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
  while (!feof(fp) ) //17是文件中航班信息的数量，写在这里停止循环防止最后一项乱码
  {
    B = (PlaneType)malloc(sizeof(PlaneNode)); // 申请航班空间

    fscanf(fp, "%d %s %s %s %s %s %s", &B->FTnumber, B->From, B->To, &B->FlightNumber,

           &B->CrewNumber, &B->FlightDate, &B->Planenumber); // 读入航班数据



    btree_insert(bt, B->FTnumber, B);



    ++ha;
  }

  fclose(fp); //读取完成
          for (;;)
  {

    int out=0;
    printf("请输入始发地(首字母大写英文名)：\n");

    char fchar[20];

    char tchar[20];

    int tn, fn, num = 0;

    scanf("%s", &fchar);

    fn = ToCode(fchar);

    printf("请输入目的地(首字母大写英文名)：\n");

    scanf("%s", &tchar);

    tn = ToCode(tchar);


    num = 100 * fn + tn;



    search(bt, num);

     printf("1.继续查询  2.退出查询：\n");

     int choice;

     scanf("%d", &choice);

      switch (choice)
      {
    case 1:
        break;
    case 2:
        out=1;
        break;
     }
    if(out==1)
        break;
  } //这一部分是搜索的循环部分，ToCode函数把汉字转换为对应的数字，再通过数字拼接得到一个独一无二的标识码，用于搜索
      }break;

     case 2:
        {


	FILE* fp = NULL; // 文件指针

     char ftn[20],fp1[20],dp[20],fn[20],ct[20],pn[20];

     int fd;

    printf("请输入始发地：");

    scanf("%s",&fp1);

    printf("请输入目的地：");

    scanf("%s",&dp);

    printf("请输入航班号：");

    scanf("%s",&fn);

     printf("请输入机载人数：");

    scanf("%s",&ct);

     printf("请输入飞行时间：");

    scanf("%d",&fd);

     printf("请输入飞机编号：");

    scanf("%s",&pn);

	int tn1,fn1,num1;
	fn1 = ToCode(fp1);
	tn1 = ToCode(dp);
	num1=100*fn1+tn1;



    printf("%d",num1);
	char szAppendStr[100];

	sprintf(szAppendStr, "%d      %s       %s         %s        %s        %d        %s\n",num1,fp1,dp,fn,ct,fd,pn);



	errno_t eResult;



	// 以附加方式打开可读/写的文件, 如果没有此文件则会进行创建，然后以附加方式打开可读/写的文件

	eResult = fopen_s(&fp, "./B-search/bookinfo.dat", "a+");



	// 打开文件失败

	if (eResult != 0)

		exit(-1);



	// 将追加内容写入文件指针当前的位置

	fputs(szAppendStr, fp);



	// 最后不要忘了,关闭打开的文件~~~

	fclose(fp);

	return 0;
















 }break;


















  }



  return 0;
}
