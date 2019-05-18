#include <string.h>
#include <stdio.h>

int main(){
    char buffer[] = "a  b";
    printf("%c\n", buffer[0]);
    char * next = strtok(buffer, " ");
    next = strtok(NULL, " ");
    printf("%s\n", next);
    return 0;
}