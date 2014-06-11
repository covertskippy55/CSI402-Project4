/* Name: Dilan Samarasinghe
   ID: DS697678
*/

typedef struct opcode{
  int code; // holds the decimal opcode value
  char mnemonic[OPCODE_LENGTH]; // holds the mnemonic string for opcode
  char format; // holds what type of instruction it is
  struct opcode *next; // pointer to next opcode node
}OPCODE;

typedef struct symbol_table{
  int lc; // holds the location of the symbol
  char symbol[LINE_LENGTH]; // holds the symbol name
  struct symbol_table *next;// pointer to the next symbol node
}SYMBOL;

typedef struct error{
  char error[LINE_LENGTH];
  struct error *next;
}ERROR;
