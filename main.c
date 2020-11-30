#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <err.h>

const char * alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define die(...) errx(EXIT_FAILURE, __VA_ARGS__)
#define pack(a, b, c, d, shift) ((a << 3*shift) + (b  << 2*shift) + (c << shift)  + d)

char map(char d){
    if( d == '+')
        return 62;
    if( d == '/')
        return 63;
    if('A' <=  d && d <= 'Z')
        return d - 'A';
    if('a' <=  d && d <= 'z')
        return d - 'a' + 26;
    if('1' <=  d && d <= '9')
        return d - '1' + 53;
    return -1;
}

void encode_block(char* out, int32_t x){
        out[0] = alphabet[(x >> 18) & 0x3f ];
        out[1] = alphabet[(x >> 12) & 0x3f ];
        out[2] = alphabet[(x >>  6) & 0x3f ];
        out[3] = alphabet[ x        & 0x3f ];
}

void decode_block(char* out, int32_t x){
        out[0] = (x >> 16) & 0xff;
        out[1] = (x >>  8) & 0xff;
        out[2] =  x        & 0xff;
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
    for(; i < (inlen / 3) * 3; i += 3, block+=4)
        encode_block(block, pack(0, in[i], in[i + 1], in[i + 2], 8));

    if(inlen % 3 == 2){
        encode_block(block, pack(0, in[i], in[i+1], 0, 8));
        out[outlen-1]  = '=';
    }

    if(inlen % 3 == 1){
        encode_block(block, in[i] << 16);
        out[outlen-2] =  out[outlen-1] = '=';
    }

    out[outlen + 1] = '\0';
    return out;
}


char* decode(char * in){
    int32_t inlen = strlen(in);
    int32_t outlen = (inlen * 6) / 8;
    int32_t padding = 0, i = 0;

    if(inlen % 4 || inlen == 0)
        die("Invalid Message Length. Must be greater than 0 and divisible by 4");

    while(in[inlen - 1 - padding] == '=')
        padding++;

    if(padding > 2)
        die("%i padding characters were found but up to 2 are okay!", padding);

    outlen -= padding;
    char * out = malloc(outlen * sizeof(char));
    char * block = out;

    for(int32_t j = 0; j < inlen - padding; j++){
        in[j] = map(in[j]);
        if(in[j] == -1){
            free(out);
            die("Invalid character at %i", j);
        }
    }

    for(; i < (inlen / 4) * 4; i += 4, block += 3)
        decode_block(block, pack(in[i], in[i + 1], in[i+2], in[i+3], 6));
    }

    if(padding == 1)
        decode_block(block, pack(in[i], in[i + 1], in[i+2], 0 , 6));

    if(padding == 2)
        decode_block(block, (in[i] << 18) +  (in[i + 1] << 12));

    out[outlen] = 0;
    return out;
}

int main(int argc, char * argv[]){
    if(argc != 3)
        die("Wrong Usage");

    char * input = argv[2];
    char * out;

    if(!strcmp(argv[1], "decode")){
        out = decode(input);
    }else if(!strcmp(argv[1], "encode")){
        out = encode(input);
    }else{
        die("Wrong Usage");
    }

    printf("%s\n", out);
    free(out);
    return EXIT_SUCCESS;
}
