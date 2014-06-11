/* Name: Dilan Samarasinghe
   ID: DS697678
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "struct_def.h"
#include "globals.h"
#include "prototypes.h"
/* This function takes nothing and returns nothing. It will create a linked list of opcodes and all the information associated with each opcode.
 */
void makeOpCodeTable(){
  FILE *opCodefp; // file pointer to opcode text file
  OPCODE *temp; // temporary opcode node
  OPCODE *current; // opcode node pointer to traverse the list
  char line[LINE_LENGTH]; // holds a line from the sourcefile
  // open the opcode file
  if((opCodefp = fopen("opcodes.txt","r")) == NULL){
    fprintf(stderr,"Error occured with opening the OpCode file\n");
    exit(1);
  }
  // read through the source file
  while( fgets(line,LINE_LENGTH,opCodefp) != NULL){
    if((temp = malloc(sizeof(OPCODE))) == NULL){
      fprintf(stderr,"Error occured with malloc \n");
      exit(1);
    }
    // scan the line for the various values and store them in the appropriate space of the node
    sscanf(line,"%d %s %c",&temp->code,temp->mnemonic,&temp->format);
    temp->next = NULL;
    // see if there is a first node
    if(head == NULL){
      head = temp;
      current = head;
    }
    // if there is then the next of the current node is the new node
    else{
      current->next = temp;
      current = temp;
    }
  }
  /*(
  for(current = head;current!= NULL;current = current->next){
    printf("%d %s %c \n",current->code,current->mnemonic,current->format);
  }
  */
							     
}
/* This function takes in a string that is the mnemonic of the opcode. It will return the correct opcode node or NULL if it doesnt exist. 
 */
OPCODE* getOpCode(char *opcode){
  OPCODE *pointer = head;
  // traverse the opcode linked list and look for the mnemonic otherwise return null
  for(;pointer!= NULL;pointer = pointer->next){
    if(strcmp(opcode,pointer->mnemonic) == 0){
      return pointer;
    }				      

  }
  return NULL;

}
/* This is the first pass function. It takes in the sourcefile pointer, and the root name and returns an int, which indicates if there were any erros in the sourcefile. It will also run the error function if there are illegal opcodes or multiply defined symbols.
 */
