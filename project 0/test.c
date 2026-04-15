#include <stdio.h>

void main(){
	int anArray[]={5,16,73,86};
	int* p=anArray;
	printf("*p=%d\n",*p);
	p++;
	printf("Now *p=%d\n",*p);
	int* q=&anArray[3];
	printf("*q=%d\n",*q);
	int** x=&q;
	**x=12;
	printf("%d\n",anArray[3]);
	*x=p;
	**x=10000;
	printf("%d\n",anArray[1]);
	q[1]=9;
	for (int i=0;i<4;i++){
		printf("anArray %d: %d\n",i,anArray[i]);
	}
}
