#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

const char * alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define die(...)    do{                                 \
                        fprintf(stderr, __VA_ARGS__);   \
                        exit(EXIT_FAILURE);             \
                    }while(0)

char map(char d){
    if('A' <=  d && d <= 'Z')
        return d - 'A';
    if('a' <=  d && d <= 'z')
        return d - 'a' + 26;
    if('0' <=  d && d <= '9')
        return d - '0' + 52;
    if(d == '+')
        return 62;
    if(d == '/')
        return 63;
    return -1;
}

void encode_block(char * block, char a, char b, char c){
        block[0] = alphabet[a >> 2];
        block[1] = alphabet[((a & 0x3) << 4) | (b >> 4)];
        block[2] = alphabet[((b & 0xf) << 2) | (c >> 6)];
        block[3] = alphabet[c & 0x3f];
}

void decode_block(char* block, char a, char b, char c, char d){
        block[0] = (a << 2) | (b >> 4);
        block[1] = ((b & 0xf) << 4) | (c >> 2);
        block[2] = ((c & 0x3) << 6) | d;
}

char * encode(char *in){
    int32_t inlen = strlen(in);
    int32_t outlen = 4 * (inlen / 3);
    int32_t i = 0;

    if(inlen == 0)
        die("empty input");

    if(inlen % 3)
        outlen += 4;

    char * out = malloc((outlen+1) * sizeof(char));
    char * block = out;
    for(; i < (inlen / 3) * 3; i += 3, block += 4)
        encode_block(block, in[i], in[i + 1], in[i + 2]);

    if(inlen % 3 == 2){
        encode_block(block, in[i], in[i+1], 0);
        out[outlen-1]  = '=';
    }

    if(inlen % 3 == 1){
        encode_block(block, in[i], 0, 0);
        out[outlen-2] = out[outlen-1] = '=';
    }

    out[outlen + 1] = '\0';
    return out;
}


char* decode(char * in){
    int32_t inlen = strlen(in);
    int32_t outlen = (inlen * 6) / 8;
    int32_t padding = 0, i = 0;

    if(inlen % 4 || inlen == 0)
        die("Invalid Message Length. Must be greater than 0 and divisible by 4\n");

    while(in[inlen - 1 - padding] == '=')
        padding++;
    outlen -= padding;

    if(padding > 2)
        die("%i padding characters were found. That is too much.\n", padding);

    char * out = malloc(outlen * sizeof(char));
    char * block = out;

    for(int32_t j = 0; j < inlen - padding; j++){
        in[j] = map(in[j]);
        if(in[j] == -1){
            free(out);
            die("Invalid character at %i\n", j);
        }
    }

    for(; i < (inlen / 4) * 4; i += 4, block += 3)
        decode_block(block, in[i], in[i + 1], in[i + 2], in[i + 3]);

    if(padding == 1)
        decode_block(block, in[i], in[i + 1], in[i + 2], 0);

    if(padding == 2)
        decode_block(block, in[i], in[i + 1], 0, 0);

    out[outlen] = '\0';
    return out;
}

int main(int argc, char * argv[]){
    if(argc != 3)
        die("Wrong Usage\n");

    char * input = argv[2];
    char * out;

    if(!strcmp(argv[1], "decode")){
        out = decode(input);
    }else if(!strcmp(argv[1], "encode")){
        out = encode(input);
    }else{
        die("Wrong Usage\n");
    }

    printf("%s\n", out);
    free(out);
    return EXIT_SUCCESS;
}
