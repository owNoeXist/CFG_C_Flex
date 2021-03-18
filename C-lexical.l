/*Parameter and variate*/
%option noyywrap nodefault

%x FUNCTION

UCN	(\\u[0-9a-fA-F]{4}|\\U[0-9a-fA-F]{8})
EXP ([Ee][-+]?[0-9]+)
ILEN ([Uu](L|l|LL|ll)?|(L|l|LL|ll)[Uu]?) 

%{
#include<FuncDef.h>

/*Declare structure*/
struct SCOPE{
	int bracket;
	int brace;
}scopeS;

struct FUNCTION{
	int sign;
	int func;
	int inlines;
	char CurFuncName[100];
}functionS;

struct NOFUNCTION{
	int define;
	int assign;
	int brace;
}noS;

struct BUF{
	int num;
	char bufs[1000][260];
}bufS;

struct GOTO{
	int marks;
	int gotos;
	int gotonum;
	char CurGotoName[100];
	char GotoName[100][100];
}gotoS;

/*Declare variables and keywords*/
char *curfilename;
int tempnum=0;
char numstring[10];

FILE *fpr,*fpw;

int IntToString(int num, char* str);
int Lexical(char* filename);
void ScopeBranch(FILE* fpr, FILE* fpw);
void StateBranch(FILE* fpr, FILE* fpw, int sign);
%}


%%
_Bool|_Complex|_Imaginary {fputs("KEY_0 ",fpw);}
auto|extern|register|static|restrict {fputs("KEY_1 ",fpw);}
inline|__inline|__inline__ {fputs("KEY_2 ",fpw);}
signed|unsigned {fputs("KEY_3 ",fpw);}
char|int|long|short|void {fputs("KEY_4 ",fpw);}
doubel|float {fputs("KEY_5 ",fpw);}
enum|struct|union {
	fputs("KEY_6 ",fpw);
	if(scopeS.bracket==0){
		functionS.sign=0;
		noS.define=1;
	}}
sizeof|const|typedef|volatile  {fputs("KEY_7 ",fpw);}


if {fputs("if ",fpw);}
else {fputs("else ",fpw);}
do {fputs("do ",fpw);}
while {fputs("while ",fpw);}
for {fputs("for ",fpw);}
switch {fputs("switch ",fpw);}
case {
	fputs("case ",fpw);
	gotoS.marks=0;}
default {
	fputs("default ",fpw);
	gotoS.marks=0;}
break {fputs("break ",fpw);}
continue {fputs("continue ",fpw);}
goto {
	fputs("goto ",fpw);
	gotoS.gotos=1;}
return {fputs("return ",fpw);}


0[0-7]*{ILEN}? |
[1-9][0-9]*{ILEN}? |
0[Xx][0-9a-fA-F]+{ILEN}? |
([0-9]*\..[0-9]+|[0-9]+\.){EXP}?[flFL]? |
[0-9]+{EXP}[flFL]? |
0[Xx]([0-9a-fA-F]*\.[0-9a-fA-F]+|[0-9a-fA-F]+\.?)[Pp][-+]?[0-9]+[flFL]? {fputs("NUM ",fpw);}



