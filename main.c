#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>

const char * b64_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char * b16_alphabet = "0123456789abcdef";

#define TEST_MSG "Base 64 Test Message"

#define INVALID_CHAR 0x7f

#define ENCODE_B64_MASK 0x3f
#define DECODE_B64_MASK 0xff

/* macros for encoding */
#define FIRST_OF_THREE(X)  ((X >> 18) & ENCODE_B64_MASK)
#define SECOND_OF_THREE(X) ((X >> 12) & ENCODE_B64_MASK) 
#define THIRD_OF_THREE(X)  ((X >>  6) & ENCODE_B64_MASK)
#define FOURTH_OF_THREE(X)  (X        & ENCODE_B64_MASK)

/* macros for decoding */
#define FIRST_OF_FOUR(X)  ((X >> 16) & DECODE_B64_MASK) 
#define SECOND_OF_FOUR(X) ((X >>  8) & DECODE_B64_MASK) 
#define THIRD_OF_FOUR(X)   (X        & DECODE_B64_MASK)

/* wrapper around fprintf(stderr, ...*/
#define ERR_PRINT(...) {      \
 fprintf(stderr, "[ERROR]: ");\
 fprintf(stderr, __VA_ARGS__);\
 fprintf(stderr, "\n");	      \
}


char * encode_b64(const char*, size_t);
char * decode_b64(const char*, size_t);
char * encode_b16(const char*, size_t);
char * decode_b16(const char*, size_t);
char   map_b64(char);
char   map_b16(char);
int    validate_b64(const char*, size_t);
int    validate_b16(const char*, size_t);

enum Coding_Mode{
		 BASE64,
		 BASE16,
		 BASE8,
		 BASE2,
};

typedef enum Coding_Mode c_mode;

char option;
FILE * in;
char *msg;
char *progname;
c_mode mode = BASE64;


int main(int argc, char ** argv){
  char opt;
  progname = argv[0];
  while((opt = getopt(argc, argv, "edhs:")) != -1){
    switch(opt){
    case 'e':
      option = 'e';
      break;
    case 'd':
      option = 'd';
      break;
    case 'h':
      mode = BASE16;
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
    ERR_PRINT("No string to de/encode_b64 was given");
    exit(EXIT_FAILURE);
  }

  char* out;
  if(option == 'e'){
    switch(mode){

    case BASE64:
    out = encode_b64(msg, strlen(msg));
    break;
    case BASE16:
    out = encode_b64(msg, strlen(msg));
    break;
    case BASE8:
    case BASE2:
    default:
      ERR_PRINT("Desired encoding mode is not yet supported!");

    }
  }

  if(option == 'd'){
    if(validate_b64(msg, strlen(msg))){
      exit(EXIT_FAILURE);
    }
    out = decode_b64(msg, strlen(msg));
  }

  if(!out){
    ERR_PRINT("This is fatal. Abort");
    exit(EXIT_FAILURE);
  }
  
  printf("%s", out);
  free(out); 
  return EXIT_SUCCESS;
}


char map_b64(char letter){
    if(letter == '=')
        return 0;
    for(size_t i = 0; i < strlen(b64_alphabet); i++){
        if(b64_alphabet[i] == letter){
            return (char) i;
        }
    }
    return INVALID_CHAR;
}


int validate_b64(const char * msg, size_t msg_len){
    int ret = 0;
    for(size_t i = 0; i < msg_len; i++){
        if(map_b64(msg[i]) == INVALID_CHAR){
            ERR_PRINT("Invalid char %c at location %li", msg[i], i+1);
            ret = 1;
        }
    }
    return ret; 
}

char * encode_b64(const char * msg, size_t msg_len){
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

char * decode_b64(const char * msg, size_t msg_len){

  if(msg_len % 4){
    ERR_PRINT("Malformed Message. Length of msg_len in decode_b64 is not divisible by 4");
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

  size_t outlen = (msg_len*6)/8 +1;
  char * out = malloc(sizeof(char) * outlen);
  if(!out){
    ERR_PRINT("Malloc failed to allocate requested %li bytes", outlen);
    return NULL;
  }

  memset(out, 0, (msg_len*6)/8);

  int stream = 0;
  int k = 0;
  size_t i = 0;
  for(; i < msg_len - padding; i += 4){
    stream = (map_b64(msg[i])     << 18) +  
             (map_b64(msg[i + 1]) << 12) +
             (map_b64(msg[i + 2]) <<  6) +
             (map_b64(msg[i + 3])); 

    out[k++] = FIRST_OF_FOUR(stream); 
    out[k++] = SECOND_OF_FOUR(stream); 
    out[k++] = THIRD_OF_FOUR(stream); 
  }
  if(padding == 1){
    stream = (map_b64(msg[i]) << 18) + (map_b64(msg[i + 1]) << 12) + (map_b64(msg[i + 2]) << 6);
    out[k++] = FIRST_OF_FOUR(stream); 
    out[k++] = SECOND_OF_FOUR(stream); 
  }

  if(padding == 2){
    stream = (map_b64(msg[i]) << 18) +  (map_b64(msg[i + 1]) << 12);
    out[k++] = FIRST_OF_FOUR(stream); 
  }

  out[(msg_len*6)/8] = '\0';


  return out;
}


char* encode_b16(const char* bytes, size_t len){
  size_t outlen = len * 2 + 1;
  char * hexstr = malloc(sizeof(char) * outlen);
  if(!hexstr){
    ERR_PRINT("Malloc failed to allocate requested %li bytes", outlen);
    return NULL;
  }
  int k = 0;
  for(size_t i = 0; i < strlen(bytes); i++){
    hexstr[k++] = b16_alphabet[(bytes[i] >> 4) & 0xF];
    hexstr[k++] = b16_alphabet[bytes[i] & 0xF];
  }
  hexstr[k++] = '\0';
  return hexstr;

}

char map_b16(char b16_char){
  for(int i = 0; i < 16; i++){
    if(b16_char == b16_alphabet[i])
      return i;
  }
  return INVALID_CHAR;
}

int validate_b16(const char * bytes, size_t len){
    int ret = 0;
    for(size_t i = 0; i < len; i++){
        if(map_b64(bytes[i]) == INVALID_CHAR){
            ERR_PRINT("Invalid char %c at location %li", msg[i], i+1);
            ret = 1;
        }
    }
    return ret; 

}

/* TODO: test this */
char * decode_b16(const char * bytes, size_t len){
  if(len % 2){
    ERR_PRINT("decode_b16: len must be divisible by 2");
    return NULL;
  }

  size_t outlen =  len /2 + 1;
  char* hexstr = malloc(sizeof(*hexstr)* outlen);
  if(!hexstr){
    ERR_PRINT("Malloc failed to allocate requested %li bytes", outlen);
    return NULL;
  }

  size_t k = 0;
  char hi, lo;
  for(size_t i = 0; i < strlen(bytes); i+=2){
    hi = map_b16(bytes[i]);
    lo = map_b16(bytes[i+1]);
    hexstr[k++] = (hi << 4) + lo;
  }
  hexstr[k] = '\0';

  return hexstr;

}
