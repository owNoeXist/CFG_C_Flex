#include<FuncDef.h>

struct Literal {
	int LineCal;
	int ImmediateCal;
	int StringCal;
	int AritCal;
	int LogicalCal;
	int CallCal;
	int BackCal;
}*f;

int GnerateLiteral(char* filename, int Nodenum);
int BlockLiteral(FILE* fp, int startline, int endline);
int LineCal(FILE* fp, int startline, int endline);
int ImmediateCal(FILE* fp, int startline, int endline);
int StringCal(FILE* fp, int startline, int endline);
int AritCal(FILE* fp, int startline, int endline);
int LogicalCal(FILE* fp, int startline, int endline);
int CallCal(FILE* fp, int startline, int endline);
int BackCal(FILE* fp, int startline, int endline);
int GnerateSemantic(char* filename, int Nodenum);

int GnerateFeature(char* filename, int Nodenum) {
	GnerateLiteral(filename, Nodenum);
	GnerateSemantic(filename, Nodenum);
	return 0;
}

/*------------------------------Code literals------------------------------------*/
int GnerateLiteral(char* filename, int Nodenum)
{
	int i = 0, j = 0, Nowline = 1;
	char buf[MAX_STRING];
	FILE *fp;
	if ((fp = fopen(filename, "r")) == NULL) {
		printf("GnerateLiteral:File open fail!");
		return -1;
	}
	/*Counts features in each base block*/
	for (i = 0; i < Nodenum; i++) {
		f = NULL;
		while (Nowline < node[i].startline) {
			fgets(buf, MAX_STRING, fp);
			Nowline++;
		}
		BlockLiteral(fp, node[i].startline, node[i].endline);
		node[i].literal[0] = f->LineCal;
		node[i].literal[1] = f->ImmediateCal;
		node[i].literal[2] = f->StringCal;
		node[i].literal[3] = f->AritCal;
		node[i].literal[4] = f->LogicalCal;
		node[i].literal[5] = f->CallCal;
		node[i].literal[6] = f->BackCal;
		free(f);
		Nowline = node[i].endline + 1;
	}
	fclose(fp);
	return 0;
}

/*Counts Feature in one base block*/
int BlockLiteral(FILE* fp, int startline, int endline)
{
	long tempfp;
	f = (struct Literal*)malloc(sizeof(struct Literal));
	tempfp = ftell(fp);
	f->LineCal = LineCal(fp, startline, endline);
	fseek(fp, tempfp, SEEK_SET);
	f->ImmediateCal = StringCal(fp, startline, endline);
	fseek(fp, tempfp, SEEK_SET);
	f->StringCal=StringCal(fp, startline, endline);
	fseek(fp, tempfp, SEEK_SET);
	f->AritCal = AritCal(fp, startline, endline);
	fseek(fp, tempfp, SEEK_SET);
	f->LogicalCal = AritCal(fp, startline, endline);
	fseek(fp, tempfp, SEEK_SET);
	f->CallCal =CallCal(fp, startline, endline);
	fseek(fp, tempfp, SEEK_SET);
	f->BackCal = BackCal(fp, startline, endline);
	return 0;
}

/*Literal 1*/
/*Counts the lines of codes in one base block*/
int LineCal(FILE* fp, int startline, int endline)
{
	int num = 0;
	num = endline - startline + 1;
	return num;
}

/*Literal 2*/
/*Counts the number of Immediate Num in one base block*/
int ImmediateCal(FILE* fp, int startline, int endline)
{
	int i = 0, num = 0;
	char buf[MAX_STRING], * temp1, * temp2;
	for (i = startline; i <= endline; i++) {
		fgets(buf, MAX_STRING, fp);
		temp1 = buf;
		while ((temp2 = strstr(temp1, "NUM")) != NULL) {
			num++;
			temp1 = temp2 + 3;
		}
	}
	return num;
}

