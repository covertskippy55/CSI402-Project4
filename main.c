/* Name: Dilan Samarasinghe
   ID: DS697678
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "struct_def.h"
#include "prototypes.h"
/* This function wil open the source file to be passed to the first and second pass. It will get the root name of the file by removing the last '.' and it will call the function to make the opcode table. It will then run the first and the second pass functions. 
 */
int main(int argc, char* argv[]){
  FILE *sourcefp; // source file pointer
  char *tempRootName; // holds the string to traverse over and remove the last '.'
  char *rootName; // the original rootname string
  int i; // temp variable
  int error; // holds 0 if there were no errors, holds 1 if there was an error
  // check if the command line args are correct
  if(argc != NUM_ARGS){
    fprintf(stderr,"Invalid usage format; Correct format is p4 infile.\n");
    exit(1);
  }  
  // open the source file and do error checking
  if((sourcefp = fopen(argv[SOURCE_FILE],"r")) == NULL){
    fprintf(stderr,"Error occured with opening of file %s.\n",argv[SOURCE_FILE]);
    exit(1);
  }
  // space for root name malloced to the length of the sourcefile's name
  if((rootName = malloc(sizeof(char)*strlen(argv[SOURCE_FILE]))) == NULL){
    fprintf(stderr,"Error occured with malloc.\n");
    exit(1);
  }

  if((tempRootName = malloc(sizeof(char)*strlen(argv[SOURCE_FILE]))) == NULL){
    fprintf(stderr,"Error occured with malloc.\n");
    exit(1);
  }

  // see if there are any '.' at all, if there isnt then the rootname is the whole thing
  if( strrchr(argv[SOURCE_FILE],'.') == NULL){
    strcpy(rootName,argv[SOURCE_FILE]);
    // printf("Root name is: %s\n",rootName);
  }
  // otherwise start at the end of the string and look for the first '.' and stop there
  else{
    strcpy(tempRootName,argv[SOURCE_FILE]);
    //printf("%s\n",tempRootName);
    for(i = strlen(argv[SOURCE_FILE]) - 1;i >= 0; i--){
      if(tempRootName[i] == '.'){
	tempRootName[i] = '\0';
	break;
      }
    }
    // copy the root name without the last extension back into the rootname
    strcpy(rootName,tempRootName);
    //printf("%s\n",rootName);

  }
  makeOpCodeTable(); // function to create the op code table from the opcode text file
  error = firstPass(sourcefp,rootName); // run the first pass and get the value that determines if there was an error
  secondPass(sourcefp,error,rootName); // run te second pass
  // printf("%d\n",error);
  //printSymbol();
  // close the source file
  if(fclose(sourcefp) == EOF){
    fprintf(stderr,"Error occured with closing source file \n");
  }
  return 0;
}
