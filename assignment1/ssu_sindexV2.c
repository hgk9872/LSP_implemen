#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <limits.h>
#include <dirent.h>
#include <libgen.h>

int find(char *FILENAME, char *PATH);
int printinfo(char *FILENAME, int index);
void ptime(time_t time);
void pmode(mode_t mode, int type);
int compare(const void *a, const void *b);
int search(char *FILENAME, char *PATH);
int count = 0;
long int total_size;
char *filelist[100];

int main(void){
    char *prompt = "20192209>";    // 프롬프트 출력
    char buffer[100];            // 프롬프트 쉘에 입력할 명령어 버퍼
    char *input[3] = {NULL, };     // 명령어에 들어가는 인자 선언
    struct timeval start, end;     // 프로그램 실행시간, 종료시간 측정

    gettimeofday(&start, NULL);  // 프로그램 실행시 시작시간 측정

    while(1)
    {
        int i = 0;

        /* 프롬프트 출력하고 명령어 입력받기 */
        fputs(prompt, stdout);
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strlen(buffer)-1] = '\0';
        if (buffer[0] == 0) continue;
        char *result = strtok(buffer, " ");

        for(i = 0 ; result != NULL ; i++)
        {
            if(i == 3){ // 4개 이상 인자 입력했을 때, 오류 출력
                printf("ERROR\n");
                break;
            }
            input[i] = result;
            result = strtok(NULL, " ");
        }
        
        /*find 명령어 입력받는 경우*/
        if ((strcmp(input[0], "find") == 0)){
            find(input[1], input[2]);
            memset(input[1], '\0', sizeof(input[0]));
            memset(input[2], '\0', sizeof(input[1]));
            continue;
        }

        /*exit 명령어 입력받는 경우*/
        if ((strcmp(input[0], "exit") == 0) && (i == 1))
        {
            printf("Prompt End\n");
            
            gettimeofday(&end, NULL); // 프로그램 종료시 실행시간 측정
            printf("Runtime: %u:%u(sec:usec)\n", end.tv_sec - start.tv_sec, end.tv_usec - start.tv_usec);
            exit(0);
        }

        /*help 명령어 + 그 외 기본 명령어가 아닌 명령어를 입력받는 경우*/
        printf("Usage:\n  > find [FILENAME] [PATH]\n     >> [INDEX] [OPTION ... ]\n  > help\n  > exit\n\n  [OPTION ... ]\n   q : report only when files differ\n   s : report when two files are the same\n   i : ignore case differences in file contents\n   r : recursively compare any subdirectories found\n");
    }
}

/* 파일명(디렉토리)와 path를 입력받아 동일한 파일목록 출력 */
int find(char *FILENAME, char *PATH)
{
    total_size = 0 ;

    if(fopen(FILENAME, "r") == NULL|| fopen(PATH, "r") == NULL)
    {
        printf("INSTRUCTION ERROR\n");
        return 0;
    }

    /* 파일 정보 출력 */
    printf("%-6s%-5s%-11s%-7s%-6s%-5s%-5s%-16s%-16s%-16s%s\n",
       "Index", "Size", "Mode", "Blocks", "Links", "UID", "GID", "Access", "Change", "Modify", "Path");
    if(printinfo(FILENAME, 0) < 0) {perror("stat");}

    count = 0;
    search(FILENAME, PATH);

    /* 동일한 파일이 없는 경우 (None) 출력 */
    if (count == 0)
    {
        printf("(None)\n");
        return -1;
    }

    /* 동일한 파일 있는 경우 배열에 추가하여 정렬 후 출력 */
    char pathlist[100][1024];

    for (int i = 0; i < count; i++)
        strcpy(pathlist[i], filelist[i]);

    qsort(pathlist, count, sizeof(pathlist[0]), compare);

    for (int i = 0; i < count; i++)
        printinfo(pathlist[i], i+1);

    for (int i = 0; i < count; i++)
        free(filelist[i]);


    /* 파일 사이즈 총 합계 출력 */
    printf("total size : %d\n", total_size);

    return 0;
}

