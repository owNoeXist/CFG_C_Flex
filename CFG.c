#include <FuncDef.h>

struct IfDeep {
	struct IfDeep* next;
	int nodenum;
	int node[1000];
};

struct Loopswitch {
	struct Loopswitch* next;
	int breaknum;
	int breaks[1000];
	int continuenum;
	int continues[1000];
};

struct IfDeep *Iftree = NULL;
struct IfDeep *Iftemp = NULL;
struct Loopswitch *Loopswitchtree = NULL;
struct Loopswitch *Loopswitchtemp = NULL;

int befnode[1000], befnum = 0;
int startline, nowline, nodenum;

int IfBranch(FILE* fp);
int LoopBranch(FILE* fp);
int DoBranch(FILE* fp);
int SwitchBranch(FILE* fp);

int StateDeal(FILE* fp);
int ScopeDeal(FILE* fp);

int AddNode();
int NodeIfStack();
int NodeBreakStack();
int NodeContinueStack();

int GotoWhere(FILE* fp, char* string, int labelnode);
int Node2before(int nodenum);
int CheckState(char* state);


int GenerateCFG(char* filename, int* nodeNum)
{
	int i = 0, j = 0, k = 0;
	long tempfp = 0;
	char nowstate[MAX_STRING];
	FILE* fp = NULL;

	/*Open dest file*/
	if ((fp = fopen(filename, "r")) == NULL) {
		printf("GenerateCFG:error open %s", filename);
		return -1;
	}
	/*Precalculate Nodenum for Memory Malloc*/
	nodenum = -1;
	while (fgets(nowstate, MAX_STRING, fp) != NULL) {
		if (strstr(nowstate, "\n") == nowstate) {
			nodenum++;
			do {
				if (fgets(nowstate, MAX_STRING, fp) == NULL)
					break;
			} while (CheckState(nowstate) == LEFT_BRACE_STATE || CheckState(nowstate) == RIGHT_BRACE_STATE \
				|| CheckState(nowstate) == DEFAULT_STATE || CheckState(nowstate) == ELSE_STATE \
				|| CheckState(nowstate) == NEWLINE_STATE);
		}
	}
	fseek(fp, 0, SEEK_SET);
	if (nodenum == 0) {
		*nodeNum = nodenum;
		return -1;
	}

	/*Initial global variable*/
	node = (struct Node*)malloc(sizeof(struct Node)*(nodenum));
	for (i = 0; i < nodenum; i++) {
		node[i].nextnode = i + 1;
		node[i].jumpnode = -1;
		memset(node[i].semantic,0, MAX_SEQUENCE*sizeof(int));
	}
	node[nodenum-1].nextnode = -1;
	Iftree = NULL;
	Iftemp = NULL;
	Loopswitchtree = NULL;
	Loopswitchtemp = NULL;
	befnum = 0;
	nodenum = 0;
	nowline = 0;
	startline = 3;

	/*Analyze association between Block*/
	for (i = 0; i < 2; i++) {
		fgets(nowstate, MAX_STRING, fp);
		nowline++;
	}
	ScopeDeal(fp);

	/*Repair node info for goto and return*/
	fseek(fp, 0, SEEK_SET);
	nodenum = -1;
	while (fgets(nowstate, MAX_STRING, fp) != NULL) {
		if (CheckState(nowstate) == LAB_STATE) {
			GotoWhere(fp, nowstate, nodenum);
		}
		else if (CheckState(nowstate) == RETURN_STATE) {
			node[nodenum].nextnode = -1;
			node[nodenum].jumpnode = -1;
		}
		else if (CheckState(nowstate) == NEWLINE_STATE) {
			nodenum++;
			do {
				tempfp = ftell(fp);
				if (fgets(nowstate, MAX_STRING, fp) == NULL)
					break;
			} while (CheckState(nowstate) == LEFT_BRACE_STATE || CheckState(nowstate) == RIGHT_BRACE_STATE\
				|| CheckState(nowstate) == DEFAULT_STATE|| CheckState(nowstate) == ELSE_STATE
				|| CheckState(nowstate) == NEWLINE_STATE);
			fseek(fp, tempfp, SEEK_SET);
		}
	}
	*nodeNum = nodenum;
	fclose(fp);

	return 0;
}

