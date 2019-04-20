//
// Created by austinmm on 4/19/19.
//

#include "Level2.h"
/*
 cat filename:

   int n;

1. int fd = open filename for READ;
2. while( n = read(fd, mybuf[1024], 1024)){
       mybuf[n] = 0;             // as a null terminated string
       // printf("%s", mybuf);   <=== THIS works but not good
       spit out chars from mybuf[ ] but handle \n properly;
   }
3. close(fd);
 */

int _cat(char* pathanme)
{
    char buf[BLKSIZE];  // a null char at end of mybuf[ ]
    int nbytes = 0;
    int fd = open_file(pathanme, 0);
    if(fd < 0){
        print_notice("cat: unable to open file for read");
        return -1;
    }
    printf("=====================  cat (fd = %02d)  =====================\n", fd);
    while((nbytes = read_file(fd, buf, BLKSIZE)) > 0){
        buf[nbytes] = 0;             // as a null terminated string
        //spit out chars from buf[ ] but handle \n properly
        printf("%s", buf);
    }
    printf("\n===========================================================\n");
    close_file(fd);
    return nbytes;
}