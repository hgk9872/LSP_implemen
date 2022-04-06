#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#define BUFFER_SIZE 1024

int find(char *FILENAME, char *PATH);

int main(void){
    char *prompt = "20192209>";
    char inst[BUFFER_SIZE]; // 파일(디렉토리) 이름
    char filename[BUFFER_SIZE]; // 파일(디렉토리) 이름
    char path[BUFFER_SIZE]; // 디렉토리
    char buffer[BUFFER_SIZE];


    while(1)
    {
        int i = 0;
       
        fputs(prompt, stdout);
        gets(buffer);
        char *result = strtok(buffer, " ");
        if(result == NULL){
            continue;
        }

        strcpy(inst, result);
        result = strtok(NULL, " "); // 파일(디렉토리) 이름
        if(result == NULL){
            printf("명령어만 입력했을 때 에러 메시지");
            continue;
        }
        strcpy(filename, result);


        printf("%s\n", inst);
        printf("%s\n", filename);
        printf("%s\n", path);

    }
}