int IfBranch(FILE* fp)
{
	int i = 0, lastif = 0;
	long tempfp = 0;
	char nowstate[MAX_STRING], laststate[MAX_STRING];

	/*Create a struct to save information of IF-ELSE*/
	if (Iftree == NULL) {
		Iftree = (struct IfDeep*)malloc(sizeof(struct IfDeep));
		Iftree->next = NULL;
	} 
	else {
		Iftemp = (struct IfDeep*)malloc(sizeof(struct IfDeep));
		Iftemp->next = Iftree;
		Iftree = Iftemp;
	}
	Iftree->nodenum = 0;

	/*Deal with IF-ELSE statement*/
	tempfp = ftell(fp);
	lastif = nodenum - 1;
	while (fgets(nowstate, MAX_STRING, fp) != NULL) {
		nowline++;
		/*Judge statement whether is blank*/
		/*YES*/
		if(CheckState(nowstate)== ELSEIF_STATE) {
			node[lastif].jumpnode = nodenum;
			lastif = nodenum;
			NodeIfStack(); 
			fgets(nowstate, MAX_STRING, fp);
			nowline++;
			AddNode();
		}
		else if (CheckState(nowstate) == ELSE_STATE) {
			node[lastif].jumpnode = nodenum;
			lastif = -1;
			NodeIfStack();
			fgets(nowstate, MAX_STRING, fp);
			nowline++;
			startline = nowline + 1;
		}
		/*NO*/
		else {
			/*Deal with statement*/
			fseek(fp, tempfp, SEEK_SET);
			nowline--;
			StateDeal(fp);
			NodeIfStack();

			/*Judge IF-ELSE whether is over*/
			tempfp = ftell(fp);
			fgets(nowstate, MAX_STRING, fp);
			nowline++;
			/*NO*/
			if (CheckState(nowstate) == ELSEIF_STATE) {
				node[lastif].jumpnode = nodenum;
				lastif = nodenum;
				fgets(nowstate, MAX_STRING, fp);
				nowline++;
				AddNode();
			}
			else if (CheckState(nowstate) == ELSE_STATE) {
				node[lastif].jumpnode = nodenum;
				lastif = nodenum;
				fgets(nowstate, MAX_STRING, fp);
				nowline++;
				startline = nowline + 1;
			}
			/*YES*/
			else {
				fseek(fp, tempfp, SEEK_SET);
				nowline--;
				break;
			}
		}
		tempfp = ftell(fp);
	}

	/*Deal with last if or ifelse*/
	if (lastif != -1) {
		node[lastif].jumpnode = nodenum;
		befnode[befnum] = lastif;
		befnum++;
	}

	/*Deal with branch information of IF-ELSE*/
	for (i = 0; i < Iftree->nodenum; i++) {
		node[Iftree->node[i]].nextnode = -1;
		node[Iftree->node[i]].jumpnode = nodenum;
		befnode[befnum] = Iftree->node[i];
		befnum++;
	}

	/*Delete the struct*/
	if (Iftree->next != NULL) {
		Iftemp = Iftree->next;
		free(Iftree);
		Iftree = Iftemp;
	}
	else {
		free(Iftree);
		Iftree = NULL;
	}

	return 0;
}

int LoopBranch(FILE* fp)
{
	int i = 0, loopnode = 0;
	long tempfp = 0;
	char nowstate[MAX_STRING], laststate[MAX_STRING];

	/*Create a struct to save information of Loop*/
	if (Loopswitchtree == NULL) {
		Loopswitchtree = (struct Loopswitch*)malloc(sizeof(struct Loopswitch));
		Loopswitchtree->next = NULL;
	}
	else {
		Loopswitchtemp = (struct Loopswitch*)malloc(sizeof(struct Loopswitch));
		Loopswitchtemp->next = Loopswitchtree;
		Loopswitchtree = Loopswitchtemp;
	}
	Loopswitchtree->breaknum = 0;
	Loopswitchtree->continuenum = 0;

	/*Deal with statement*/
	loopnode = nodenum - 1;
	StateDeal(fp);

	/*Add and repair edge with loopnode*/
	for (i = 0; i < befnum; i++)
		node[befnode[i]].nextnode = -1;
	node[loopnode].jumpnode = nodenum;
	Node2before(loopnode);
	for (i = 0; i < Loopswitchtree->continuenum; i++) {
		node[Loopswitchtree->continues[i]].nextnode = -1;
		node[Loopswitchtree->continues[i]].jumpnode = loopnode;
	}
	for (i = 0; i < Loopswitchtree->breaknum; i++) {
		node[Loopswitchtree->breaks[i]].nextnode = -1;
		node[Loopswitchtree->breaks[i]].jumpnode = nodenum;
		befnode[befnum] = Loopswitchtree->breaks[i];
		befnum++;
	}

	/*Delete the struct*/
	if (Loopswitchtree->next != NULL) {
		Loopswitchtemp = Loopswitchtree->next;
		free(Loopswitchtree);
		Loopswitchtree = Loopswitchtemp;
	}
	else {
		free(Loopswitchtree);
		Loopswitchtree = NULL;
	}

	return 0;
}

