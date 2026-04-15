#include <stdio.h>
#include <string.h>

int parse_str_to_list(const char* string, char** token_list[]) {
    char str[strlen(string) + 1];
    strcpy(str, string); 
    const char* delimiter="/\n";
    char* ptr = strtok(str, delimiter); 
    int i = 0;

    while (ptr != NULL) {
       
        token_list[i] = ptr; 
        printf("%s\n", token_list[i]); 
        
        ptr = strtok(NULL, delimiter);
        i++;
    }
    return i;
}




int main(void) {
    char** token_list[100];  
    char* example="/home/user/text.txt";
    int num_of_tokens=parse_str_to_list(example, token_list);
    printf("Number of tokens: %d\n",num_of_tokens);
    for (int i=0;i<num_of_tokens;i++){
		printf("%s.\n",token_list[i]);
	}
    
    return 0;
}

