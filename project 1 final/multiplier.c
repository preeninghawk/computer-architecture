#include <stdio.h>
#include <stdlib.h>

void num_to_bits(unsigned int x){
	int i=0;
	int len=sizeof(int)*8;
	char lst[31];
	for (i=len-1; i>=0; i--){
		char bit = x&(1u<<i)? '1':'0';
		putchar(bit);
		lst[i]=bit;
		bit=0;
	}
	printf("\n");
	int* add=lst;
	return &add;
}

void main(){
	/*int a=8;
	printf("%d in binary: ", a);
	num_to_bits(a);*/
	
	int cluster[]={1,7,2,3,4};
	int* x=cluster;
	for (int i=4;i>=0;i--){
		printf("%p\n", *x);
		x++;
	}
}
