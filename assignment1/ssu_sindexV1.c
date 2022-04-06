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
    char *prompt = "20192209>";
    char buffer[100];
    char *input[3] = {NULL, };
    struct timeval start, end;

    gettimeofday(&start, NULL);

    while(1)
    {
        int i = 0;
        

        fputs(prompt, stdout);
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strlen(buffer)-1] = '\0';
        if (buffer[0] == 0) continue;
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

//        printf("%s\n", input[0]);
//        printf("%s\n", input[1]);
//        printf("%s\n", input[2]);
        
        
        if ((strcmp(input[0], "find") == 0)){
            find(input[1], input[2]);
            memset(input[1], '\0', sizeof(input[0]));
            memset(input[2], '\0', sizeof(input[1]));
            continue;
        }

        if ((strcmp(input[0], "exit") == 0) && (i == 1))
        {
            printf("Prompt End\n");
            gettimeofday(&end, NULL);
            printf("Runtime: %u:%u(sec:usec)\n", end.tv_sec - start.tv_sec, end.tv_usec - start.tv_usec);
            exit(0);
        }

        printf("Usage:\n  > find [FILENAME] [PATH]\n     >> [INDEX] [OPTION ... ]\n  > help\n  > exit\n\n  [OPTION ... ]\n   q : report only when files differ\n   s : report when two files are the same\n   i : ignore case differences in file contents\n   r : recursively compare any subdirectories found\n");
    }
}

int find(char *FILENAME, char *PATH)
{
    total_size = 0 ;

    if(fopen(FILENAME, "r") == NULL|| fopen(PATH, "r") == NULL)
    {
        printf("INSTRUCTION ERROR\n");
        return 0;
    }

    printf("%-6s%-5s%-11s%-7s%-6s%-5s%-5s%-16s%-16s%-16s%s\n",
       "Index", "Size", "Mode", "Blocks", "Links", "UID", "GID", "Access", "Change", "Modify", "Path");
    if(printinfo(FILENAME, 0) < 0) {perror("stat");}

    count = 0;
    search(FILENAME, PATH);

    if (count == 0)
    {
        printf("(None)\n");
        return -1;
    }

    char pathlist[100][1024];

    for (int i = 0; i < count; i++)
        strcpy(pathlist[i], filelist[i]);

    qsort(pathlist, count, sizeof(pathlist[0]), compare);

    for (int i = 0; i < count; i++)
        printinfo(pathlist[i], i+1);

    for (int i = 0; i < count; i++)
        free(filelist[i]);

    printf("total size : %d\n", total_size);

    return 0;
}

int printinfo(char *FILENAME, int index)
{
    struct stat st;
    int type;
    char buf[PATH_MAX];
    realpath(FILENAME, buf);

    if (stat(FILENAME, &st) == -1){
        return -1;}

    if (S_ISREG(st.st_mode))
        type = 1;
    else
        type = 0;

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

void ptime(time_t time)
{
    struct tm* t;
    t = localtime(&time);

    printf("%02d-%02d-%02d %02d:%02d  ",
         t->tm_year - 100, t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min);
    
}

void pmode(mode_t mode, int type)
{
    if (type == 1)
        printf("-");
    else printf("d"); 
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

int search(char *FILENAME, char *PATH)
{
    DIR *dir = NULL;
    struct dirent *file = NULL;
    struct stat st;
    struct stat fst;
    char filename[1024];
    char buf[PATH_MAX];
    char buf2[PATH_MAX];
    char *buf3;
    char *buf4;
    char fname[256];
//    char *filelist[100];
    int len, i;


    if ((dir = opendir(PATH)) == NULL) 
    {
        return -1;
    }

    while ((file = readdir(dir)) != NULL)
    {
        if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
        {
            continue;
        }

        sprintf(filename, "%s/%s", PATH, file->d_name);

        if(stat(filename, &st) == -1 || stat(FILENAME, &fst) == -1)
        {
            continue;
        }

        if(!(S_ISDIR(st.st_mode)) && !(S_ISREG(st.st_mode)))
        {
            continue;
        }

        realpath(filename, buf);
        realpath(FILENAME, buf2);

        if (strcmp(buf, buf2) == 0)
        {
            continue;
        }

        if ((strcmp(file->d_name, basename(buf2)) == 0) && (st.st_size == fst.st_size) )
        {
//            printf("file equal ~ %s\n", file->d_name);
            len = strlen(buf) + 1;
            char *tmp = malloc(sizeof(char)*(len));
            strcpy(tmp, buf);
            filelist[count] = tmp;
            count++;
        }


        if(S_ISDIR(st.st_mode))
        {
            search(FILENAME, filename);
        }
        if (S_ISREG(st.st_mode))
            total_size += fst.st_size;
    }

//    for (i = 0; i < count; i++)
//        printf("------->%s\n", filelist[count]);

//    for (i = 0; i < count; i++)
//        free(filelist[i]);
    closedir(dir);

    return 0;
}

int compare(const void *a, const void *b)
{
    return strcmp((char *)a, (char *)b);
}
