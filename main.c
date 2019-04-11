#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>

const char * b64_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define TEST_MSG "Base 64 Test Message"

#define INVALID_CHAR 0x7f

#define ENCODE_MASK 0x3f
#define DECODE_MASK 0xff

/* macros for encoding */
#define FIRST_OF_THREE(X)  ((X >> 18) & ENCODE_MASK)
#define SECOND_OF_THREE(X) ((X >> 12) & ENCODE_MASK) 
#define THIRD_OF_THREE(X)  ((X >>  6) & ENCODE_MASK)
#define FOURTH_OF_THREE(X)  (X        & ENCODE_MASK)

/* macros for decoding */
#define FIRST_OF_FOUR(X)  ((X >> 16) & DECODE_MASK) 
#define SECOND_OF_FOUR(X) ((X >>  8) & DECODE_MASK) 
#define THIRD_OF_FOUR(X)   (X        & DECODE_MASK)

/* wrapper around fprintf(stderr, ...*/
#define ERR_PRINT(...) {      \
 fprintf(stderr, "[ERROR]: ");\
 fprintf(stderr, __VA_ARGS__);\
 fprintf(stderr, "\n");	      \
}


char * encode(const char*, size_t);
char * decode(const char*, size_t);
char   map(char);
int    validate(const char*, size_t);

char option;
FILE * in;
char *msg;
char *progname;

int main(int argc, char ** argv){
  char opt;
  progname = argv[0];
  while((opt = getopt(argc, argv, "eds:")) != -1){
    switch(opt){
    case 'e':
      option = 'e';
      break;
    case 'd':
      option = 'd';
      break;
    case 's':
      msg = optarg;
      break;
    case '?':
        ERR_PRINT("None of the necessary options was specified!");
	printf("Usage: %s [-d|-e] [-f] [filename]\n", progname);
	exit(EXIT_FAILURE);
    }
  }

  /* if in is at this point still null then the flag -f was missing*/
  if(!msg){
    ERR_PRINT("No string to de/encode was given");
    exit(EXIT_FAILURE);
  }

  char* out;
  if(option == 'e'){
    out = encode(msg, strlen(msg));
  }

  if(option == 'd'){
    if(validate(msg, strlen(msg))){
      exit(EXIT_FAILURE);
    }
    out = decode(msg, strlen(msg));
  }

  if(!out){
    ERR_PRINT("This is fatal. Abort");
    exit(EXIT_FAILURE);
  }
  
  printf("%s", out);
  free(out); 
  return EXIT_SUCCESS;
}


char map(char letter){
    if(letter == '=')
        return 0;
    for(size_t i = 0; i < strlen(b64_alphabet); i++){
        if(b64_alphabet[i] == letter){
            return (char) i;
        }
    }
    return INVALID_CHAR;
}


int validate(const char * msg, size_t msg_len){
    int ret = 0;
    for(size_t i = 0; i < msg_len; i++){
        if(map(msg[i]) == INVALID_CHAR){
            ERR_PRINT("Invalid char %c at location %li", msg[i], i+1);
            ret = 1;
        }
    }
    return ret; 
}

char * encode(const char * msg, size_t msg_len){
  size_t chunks = 4 * ceil(msg_len/3.0);
  size_t leftover = msg_len % 3;

  char * msg_chunks = malloc(sizeof(char) * chunks + 1);
  if(!msg_chunks){
    ERR_PRINT("Internal Error trying to allocate memory!" );
    return NULL;
  }
  memset(msg_chunks, 0 ,chunks);

  int k = 0;

  int stream = 0;
  size_t i = 0;
  for(; i < msg_len - leftover; i+=3){
    stream = (msg[i] << 16) + (msg[i + 1]  << 8) + (msg[i + 2]); 

    msg_chunks[k++] = b64_alphabet[FIRST_OF_THREE(stream)];
    msg_chunks[k++] = b64_alphabet[SECOND_OF_THREE(stream)];
    msg_chunks[k++] = b64_alphabet[THIRD_OF_THREE(stream)];
    msg_chunks[k++] = b64_alphabet[FOURTH_OF_THREE(stream)];
  }

  if(leftover == 2){
    stream = (msg[i] << 16) + (msg[i + 1]  << 8);
    msg_chunks[k++] = b64_alphabet[FIRST_OF_THREE(stream)];
    msg_chunks[k++] = b64_alphabet[SECOND_OF_THREE(stream)];
    msg_chunks[k++] = b64_alphabet[THIRD_OF_THREE(stream)];
    msg_chunks[k]  = '=';
  }

  if(leftover == 1){
    stream = (msg[i] << 16);
    msg_chunks[k++] = b64_alphabet[FIRST_OF_THREE(stream)];
    msg_chunks[k++] = b64_alphabet[SECOND_OF_THREE(stream)];
    msg_chunks[k] =  msg_chunks[k + 1] = '=';
  }

  msg_chunks[chunks] = '\0';

  return msg_chunks;
}

char * decode(const char * msg, size_t msg_len){

  if(msg_len % 4){
    ERR_PRINT("Malformed Message. Length of msg_len in decode is not divisible by 4");
    return NULL;
  }


  /* determine size of padding */
  size_t padding = 0;
  while(msg[msg_len - padding] == '='){
    padding++;
  }   

  if(padding > 2){
    ERR_PRINT("%li padding characters were found but up to 2 are okay!", padding);
    return NULL;
  }

  char * out = malloc(sizeof(char) * (msg_len*6)/8 + 1);
  memset(out, 0, (msg_len*6)/8);

  int stream = 0;
  int k = 0;
  size_t i = 0;
  for(; i < msg_len - padding; i += 4){
    stream = (map(msg[i])    << 18) +  
             (map(msg[i + 1]) << 12) +
             (map(msg[i + 2]) <<  6) +
             (map(msg[i + 3])); 

    out[k++] = FIRST_OF_FOUR(stream); 
    out[k++] = SECOND_OF_FOUR(stream); 
    out[k++] = THIRD_OF_FOUR(stream); 
  }
  if(padding == 1){
    stream = (map(msg[i]) << 18) + (map(msg[i + 1]) << 12) + (map(msg[i + 2]) << 6);
    out[k++] = FIRST_OF_FOUR(stream); 
    out[k++] = SECOND_OF_FOUR(stream); 
  }

  if(padding == 2){
    stream = (map(msg[i]) << 18) +  (map(msg[i + 1]) << 12);
    out[k++] = FIRST_OF_FOUR(stream); 
  }

  out[(msg_len*6)/8] = '\0';


  return out;
}