int DoBranch(FILE* fp)
{
	int i = 0, loopnode = 0;
	long tempfp = 0;
	char nowstate[MAX_STRING];

	/*Create a struct to save information of Loop*/
	if (Loopswitchtree == NULL) {
		Loopswitchtree = (struct Loopswitch*)malloc(sizeof(struct Loopswitch));
		Loopswitchtree->next = NULL;
	}
	else {
		Loopswitchtemp = (struct Loopswitch*)malloc(sizeof(struct Loopswitch));
		Loopswitchtemp->next = Loopswitchtree;
		Loopswitchtree = Loopswitchtemp;
	}
	Loopswitchtree->breaknum = 0;
	Loopswitchtree->continuenum = 0;

	/*Deal with statement*/
	loopnode = nodenum;
	StateDeal(fp);

	/*Deal with while statement*/
	node[nodenum].jumpnode = loopnode;
	fgets(nowstate, MAX_STRING, fp);
	nowline++;
	fgets(nowstate, MAX_STRING, fp);
	nowline++;
	AddNode();

	/*Add and repair edge*/
	for (i = 0; i < Loopswitchtree->continuenum; i++) {
		node[Loopswitchtree->continues[i]].nextnode = -1;
		node[Loopswitchtree->continues[i]].jumpnode = nodenum - 1;
	}
	for (i = 0; i < Loopswitchtree->breaknum; i++) {
		node[Loopswitchtree->breaks[i]].nextnode = -1;
		node[Loopswitchtree->breaks[i]].jumpnode = nodenum;
		befnode[befnum] = Loopswitchtree->breaks[i];
		befnum++;
	}

	/*Delete the struct*/
	if (Loopswitchtree->next != NULL) {
		Loopswitchtemp = Loopswitchtree->next;
		free(Loopswitchtree);
		Loopswitchtree = Loopswitchtemp;
	}
	else {
		free(Loopswitchtree);
		Loopswitchtree = NULL;
	}

	return 0;
}

int SwitchBranch(FILE* fp)
{
	int i = 0;
	int defaults = -1, lastcase = -1;
	int casenum = 0, beforecase[100];
	int statenum = 0, beforestate[100];
	long tempfp = 0;
	char nowstate[MAX_STRING];

	if (Loopswitchtree == NULL) {
		Loopswitchtree = (struct Loopswitch*)malloc(sizeof(struct Loopswitch));
		Loopswitchtree->next = NULL;
	}
	else {
		Loopswitchtemp = (struct Loopswitch*)malloc(sizeof(struct Loopswitch));
		Loopswitchtemp->next = Loopswitchtree;
		Loopswitchtree = Loopswitchtemp;
	}
	Loopswitchtree->breaknum = 0;
	Loopswitchtree->continuenum = -1;

	/*Judge switch whether has brace*/
	node[nodenum - 1].nextnode = -1;
	lastcase = nodenum - 1;
	tempfp = ftell(fp);
	fgets(nowstate, MAX_STRING, fp);
	nowline++;
	switch (CheckState(nowstate)) {
	case LEFT_BRACE_STATE:
		fgets(nowstate, MAX_STRING, fp);
		nowline++;
		startline = nowline + 1;
		tempfp = ftell(fp);
		while (fgets(nowstate, MAX_STRING, fp) != NULL) {
			nowline++;
			if (CheckState(nowstate) == RIGHT_BRACE_STATE) {
				fgets(nowstate, MAX_STRING, fp);
				nowline++;
				startline = nowline + 1;
				break;
			}
			else if (CheckState(nowstate) == CASE_STATE) {
				node[lastcase].jumpnode = nodenum;
				lastcase = nodenum;
				beforecase[casenum] = nodenum;
				casenum++;
				fgets(nowstate, MAX_STRING, fp);
				nowline++;
				AddNode();
			}
			else if (CheckState(nowstate) == DEFAULT_STATE) {
				defaults = -2;
				fgets(nowstate, MAX_STRING, fp);
				nowline++;
				startline = nowline + 1;
			}
			else {
				fseek(fp, tempfp, SEEK_SET);
				nowline--;
				if (defaults == -2)
					defaults = nodenum;
				for (i = 0; i < casenum; i++)
					node[beforecase[i]].nextnode = nodenum;
				for (i = 0; i < statenum; i++)
					node[beforestate[i]].jumpnode = nodenum;
				SwitchDeal(fp);
				casenum = 0;
				for (i = 0; i < befnum; i++) 
					beforestate[i] = befnode[i];
				statenum = befnum;
			}
			tempfp = ftell(fp);
		}
		break;
	case CASE_STATE:
		fgets(nowstate, MAX_STRING, fp);
		nowline++;
		AddNode();
		StateDeal(fp);
		for (i = 0; i < befnum; i++)
			beforestate[i] = befnode[i];
		statenum = befnum;
		break;
	case DEFAULT_STATE:
		fgets(nowstate, MAX_STRING, fp);
		nowline++;
		startline = nowline + 1;
		StateDeal(fp);
		for (i = 0; i < befnum; i++)
			beforestate[i] = befnode[i];
		statenum = befnum;
		break;
	default:
		fseek(fp, tempfp, SEEK_SET);
		nowline--;
		StateDeal(fp);
	}

	if (defaults > 0)
		node[lastcase].jumpnode = defaults;
	else
		node[lastcase].jumpnode = nodenum;
	for (i = 0; i < statenum; i++)
		befnode[i] = beforestate[i];
	befnum = statenum;

	for (i = 0; i < Loopswitchtree->breaknum; i++) {
		node[Loopswitchtree->breaks[i]].nextnode = -1;
		node[Loopswitchtree->breaks[i]].jumpnode = nodenum;
		befnode[befnum] = Loopswitchtree->breaks[i];
		befnum++;
	}

	if (Loopswitchtree->next != NULL) {
		Loopswitchtemp = Loopswitchtree->next;
		free(Loopswitchtree);
		Loopswitchtree = Loopswitchtemp;
	}
	else {
		free(Loopswitchtree);
		Loopswitchtree = NULL;
	}

	return 0;
}


