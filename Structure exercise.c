#include <stdio.h>
#include <stdlib.h>

struct myStructure{
	int myNum;
	char myLetter;
};

int main(void){
	struct myStructure example;
	
	example.myNum=110;
	example.myLetter='A';
	printf("My number=%d\n", example.myNum);
	printf("My letter=%c\n", example.myLetter);
	
	return 0;
}
