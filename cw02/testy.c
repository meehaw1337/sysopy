#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> 
#include <fcntl.h>

int main(){
    int filedesc = open("cokolwiek", O_RDWR);
    lseek(filedesc, 5, SEEK_SET);
    write(filedesc, "ab", 2);
    return 0;
}