/*Deal with statement*/
int StateDeal(FILE* fp)
{
	int i = 0;
	long tempfp = 0;
	char nowstate[MAX_STRING],laststate[MAX_STRING];

	do {
		fgets(nowstate, MAX_STRING, fp);
		nowline++;
	} while (CheckState(nowstate)== LAB_STATE);
	tempfp = ftell(fp);
	fgets(laststate, MAX_STRING, fp);
	nowline++;
	switch (CheckState(nowstate)) {
	case LEFT_BRACE_STATE:
		startline = nowline + 1;
		ScopeDeal(fp);
		break;
	case IF_STATE:
		AddNode();
		IfBranch(fp);
		break;
	case FOR_STATE:
	case WHILE_STATE:
		AddNode();
		LoopBranch(fp);
		break;
	case DO_STATE:
		AddNode();
		DoBranch(fp);
		break;
	case SWITCH_STATE:
		AddNode();
		SwitchBranch(fp);
		break;
	case BREAK_STATE:
		NodeBreakStack();
		AddNode();
		break;
	case CONTINUE_STATE:
		NodeContinueStack();
		AddNode();
		break;
	default:
		AddNode();
	}
	return 0;
}

/*Scoped with braces{}*/
int ScopeDeal(FILE* fp)
{
	int i = 0;
	long tempfp = 0;
	char nowstate[MAX_STRING], laststate[MAX_STRING];

	while (fgets(nowstate, MAX_STRING, fp) != NULL) {
		nowline++;
		/*Exit from scope*/
		if (CheckState(nowstate) == RIGHT_BRACE_STATE) {
			fgets(nowstate, MAX_STRING, fp);
			nowline++;
			startline = nowline + 1;
			break;
		}
		/*Generate new block*/
		else if (CheckState(nowstate) == NEWLINE_STATE) {
			switch (CheckState(laststate)) {
			case IF_STATE:
				AddNode();
				IfBranch(fp);
				break;
			case FOR_STATE:
			case WHILE_STATE:
				AddNode();
				LoopBranch(fp);
				break;
			case DO_STATE:
				AddNode();
				DoBranch(fp);
				break;
			case SWITCH_STATE:
				AddNode();
				SwitchBranch(fp);
				break;
			case LEFT_BRACE_STATE:
				startline = nowline + 1;
				ScopeDeal(fp);
				break;
			case BREAK_STATE:
				NodeBreakStack();
				AddNode();
				break;
			case CONTINUE_STATE:
				NodeContinueStack();
				AddNode();
				break;
			default:
				AddNode();
			}
		}
		strcpy(laststate, nowstate);
	}
	return 0;
}

