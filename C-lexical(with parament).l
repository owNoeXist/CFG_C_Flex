/*Parameter and variate*/
%option noyywrap nodefault

%x FUNCTION

UCN	(\\u[0-9a-fA-F]{4}|\\U[0-9a-fA-F]{8})
EXP ([Ee][-+]?[0-9]+)
ILEN ([Uu](L|l|LL|ll)?|(L|l|LL|ll)[Uu]?) 

%{
#include<FuncDef.h>

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

struct PARAMETER{
	int sign;
	int mmxsign;
	int mmxnum;
	char mmxs[10][260];
	int argsign;
	int argnum;
	char args[20][260];
}argS;

struct GOTO{
	int marks;
	int gotos;
	int gotonum;
	char CurGotoName[100];
	char GotoName[10][100];
}gotoS;

struct NOFUNCTION{
	int define;
	int assign;
	int brace;
}noS;

char *curfilename;

char KEY[10]="KEY ";
char NUM[10]="NUM";
char TAG[10]="TAG";
char JMP[10]="-JUMP-";
char CALA[10]="-CAL-";
char SYM[10]="-SYM-";
char ARG[10]="-ARG-";
char STRING[10]="STRING";
char FUNCTIONS[10]="FUNCTION-";

int tempnum=0;

FILE *fpr,*fpw;
%}


%%

_Bool|_Complex|_Imaginary {
	fwrite(KEY,sizeof(char),strlen(KEY),fpw);
}
auto|extern|register|static|restrict {
	fwrite(KEY,sizeof(char),strlen(KEY),fpw);
}
inline|__inline|__inline__ {
	fwrite(KEY,sizeof(char),strlen(KEY),fpw);
	functionS.inlines=1;
}
char|int|long|short|signed|unsigned|void {
	fwrite(KEY,sizeof(char),strlen(KEY),fpw);
}
doubel|float {
	fwrite(KEY,sizeof(char),strlen(KEY),fpw);
	if(functionS.func==0&&functionS.sign==2){
		argS.sign=1;
		argS.mmxsign=0;
	}
}
enum|struct|union {
	fwrite(KEY,sizeof(char),strlen(KEY),fpw);
	if(scopeS.bracket==0){
		functionS.sign=0;
		noS.define=1;
	}
}
sizeof|const|typedef|volatile  {fwrite(KEY,sizeof(char),strlen(KEY),fpw);}


if {fwrite(yytext,sizeof(char),strlen(yytext),fpw);}
else {fwrite(yytext,sizeof(char),strlen(yytext),fpw);}
do {fwrite(yytext,sizeof(char),strlen(yytext),fpw);}
while {fwrite(yytext,sizeof(char),strlen(yytext),fpw);}
for {fwrite(yytext,sizeof(char),strlen(yytext),fpw);}
switch {fwrite(yytext,sizeof(char),strlen(yytext),fpw);}
case {fwrite(yytext,sizeof(char),strlen(yytext),fpw);gotoS.marks=0;}
default {fwrite(yytext,sizeof(char),strlen(yytext),fpw);gotoS.marks=0;}
break {fwrite(yytext,sizeof(char),strlen(yytext),fpw);}
continue {fwrite(yytext,sizeof(char),strlen(yytext),fpw);}
goto {fwrite(yytext,sizeof(char),strlen(yytext),fpw);gotoS.gotos=1;fputc(' ',fpw);}
return {fwrite(yytext,sizeof(char),strlen(yytext),fpw);fputc(' ',fpw);}


0[0-7]*{ILEN}? |
[1-9][0-9]*{ILEN}? |
0[Xx][0-9a-fA-F]+{ILEN}? |
([0-9]*\..[0-9]+|[0-9]+\.){EXP}?[flFL]? |
[0-9]+{EXP}[flFL]? |
0[Xx]([0-9a-fA-F]*\.[0-9a-fA-F]+|[0-9a-fA-F]+\.?)[Pp][-+]?[0-9]+[flFL]? {fwrite(NUM,sizeof(char),strlen(NUM),fpw);}


