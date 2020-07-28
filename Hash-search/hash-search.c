#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/*
首先需要四个类型的txt文件，分别是customer.txt(这个是总的客户名单）
客户名.txt（这个是每个客户的航班信息，以客户的姓名作为名字）
航班号HC.txt（这个是每个航班的候补客户名单，以航班号+HC作为名字）
航班号RC.txt（这个是每个航班的客户名单，以航班号+RC作为名字）
在main函数里要初始化客户表、哈希表、航班信息如下既写入insert_customer();
inithashtable();
insert_hashtable();语句，全都是void不用加任何东西
查询功能通过search_cnumber();即可实现，全都是void不用加任何东西
四种txt文件的内容另外说明 
*/

//定义哈希函数的除数
#define P 50
//定义哈希表长
#define SIZE 50 

void ReadFlightIn(int cnn, int hsk); 
void inithashtable(void); 
void insert_hashtable(void); 
void print_customer_information(int fnum, int has);
void search_cnumber(void); 
void insert_customer(void); 
void RriteFlightIn(void);

//定义客户姓名与对应的编号
typedef struct customer
{
    char cname[30]; //客户姓名
} CL;

CL clist[SIZE];//定义客户信息表长

int cn = 0;//定义客户数量

//定义航班信息中的预定客户
typedef struct reserve_customer
{
    char cname[10];
    int level;
} RC;

//定义航班信息中候补客户
typedef struct houbu_customer
{
    char cname[10];
} HC;

//定义航班信息结构
typedef struct HashNode_Struct
{
    int Hkey; //哈希值
    char star_station[10]; //出发站
    char trminal_station[10]; //终点站
    char fly_num[10]; //航班号
    char fly_duration[10]; //飞行时间
    RC rclist[100]; 
    char yupiao[10]; //余票量
	char chengyuandinge[10]; //成员定额
    HC hclist[100]; 
    int rcnumber; //预定客户数
    int hcnumber; //候补客户数
} HNS;

typedef struct Hash_struct
{
    int flag;       //判断是否为空(该数据节点是否已被占用)
    HNS value[30];  //乘客所有的航班信息
    int fnum;       //乘客的航班信息总数(取值0-29)
    char cname[30]; //乘客名，在查找的时候哈希值相同的情况下判断是否为这个乘客
} HS;
HS hashtable[SIZE];


//从文件读取客户航班信息
void ReadFlightIn(int cnn, int hsk) 
{
     //航班信息文件
    FILE *fp;
    char fname[20];
    //订票客户文件
    FILE *fp2;
    char fname2[20];
    //候补客户文件
    FILE *fp3;
    char fname3[20];
   
    int rcnumber = 0;
    int hcnumber = 0;
    int fnum = 0; //标识特定乘客的航班信息数(0-29)

    //读取（cnn，hsk）这个乘客的航班信息
    sprintf(fname, "./Hash-search/%s.txt", clist[cnn].cname);
    fp = fopen(fname, "rt");
    while (fscanf(fp, "%s   %s    %s    %s    %s    %s", hashtable[hsk].value[fnum].star_station,
                  hashtable[hsk].value[fnum].trminal_station, hashtable[hsk].value[fnum].fly_num,
                  hashtable[hsk].value[fnum].fly_duration,hashtable[hsk].value[fnum].chengyuandinge,
				  hashtable[hsk].value[fnum].yupiao) != EOF)
    {
        rcnumber = 0;
        hcnumber = 0;

        //读取订票客户文件
        sprintf(fname2, "./Hash-search/%sRC.txt", hashtable[hsk].value[fnum].fly_num);
        fp2 = fopen(fname2, "rt");
        while (fscanf(fp2, "%s    %d", hashtable[hsk].value[fnum].rclist[rcnumber].cname,
                      &hashtable[hsk].value[fnum].rclist[rcnumber].level) != EOF) //读取航班信息文件
        {
            rcnumber++; 
        }
        hashtable[hsk].value[fnum].rcnumber = rcnumber; //预定客户数
        fclose(fp2);
        
        //读取候补客户文件
        sprintf(fname3, "./Hash-search/%sHC.txt", hashtable[hsk].value[fnum].fly_num);
        fp3 = fopen(fname3, "rt");
        while (fscanf(fp3, "%s", hashtable[hsk].value[fnum].hclist[hcnumber].cname) != EOF)
        {
            hcnumber++; 
        }
        hashtable[hsk].value[fnum].hcnumber = hcnumber; //候补客户数
        fclose(fp3);
        
        fnum++; 
    }
    hashtable[hsk].fnum = fnum; //更新乘客航班信息总数
    fclose(fp);
}

//客户航班信息写入.txt文件
void WriteFlightIn(int cnn, int hsk)
{
    //航班客户文件
    FILE* fp = NULL;
    char cname[20];
    //航班信息文件
    FILE* fp1;
    char fname1[20];
    //订票客户文件
    FILE* fp2;
    char fname2[20];
    char cname2[20]; //订票客户姓名
    int level; //订票客户等级
    //候补客户文件
    FILE* fp3;
    char fname3[20];
    char cname3[20]; //候补客户姓名

    int fnum = 0;
    int rcnumber = 0;
    int hcnumber = 0;

    char star_station[10];
    char trminal_station[10];
    char fly_num[10];
    char fly_duration[10];
    char yupiao[10];
    char chengyuandinge[10];

    fp = fopen("./Hash-search/customer.txt", "w");
    if (fp != NULL)
    {
        printf("请输入航班客户姓名: \n");
        scnaf("%s", cname);
        fprintf(fp, "%s", cname);
        fclose(fp);

        printf("请输入%s的航班信息总数\n", cname);
        scanf("%d", &fnum);
        for (int i = 0; i < fnum; i++)
        {
            //写入航班信息文件
            printf("请输入出发站,终点站,航班号,飞行时间,成员定额,余票量\n");
            scanf("%s%s%s%s%s%s\n", star_station, trminal_station, fly_num, fly_duration, chengyuandinge, yupiao);
            sprintf(fname1, "./Hash-search/%s.txt", cname);
            fp1 = fopen(fname1, "w");
            fprintf(fp1, "%s %s  %s  %s  %s  %s", star_station,trminal_station, 
                    fly_num, fly_duration, chengyuandinge,yupiao);
            fclose(fp1);
        
            //写入订票客户文件
            printf("请输入预定客户数\n");
            scanf("%d", &rcnumber);
            for (int j = 0; j < rcnumber; j++)
            {
                printf("请输入订票客户姓名,客票等级\n");
                scanf("%s   %d", cname2, &level);
                sprintf(fname2, "./Hash-search/%sRC.txt", fly_num);
                fp2 = fopen(fname2, "w");
                fprintf(fp2, "%s    %d", cname2, level);      
                fclose(fp2);   
            }

            //写入候补客户文件
            printf("请输入候补客户数\n");
            scanf("%d", &hcnumber);
            for (int k = 0; k < hcnumber; k++)
            {
                printf("请输入候补客户姓名\n");
                scanf("%s", cname3);
                fprintf(fname3, "./Hash-search/%sHC.txt", fly_num);
                fp3 = fopen(fname3, "w");
                fprintf(fp3, "%s", cname3);
                fclose(fp3);   
            }
        }   
    }
}

//初始化哈希表
void inithashtable(void)
{
    int i = 0;
    for (; i < SIZE; i++)
    {
        hashtable[i].flag = 0;
    }
}

//计算哈希值并插入哈希表中去
void insert_hashtable(void) 
{
    int cnn;
    int hasaka;
    int changdu;
    int xhcs;
    int zhi;
    cnn = 0;
    zhi = 0;
    for (; cnn < cn; cnn++)
    {
        changdu = strlen(clist[cnn].cname); //计算字符串的长度,strlen的返回值为(int)cname的长度
        for (xhcs = 0; xhcs < changdu; xhcs++) 
        {
            zhi = clist[cnn].cname[xhcs] + zhi; //字符转整型
        } 
        zhi = abs(zhi); //求绝对值函数，包含在<stdlib.h>头文件里
        hasaka = zhi % P; //hasaka为最终哈希值
        if (hashtable[hasaka].flag == 0) //表示该表节点可存放客户信息
        {
			ReadFlightIn(cnn, hasaka); //从文件读取信息存储到哈希表里的对应节点        
            hashtable[hasaka].flag = 1; //flag=1表示该空间被占用(已有客户信息)
            strcpy(hashtable[hasaka].cname, clist[cnn].cname); //将该客户姓名复制给哈希表对应位置的客户姓名
        }
        //利用开放地址法（的线性探查法）解决哈希冲突
        else
        {
            for (; hasaka < SIZE; hasaka++)
            {
                if (hashtable[hasaka].flag == 0)
                {
                    ReadFlightIn(cnn, hasaka);
                    hashtable[hasaka].flag = 1;
                    strcpy(hashtable[hasaka].cname, clist[cnn].cname);
                }
            }
        }
    }
}

//打印客户信息
void print_customer_information(int fnum, int has) //fnum为乘客航班信息总数，has为哈希值
{
    int i;
    int y;
    int k;
    k = 0;
    y = 0;
    i = 0;
    for (; i < fnum; i++)
    {
        printf("\n出发地\t目的地\t航班号\t飞行时间\t成员定额\t余票量\t预定乘客人数\t候补人数");
        printf("\n%s\t%s\t%s\t%s\t\t%s\t\t%s\t%d\t\t%d", 
               hashtable[has].value[i].star_station,
               hashtable[has].value[i].trminal_station,
               hashtable[has].value[i].fly_num,
               hashtable[has].value[i].fly_duration,
               hashtable[has].value[i].chengyuandinge,
               hashtable[has].value[i].yupiao,
               hashtable[has].value[i].rcnumber,
               hashtable[has].value[i].hcnumber);
        printf("\n乘客名\t乘客等级");
        for (y = 0; y < hashtable[has].value[i].rcnumber; y++)
        {
            printf("\n%s\t%d", hashtable[has].value[i].rclist[y].cname,
                   hashtable[has].value[i].rclist[y].level);
        }
        printf("\n候补乘客姓名");
        for (k = 0; k < hashtable[has].value[i].hcnumber; k++)
        {
            printf("\n%s", hashtable[has].value[i].hclist[k].cname);
        }
    }
}

//查找客户信息
void search_cnumber(void)
{
    char name[30];
    int flag = 0;
    int HK;
    int xhc;    //循环此时
    int zh = 0; //值
    int cdd;    //字符串长度
    printf("请输入你要查找的客户姓名：\n");
    scanf("%s", name);

    cdd = strlen(name);
    for (xhc = 0; xhc < cdd; xhc++)
    {
        zh = name[xhc] + zh;
    }
    zh = abs(zh); //通过计算字符串值来进行哈希值计算
    HK = zh % P; //HK为哈希值
    if (strcmp(name, hashtable[HK].cname) == 0)
    {
        print_customer_information(hashtable[HK].fnum, HK);
        flag = 1;
    }
    else//冲突时继续线性探测
    {
        for (; HK < SIZE; HK++)
        {
            if (strcmp(name, hashtable[HK].cname) == 0)
            {
                print_customer_information(hashtable[HK].fnum, HK);
                flag = 1;
                break;
            }
        }
    }
    if (flag == 0)
    {
        printf("客户信息不存在");
    }
}

//读取客户信息表
void insert_customer(void)
{
    FILE *fp;
    fp = fopen("./Hash-search/customer.txt", "rt");
    while (fscanf(fp, "%s", clist[cn].cname) != EOF)
    {
        cn++;
    }
    fclose(fp);  
}



/*
int main()
{
    int choice =0;
    insert_customer();
   	inithashtable();
   	insert_hashtable();
    while (true)
    {
        printf("\n1.查询客户信息\n");
        printf("2.退出系统\n");
        printf("请输入你要的操作:\n");
        scanf("%d",&choice);
        switch(choice)
        {
            case 1 : search_cnumber();
                     break;
            case 2 : printf("谢谢使用!\n");
                     //system("pause");
                     //system("cls");
                     exit(0);
            default: printf("输入错误，请重输:\n");
                     break;
        }
    }
    return 0;
}
*/