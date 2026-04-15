#include <stdio.h>
#include <string.h>

int str_to_token(char* string){
	
	char s[strlen(string) + 1];
    strcpy(s, string);
    printf("%s",s);
	char d[]=" ";
	char* token_list[100];
	int i=0;
	
	
	char* portion=strtok(s,d);
	while (portion != NULL){
		printf("%s\n", portion);
		token_list[i]=portion;
		i++;
		portion=strtok(NULL,d);
	}
	
	for (int j=0;j<i;j++){
		printf("Token: %s\n",token_list[j]);
	}
	
	return i;
}

void main(){
	char sample[]="roses are red violets are blue";

	int output=str_to_token(sample);
	for (int i=0;i<5;i++){
		printf("%c\n",*(ptr+i));
	}
	printf("%d\n", output);
}

