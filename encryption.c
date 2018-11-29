#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define CHUNKSIZE 1000

void encodefunc(char*, char*, int);

int main(int argv, char* argc[]){

  if(argv < 3){
    perror("not enough arguments, input should be: encryption textfile keyfile\n");
    fflush(stdout);
    return 1;
  }
  char* input = argc[1];
  char* key = argc[2];

  //  printf("input: %s\noutput:%s\n", input, output);
  FILE *fd1, *fd2;                                       //file descriptors 1 and 2

  fd1= fopen(input, "r");
  if(!fd1){
    perror("fd1 open() was an utter failure");
    fflush(stdout);
    return 1;
  }

  fd2 = fopen(key, "r");
  if(!fd2){
    perror("fd2 open() was an utter failure");
    fflush(stdout);
    return 1;
  }


  fseek(fd1, 0, SEEK_END);               // https://stackoverflow.com/questions/174531/how-to-read-the-content-of-a-file-to-a-string-in-c
  int inputlen = ftell(fd1);
  fseek(fd2, 0, SEEK_END);
  int keylen = ftell(fd2);

  if(inputlen > keylen){
    perror("Key isn't big enough for the input file\n");
    fflush(stdout);
    return 1;
  }

  //reset file pointer to beginning
  fseek(fd1, 0, SEEK_SET);
  fseek(fd2, 0, SEEK_SET);

  char* buffer= malloc(inputlen * sizeof(char));

  memset(buffer, '\0', inputlen*sizeof(char));
  if(fgets(buffer, inputlen, fd1)!=NULL){
    puts(buffer);
  //  buffer[inputlen-1]='\0';
  }
  fclose(fd1);
  //now do the same for the key

  char* keybuff = malloc(keylen*sizeof(char));
  if(fgets(keybuff, keylen, fd2)!=NULL){
    puts(keybuff);
    keybuff[keylen-1]='\0';
  }
  fclose(fd2);

  //for encryption just add an subtrct 64
  encodefunc(buffer, keybuff, inputlen);


  free(buffer);
  free(keybuff);



  return 0;
}



void encodefunc(char* msg, char* key, int len){

  char* msgptr;
  char* keyptr;
  char* codeptr;
  char* code = malloc(sizeof(char)* len);
  int i;
  int currchar;

  for(i=0, keyptr=key, msgptr = msg, codeptr = code; i<len; i++, msgptr++, keyptr++, codeptr++){     //learned how to do this from mike baileys 450 and 491 class assignments....so fucking cool
  //printf("%c", *keyptr);        //alone prints full message
  //currchar = atoi(*msgptr) + atoi(*keyptr);
  currchar = (*msgptr - '\0') + (*keyptr-'\0');       //message + key between 0-26
  currchar = currchar %26;                            //message + key (mod 26) 27 for space
  //currchar +=65;
  //printf("original: %c | key: %c | new: %c %d\n", *msgptr, *keyptr, 'A'+currchar, currchar);
  //printf("original: %c | new: %c %d\n", *msgptr, 'A'+currchar, currchar);

  *codeptr = 'A'+currchar;
  //  code[i]='A'+currchar;
  }

  printf("encrypted: %s\n", code);


//decrypt
  for(i=0, keyptr=key, msgptr = msg, codeptr = code; i<len; i++, msgptr++, keyptr++, codeptr++){
    currchar = (*codeptr-'\0')-(*keyptr -'\0');   //cypher text minus key     between 0-26
  //  printf("charnum: %d\n",currchar);
    if(currchar < 0){
      currchar+=26;
    }
        //printf("charnum again: %d\n",currchar);
    //currchar = (*codeptr - 'A')+currchar;
    //printf("decrypted: %c\n", 'A'+currchar);
    //printf("original: %c | new: %c %d\n", *msgptr, 'A'+currchar, currchar);
    //printf("original: %c | new: %c | key: %c %d\n", *msgptr, 'A'+currchar,*keyptr, *codeptr-'A');

    if(currchar>26){
      //currchar = 32;
      printf(" ");
    }
    else{

    printf("%c", 'A'+currchar);
    }
  }
  printf("\n");
}
