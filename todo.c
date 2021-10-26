#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static int listCmp(const void *a, const void *b) {
    const char *str1 = * (char * const *) a;
    const char *str2 = * (char * const *) b;
    if(str1[strlen(str1) - 1] == '*' && str2[strlen(str2) - 1] == '*') {
        return strcmp(str1, str2);
    } else if(str1[strlen(str1) - 1] == '*') {
        return 1;
    } else if(str2[strlen(str2) - 1] == '*') {
        return -1;
    } else {
        return strcmp(str1, str2);
    }
}

static int intCmp(const void *a, const void *b) {
   return (* (int *) a < * (int *) b) - (* (int *) a > * (int *) b);
}

int main(int argc, char **argv) { //spaghetti starts here
    FILE *fp = fopen("./todo.txt","r");
    char buffer[100];
    fgets(buffer, sizeof(buffer), fp);
    int numLines = atoi(buffer);
    char **arr = (char **)malloc(sizeof(char *) * numLines);
    for(int i = 0; i < numLines; i++) {
        fgets(buffer, sizeof(buffer), fp);
        strtok(buffer, "\n");
        arr[i] = (char *)malloc(sizeof(char) * (strlen(buffer) + 1));
        strcpy(arr[i], buffer);
    }
    fclose(fp);
    if(argc == 1) {
        if(!numLines) {
            printf("list is empty :)\n");
            return 0;
        }
        for(int i = 0; i < numLines; i++) {
            if(arr[i][strlen(arr[i]) - 1] == '*') {
                char temp[100];
                strtok(arr[i], "*");
                sprintf(temp, "echo -e \"%2d. \\e[9m%s\\e[0m\"", i + 1, arr[i]);
                system(temp);
                strcat(arr[i], "*");
            } else {
                printf("%2d. %s\n", i + 1, arr[i]);
            }
        }
    } else if(!strcmp(argv[1], "add")) {
        if(argc < 3) {
            printf("ERROR Incorrect number of command line arguments\nExpected: %s add \"string1\" \"string2\" ...\n", argv[0]);
            return 1;
        }
        arr = (char **)realloc(arr, sizeof(char *) * (numLines + argc - 2));
        for(int i = 2; i < argc; i++) {
            arr[numLines] = (char *)malloc(sizeof(char) * (strlen(argv[i]) + 1));
            strcpy(arr[numLines], argv[i]);
            numLines++;
        }
    } else if(!strcmp(argv[1], "edit")) {
        if(!numLines) {
            printf("list is empty :)\n");
            return 0;
        }
        if(argc != 4) {
            printf("ERROR Incorrect number of command line arguments\nExpeced: %s edit index \"string\"\n", argv[0]);
            return 1;
        }
        int index = atoi(argv[2]) - 1;
        if(index < 0 || index > numLines - 1) {
            printf("ERROR Values exceed range of [1, %d]\n", numLines);
            return 1;
        }
        arr[index] = (char *)malloc(sizeof(char) * (strlen(argv[3]) + 1));
        strcpy(arr[index], argv[3]);
    } else if(!strcmp(argv[1], "swap")) {
        if(!numLines) {
            printf("list is empty :)\n");
            return 0;
        }
        if(argc != 4) {
            printf("ERROR Incorrect number of command line arguments\nExpected: %s swap index1 index2\n", argv[0]);
            return 1;
        }
        int index1 = atoi(argv[2]) - 1;
        int index2 = atoi(argv[3]) - 1;
        if(index1 < 0 || index1 > numLines - 1 || index2 < 0 || index2 > numLines - 1) {
            printf("ERROR Values exceed range of [1, %d]\n", numLines);
            return 1;
        }
        char swap[100];
        strcpy(swap, arr[index1]);
        arr[index1] = (char *)malloc(sizeof(char) * (strlen(arr[index2]) + 1));
        strcpy(arr[index1], arr[index2]);
        arr[index2] = (char *)malloc(sizeof(char) * (strlen(swap) + 1));
        strcpy(arr[index2], swap);
    } else if(!strcmp(argv[1], "sort")) {
        if(!numLines) {
            printf("list is empty :)\n");
            return 0;
        }
        qsort(arr, numLines, sizeof(char *), listCmp);
    } else if(!strcmp(argv[1], "done")) {
        if(!numLines) {
            printf("list is empty :)\n");
            return 0;
        }
        if(argc < 3) {
            printf("ERROR Incorrect number of command line arguments\nExpected: %s done index1 index2 ...\n", argv[0]);
            return 1;
        }
        for(int i = 2; i < argc; i++) {
            int index = atoi(argv[i]) - 1;
            if(index < 0 || index > numLines - 1) {
                printf("ERROR Values exceed range of [1, %d]\n", numLines);
                return 1;
            }
            if(arr[index][strlen(arr[index]) - 1] != '*') {
                arr[index] = (char *)realloc(arr[index], sizeof(char) * (strlen(arr[index]) + 1));
                strcat(arr[index], "*");
            }
        }
    } else if(!strcmp(argv[1], "remove") || !strcmp(argv[1], "rm")) {
        if(!numLines) {
            printf("list is empty :)\n");
            return 0;
        }
        if(argc < 3) {
            printf("ERROR Incorrect number of command line arguments\nExpected: %s rm index1 index2 ...\n", argv[0]);
            return 1;
        }
        int rmArr[argc - 2];
        for(int i = 2; i < argc; i++) {
            rmArr[i - 2] = atoi(argv[i]) - 1;
        }
        qsort(rmArr, argc - 2, sizeof(int), intCmp);
        for(int i = 0; i < argc - 2; i++) {
            if(rmArr[i] < 0 || rmArr[i] > numLines - 1) {
                printf("ERROR Values exceed range of [1, %d]\n", numLines);
                return 1;
            }
            for(int j = rmArr[i]; j < numLines; j++) {
                arr[j] = (char *)malloc(sizeof(char) * (strlen(arr[j + 1]) + 1));
                strcpy(arr[j], arr[j + 1]);
            }
            numLines--;
        }
        arr = (char **)realloc(arr, sizeof(char *) * numLines);
    } else if(!strcmp(argv[1], "clear")) {
        if(!numLines) {
            printf("list is empty :)\n");
            return 0;
        }
        printf("Are you sure you want to clear the list? [y/N] ");
        char ans[4];
        fgets(ans, sizeof(ans), stdin);
        strtok(ans, "\n");
        for(int i = 0; i < strlen(ans); i++) {
            ans[i] = tolower(ans[i]);
        }
        if(!strcmp(ans, "y") || !strcmp(ans, "yes")) numLines = 0;
    } else if(!strcmp(argv[1], "help")) {
        printf(" add    Add given strings to the end of the list\n\tUsage: %s add \"string1\" \"string2\" ...\n", argv[0]);
        printf(" edit   Replace specified index with a new string\n\tUsage: %s edit index \"string\"\n", argv[0]);
        printf(" swap   Swap two elements at specified indexes\n\tUsage: %s swap index1 index2\n", argv[0]);
        printf(" sort   Sort list in alphebtical order placing completed items at the bottom\n\tUsage %s sort\n", argv[0]);
        printf(" done   Cross out specificed indexes from the list\n\tUsage: %s done index1 index2 ...\n", argv[0]);
        printf(" rm     Remove specified index from the list\n\tUsage: %s rm index1 index2 ...\n", argv[0]);
        printf(" clear  Clear list, user will be prompted\n\tUsage: %s clear\n", argv[0]);
        return 0;
    } else {
        printf("ERROR Invalid argument\nFor a list of availble commands type: %s help\n", argv[0]);
        return 1;
    }
    fp = fopen("./todo.txt","w");
    fprintf(fp, "%d\n", numLines);
    for(int i = 0; i < numLines; i++) {
        fprintf(fp, "%s\n", arr[i]);
    }
    fclose(fp);
    return 0;
}