int firstPass(FILE *sourceFile,char *rootName){
  
  char line[LINE_LENGTH]; // holds a line from the sourcefile
  int location; // the location counter
  int line_number; // the line number counter
  char *token; // string pointer for tokenizing
  char opcode[LINE_LENGTH]; // holds the mnemonic of an opcode
  //int i;
  //int symbolError;
  // char *firstop; // pointer to the first operand after the opcode
  //  char *secondop; // pointer to the second operand after opcode
  //  char *thirdop; // pointer to the third operand
  OPCODE *tempopcode; // we use this poitner to see if the opcode was valid
  int increment = 0; // this tells us if we need to increment the location counter, when we find a .word or .resw pseduo op we directly add to the location counter, this we dont need to manually increment by one, thats what this is used for 
  int gotErrors = 0; 
  location = 0;
  line_number = 0;
  shead = NULL; // intialize the pointer to the starting errors linked list and symbol linked list to be null
  ehead = NULL; 
  while(fgets(line,LINE_LENGTH,sourceFile) != NULL){
    /* for(i = strlen(line)-1;i>= 0;i--){
      if(line[i] == '\n'){
	line[i] = '\0';
	break;
      }
      }*/
    // check if the line is a comment
    if(line[0] == '#'){
      line_number++;
    }
    // check if its a blank line
    else if(line[0]== '\n'){
      line_number++;
    }
    else if(line[0] == '\0'){
      line_number++;
    }
    
    else{
      line_number++;
      // get the first token(this is either an opcode or a label)
      token = strtok(line," \t\n");
      // check to see if its one of the two pseudo codes
	if(strcmp(token,".text") == 0){
	  line_number++;
	}
	else if(strcmp(token,".data") == 0){
          line_number++;
        }
	// check if its a label by checking the end character
	else if(token[strlen(token)-1] == ':'){
	  token[strlen(token)-1] = '\0'; // remove the last character so we can search for it
	  //if the current label is not in the symbol table add it
	  //symbolError = isSymbol(token);
	    if(isSymbol(token) == 0){
	      // printf("Not defined: %s, please add\n",token);
	      addSymbol(token,location);
	      increment = 1; // now we need to increment the location counter
	      token = strtok(NULL," \t\n"); // this will give us the opcode since the first token was a label
	      // check if the opcode is one of the psuedo codes to reserve space
	      if(strcmp(token,".word")== 0){ // if its .word then get the number after the : 
		token = strtok(NULL," :");
		token = strtok(NULL," \t\n#");
		//printf("%s\n",token);
		location += atoi(token);// add this to the location counter and make sure not to increment it again later
		increment = 0;
	      }
	      // if its just .resw then get the value and add it to the location counter
	      else if(strcmp(token,".resw") == 0){
		token = strtok(NULL," \t\n#"); 
		location += atoi(token);
		increment = 0;
	      }
	      else{
		// if we had no multiply defined symbols check if the opcode is valid, oterhwise start the error process because we have an illegal opcode.
		strcpy(opcode,token);
		tempopcode = getOpCode(opcode);
		if(tempopcode == NULL){
		  startError(sourceFile,rootName);
		  gotErrors = 1;
		  return gotErrors;
		}/*
		// if the opcode is valid then see if the opcode is J type, because thats the only time we have a label as one of the operands. IGNORE THIS SECTION, ITS HORRIBLE AND DOESNT WORK HENCE WHY ITS COMMENTED OUT
		else if(tempopcode->format == 'J'){
		  token = strtok(NULL," \t\n");
		  firstop = strtok(token," , \t\n"); // get the first operand
		  // if you do have a first operand then see if its a register, otherwise its a label
		  if(firstop !=NULL){
		    if(firstop[0]!='$'){
		      // we check if the label is in the symbol table, if its not there start error process
		      if(isSymbol(firstop) == 0){
			//startError(sourceFile,rootName);
			gotErrors = 2;
			
		       return gotErrors;
		      }
		    }
		  }
		  // get the second second operand then do the same thing
		  secondop = strtok(NULL," , \t\n");
		  if(secondop !=NULL){
		    if(secondop[0]!='$'){
		      if(isSymbol(secondop) == 0){
			//startError(sourceFile,rootName);
			gotErrors = 2;
		       
			 return gotErrors;
		      }
		    }
		  }
		  // samething with the third operand. We can only have a maximum of three operands.
		  thirdop = strtok(NULL," , \t\n");
		  if(thirdop !=NULL){
		    if(thirdop[0]!='$'){
		      if(isSymbol(thirdop) == 0){
			//startError(sourceFile,rootName);
			gotErrors = 2;
			
			return gotErrors;
		      }
		    }
		  }
		  }*/
		}
	    }
	    
	    // if the symbol is current in the symbol table and we see it again that means we have a multiply defined symbol so start the error process.
	    else if(isSymbol(token) == 1){
	      //printf("Symbol: %s, is multiply defined\n",token);
	      startError(sourceFile,rootName); // error function
	      gotErrors = 1;
	      return gotErrors; // return 1 since we had an error
	    }
	    // if we havent increment the location counter( we didnt have .word or .resw), then increment the location counter
	    if(increment == 1){
	      location++;
	    }
	    
	}
	
	else{
	  // if we had no multiply defined symbols check if the opcode is valid, oterhwise start the error process because we have an illegal opcode.
	  strcpy(opcode,token);
	  tempopcode = getOpCode(opcode);
	  if(tempopcode == NULL){
	    startError(sourceFile,rootName);
	    gotErrors = 1;
	    return gotErrors;
	  }/*
	  // if the opcode is valid then see if the opcode is J type, because thats the only time we have a label as one of the operands
	  else if(tempopcode->format == 'J'){
	    token = strtok(NULL," \t\n");
	    firstop = strtok(token," , \t\n"); // get the first operand 
	    // if you do have a first operand then see if its a register, otherwise its a label
	    if(firstop !=NULL){
	      if(firstop[0]!='$'){
		// we check if the label is in the symbol table, if its not there start error process
		if(isSymbol(firstop) == 0){
		  //startError(sourceFile,rootName);
		  gotErrors = 2;
		  
		  //return gotErrors;
		}
	      }
	    }
	    // get the second second operand then do the same thing
	    secondop = strtok(NULL," , \t\n");
            if(secondop !=NULL){
              if(secondop[0]!='$'){
                if(isSymbol(secondop) == 0){
		  // startError(sourceFile,rootName);
                  gotErrors = 2;
		 
		  // return gotErrors;
                }
              }
            }
	    // samething with the third operand. We can only have a maximum of three operands. 
	    thirdop = strtok(NULL," , \t\n");
            if(thirdop !=NULL){
              if(thirdop[0]!='$'){
                if(isSymbol(thirdop) == 0){
                  //startError(sourceFile,rootName);
                  gotErrors = 2;
                  //return gotErrors;
                }
              }
            }
	    }*/
	  location++;
	}
	
	//printf("%s\n",token);
       
	
      
      
    }
    
      // printf("%s\n",line);
  }

  // printf("%d\n",line_number);
  return gotErrors; // if we reach this point that means weve had no errors.
 
}
/* This function takes in a symbol name then checks if its in the symbol table. It returns an int, 0 = not found 1 = found. */
int isSymbol(char *symbol){
  int found = 0;
  SYMBOL *current = shead; // start at the head then traverse through looking at the symbol name
  for(;current != NULL;current = current->next){
    if(strcmp(symbol,current->symbol) == 0){
      found = 1;
      return found;
    }

  }
  return found;
}
/* This function takes in a symbol name and a location int and adds it to the symbol table. It returns nothing 
 */