/*Special condition of switch*/
int SwitchDeal(FILE* fp)
{
	int i = 0;
	long tempfp = 0;
	char nowstate[MAX_STRING], laststate[MAX_STRING];

	tempfp = ftell(fp);
	while (fgets(nowstate, MAX_STRING, fp) != NULL) {
		nowline++;
		/*Exit from scope*/
		if (CheckState(nowstate) ==CASE_STATE ||CheckState(nowstate) ==DEFAULT_STATE \
			||CheckState(nowstate) == RIGHT_BRACE_STATE) {
			fseek(fp, tempfp, SEEK_SET);
			nowline--;
			break;
		}
		/*Generate new block*/
		else if (CheckState(nowstate) == NEWLINE_STATE) {
			switch (CheckState(laststate)) {
			case IF_STATE:
				AddNode();
				IfBranch(fp);
				break;
			case FOR_STATE:
			case WHILE_STATE:
				AddNode();
				LoopBranch(fp);
				break;
			case DO_STATE:
				AddNode();
				DoBranch(fp);
				break;
			case SWITCH_STATE:
				AddNode();
				SwitchBranch(fp);
				break;
			case LEFT_BRACE_STATE:
				startline = nowline + 1;
				ScopeDeal(fp);
				break;
			case BREAK_STATE:
				NodeBreakStack();
				AddNode();
				break;
			case CONTINUE_STATE:
				NodeContinueStack();
				AddNode();
				break;
			default:
				AddNode();
			}
		}
		strcpy(laststate, nowstate);
		tempfp = ftell(fp);
	}
	return 0;
}

/*Add new node*/
int AddNode()
{
	node[nodenum].nodenum = nodenum;
	node[nodenum].startline = startline;
	node[nodenum].endline = nowline - 1;
	startline = nowline + 1;
	befnum = 1;
	befnode[0] = nodenum;
	nodenum++;
	return 0;
}

int NodeIfStack()
{
	int i = 0;
	for (i = 0; i < befnum; i++) {
		Iftree->node[Iftree->nodenum] = befnode[i];
		Iftree->nodenum++;
	}
	return 0;
}

int NodeBreakStack()
{
	Loopswitchtree->breaks[Loopswitchtree->breaknum] = nodenum;
	Loopswitchtree->breaknum++;
	return 0;
}

int NodeContinueStack()
{
	struct Loopswitch *Loopswitchtest = NULL;
	Loopswitchtest = Loopswitchtree;
	while(Loopswitchtest->continuenum == -1) {
		Loopswitchtest = Loopswitchtest->next;
	}
	Loopswitchtest->continues[Loopswitchtest->continuenum] = nodenum;
	Loopswitchtest->continuenum++;
	return 0;
}

/*Generate edge of goto*/
int GotoWhere(FILE* fp, char* string, int labelnode)
{
	int nodenum = -1;
	long gotofp = 0, lastfp = 0;
	char nowstate[MAX_STRING], *temp;
	char String[100] = "JUM-";
	temp = strstr(string, "LAB-");
	temp = strstr(temp, " ");
	temp[0] = '\0';
	strcat(String, &strstr(string, "LAB-")[4]);
	gotofp = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	while (fgets(nowstate, MAX_STRING, fp) != NULL) {
		if (strstr(nowstate, String) != NULL) {
			node[nodenum].nextnode = -1;
			node[nodenum].jumpnode = labelnode;
		}
		else if (CheckState(nowstate) == NEWLINE_STATE) {
			nodenum++;
			do {
				lastfp = ftell(fp);
				if (fgets(nowstate, MAX_STRING, fp) == NULL)
					break;
			} while (CheckState(nowstate) == LEFT_BRACE_STATE || CheckState(nowstate) == RIGHT_BRACE_STATE\
				|| CheckState(nowstate) == DEFAULT_STATE || CheckState(nowstate) == ELSE_STATE
				|| CheckState(nowstate) == NEWLINE_STATE);
			fseek(fp, lastfp, SEEK_SET);
		}
	}
	fseek(fp, gotofp, SEEK_SET);
	return nodenum;
}

/*Deal with one block of statement*/
int Node2before(int nodenum)
{
	int i;
	for (i = 0; i < befnum; i++) {
		node[befnode[i]].jumpnode = nodenum;
	}
	befnum = 1;
	befnode[0] = nodenum;
	return 0;
}