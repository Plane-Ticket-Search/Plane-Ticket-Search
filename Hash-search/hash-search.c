#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define P 50
#define SIZE 50
//�����ϣ�����ĳ����͹�ϣ��

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
//����ͻ��������Ӧ�ı��

CL clist[50];
//����ͻ���Ϣ��
int cn = 0;
//����ͻ�����

typedef struct reserve_customer
{
    char cname[10];
    int level;
}RC;
//���庽����Ϣ�е�Ԥ���ͻ�

typedef struct houbu_customer
{   
    char cname[10];
}HC;
//���庽����Ϣ�к򲹿ͻ�

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
//���庽����Ϣ�ṹ

typedef struct Hash_struct
{
    int flag;//�ж��Ƿ�Ϊ�� 
    HNS value[30];//�˿����еĺ�����Ϣ 
    int fnum;//�˿͵ĺ�����Ϣ���� 
    char cname[30];//�˿������ڲ��ҵ�ʱ���ϣֵ��ͬ��������ж��Ƿ�Ϊ����˿� 
}HS;

HS hashtable[SIZE];

void ReadFlightIn(int cnn,int hsk)
{
    FILE *fp;
    char fname[20];
    //������Ϣ�ļ�
    FILE *fp2;
    char fname2[20];
    //��Ʊ�ͻ��ļ�
    FILE *fp3;
    char fname3[20];
    //�򲹿ͻ��ļ�
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
        &hashtable[hsk].value[fnum].rclist[rcnumber].level) != EOF)//��ȡ������Ϣ�ļ�
        {
			rcnumber++;
        }
        hashtable[hsk].value[fnum].rcnumber=rcnumber;
        fclose(fp2);
        //��ȡ��Ʊ�ͻ��ļ�
        
        sprintf(fname3,"%sHC.txt",hashtable[hsk].value[fnum].fly_num);
        fp3 = fopen(fname3,"rt");
        
        while(fscanf(fp3,"%s",hashtable[hsk].value[fnum].hclist[hcnumber].cname) != EOF)
        {
			hcnumber++;
        }
        hashtable[hsk].value[fnum].hcnumber=hcnumber;
        fclose(fp3);
         //��ȡ�򲹿ͻ��ļ�
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
//��ʼ����ϣ��

void insert_hashtable(void)//�����ϣֵ�������ϣ����ȥ 
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

void print_customer_information(int fnum,int has)//��ӡ�ͻ���Ϣ 
{
    int i;
    int y;
    int k;
    k=0;
    y=0;
    i=0;
    for(;i<fnum;i++)
    {
        printf("\n������\tĿ�ĵ�\t�����\t����ʱ��\tԤ���˿�����\t������"); 
        printf("\n%s\t%s\t%s\t%s\t\t%d\t\t%d",hashtable[has].value[i].star_station,
        hashtable[has].value[i].trminal_station,
        hashtable[has].value[i].fly_num,
        hashtable[has].value[i].fly_duration,
        hashtable[has].value[i].rcnumber,
        hashtable[has].value[i].hcnumber);
		printf("\n�˿���\t�˿͵ȼ�");
        for (y=0;y<hashtable[has].value[i].rcnumber; y++)
        {
			printf("\n%s\t%d",hashtable[has].value[i].rclist[y].cname,
            hashtable[has].value[i].rclist[y].level);
        }
        printf("\n�򲹳˿���");
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
    int xhc;//ѭ����ʱ 
    int zh=0;//ֵ 
    int cdd;//�ַ������� 
    printf("����������Ҫ�ҵĿͻ�������\n");
    scanf("%s",name);
    
  	cdd=strlen(name);
  	for(xhc=0;xhc<cdd;xhc++)
  	{
	  	zh=name[xhc]+zh;
	}
	zh=abs(zh);//ͨ�������ַ���ֵ�����й�ϣֵ���� 
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
    	printf("�ͻ���Ϣ������"); 	
     }
 }
//���ҿͻ���Ϣ

void insert_customer(void)
{
    FILE *fp;
    fp = fopen("customer.txt","rt");
    while(fscanf(fp,"%s",clist[cn].cname) != EOF)
    {
		cn++;
   	} 
	fclose(fp);
    //��ȡ�ͻ���Ϣ��
}