/* 파일 정보 출력하는 함수 */
int printinfo(char *FILENAME, int index)
{
    struct stat st;
    int type;
    char buf[PATH_MAX];
    realpath(FILENAME, buf);

    if (stat(FILENAME, &st) == -1){
        return -1;}

    /* 정규파일, 디렉토리파일 구분 */
    if (S_ISREG(st.st_mode))
        type = 1;    // 정규파일인 경우 type = 1
    else
        type = 0;    // 디렉토리파일인 경우 type = 0

    printf("%-6d", index);
    printf("%-5lld", st.st_size);
    pmode(st.st_mode, type);
    printf("%-7d", st.st_blocks);
    printf("%-6d", st.st_nlink);
    printf("%-5d", st.st_uid);
    printf("%-5d", st.st_gid);
    ptime(st.st_atime);
    ptime(st.st_mtime);
    ptime(st.st_ctime);
    printf("%s\n", buf);

    return 0;

}

/* 파일 시간에 대한 정보 출력 */
void ptime(time_t time)
{
    struct tm* t;
    t = localtime(&time);

    printf("%02d-%02d-%02d %02d:%02d  ",
         t->tm_year - 100, t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min);
    
}

/* 파일 mode에 관한 정보 출력 */
void pmode(mode_t mode, int type)
{
    if (type == 1)  // 정규파일인 경우
        printf("-");
    else printf("d"); // 디렉토리파일인 경우
    printf("%s", (mode & S_IRUSR) ? "r" : "-");
    printf("%s", (mode & S_IWUSR) ? "w" : "-");
    printf("%s", (mode & S_IXUSR) ? "x" : "-");
    printf("%s", (mode & S_IRGRP) ? "r" : "-");
    printf("%s", (mode & S_IWGRP) ? "w" : "-");
    printf("%s", (mode & S_IXGRP) ? "x" : "-");
    printf("%s", (mode & S_IROTH) ? "r" : "-");
    printf("%s", (mode & S_IWOTH) ? "w" : "-");
    printf("%s ", (mode & S_IXOTH) ? "x" : "-");
}

/* 입력받은 파일(디렉토리)을 입력받은 path에서 재귀적으로 찾는 함수 */
int search(char *FILENAME, char *PATH)
{
    DIR *dir = NULL;
    struct dirent *file = NULL;
    struct stat ist;
    struct stat fst;
    char index_file[1024];
    char index_path[PATH_MAX];
    char file_path[PATH_MAX];
    int len, i;

    /* 입력된 path의 디렉토리로 들어감 */
    if ((dir = opendir(PATH)) == NULL) 
    {
        return -1;
    }

    /* 들어간 디렉토리에 존재하는 파일들 모두 읽음 */
    while ((file = readdir(dir)) != NULL)
    {

        /* ".", ".." 인 경우 제외 */
        if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
        {
            continue;
        }

        sprintf(index_file, "%s/%s", PATH, file->d_name); // 상대경로

        if(stat(index_file, &ist) == -1 || stat(FILENAME, &fst) == -1)
        {
            continue;
        }

        if(!(S_ISDIR(ist.st_mode)) && !(S_ISREG(ist.st_mode))) // 정규파일도 아니고, 디렉토리도 아닌 경우 작업 안함
        {
            continue;
        }

        realpath(index_file, index_path);  // 상대경로를 절대경로로 바꿈
        realpath(FILENAME, file_path);

        if (strcmp(index_path, file_path) == 0) // 찾는 파일(디렉토리)인 경우 비교 안함
        {
            continue;
        }

        /* 파일명이 같고, 크기가 같은 경우 전역변수로 선언한 배열에 추가*/
        if ((strcmp(file->d_name, basename(file_path)) == 0) && (ist.st_size == fst.st_size) )
        {
            len = strlen(index_path) + 1;
            char *tmp = malloc(sizeof(char)*(len));
            strcpy(tmp, index_path);
            filelist[count] = tmp;
            count++;
        }

        /* 파일이 디렉토리인 경우 내부를 재귀적으로 탐색 */
        if(S_ISDIR(ist.st_mode))
        {
            search(FILENAME, index_file);
        }

        /* 일반 파일인 경우 파일 크기 합산 */
        if (S_ISREG(ist.st_mode))
            total_size += fst.st_size;
    }

    closedir(dir);

    return 0;
}

/* 오름차순으로 정렬하는 함수 */
int compare(const void *a, const void *b)
{
    return strcmp((char *)a, (char *)b);
}
