#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#define BUFFER_SIZE 1024

int find(char *FILENAME, char *PATH);

int main(void){
    char *prompt = "20192209>";
    char buffer[100];
    

    while(1)
    {
        int i = 0;
        char *input[3] = {NULL, };

        fputs(prompt, stdout);
        gets(buffer);
        char *result = strtok(buffer, " ");

        for(i = 0 ; result != NULL ; i++)
        {
            if(i == 3){ // 4개 이상 인자 입력했을 때, 오류 출력
                printf("인자를 4개이상 입력했습니다.\n");
                break;
            }
            input[i] = (char*)malloc(strlen(result) + 1);
            strcpy(input[i], result);
            result = strtok(NULL, " ");
        }

        printf("%s\n", input[0]);
        printf("%s\n", input[1]);
        printf("%s\n", input[2]);

        //if (strcmp(input[0], "find") == 0){
        //    find(input[1], input[2]);
        //    continue;
        //}

        for(int i = 0; i < 3; i++)   // 할당받은 메모리(3개)를 반환시켜 준다.
            free(input[i]);

        printf("%s\n", input[0]);
        printf("%s\n", input[1]);
        printf("%s\n", input[2]);
    }
}