\'([^'\\]|\\['"?\\abfnrtv]|\\[0-7]{1,3}|\\[Xx][0-9a-fA-F]+|{UCN})+\' |
L?\"([^"\\]|\\['"?\\abfnrtv]|\\[0-7]{1,3}|\\[Xx][0-9a-fA-F]+|{UCN})*\" {fputs("STRING ",fpw);}



";" {
	fputs("; ",fpw);
	if(functionS.func==0)
		functionS.sign=0;
	else
		gotoS.marks=1;
	if(noS.define==0&&noS.assign==0&&scopeS.bracket==0)
		fputc('\n',fpw); 
	else if(noS.brace==0&&scopeS.bracket==0){
		noS.define=0;
		noS.assign=0;
		fputc('\n',fpw);
	}
}


"("	{
	fputs("( ",fpw);
	scopeS.bracket++;
	if(functionS.sign==1&&noS.assign==0){
		functionS.sign=2;
	}}
")" {
	fputs(") ",fpw);
	scopeS.bracket--;}
"["	{fputs("[ ",fpw);}
"]"	{fputs("] ",fpw);}
"{" {
	scopeS.brace++;
	if(scopeS.bracket!=0){
		fputs("{ ",fpw);
	}
	else if(functionS.func==0){
		if(functionS.sign==2){
			fputs("{ ",fpw);
			if(functionS.inlines==0){
				fputs("FUNCTION-",fpw);
				fwrite(functionS.CurFuncName,sizeof(char),strlen(functionS.CurFuncName),fpw);
			}
			fputc('\n',fpw);
			functionS.func=1;
			bufS.num=0;
			gotoS.marks=1;
			gotoS.gotos=0;
			gotoS.gotonum=0;
			noS.define=0;
		}
		else if(noS.assign==1||noS.define==1){
			noS.brace++;
			fputs("{ ",fpw);
		}
		else{
			fputs("\n{ \n",fpw);
		}
	}
	else{
		if(noS.assign==1||noS.define==1){
			noS.brace++;
			fputs("{ ",fpw);
		}
		else{
			fputs("\n{ \n",fpw);
		}
	}}
"}" {
	scopeS.brace--;
	if(scopeS.bracket!=0){
		fputs("} ",fpw);
	}
	else if(noS.brace>0){
		fputs("} ",fpw);
		noS.brace--;
	}
	else{
		fputs("\n} ",fpw);
	 	if(functionS.func==1&&scopeS.brace==0){
			functionS.sign=0;
			functionS.func=0;
			if(functionS.inlines==0){
				fputs("FUNCTION-",fpw);
				fwrite(functionS.CurFuncName,sizeof(char),strlen(functionS.CurFuncName),fpw);
			}
			functionS.inlines=0;
		}
		fputc('\n',fpw);
	}}
"."|"->"  {fputs("STR ",fpw);}

"++"|"--"|"+"|"-"|"/"|"%"|"*" {fputs("CAL_0 ",fpw);}
"&"|"~"|"!" {fputs("CAL_1 ",fpw);}
"<<"|">>" {fputs("CAL_2 ",fpw);}
"<"|">"|"<="|">="|"=="|"!=" {fputs("CAL_3 ",fpw);}
"^"|"|"|"&&"|"||" {fputs("CAL_4 ",fpw);}
"*="|"/="|"%="|"+="|"-=" {fputs("CAL_5 ",fpw);}
"<<="|">>="|"&="|"^="|"|=" {fputs("CAL_6 ",fpw);}
"=" {
	fputs("CAL_7 ",fpw);
	if(scopeS.bracket==0){
		functionS.sign=0;
		noS.assign=1;
	}}
"?"	{
	fputs("CAL_8 ",fpw);
	gotoS.marks=-1;}
":" {
	/*The colon belongs to goto statement's mark*/
	if(functionS.func==1){
		/*Ignore the colon in bracket*/
		if(scopeS.bracket!=0){
			fputs("CAL_9  ",fpw);
		}
		/*The colon belongs to label*/
		else if(gotoS.marks==2){
			for(tempnum=0;tempnum<gotoS.gotonum;tempnum++){
				if(strcmp(gotoS.GotoName[tempnum],gotoS.CurGotoName)==0)
					break;
			}
			if(tempnum==gotoS.gotonum){
				strcpy(gotoS.GotoName[tempnum],gotoS.CurGotoName);
				gotoS.gotonum++;
			}
			fputs("LAB-",fpw);
			IntToString(tempnum,numstring);
			fwrite(numstring, sizeof(char), strlen(numstring), fpw);
			fputs(": \n",fpw);
			gotoS.marks=1;
		}
		/*The colon belongs to case or default*/
		else if(gotoS.marks==0){
			fputs(": \n",fpw);
			gotoS.marks=1;
		}
		else{
			fputs("CAL_9 ",fpw);
		}
	}
	else
		fputs("CAL_9  ",fpw);
	}
"," {
	fputs(", ",fpw);
	if(functionS.func==0&&functionS.sign==1)
		functionS.sign==0;}


"#"|"##" {fputs("MYS ",fpw);}
"..."|"<%"|"<:"|":>"|"%>"|"%:"|"%:%:" {fputs("MYS ",fpw);}


([_a-zA-Z]|{UCN})([_a-zA-Z0-9]|{UCN})* {
	/*Record function name*/
	if(functionS.func==0){
		fputs("VAR ",fpw);
		if(scopeS.bracket==0){
			strcpy(functionS.CurFuncName,yytext);
			functionS.sign=1;
		}
	}
	/*Record parameter name*/
	else{
		for(tempnum=0;tempnum<bufS.num;tempnum++){
			if(strcmp(bufS.bufs[tempnum],yytext)==0){
				break;
			}
		}
		if(tempnum==bufS.num){
			strcpy(bufS.bufs[bufS.num],yytext);
			bufS.num++;
		}
		fputs("BUF_",fpw);
		IntToString(tempnum,numstring);
		fwrite(numstring, sizeof(char), strlen(numstring), fpw);
		/*Record goto statement's label name*/
		if(gotoS.marks==1){
			strcpy(gotoS.CurGotoName,yytext);
			gotoS.marks=2;
		}
		/*Record name that goto statement lead*/
		if(gotoS.gotos==1){
			for(tempnum=0;tempnum<gotoS.gotonum;tempnum++){
				if(strcmp(gotoS.GotoName[tempnum],yytext)==0)
					break;
			}
			if(tempnum==gotoS.gotonum){
				strcpy(gotoS.GotoName[tempnum],yytext);
				gotoS.gotonum++;
			}
			fputs("JUM-",fpw);
			IntToString(tempnum,numstring);
			fwrite(numstring, sizeof(char), strlen(numstring), fpw);
			gotoS.gotos=0;
		}
	}}


[ \t\n]+ 
<<EOF>> {fputc('\n',fpw);yyterminate();}
. {printf("%s:Mystery character '%s'\n",curfilename,yytext);}

%%
/*=============================================================================================================*/
/*------------------------------Transfer int to string-------------------------------*/
int IntToString(int num, char* str)
{
	int i=0,j=0; 
	char temp[10]="";
	if(num!=0){
		while(num){
        	temp[i++]=num%10+'0';
        	num/=10;  
    	}
    	while(i>0)
        	str[j++]=temp[--i];
    	str[j]=' ';
    	str[j+1]='\0';
	}
	else{
		strcpy(str, "0 ");
	}
	return 0;
}
/*------------------------Delete information that gcc added--------------------------*/
int DelSharp(char* srcfile, char* destfile)
{
	char buf[MAX_STRING];

	/*open source file*/
	if ((fpr = fopen(srcfile, "r")) == NULL) {
		printf("Lexical:error open source file %s\n", srcfile);
		return -1;
	}

	/*open dest file*/
	if ((fpw = fopen(destfile, "w")) == NULL) {
		printf("Lexical:error open dest file %s\n", destfile);
		return -1;
	}

	/*initial Variables*/
	while (fgets(buf, MAX_STRING, fpr) != NULL) {
		if (buf[0] != '#')
			fwrite(buf, strlen(buf), sizeof(char), fpw);
	}

	fclose(fpr);
	fclose(fpw);
	return 0;
}

/*--------------------------------Lexical Analyze------------------------------------*/
int LexicalAnalyze(char* srcfile,  char* destfile)
{
	/*Lexical Analyze*/
	curfilename=srcfile;
	scopeS.brace=0;
	scopeS.bracket=0;
	functionS.func=0;
	bufS.num=0;
	functionS.sign=0;
	functionS.inlines=0;
	noS.assign=0;
	noS.define=0;
	noS.brace=0;
	
	if((fpr = fopen(srcfile, "r"))==NULL){
		printf("Lexical:error open source file %s\n", srcfile);
		return -1;
	}
	if((fpw = fopen(destfile, "w"))==NULL){
		printf("Lexical:error open dest file %s\n", destfile);
		return -1;
	}   

	yyrestart(fpr);
	yylex();
	
	fclose(fpr);
	fclose(fpw);
	remove(srcfile);
}

/*-----------------------Line Divide Repair and Block Divide-------------------------*/
int LineRepair(char* srcfile, char* destfile)
{
	int i=0, brackets=0;
	char buf[MAX_STRING], temp[MAX_STRING], *funcname;
	char funcfile[MAX_PATH];

	/*open source file*/
	if ((fpr = fopen(srcfile, "r")) == NULL) {
		printf("LineRepair:error open source file %s\n", srcfile);
		return -1;
	}

	/*Repair some error of line divide in lexical*/
	while (fgets(buf, MAX_STRING, fpr) != NULL) {

		/*Abort part that not in functions*/
		if ((funcname = strstr(buf, "FUNCTION-")) != NULL) {

			/*open dest file*/
			strcpy(funcfile, destfile);
			strcat(funcfile, "--");
			strcat(funcfile, funcname + 9);
			funcfile[strlen(funcfile)-1]='\0';
			strcat(funcfile, ".FunctionBasicBlock");
			if ((fpw = fopen(funcfile, "w")) == NULL) {
				printf("LineRepair:error open dest file %s\n", funcfile);
				return -1;
			}
			fwrite(buf, sizeof(char), strlen(buf), fpw);

			/*Divede lines for function*/
			while (fgets(buf, MAX_STRING, fpr) != NULL) {
				/*Function end*/
				if ((funcname = strstr(buf, "FUNCTION-")) != NULL) {
					fwrite(buf, sizeof(char), strlen(buf), fpw);
					fclose(fpw);
					break;
				}
				/*Deal with all the line by loop*/
				while(1){
					/*Deal with token with '()'*/
					if (strstr(buf, "if") == buf || strstr(buf, "else if") == buf \
						|| strstr(buf, "while")==buf||strstr(buf, "for") == buf) {
						i = 0;
						while (buf[i] != '(')
							i++;
						brackets = 1;
						while (brackets != 0) {
							i++;
							if (buf[i] == '(')
								brackets++;
							else if (buf[i] == ')')
								brackets--;
						}
						if(buf[i + 2]!='\n'){
							strcpy(temp, buf);
							temp[i + 2] = '\n';
							temp[i + 3] = '\0';
							fwrite(temp, sizeof(char), strlen(temp), fpw);
							strcpy(temp, &buf[i + 2]);
							strcpy(buf, temp);
						}
						else{
							fwrite(buf, sizeof(char), strlen(buf), fpw);
							break;
						}
					}
					/*Deal with token without '()'*/
					else if (strstr(buf, "do") == buf || strstr(buf, "else") == buf) {
						i = 0;
						while (buf[i] != ' ')
							i++;
						if(buf[i+1] != '\n'){
							strcpy(temp, buf);
							temp[i + 1] = '\n';
							temp[i + 2] = '\0';
							fwrite(temp, sizeof(char), strlen(temp), fpw);
							strcpy(temp, &buf[i + 1]);
							strcpy(buf, temp);
						}
						else{
							fwrite(buf, sizeof(char), strlen(buf), fpw);
							break;
						}
					}
					/*Line has no structure token*/
					else{
						fwrite(buf, sizeof(char), strlen(buf), fpw);
						break;
					}
				}
			}
			Blockdiv(funcfile);
		}
	}

	fclose(fpr);
	remove(srcfile);
	return 0;
}

int Blockdiv(char* funcfile)
{
	int i = 0, func = 0;
	char buf[MAX_STRING], temp[MAX_STRING];
	char srcfile[MAX_PATH], destfile[MAX_PATH];
	FILE *fpr, *fpw;
	strcpy(srcfile, funcfile);
	strcpy(destfile, funcfile);
	strcat(destfile, ".temp");

	/*Divide function and block*/
	if ((fpr = fopen(srcfile, "r")) == NULL) {
		printf("Blockdiv:error open source file %s\n", srcfile);
		return -1;
	}
	if ((fpw = fopen(destfile, "w")) == NULL) {
		printf("Blockdiv:error open source file %s\n", destfile);
		return -1;
	}
	while (fgets(buf, MAX_STRING, fpr) != NULL) {
		/*Write block info to function file*/
		fwrite(buf, sizeof(char), strlen(buf), fpw);
		fputc('\n', fpw);
		ScopeBranch(fpr, fpw);
	}
	fclose(fpr);
	fclose(fpw);

	/*Delete the extra blank line*/
	if ((fpr = fopen(destfile, "r")) == NULL) {
		printf("Blockdiv:error open source file %s\n", destfile);
		return -1;
	}
	if ((fpw = fopen(srcfile, "w")) == NULL) {
		printf("Blockdiv:error open source file %s\n", srcfile);
		return -1;
	}
	strcpy(buf, "Begin");
	while (fgets(temp, MAX_STRING, fpr) != NULL) {
		if (strstr(buf, "\n") != buf || strstr(temp, "\n") != temp)
			fwrite(temp, sizeof(char), strlen(temp), fpw);
		strcpy(buf, temp);
	}
	fclose(fpr);
	fclose(fpw);

	remove(destfile);
	return 0;
}

void ScopeBranch(FILE* fpr, FILE* fpw)
{
	int i = 0, breaksign = 0;;
	long tempfp = 0;
	char nowstate[MAX_STRING];

	while (fgets(nowstate, MAX_STRING, fpr) != NULL) {
		switch (CheckState(nowstate)) {
		case RIGHT_BRACE_STATE:
			fputc('\n', fpw);
			fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
			fputc('\n', fpw);
			breaksign = 1;
			break;
		case IF_STATE:
			fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
			fputc('\n', fpw);
			StateBranch(fpr, fpw, 0);
			break;
		case ELSEIF_STATE:
		case ELSE_STATE:
			fputc('\n', fpw);
			fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
			fputc('\n', fpw);
			StateBranch(fpr, fpw, 0);
			break;
		case FOR_STATE:
		case WHILE_STATE:
			fputc('\n', fpw);
			fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
			fputc('\n', fpw);
			StateBranch(fpr, fpw, 0);
			break;
		case DO_STATE:
			fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
			fputc('\n', fpw);
			StateBranch(fpr, fpw, 1);
			break;
		case SWITCH_STATE:
			fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
			fputc('\n', fpw);
			StateBranch(fpr, fpw, 0);
			break;
		case BREAK_STATE:
		case CONTINUE_STATE:
		case RETURN_STATE:
		case GOTO_STATE:
			fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
			fputc('\n', fpw);
			break;
		case CASE_STATE:
		case DEFAULT_STATE:
			fputc('\n', fpw);
			fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
			fputc('\n', fpw);
			break;
		case LAB_STATE:
			fputc('\n', fpw);
			fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
			break;
		case LEFT_BRACE_STATE:
			fputc('\n', fpw);
			fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
			fputc('\n', fpw);
			ScopeBranch(fpr, fpw);
			break;
		default:
			fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
		}
		if (breaksign == 1)
			break;
	}
	return;
}

void StateBranch(FILE* fpr, FILE* fpw, int sign)
{
	int i = 0;
	long tempfp = 0;
	char nowstate[MAX_STRING], laststate[MAX_STRING];

	fgets(nowstate, MAX_STRING, fpr);
	/*Deal with jump mark*/
	while (CheckState(nowstate) == LAB_STATE) {
		fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
		fgets(nowstate, MAX_STRING, fpr);
	}
	switch (CheckState(nowstate)) {
	case IF_STATE:
		fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
		fputc('\n', fpw);
		StateBranch(fpr, fpw, 0);
		break;
	case ELSEIF_STATE:
	case ELSE_STATE:
		fputc('\n', fpw);
		fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
		fputc('\n', fpw);
		StateBranch(fpr, fpw, 0);
		break;
	case FOR_STATE:
	case WHILE_STATE:
		fputc('\n', fpw);
		fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
		fputc('\n', fpw);
		StateBranch(fpr, fpw, 0);
		break;
	case DO_STATE:
		fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
		fputc('\n', fpw);
		StateBranch(fpr, fpw, 1);
		break;
	case SWITCH_STATE:
		fputc('\n', fpw);
		fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
		fputc('\n', fpw);
		StateBranch(fpr, fpw, 0);
		break;
	case BREAK_STATE:
	case CONTINUE_STATE:
	case RETURN_STATE:
	case GOTO_STATE:
		fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
		fputc('\n', fpw);
		break;
	case CASE_STATE:
	case DEFAULT_STATE:
		fputc('\n', fpw);
		fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
		fputc('\n', fpw);
		StateBranch(fpr, fpw, 0);
		break;
	case LEFT_BRACE_STATE:
		fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
		fputc('\n', fpw);
		ScopeBranch(fpr, fpw);
		break;
	default:
		fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
	}
	fputc('\n', fpw);

	/*Deal with while state in do-while statement*/
	if (sign == 1) {
		fgets(nowstate, MAX_STRING, fpr);
		fgets(laststate, MAX_STRING, fpr);
		i = 0;
		while (nowstate[i] != '\n') {
			i++;
		}
		nowstate[i] = ';';
		nowstate[i+1] = ' ';
		nowstate[i+2] = '\0';
		fwrite(nowstate, sizeof(char), strlen(nowstate), fpw);
		fputc('\n', fpw);
		fputc('\n', fpw);
	}

	return 0;
}
/*------------------------------Lexical Analyze Entrance-------------------------------*/
int LexicalEntrance(char* srcfiles,  char* destfiles)
{  
	char srcfile[MAX_PATH],destfile[MAX_PATH];

	/*Delete the sharp that gcc added*/
	strcpy(srcfile, srcfiles);
	strcpy(destfile, destfiles);
	strcat(destfile, ".WithoutSharp");
	DelSharp(srcfile, destfile);

	/*Lexical Analyze*/
	strcpy(srcfile,destfile);
	strcpy(destfile, destfiles);
	strcat(destfile, ".Lexical");
	LexicalAnalyze(srcfile,destfile);
	
	/*Line Divide Repair and Block Divide*/
	LineRepair(destfile, destfiles);

    return 0;  
}