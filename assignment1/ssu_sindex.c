#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

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
            input[i] = result;
            result = strtok(NULL, " ");
        }

        if (strcmp(input[0], "find") == 0){
            find(input[1], input[2]);
            continue;
        }
    }
}
int find(char *FILENAME, char *PATH){
   
//    if(FILENAME == NULL || PATH == NULL)
//    {
//        printf("ERROR 2개의 인자를 입력해야 합니다\n");
//        return 0;
//    }
//
//    if(fopen("test1.txt", "w") == NULL|| fopen("test2.txt", "w") == NULL)
//    {
//        printf("파일이 존재하지 않습니다\n");
//        return 0;
//    }
//
//    printf("%s\n%s", FILENAME, PATH);
//
    return 0;
}
