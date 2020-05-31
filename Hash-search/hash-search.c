#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define P 50
#define SIZE 50
//定义哈希函数的除数和哈希表长

void ReadFlightIn(int cnn,int hsk);
void inithashtable(void);
void insert_hashtable(void);
void print_customer_information(int fnum,int has);
void search_cnumber(void);
void insert_customer(void);

typedef struct customer
{
    char cname[30];
}CL;
//定义客户姓名与对应的编号

CL clist[50];
//定义客户信息表长
int cn = 0;
//定义客户数量

typedef struct reserve_customer
{
    char cname[10];
    int level;
}RC;
//定义航班信息中的预定客户

typedef struct houbu_customer
{   
    char cname[10];
}HC;
//定义航班信息中候补客户

typedef struct HashNode_Struct
{
    int Hkey;
    char star_station[10];
    char trminal_station[10];
    char fly_num[10];
    char fly_duration[10];
    RC rclist[100];
    HC hclist[100];
    int rcnumber;
    int hcnumber;
} HNS;
//定义航班信息结构

typedef struct Hash_struct
{
    int flag;//判断是否为空 
    HNS value[30];//乘客所有的航班信息 
    int fnum;//乘客的航班信息总数 
    char cname[30];//乘客名，在查找的时候哈希值相同的情况下判断是否为这个乘客 
}HS;

HS hashtable[SIZE];

void ReadFlightIn(int cnn,int hsk)
{
    FILE *fp;
    char fname[20];
    //航班信息文件
    FILE *fp2;
    char fname2[20];
    //订票客户文件
    FILE *fp3;
    char fname3[20];
    //候补客户文件
    int rcnumber=0;
    int hcnumber=0;
    
    int fnum = 0;
	
    sprintf(fname,"%s.txt",clist[cnn].cname);
    fp = fopen(fname,"rt");
    while(fscanf(fp,"%s %s  %s  %s",hashtable[hsk].value[fnum].star_station,
    hashtable[hsk].value[fnum].trminal_station,hashtable[hsk].value[fnum].fly_num,
    hashtable[hsk].value[fnum].fly_duration) != EOF)
    {
        rcnumber = 0;
        hcnumber = 0;
		sprintf(fname2,"%sRC.txt",hashtable[hsk].value[fnum].fly_num);
        
        fp2 = fopen(fname2,"rt");
        
		while(fscanf(fp2,"%s    %d",hashtable[hsk].value[fnum].rclist[rcnumber].cname,
        &hashtable[hsk].value[fnum].rclist[rcnumber].level) != EOF)//读取航班信息文件
        {
			rcnumber++;
        }
        hashtable[hsk].value[fnum].rcnumber=rcnumber;
        fclose(fp2);
        //读取订票客户文件
        
        sprintf(fname3,"%sHC.txt",hashtable[hsk].value[fnum].fly_num);
        fp3 = fopen(fname3,"rt");
        
        while(fscanf(fp3,"%s",hashtable[hsk].value[fnum].hclist[hcnumber].cname) != EOF)
        {
			hcnumber++;
        }
        hashtable[hsk].value[fnum].hcnumber=hcnumber;
        fclose(fp3);
         //读取候补客户文件
        fnum++;
    }
    hashtable[hsk].fnum=fnum;
    fclose(fp);
}

void inithashtable(void)
{
    int i = 0;
    for(;i<SIZE;i++)
    {
        hashtable[i].flag=0;
    }
}
//初始化哈希表

void insert_hashtable(void)//计算哈希值并插入哈希表中去 
{
    int cnn;
    int hasaka;
    int changdu;
    int xhcs;
    int zhi;
    cnn=0;
    zhi=0;
    for(;cnn<cn;cnn++)
    {
		changdu=strlen(clist[cnn].cname);
        for(xhcs=0;xhcs<changdu;xhcs++)
        {
        	zhi=clist[cnn].cname[xhcs]+zhi;
        }
        zhi=abs(zhi);
        hasaka=zhi%P;
        if(hashtable[hasaka].flag==0)
        {
			ReadFlightIn(cnn,hasaka);
            hashtable[hasaka].flag=1;
            strcpy(hashtable[hasaka].cname,clist[cnn].cname);
        }
        else
        {
            for(;hasaka<SIZE;hasaka++)
            {
                if(hashtable[hasaka].flag==0)
                {
                    ReadFlightIn(cnn,hasaka);
                    hashtable[hasaka].flag=1;
                    strcpy(hashtable[hasaka].cname,clist[cnn].cname);
                }
            }
        }
        
    }
}

void print_customer_information(int fnum,int has)//打印客户信息 
{
    int i;
    int y;
    int k;
    k=0;
    y=0;
    i=0;
    for(;i<fnum;i++)
    {
        printf("\n出发地\t目的地\t航班号\t飞行时间\t预定乘客人数\t候补人数"); 
        printf("\n%s\t%s\t%s\t%s\t\t%d\t\t%d",hashtable[has].value[i].star_station,
        hashtable[has].value[i].trminal_station,
        hashtable[has].value[i].fly_num,
        hashtable[has].value[i].fly_duration,
        hashtable[has].value[i].rcnumber,
        hashtable[has].value[i].hcnumber);
		printf("\n乘客名\t乘客等级");
        for (y=0;y<hashtable[has].value[i].rcnumber; y++)
        {
			printf("\n%s\t%d",hashtable[has].value[i].rclist[y].cname,
            hashtable[has].value[i].rclist[y].level);
        }
        printf("\n候补乘客名");
        for (k=0;k<hashtable[has].value[i].hcnumber; k++)
        {
			printf("\n%s",hashtable[has].value[i].hclist[k].cname);
        }
    }
}

void search_cnumber(void)
{
    char name[30];
    int flag=0;
    int HK;
    int xhc;//循环此时 
    int zh=0;//值 
    int cdd;//字符串长度 
    printf("请输入你所要找的客户姓名：\n");
    scanf("%s",name);
    
  	cdd=strlen(name);
  	for(xhc=0;xhc<cdd;xhc++)
  	{
	  	zh=name[xhc]+zh;
	}
	zh=abs(zh);//通过计算字符串值来进行哈希值计算 
	HK=zh%P;
	if(strcmp(name,hashtable[HK].cname)==0)
        {
			print_customer_information(hashtable[HK].fnum,HK);
			flag=1;
        }
        else
        {
			for(;HK<SIZE;HK++)
            {
				if(strcmp(name,hashtable[HK].cname)==0)
                {
					print_customer_information(hashtable[HK].fnum,HK);
					flag=1;
					break;
     			}
            }
        }
     if(flag==0)
	{
    	printf("客户信息不存在"); 	
     }
 }
//查找客户信息

void insert_customer(void)
{
    FILE *fp;
    fp = fopen("customer.txt","rt");
    while(fscanf(fp,"%s",clist[cn].cname) != EOF)
    {
		cn++;
   	} 
	fclose(fp);
    //读取客户信息表
}

