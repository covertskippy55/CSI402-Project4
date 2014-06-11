/* Name: Dilan Samarasinghe
   ID: DS697678
*/

void makeOpCodeTable();
OPCODE* getOpCode(char*);
int firstPass(FILE*,char*);
int isSymbol(char*);
void addSymbol(char*,int);
void startError(FILE*,char*);
void addError(char*);
void secondPass(FILE*,int,char*);
void printSymbol();
SYMBOL* getSymbol(char*);
void rFormat(int,int,char*,char*,char*);
void iFormat(int,int,char*,char*,char*);
void jFormat(int,int,char*,char*,char*);
