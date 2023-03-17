#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include<ctype.h>


char* uppercase(char* str) {
    char* temp = (char*)malloc(strlen(str)*sizeof(char));
    strcpy(temp, str);
    char cur;
    int i = 0;
    while(str[i]) {
        temp[i] = toupper(temp[i]);
        i++;
    }
    return temp;
}

void main() {
    char* t = "aaaa";
    char* s = uppercase(t);
    printf("==> %s", s);
}