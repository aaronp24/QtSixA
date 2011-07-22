/*
 * hidraw-dump - program to dump information from hidraw devices (just like hcidump)
 * written by falkTX, 2009  (used some code from sixhidtest.c)
 *
 * Compile with: gcc -Wall hidraw-dump.c -o hidraw-dump
 * Run with: sudo su root -c "./hidraw-dump < /dev/hidrawX"
 *
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    unsigned char buf[128];
    int nr;

    while ( (nr=read(0, buf, sizeof(buf))) ) {
        int i;

        if ( nr < 0 ) {
            perror("read(stdin)");
            exit(1);
        }

        for (i=0; i <= nr; i++) {
            int val = buf[i];

            if (val < 10)
                printf("00%i ", val);
            else if (val < 100)
                printf("0%i ", val);
            else
                printf("%i ", val);

            if (i == nr)
                printf("\n \n");
            else if (i % 20 == 0 && i != 0)
                printf("\n");
        }
    }
    return 0;
} //The End
