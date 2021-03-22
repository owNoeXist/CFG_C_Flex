#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <io.h>

#define NORMAL_STATE 0
#define IF_STATE 1
#define ELSEIF_STATE 2
#define ELSE_STATE 3
#define FOR_STATE 4
#define WHILE_STATE 5
#define DO_STATE 6
#define SWITCH_STATE 7
#define CASE_STATE 8
#define DEFAULT_STATE 9
#define BREAK_STATE 10
#define CONTINUE_STATE 11
#define GOTO_STATE 12
#define LAB_STATE 13
#define RETURN_STATE 14
#define LEFT_BRACE_STATE 15
#define RIGHT_BRACE_STATE 16
#define LEFT_BRACKET_STATE 17
#define RIGHT_BRACKET_STATE 18
#define LEFT_MIDBRACE_STATE 19
#define RIGHT_MIDBRACE_STATE 20
#define COLON_STATE 21
#define COMMA_STATE 22
#define SELMICOLON_STATE 23
#define NUM_STATE 24
#define STRING_STATE 25 
#define STR_STATE 26
#define MYS_STATE 27
#define NEWLINE_STATE 28
#define KEY_STATE 30
#define CAL_STATE 40
#define BUF_STATE 50

#define INSTRUCTION_TRANSFER 1
#define INSTRUCTION_ARITHMETICAL 2
#define INSTRUCTION_LOGICAL 3
#define INSTRUCTION_CLUSTER 4
#define INSTRUCTION_JUMP 5
#define INSTRUCTION_CALL 6
#define INSTRUCTION_TRANSFER_ARITHMETICAL 7
#define INSTRUCTION_TRANSFER_LOGICAL 8
#define INSTRUCTION_RETURN 9
#define INSTRUCTION_LEFT 10
#define INSTRUCTION_RIGHT 11


#define MAX_STRING 5000
#define MAX_SEQUENCE 200

struct Node {
	int nodenum;
	int startline;
	int endline;
	int nextnode;
	int jumpnode;
	int literal[7];
	int semantic[MAX_SEQUENCE];
}*node;

/*Calculate.ida file*/
int CheckI64(char* srcDir);
/*add "" to dir*/
int GenerateDir(char* strings);

/*Clear annotations and precompile*/
//#pragma comment(lib,"Pretreatment.lib")
//__declspec(dllimport) int CleanAnnotation(char* srcdir, char* destdir);
/*Judge which type of state the state belong to*/
extern int CheckState(char* state);
extern int CheckToken(char* state);
/*Lexical analyzsis of C-file and repair line divide*/
extern int LexicalEntrance(char* srcfiles, char* destfiles);
/*Generate CFG of one function*/
extern int GenerateCFG(char* filename, int* nodeNum);
/*Generate Feature for each block of one function*/
extern int GnerateFeature(char* filename, int Nodenum);
/*Generate jSON style data*/
extern int GenerateJson(char* dir, char* filename, int Nodenum);