\'([^'\\]|\\['"?\\abfnrtv]|\\[0-7]{1,3}|\\[Xx][0-9a-fA-F]+|{UCN})+\' |
L?\"([^"\\]|\\['"?\\abfnrtv]|\\[0-7]{1,3}|\\[Xx][0-9a-fA-F]+|{UCN})*\" {fwrite(STRING,sizeof(char),strlen(STRING),fpw);}


";" {
	fwrite(yytext,sizeof(char),strlen(yytext),fpw);
	if(functionS.func==0){
		functionS.sign=0;
	}
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
	fwrite(yytext,sizeof(char),strlen(yytext),fpw);
	scopeS.bracket++;
	if(functionS.sign==1&&noS.assign==0){
		functionS.sign=2;
		argS.argnum=0;
		argS.argsign=0;
		argS.mmxnum=0;
		argS.mmxsign=0;
	}
}


")" {
	fwrite(yytext,sizeof(char),strlen(yytext),fpw);
	scopeS.bracket--;
	if(functionS.sign==2&&scopeS.bracket==0){
		if(argS.sign==0){
			if(argS.argsign==1)
				argS.argnum++;
			argS.argsign=0;
		}
		else{
			if(argS.mmxsign==1)
				argS.mmxnum++;
			argS.mmxsign=0;
			argS.sign=0;
		}
	}
}


"["|"]"	{
	fwrite(yytext,sizeof(char),strlen(yytext),fpw);
}


"{" {
	scopeS.brace++;
	if(scopeS.bracket!=0){
		fwrite(yytext,sizeof(char),strlen(yytext),fpw);
	}
	else if(functionS.func==0){
		if(functionS.sign==2){
			fwrite(yytext,sizeof(char),strlen(yytext),fpw);
			if(functionS.inlines==0){
				fwrite(FUNCTIONS,sizeof(char),strlen(FUNCTIONS),fpw);
				fwrite(functionS.CurFuncName,sizeof(char),strlen(functionS.CurFuncName),fpw);
			}
			functionS.func=1;
			gotoS.marks=0;
			gotoS.gotos=0;
			gotoS.gotonum=0;
			fputc('\n',fpw);
			noS.define=0;
		}
		else if(noS.assign==1||noS.define==1){
			noS.brace++;
			fwrite(yytext,sizeof(char),strlen(yytext),fpw);
		}
		else{
			fputc('\n',fpw);
			fwrite(yytext,sizeof(char),strlen(yytext),fpw);
			fputc('\n',fpw);
		}
	}
	else{
		if(noS.assign==1||noS.define==1){
			noS.brace++;
			fwrite(yytext,sizeof(char),strlen(yytext),fpw);
		}
		else{
			fputc('\n',fpw);
			fwrite(yytext,sizeof(char),strlen(yytext),fpw);
			fputc('\n',fpw);
		}
	}
}


"}" {
	scopeS.brace--;
	if(scopeS.bracket!=0){
		fwrite(yytext,sizeof(char),strlen(yytext),fpw);
	}
	else if(noS.brace>0){
		fwrite(yytext,sizeof(char),strlen(yytext),fpw);
		noS.brace--;
	}
	else{
		fputc('\n',fpw);
		fwrite(yytext,sizeof(char),strlen(yytext),fpw);
	 	if(functionS.func==1&&scopeS.brace==0){
	 		argS.argnum=0;
			argS.mmxnum=0;
			functionS.sign=0;
			functionS.func=0;
			if(functionS.inlines==0){
				fwrite(FUNCTIONS,sizeof(char),strlen(FUNCTIONS),fpw);
				fwrite(functionS.CurFuncName,sizeof(char),strlen(functionS.CurFuncName),fpw);
			}
			functionS.inlines=0;
		}
		fputc('\n',fpw);
	}

}


"."|"->"  {fwrite(yytext,sizeof(char),strlen(yytext),fpw);}
"++"|"--"|"+"|"-"|"/"|"%"|"*" {fwrite(CALA,sizeof(char),strlen(CALA),fpw);}
"&"|"~"|"!" {fwrite(SYM,sizeof(char),strlen(SYM),fpw);}
"<<"|">>" {fwrite(SYM,sizeof(char),strlen(SYM),fpw);}
"<"|">"|"<="|">=" {fwrite(SYM,sizeof(char),strlen(SYM),fpw);}
"=="|"!=" {fwrite(SYM,sizeof(char),strlen(SYM),fpw);}
"^"|"|"|"&&"|"||" {fwrite(SYM,sizeof(char),strlen(SYM),fpw);}


"?"	{
	fwrite(SYM,sizeof(char),strlen(SYM),fpw);
	gotoS.marks=-1;
}


":" {
	fwrite(SYM,sizeof(char),strlen(SYM),fpw);
	/*The colon belongs to goto statement's mark*/
	if(functionS.func==1){
		if(gotoS.marks==2){
			for(gotoS.marks=0;gotoS.marks<gotoS.gotonum;gotoS.marks++){
				if(strcmp(gotoS.GotoName[gotoS.marks],gotoS.CurGotoName)==0)
					break;
			}
			if(gotoS.marks==gotoS.gotonum){
				strcpy(gotoS.GotoName[gotoS.marks],gotoS.CurGotoName);
				gotoS.gotonum++;
			}
			fwrite(JMP,sizeof(char),strlen(JMP),fpw);
			fputc((gotoS.marks+'0'),fpw);
			fputc('\n',fpw);
			gotoS.marks=1;
		}
		/*The colon belongs to case or default*/
		else if(gotoS.marks==0){
			fputc('\n',fpw);
			gotoS.marks=1;
		}
	}
}


"*="|"/="|"%="|"+="|"-=" {fwrite(CALA,sizeof(char),strlen(CALA),fpw);}
"<<="|">>="|"&="|"^="|"|=" {fwrite(SYM,sizeof(char),strlen(SYM),fpw);}

"=" {
	fwrite(SYM,sizeof(char),strlen(SYM),fpw);
	if(scopeS.bracket==0){
		functionS.sign=0;
		noS.assign=1;
	}
}


"," {
	fwrite(yytext,sizeof(char),strlen(yytext),fpw);
	fputc(' ',fpw);
	if(functionS.func==0&&functionS.sign==1)
		functionS.sign==0;
	else if(functionS.func==0&&functionS.sign==2){
		if(argS.sign==0){
			if(argS.argsign==1)
				argS.argnum++;
			argS.argsign=0;
		}
		else{
			if(argS.mmxsign==1)
				argS.mmxnum++;
			argS.mmxsign=0;
			argS.sign=0;
		}
	}
}


"#"|"##" {fwrite(SYM,sizeof(char),strlen(SYM),fpw);}
"..."|"<%"|"<:"|":>"|"%>"|"%:"|"%:%:" {fwrite(yytext,sizeof(char),strlen(yytext),fpw);}


([_a-zA-Z]|{UCN})([_a-zA-Z0-9]|{UCN})* {
	fwrite(TAG,sizeof(char),strlen(TAG),fpw);
	/*Record function name*/
	if(functionS.func==0&&scopeS.bracket==0){
		strcpy(functionS.CurFuncName,yytext);
		functionS.sign=1;
	}
	/*Record parameter name*/
	else if(functionS.func==0&&functionS.sign==2){
		if(argS.sign==0){
			argS.argsign=1;
			strcpy(argS.args[argS.argnum],yytext);
		}
		else{
			argS.mmxsign=1;
			strcpy(argS.mmxs[argS.mmxnum],yytext);
		}
	}
	else{
		if(functionS.func==1){
			for(tempnum=0;tempnum<argS.argnum;tempnum++){
				if(strcmp(argS.args[tempnum],yytext)==0){
					fwrite(ARG,sizeof(char),strlen(ARG),fpw);
					if(tempnum<6)
						fputc((tempnum+'0'),fpw);
					else
						fputc('6',fpw);
					break;
				}
			}
			for(tempnum=0;tempnum<argS.mmxnum;tempnum++){
				if(strcmp(argS.mmxs[tempnum],yytext)==0){
					fwrite(ARG,sizeof(char),strlen(ARG),fpw);
					fputc('7',fpw);
					break;
				}
			}
		}
		/*Record goto statement's mark name*/
		if(gotoS.marks==1){
			strcpy(gotoS.CurGotoName,yytext);
			gotoS.marks=2;
		}
		/*Record name that goto statement lead*/
		if(gotoS.gotos==1){
			for(gotoS.gotos=0;gotoS.gotos<gotoS.gotonum;gotoS.gotos++){
				if(strcmp(gotoS.GotoName[gotoS.gotos],yytext)==0)
					break;
			}
			if(gotoS.gotos==gotoS.gotonum){
				strcpy(gotoS.GotoName[gotoS.gotos],yytext);
				gotoS.gotonum++;
			}
			fwrite(JMP,sizeof(char),strlen(JMP),fpw);
			fputc((gotoS.gotos+'0'),fpw);
			gotoS.gotos=0;
		}
	}
}


[ \t\n]+ 


<<EOF>> {
	fputc('\n',fpw);yyterminate();
}


. {
	printf("%s:Mystery character '%s'\n",curfilename,yytext);
}


%%

/*------------------------------Lexical Analyze---------------------------------------*/

int Lexical(char* filename)
{  
	char buf[10000];
	char srcfile[MAX_PATH],destfile[MAX_PATH];

	/*open source file*/
	strcpy(srcfile,filename);
	if((fpr = fopen(srcfile, "r"))==NULL){
		printf("Lexical:error open source file %s\n", srcfile);
		return -1;
	}

	/*open dest file*/
	strcpy(destfile, filename);
	strcat(destfile, "-Lexical");
	if((fpw = fopen(destfile, "w"))==NULL){
		printf("Lexical:error open dest file %s\n", destfile);
		return -1;
	}   
    
	/*initial Variables*/
	curfilename=srcfile;
	scopeS.brace=0;
	scopeS.bracket=0;
	functionS.func=0;
	argS.argnum=0;
	argS.mmxnum=0;
	functionS.sign=0;
	functionS.inlines=0;
	noS.assign=0;
	noS.define=0;
	noS.brace=0;

	/*Search dir for C file*/
	yyrestart(fpr);
	yylex();
					
	fclose(fpr);
	fclose(fpw);
	remove(srcfile);
	rename(destfile, srcfile);
    return 0;  
}

/*------------------------------Line Divide Repair--------------------------------*/

int LineRepair(char *filename)
{
	int i=0, brackets=0;
	char buf[10000],temp[10000];
	char srcfile[MAX_PATH], destfile[MAX_PATH];
	FILE *fpr,*fpw;

	/*open source file*/
	strcpy(srcfile, filename);
	if ((fpr = fopen(srcfile, "r")) == NULL) {
		printf("LineRepair:error open source file %s\n", srcfile);
		return -1;
	}

	/*open dest file*/
	strcpy(destfile, filename);
	strcat(destfile, "-LineRepair");
	if ((fpw = fopen(destfile, "w")) == NULL) {
		printf("LineRepair:error open dest file %s\n", destfile);
		return -1;
	}

	/*Repair some error of line divide in lexical*/
	while (fgets(buf, 10000, fpr) != NULL) {
retry:
		if (strstr(buf, "if") == buf || strstr(buf, "elseif") == buf \
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
			if(buf[i+1]=='\n'){
				fwrite(buf, sizeof(char), strlen(buf), fpw);
			}
			else{
				strcpy(temp, buf);
				temp[i + 1] = '\n';
				temp[i + 2] = '\0';
				fwrite(temp, sizeof(char), strlen(temp), fpw);
				strcpy(temp, &buf[i + 1]);
				strcpy(buf, temp);
				goto retry;
			}
		}
		else if (strstr(buf, "do") == buf) {
			if(buf[2]=='\n'){
				fwrite(buf, sizeof(char), strlen(buf), fpw);
			}
			else{
				strcpy(temp, buf);
				temp[2] = '\n';
				temp[3] = '\0';
				fwrite(temp, sizeof(char), strlen(temp), fpw);
				strcpy(temp, &buf[2]);
				strcpy(buf, temp);
				goto retry;
			}
		}
		else if (strstr(buf, "else") == buf) {
			if(buf[4]=='\n'){
				fwrite(buf, sizeof(char), strlen(buf), fpw);
			}
			else{
				strcpy(temp, buf);
				temp[4] = '\n';
				temp[5] = '\0';
				fwrite(temp, sizeof(char), strlen(temp), fpw);
				strcpy(temp, &buf[4]);
				strcpy(buf, temp);
				goto retry;
			}
		}
		else
			fwrite(buf, sizeof(char), strlen(buf), fpw);
	}

	fclose(fpr);
	fclose(fpw);
	remove(srcfile);
	rename(destfile, srcfile);
	return 0;
}

/*--------------------------------Module entrance-------------------------------------*/

int LexicalLine(char* filename)
{
	Lexical(filename);
	LineRepair(filename);
	return 0;
}