void addSymbol(char *symbol, int location){
  SYMBOL *temp;
  SYMBOL *current = shead;
  // malloc space for a symbol node
  if((temp = malloc(sizeof(SYMBOL))) == NULL){
    fprintf(stderr,"Error occured with malloc\n");
    exit(1);
  }
  // copy the information to the node.
  strcpy(temp->symbol,symbol);
  temp->lc = location;
  temp->next = NULL;
  // if this is the first node then add it at the start otherwise traverse through the list until you find node with the next being null then make next point to it.
  if(shead == NULL){
    shead = temp;
    current = shead;
  }
  else{
    while(current->next != NULL){
      current = current->next;
    }
    current->next = temp;

  }

}
/* This is just a test function to print the symbol table, to make sure the values are added properly.
 */
void printSymbol(){
  SYMBOL *current = shead;
  for(;current!=NULL;current = current->next){
    printf("Symbol: %s, location: %d \n",current->symbol,current->lc);
  }
}
/* 
   This function will take in a symbol name then search for it in the symbol table. It will return the symbol node or NULl if it doesnt exist.
 */
SYMBOL* getSymbol(char *symbol){
  SYMBOL *current = shead;// a pointer to a symbol node starting at the first node
  // traverse the list, if we find it return it, otherwise return null
  for(;current !=NULL;current = current->next){
    if(strcmp(current->symbol,symbol) == 0){
      return current;
    }
  }
  return NULL;
}
/* This is the error function. This gets run if there is one of the three errors in the source file. It takes in the root name and the source file pointer. It retunrs nothing. 
 */
void startError(FILE *sourceFile,char *rootname){
  char line[LINE_LENGTH]; //
  FILE *errfp;
  char *errString ={".err"}; // string to concat to get the error file
  char *rootNameCopy; // copy of the root name to concat with
  char *token; // this helps to tokenize
  OPCODE *check; // stores an opcode node
  ERROR *pointer; // pointer to an error node
  int location = 0;
  int lineNumber = 1;
  //SYMBOL *current;
  SYMBOL *temp;
  rootNameCopy = malloc(LINE_LENGTH+EXTENSION_LENGTH);
  rewind(sourceFile);// start at the top of the source file because we didnt reopen it
  strcpy(rootNameCopy,rootname);
  strcat(rootNameCopy,errString);// rootNameCopy now has the error file name
  // printf("%s\n",rootNameCopy);
  // open the error file for writing
  if((errfp = fopen(rootNameCopy,"w")) == NULL){
    fprintf(stderr,"Error occured with opening of file %s \n",rootNameCopy);
    exit(1);
  }
  // read through the source file then copy that and the line number to the error file
  while(fgets(line,LINE_LENGTH,sourceFile) != NULL){
    fprintf(errfp,"%d \t %s",lineNumber++,line);

  }
  fprintf(errfp,"\n");
  lineNumber = 0;
  rewind(sourceFile);// rewind the sourcefile again becuase now we do the actual error detection.
  while(shead!=NULL){ // loop through the symbol file and remove everything because we are goign to recreate it here
    temp = shead->next;
    free(shead);
    shead = temp;
  }// this loop is the same as the one in the first pass, except when we find an error we print it to the error file instead of starting the error process.
    while(fgets(line,LINE_LENGTH,sourceFile) != NULL){
      // ignore comment lines and blank lines.
      if(line[0] == '#'){
	lineNumber++;
      }
      
      else if(line[0]== '\n'){
	lineNumber++;
      }
      else if(line[0] == '\0'){
	lineNumber++;
      }
      else{
	// get the first token, then ignore this line if it says .text or .data
	token = strtok(line," \t\n");
       
	  if(strcmp(token,".text") == 0){
	    lineNumber++;
	  }
	  else if(strcmp(token,".data") == 0){
	    lineNumber++;
	  }
	  // see if the token is a label
	  else if(token[strlen(token)-1] == ':'){
	    token[strlen(token)-1] = '\0';
	    // check if the label is in the symbol table, if not add it
            if(isSymbol(token)== 0){
	      //  printf("Not defined: %s, please add\n",token);
              addSymbol(token,location);
              location++;
            }
	    // if its in the symbol table add add it to an error linked list to be printed later
            else if(isSymbol(token) == 1){
	      addError(token);
              //printf("Symbol: %s, is multiply defined\n",token);
            }
	    // get the next token which is the opcode then see if its a valid opcode, otherwise print to the error file
	    token = strtok(NULL," \t\n");
	    if(token !=NULL){
	      if(token[0] == '#'){
		
	      }
	      else if(strcmp(token,".resw") == 0){
		lineNumber++;
	      }
	      else if(strcmp(token,".word") == 0){
		lineNumber++;
	      }
	      
	      else if((check = getOpCode(token)) == NULL){
		// printf("invalid opcode \n");
		fprintf(errfp,"Invalid opcode %s at line %d\n",token,lineNumber);
	      }

	    }
	  }
	  // otherwise make sure the its not a comment and make sure its not .word or .reswr
	  else if(token[0] == '#'){
	   
	  }
	  else if(strcmp(token,".resw") == 0){
	    lineNumber++;
	  }
	  else if(strcmp(token,".word") == 0){
	    lineNumber++;
	  }
	  // because its not a label the first token is then an opcode, so check if its valid, otherwise output to error file
	  else{
	    //printf("its an opcode %s\n",token);
	    if((check = getOpCode(token)) == NULL){
	      // printf("invalid opcode \n");
	      fprintf(errfp,"Invalid opcode %s at line %d\n",token,lineNumber); 
	    }
	  }
	  //printf("%s\n",token);

	
	lineNumber++;
      }

    }
    fprintf(errfp,"\nMultiply defined symbols: \n");
    // traverse through the error linked list and print out the names of the multiply defined symbols on to the erorr file.
    for(pointer = ehead;pointer!=NULL;pointer=pointer->next){
      fprintf(errfp,"%s\n",pointer->error);
    }
    if(fclose(errfp) == EOF){
      fprintf(stderr,"Error occured with closing of error file\n");
    }
    return;

}
/* This function will take in a string that is the name of a symbol that is multiply defined and add it to the error linked list. It will return nothing.
 */
