#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main()
{
	char buffer[200];
	int i;
	int N=200000;
	int orderkey,custkey;
	char orderstatus;
	float totalprice;
	char clerk[18],comments[82];
	int j;
	FILE *fp=fopen("D:\\test.txt","w");

	srand(time(0));
	for (i=0;i<N;i++)
	{
		orderkey = i;
		custkey = 2000000 - i*10;
		orderstatus = rand()%26+65;
		totalprice  = rand()%20000 / 100.0;
		clerk[0]='\'';
		for (j=1;j<rand()%16+1;j++)
			{clerk[j]=rand()%26+65;}
		clerk[j]='\'';
		clerk[j+1]=0;
		comments[0] = '\'';
		for (j=1;j<rand()%80+1;j++)
			{comments[j]=rand()%26+65;}
		comments[j]='\'';
		comments[j+1]=0;
		sprintf(buffer,"insert into orders values(%d,%d,%c,%.3f,%s,%s);",orderkey,custkey,orderstatus,totalprice,clerk,comments);
		fprintf(fp,"%s\n",buffer);
	}
}
