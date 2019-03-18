#include <stdio.h>
#include <string.h>

const char * b64_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char * encode(char*, size_t);
char * padd(char* msg, size_t padding);


int main(int argc, char ** argv){
    /* read str from stdout and de/encode */
    printf("Length of b46_alphabet: %li\n", strlen(b64_alphabet));
    return 0;

}