void addError(char *error){
  ERROR *current;
  ERROR *temp;
  //allocate space for a node of struct error
  if((temp = malloc(sizeof(ERROR))) == NULL){
    fprintf(stderr,"Error occured with malloc\n");
    exit(1);
  }
  strcpy(temp->error,error);
  temp->next = NULL;
  // check if this is the first error node, then make ehead  point to it otherwise traverse the linked list until you find the last node then add the temp there
  if(ehead == NULL){
    ehead = temp;
    current = ehead;
  }
  else{
    current = ehead;
    while(current->next!=NULL){
      current = current->next;
    }
    current->next = temp;
   
  }
}
/* This is the second pass of the assembler. It takes in the source file pointer, an int that tells us whether there was an error during the first pass as well as the root name. If there was an error it will open the error file for appending and check for undefined symbols.Otherwise it will check for undefined symbols and open the error file for writing . Based on if it finds any undefined it will set a flag which is used to determine if the actual assembly should be done. This function returns nothing.
 */
void secondPass(FILE *sourcefp,int error,char *rootName){
  int i; // temp counter
  char line[LINE_LENGTH]; // holds a line of the source file
  char rootNameCopy[LINE_LENGTH+EXTENSION_LENGTH];// holds the rootname+extension
  FILE *objfp; // object file pointer
  FILE *symfp; // symbol file pointer
  FILE *errfp; // error file pointer
  char *token; // token pointer for tokenization
  char *firstop; // pointer to the first operand
  char *secondop; // pointer to the second operand
  char *thirdop; // pointer to the third operand
  OPCODE *tempopcode; // holds an opcode node
  SYMBOL *tempsymbol; // holds a symbol node
  int value; // this holds the value of a .word directive
  char opcode[LINE_LENGTH]; // name of the opcode
  int undefined = 0; // this tells us if there were any undefined symbols in the file
  char *errString = {".err"}; // extension for error file
  char *symString = {".sym"}; // extension for symbol file
  char *objString = {".obj"}; // extension for object file
  int location = 0; // holds the location counter
  int increment = 0;// this tells us if we have to increment the location counter or not. we dont have to increment if we find a .word or .resw because the LC gets changed automatically. 
  // go through the memory array and initialize everything to zero.
  int line_number = 0;
  
  // if there were illegal opcodes or multiply defined symbols run this section
  if(error == 1){
    strcpy(rootNameCopy,rootName);
    strcat(rootNameCopy,errString);
    // get the name of the error file then open up the error file for appending
    if((errfp = fopen(rootNameCopy,"a")) == NULL){
      fprintf(stderr,"Error occured with opening file %s \n",rootNameCopy);
      exit(1);
    }
    fprintf(errfp,"\nFollowing symbols were undefined: \n");
    rewind(sourcefp);// start at the start of the file
    while(fgets(line,LINE_LENGTH,sourcefp) != NULL){
      // skip any blank or comment lines
      if(line[0] == '#'){
	line_number++;
      }

      else if(line[0]== '\n'){
	line_number++;
      }
      else if(line[0] == '\0'){
	line_number++;
      }

      else{
	line_number++;
	// get the first token then check if its .text or .data
	token = strtok(line," \t\n");
	if(strcmp(token,".text") == 0){
	}
	else if(strcmp(token,".data") == 0){
	}
	//otherwise check if its a label
	else if(token[strlen(token)-1] == ':'){
	  token = strtok(NULL," \t\n");
	  // printf(token);
	  if(strcmp(token,".word") == 0){
	  }
	  else if(strcmp(token,".resw") == 0){
	  }
	  else{
	    // printf(token);
	    // printf("\n");
	    // get the opcode and check if its of type J
	    strcpy(opcode,token);
	    tempopcode = getOpCode(opcode);
	    if(tempopcode !=NULL){
	    //printf("%c\n",tempopcode->format);
	    if(tempopcode->format == 'J'){
	      //printf("J instrction found\n");
	      token = strtok(NULL," \t\n");
	      //printf("%s\n",token);
	      //get the first opcode and if its not null then see if its a register. if its not a register that means its a label so check if its defined
	      firstop = strtok(token," , \t\n ");
	      if(firstop !=NULL){
		//printf("%s\n",firstop);
		if(firstop[0]== '$'){
		  // printf("Its a register \n");
		}
		else{
		  if(isSymbol(firstop) == 0){
		    //startError(sourcefp,rootName);
			undefined = 1;
		    fprintf(errfp,"%s\n",firstop);
		  }
		}
	      }
	      // do the same stuff for the second operand if its not null
	      secondop = strtok(NULL," ,\t\n");
	      if(secondop!=NULL){
		//printf("%s\n",secondop);
		if(secondop[0]== '$'){
                  //printf("Its a register \n");
                }
                else{
                  if(isSymbol(secondop) == 0){
		    //startError(sourcefp,rootName);
					undefined = 1;
                    fprintf(errfp,"%s\n",secondop);
                  }
                }

	      }
	      // do the same thing for the third operand if its not nulll
	      thirdop = strtok(NULL," ,\t\n");
	      if(thirdop!=NULL){
		//printf("%s\n",thirdop);
		if(thirdop[0]== '$'){
		  // printf("Its a register \n");
                }
                else{
                  if(isSymbol(thirdop) == 0){
		    // startError(sourcefp,rootName);
					undefined = 1;
                    fprintf(errfp,"%s\n",thirdop);
                  }
                }

	      }
   	    }
	    }
	  }
	}
	// otherwise the first token is not a label, so check if its one of the pseudo codes and if its not then do the same thing as above without having to skip the label
	else{
	  //printf("%s\n",token);
	  if(strcmp(token,".word") == 0){
          }
          else if(strcmp(token,".resw") == 0){
          }
          else{
            // printf(token);
            // printf("\n");
            strcpy(opcode,token);
            tempopcode = getOpCode(opcode);
            if(tempopcode !=NULL){
	      //printf("%c\n",tempopcode->format);
	      if(tempopcode->format == 'J'){
		//printf("J instrction found\n");
		token = strtok(NULL," \t\n");
		//printf("%s\n",token);
		firstop = strtok(token," , \t\n ");
		if(firstop !=NULL){
		  //printf("%s\n",firstop);
		  if(firstop[0]== '$'){
		    //printf("Its a register \n");
		  }
		  else{
		    if(isSymbol(firstop) == 0){
		      // startError(sourcefp,rootName);
			  undefined = 1;
		      fprintf(errfp,"%s\n",firstop);
		    }
		  }
		}
		secondop = strtok(NULL," ,\t\n");
		if(secondop!=NULL){
		  //printf("%s\n",secondop);
		  if(secondop[0]== '$'){
		    //printf("Its a register \n");
		  }
		  else{
		    if(isSymbol(secondop) == 0){
		      //startError(sourcefp,rootName);
			  undefined = 1;
		      fprintf(errfp,"%s\n",secondop);
		    }
		  }

		}
		thirdop = strtok(NULL," ,\t\n");
		if(thirdop!=NULL){
		  // printf("%s\n",thirdop);
		  if(thirdop[0]== '$'){
		    //printf("Its a register \n");
		  }
		  else{
		    if(isSymbol(thirdop) == 0){
		      //startError(sourcefp,rootName);
			  undefined = 1;
		      fprintf(errfp,"%s\n",thirdop);
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
    // close the error file
    if(fclose(errfp) == EOF){
      fprintf(stderr,"Error occured with closing of error file \n");
    }
  }  // this else gets run if there were no multiply defined symbols or illegal opcodes. We have to check for undfined symbols no matter what.
	else{
	strcpy(rootNameCopy,rootName);
    strcat(rootNameCopy,errString);
    // get the name of the error file then open up the error file for writing
    if((errfp = fopen(rootNameCopy,"w")) == NULL){
      fprintf(stderr,"Error occured with opening file %s \n",rootNameCopy);
      exit(1);
    }
    line_number = 1;
    rewind(sourcefp);
    // copy the source file and the line number to the error file
    while(fgets(line,LINE_LENGTH,sourcefp) !=NULL){
      fprintf(errfp,"%d \t %s",line_number++,line);
    }
    fprintf(errfp,"\nFollowing symbols were undefined: \n");
    rewind(sourcefp);
    // start at the begining of the source file and run line by line till end, here we do the exact same thing as the above if statement except we opeoened the file for writing instead of appending
    while(fgets(line,LINE_LENGTH,sourcefp) != NULL){
      // skip any blank or comment lines
      if(line[0] == '#'){
	line_number++;
      }

      else if(line[0]== '\n'){
	line_number++;
      }
      else if(line[0] == '\0'){
	line_number++;
      }

      else{
	line_number++;
	token = strtok(line," \t\n");
	if(strcmp(token,".text") == 0){
	}
	else if(strcmp(token,".data") == 0){
	}
	//do this section if the first token is a label
	else if(token[strlen(token)-1] == ':'){
	  token = strtok(NULL," \t\n");
	  // printf(token);
	  if(strcmp(token,".word") == 0){
	  }
	  else if(strcmp(token,".resw") == 0){
	  }
	  else{
	    // printf(token);
	    // printf("\n");
	    // get the opcode and check if its of type J
	    strcpy(opcode,token);
	    tempopcode = getOpCode(opcode);
	    if(tempopcode !=NULL){
	    //printf("%c\n",tempopcode->format);
	    if(tempopcode->format == 'J'){
	      //printf("J instrction found\n");
	      token = strtok(NULL," \t\n");
	      //printf("%s\n",token);
	      //get the first opcode and if its not null then see if its a register. if its not a register that means its a label so check if its defined
	      firstop = strtok(token," , \t\n ");
	      if(firstop !=NULL){
		//printf("%s\n",firstop);
		if(firstop[0]== '$'){
		  // printf("Its a register \n");
		}
		else{
		  if(isSymbol(firstop) == 0){
		    //startError(sourcefp,rootName);
			undefined = 1;
		    fprintf(errfp,"%s\n",firstop);
		  }
		}
	      }
	      // do the same stuff for the second operand if its not null
	      secondop = strtok(NULL," ,\t\n");
	      if(secondop!=NULL){
		//printf("%s\n",secondop);
		if(secondop[0]== '$'){
                  //printf("Its a register \n");
                }
                else{
                  if(isSymbol(secondop) == 0){
		    //startError(sourcefp,rootName);
					undefined = 1;
                    fprintf(errfp,"%s\n",secondop);
                  }
                }

	      }
	      // do the same thing for the third operand if its not nulll
	      thirdop = strtok(NULL," ,\t\n");
	      if(thirdop!=NULL){
		//printf("%s\n",thirdop);
		if(thirdop[0]== '$'){
		  // printf("Its a register \n");
                }
                else{
                  if(isSymbol(thirdop) == 0){
		    // startError(sourcefp,rootName);
					undefined = 1;
                    fprintf(errfp,"%s\n",thirdop);
                  }
                }

	      }
   	    }
	    }
	  }
	}
	// otherwise the first token is not a label, so check if its one of the pseudo codes and if its not then do the same thing as above
	else{
	  //printf("%s\n",token);
	  if(strcmp(token,".word") == 0){
          }
          else if(strcmp(token,".resw") == 0){
          }
          else{
            // printf(token);
            // printf("\n");
            strcpy(opcode,token);
            tempopcode = getOpCode(opcode);
            if(tempopcode !=NULL){
	      //printf("%c\n",tempopcode->format);
	      if(tempopcode->format == 'J'){
		//printf("J instrction found\n");
		token = strtok(NULL," \t\n");
		//printf("%s\n",token);
		firstop = strtok(token," , \t\n ");
		if(firstop !=NULL){
		  //printf("%s\n",firstop);
		  if(firstop[0]== '$'){
		    //printf("Its a register \n");
		  }
		  else{
		    if(isSymbol(firstop) == 0){
		      // startError(sourcefp,rootName);
			  undefined = 1;
		      fprintf(errfp,"%s\n",firstop);
		    }
		  }
		}
		secondop = strtok(NULL," ,\t\n");
		if(secondop!=NULL){
		  //printf("%s\n",secondop);
		  if(secondop[0]== '$'){
		    //printf("Its a register \n");
		  }
		  else{
		    if(isSymbol(secondop) == 0){
		      //startError(sourcefp,rootName);
			  undefined = 1;
		      fprintf(errfp,"%s\n",secondop);
		    }
		  }

		}
		thirdop = strtok(NULL," ,\t\n");
		if(thirdop!=NULL){
		  // printf("%s\n",thirdop);
		  if(thirdop[0]== '$'){
		    //printf("Its a register \n");
		  }
		  else{
		    if(isSymbol(thirdop) == 0){
		      //startError(sourcefp,rootName);
			  undefined = 1;
		      fprintf(errfp,"%s\n",thirdop);
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
    // close the error file
    if(fclose(errfp) == EOF){
      fprintf(stderr,"Error occured with closing of error file \n");
    }
  }
	
	
  
  // Here we check if there were any errors caught by the first pass or any errors caught by the second pass, if there arent do the assembling and print the symbol table
  if(undefined == 0 && error == 0){
    //intialize the memory to be zero
	for(i = 0;i < MAX_MEMORY; i++){
		memory[i] = 0;
	}	
	remove(rootNameCopy);
	// get and open the symbol file
    strcpy(rootNameCopy,rootName);
    strcat(rootNameCopy,symString);
    // printf("%s\n",rootNameCopy);
    if((symfp = fopen(rootNameCopy,"w"))== NULL){
      fprintf(stderr,"Error occured with opening of symbol file \n");
      exit(1);
    }
    // loop through the symbol linked list and print it to the symbol file.
    for(tempsymbol = shead;tempsymbol != NULL; tempsymbol = tempsymbol->next){
      fprintf(symfp,"%s    \t  %d \n",tempsymbol->symbol,tempsymbol->lc);
    }
    // close the symbol file.
    if( fclose(symfp) == EOF){
      fprintf(stderr,"Error occureding with closing of symbol file\n");
    }    
    // get and open the object file
    strcpy(rootNameCopy,rootName);
    strcat(rootNameCopy,objString); // get the object file name
    //  printf(rootNameCopy);
    //printf("\n");
    
    if((objfp = fopen(rootNameCopy,"w")) == NULL){
      fprintf(stderr,"Error occured with opening of object file.\n");
      exit(1);
    }
    rewind(sourcefp);
    while(fgets(line,LINE_LENGTH,sourcefp) != NULL){
      if(line[0] == '#'){
	
      }
      else if(line[0] == '\n'){
	
      }
      else if(line[0] == '\0'){
      }
      else{
	token = strtok(line," \t\n ");// get the first token
	// if its either .text or .data ignore it
	if(strcmp(token,".text") == 0){
	  increment = 0;
	}
	else if(strcmp(token,".data")==0){
	  increment = 0;
	   }
	else if(token[strlen(token)-1] == ':'){
	  token = strtok(NULL," \t\n ");// get the next token which is the opcode since the fist token was the label
	  // printf("%s\n",token);
	  // if this token is the pseduo directive .word
	  if(strcmp(token,".word") == 0){
	    token = strtok(NULL," : ");// get the number to the left of the :, which is the number to intilize it.
	    value = atoi(token);
	    // printf("%d\n",value);
	    token = strtok(NULL,"\t \n "); // this is the number of spaces we need to skip
	    // start at the current location then set the index values to the value on the left until we reach the number on the right
	       // printf("location: %d \t location+token: %d\n",location,location+atoi(token));
	    for( i = location; i < location+atoi(token);i++){
	      memory[i] = value;
	      //printf("%d\n",memory[i]);
	    }
	    //printf("%s\n",token);
	    increment = 0;
	    location += atoi(token);
	  }
	  else if(strcmp(token,".resw")== 0){
	    token = strtok(NULL," \t\n");
	    location+=atoi(token);
	    increment = 0;
	  }
	  else{
	    increment = 1;
	    tempopcode = getOpCode(token); // get the opcode node for the current opcode, then depending on the format run the appropriate function
	       token = strtok(NULL," \t\n");
	       // printf("%s\n",token);
	       //printf("%s %d %c \n",tempopcode->mnemonic,tempopcode->code,tempopcode->format);
	       // also get all three operands using strtok
	       firstop = strtok(token," , \t\n#");
	       if(firstop !=NULL){
		 //printf("%s\t",firstop);
	       }
	       secondop = strtok(NULL," , \t\n#");
	       if(secondop!=NULL){
		 //printf("%s\t",secondop);
	       }
	       thirdop = strtok(NULL," ,\t\n#");
	       if(thirdop!=NULL){
		 //printf("%s\t",thirdop);
	       }
	       //printf("\n");
	     
	       if(tempopcode->format == 'J'){
			jFormat(location,tempopcode->code,firstop,secondop,thirdop);
	       }
	       else if(tempopcode->format == 'I'){
		 iFormat(location,tempopcode->code,firstop,secondop,thirdop);
	       }
	       else if(tempopcode->format == 'R'){
		 rFormat(location,tempopcode->code,firstop,secondop,thirdop);
	       }
	     }
	   }
	   else{
	     // samething as above if statement except the first token is the opcode so no need to skip it
	     //printf("%s\n",token);
	    
	     if(strcmp(token,".word") == 0){	      	       
	       increment = 0;
	     }
	     else if(strcmp(token,".resw")==0){
	       increment = 0;
	     }
	     else{
	       increment = 1;
	       tempopcode = getOpCode(token);
               token = strtok(NULL," \t\n");
               // printf("%s\n",token);
	       // printf("%s %d %c \n",tempopcode->mnemonic,tempopcode->code,tempopcode->format);
               firstop = strtok(token," , \t\n#");
               if(firstop !=NULL){
                 //printf("%s\t",firstop);
               }
               secondop = strtok(NULL," , \t\n#");
               if(secondop!=NULL){
                 //printf("%s\t",secondop);
               }
	       thirdop = strtok(NULL,", \t\n#");
               if(thirdop!=NULL){
                 // printf("%s\t",thirdop);
               }
	       //printf("\n");
	       if(tempopcode!=NULL){
		 if(tempopcode->format == 'J'){
		   jFormat(location,tempopcode->code,firstop,secondop,thirdop);
		 }
		 else if(tempopcode->format == 'I'){
		   iFormat(location,tempopcode->code,firstop,secondop,thirdop);
		 }
		 else if(tempopcode->format == 'R'){
		   // printf("The opcode is: %s\n",tempopcode->mnemonic);
		   rFormat(location,tempopcode->code,firstop,secondop,thirdop);
		 }
	       }
	       
	     }
	   }
	   if(increment){
	     location++;
	   }   
      }
    }
    // print the memory array up to the last location we used
    for(i = 0; i < location;i++){
      fprintf(objfp,"%X \t %X \n",i,memory[i]);
    }
    if(fclose(objfp) == EOF){
      fprintf(stderr,"Error occured with closing of object file\n");
    }
  }
}

/* This function and the next two functions follow the same procedure, they take in the current location, the opcode numeric code, and three opcodes. It assembles the object file by using an int to store the values and shifting that int before binary oring it with the current location. It  returns nothing.
 */
void rFormat(int location,int opcode,char* firstop, char *secondop,char *thirdop){
  int temp;// this is the value that we will use to mask
  temp = opcode; // first get the opcode numeric code
  temp = temp<<OPCODE_SHIFT; // shift it all the way to the left and or it with the current value at current location
  memory[location] = memory[location] | temp;
  if(firstop!=NULL){
    // if the first op isnt empty then check if its a register, if it is then remove the $ sign and set temp equal to it. Then shift it to the rigth spot and or it with the current location index value
    if(firstop[0] == '$'){
      memmove(firstop,firstop+1,strlen(firstop));
      //printf("Firstop = %s\n",firstop);
      temp = atoi(firstop);
      temp = temp << REGT_SHIFT;
      memory[location] = memory[location] | temp;
    }
  }
  // do the samething for the second operand
  if(secondop!=NULL){
    if(secondop[0] == '$'){
      memmove(secondop,secondop+1,strlen(secondop));
      //printf("Secondop = %s\n",secondop);
      temp = atoi(secondop);
      temp = temp << REGS1_SHIFT;
      memory[location] = memory[location] | temp;
    }
  }
  // do the samething with the third op, except this time we also have to check if its simply and int and not a register because if its an int then its a shift amount
  if(thirdop!=NULL){
    if(thirdop[0] == '$'){
      memmove(thirdop,thirdop+1,strlen(thirdop));
      //printf("Thirdop = %s \n",thirdop);
      temp = atoi(thirdop);
      temp = temp << REGS2_SHIFT;
      memory[location] = memory[location] | temp;
    }
  
    else{
      temp = atoi(thirdop);
      temp = temp << SHIFTAMT_SHIFT;
      memory[location] = memory[location] | temp;
    }
  }

}
/* same thing as the above function except for I functions, thus the shifting places are different but the method is the same */
void iFormat(int location, int opcode, char* firstop, char* secondop, char *thirdop){
  char *reg;
  int temp;
  char *immediate;
  temp = opcode;
  temp = temp << OPCODE_SHIFT;
  memory[location] = memory[location] | temp;
  if(firstop != NULL){
    if(firstop[0] == '$'){
      memmove(firstop,firstop+1,strlen(firstop));
      temp = atoi(firstop);
      temp = temp<< REGS2_SHIFT;
      memory[location] = memory[location] | temp;
    }
    
  }
  if(secondop!=NULL){
    if(secondop[0] == '$'){
      memmove(secondop,secondop+1,strlen(secondop));
      temp = atoi(secondop);
      temp = temp << REGS1_SHIFT;
      memory[location] = memory[location] | temp;
    }
    // with the i format the second operand can be a base+ displacement form, so we have to check if the second op has a ( in it. if it does we have to strtok to get the number before the ( and the number after the ( use 0x0000ffff to ensure if its a negative number that the left 16 bits are still zeroes. then do the same thing as above.
    else if(strrchr(secondop, '('   ) != NULL){
      immediate = strtok(secondop," ( \t");
      // printf("%s\t",immediate);
      reg = strtok(NULL,"$ \t \n");
      reg[strlen(reg)-1] = '\0';
      // printf("%s \n",reg);
      temp = atoi(reg);
      temp = temp << REGS1_SHIFT;
      memory[location] = memory[location] | temp;
      temp = atoi(immediate);
      // printf("%d\n",temp);
      temp = temp & CLEAR_LEFT;
      //printf("%d \n",temp);
      memory[location] = memory[location] | temp;
    }
    else{
      temp = atoi(secondop);
      temp = temp & CLEAR_LEFT;
      memory[location] = memory[location] |temp;
    }
  }
  if(thirdop !=NULL){
    temp = atoi(thirdop);
    temp = temp & CLEAR_LEFT;
    memory[location] = memory[location] | temp;

  }




}
/* This function is the same as above function */
void jFormat(int location,int opcode, char *firstop,char *secondop, char *thirdop){
  int temp;
  SYMBOL *stemp;
  temp = opcode;
  temp = temp << OPCODE_SHIFT;
  memory[location] = memory[location] | temp;
  if(firstop!= NULL){
    if(firstop[0] == '$'){
      memmove(firstop,firstop+1,strlen(firstop));
      temp = atoi(firstop);
      temp = temp << REGS2_SHIFT;
      memory[location] = memory[location] | temp;
    }
    else{
      stemp = getSymbol(firstop);
      if(stemp !=NULL){
	temp = stemp->lc;
	temp = temp & 0x0000FFFF;
	memory[location] = memory[location] | temp;
      }
    }

  }
  if(secondop !=NULL){
    if(secondop[0] == '$'){
      memmove(secondop,secondop+1,strlen(secondop));
      temp = atoi(secondop);
      temp = temp << REGS1_SHIFT;
      memory[location] = memory[location] | temp;
    }
    else{
      // j format is the only time we can have a label, so if its not a register then its a label. so get the node with the current symbol and get the location of that symbol
      stemp = getSymbol(secondop);
      if(stemp !=NULL){
	temp = stemp->lc;
	temp = temp & CLEAR_LEFT;
	memory[location] = memory[location] | temp;
      }
    }
  }
  if(thirdop != NULL){
    if((stemp = getSymbol(thirdop)) !=NULL){
      temp = stemp->lc;
      temp = temp & CLEAR_LEFT;
      memory[location] = memory[location] | temp;
    }
  }

}