/*Literal 3*/
/*Counts the number of string constants in one base block*/
int StringCal(FILE* fp,int startline,int endline)
{
	int i = 0, num = 0;
	char buf[MAX_STRING],*temp1,*temp2;
	for (i = startline; i <= endline; i++) {
		fgets(buf, MAX_STRING, fp);
		temp1 = buf;
		while ((temp2 = strstr(temp1, "STRING")) != NULL) {
			num++;
			temp1 = temp2 + 6;
		}
	}
	return num;
}

/*Literal 4*/
/*Counts the number of Arithmetic Instructions in one base block*/
int AritCal(FILE* fp, int startline, int endline)
{
	int i = 0, num = 0, state = 0;
	char buf[MAX_STRING], *temp1, *temp2;
	for (i = startline; i <= endline; i++) {
		fgets(buf, MAX_STRING, fp);
		temp1 = buf;
		while ((temp2 = strstr(temp1, "CAL")) != NULL) {
			state = CheckToken(temp2);
			if (state == INSTRUCTION_ARITHMETICAL || state == INSTRUCTION_TRANSFER_ARITHMETICAL) {
				num++;
			}
			temp1 = temp2 + 5;
		}
	}
	return num;
}

/*Literal 5*/
/*Counts the number of Logical Instructions in one base block*/
int LogicalCal(FILE* fp, int startline, int endline)
{
	int i = 0, num = 0, state = 0;
	char buf[MAX_STRING], * temp1, * temp2;
	for (i = startline; i <= endline; i++) {
		fgets(buf, MAX_STRING, fp);
		temp1 = buf;
		while ((temp2 = strstr(temp1, "CAL")) != NULL){
			state = CheckToken(temp2);
			if (state == INSTRUCTION_LOGICAL || state == INSTRUCTION_TRANSFER_LOGICAL) {
				num++;
			}
			temp1 = temp2 + 5;
		}
	}
	return num;
}

/*Literal 6*/
/*Counts the number of Call Instructions in one base block*/
int CallCal(FILE* fp, int startline, int endline)
{
	int i = 0, num = 0;
	char buf[MAX_STRING], *temp, *temp1;
	for (i = startline; i <= endline; i++) {
		fgets(buf, MAX_STRING, fp);
		temp = buf;
		while (strstr(temp, "BUF")!= NULL) {
			temp = strstr(temp, " ") + 1;
			if (temp[0]=='(') {
				num++;
				break;
			}
		}
	}
	return num;
}

/*Literal 7*/
/*Counts the number of CallBack Instructions in one base block*/
int BackCal(FILE* fp, int startline, int endline)
{
	int i = 0, num = 0;
	char buf[MAX_STRING], *temp1, *temp2;
	for (i = startline; i <= endline; i++) {
		fgets(buf, MAX_STRING, fp);
		temp1 = buf;
		while ((temp2 = strstr(temp1, "return")) != NULL) {
			num++;
			temp1 = temp2 + 6;
		}
	}
	return num;
}

/*------------------------------Semantic Sequence---------------------------------*/
int GnerateSemantic(char* filename, int Nodenum)
{
	int nodes = 0, nextLine = 1, state = 0;
	char nowstate[MAX_STRING], *temp, *temp1;
	FILE* fp;
	if ((fp = fopen(filename, "r")) == NULL) {
		printf("GnerateSemantic:File open fail!");
		return -1;
	}
	/*Counts features in each base block*/
	for (nodes = 0; nodes < Nodenum; nodes++) {
		while (nextLine < node[nodes].startline) {
			fgets(nowstate, MAX_STRING, fp);
			nextLine++;
		}
		int i = 0;
		while (nextLine <= node[nodes].endline) {
			fgets(nowstate, MAX_STRING, fp);
			temp = nowstate;
			while (i < MAX_SEQUENCE) {
				state = CheckToken(temp);
				if (strstr(temp, "BUF")== temp) {
					temp1 = strstr(temp, " ") + 1;
					if (temp1[0] == '(') {
						node[nodes].semantic[i++] = 5;
					}
				}
				else if (state >= 0) {
					node[nodes].semantic[i++] = state;
				}
				else if (state == -2) {
					break;
				}
				temp = strstr(temp, " ") + 1;
			}
			nextLine++;
		}
	}
	fclose(fp);
	return 0;
}