#include<FuncDef.h>

/*---------------------------------------Calculate .ida file------------------------------------*/
int CheckI64(char* srcDir)
{
	char buf[MAX_STRING];
	HANDLE hListFile;
	WIN32_FIND_DATA FindFileData;

	strcpy(buf, srcDir);
	strcat(buf, "*");
	hListFile = FindFirstFileA(buf, &FindFileData);
	if (hListFile == INVALID_HANDLE_VALUE) {
		printf("CalIdaFile:Search for file error %d", GetLastError());
		return 1;
	}
	else {
		do {
			if (!(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) \
				&&strstr(FindFileData.cFileName, ".i64") != NULL) {
				strcpy(buf, srcDir);
				strcat(buf, FindFileData.cFileName);
				remove(buf);
				return 1;
			}
		} while (FindNextFileA(hListFile, &FindFileData));
	}

	return 0;
}


/*--------------------------------------add "" to dir----------------------------------*/
int GenerateDir(char* strings)
{
	int i = 0;
	char buf[MAX_STRING];
	while (strings[i] != '\0'&&strings[i] != ' ') {
		i++;
	}
	if (strings[i] != '\0') {
		strcpy(buf, "\"");
		strcat(buf, strings);
		strcat(buf, "\" ");
		strcpy(strings, buf);
	}
	return 0;
}

/*-------------------------Judge which type of state the state belong to------------------*/
int CheckState(char* state)
{
	if (strstr(state, "if") == state) {
		return IF_STATE;
	}
	else if (strstr(state, "else if") == state) {
		return ELSEIF_STATE;
	}
	else if (strstr(state, "else") == state) {
		return ELSE_STATE;
	}
	else if (strstr(state, "for") == state) {
		return FOR_STATE;
	}
	else if (strstr(state, "while") == state) {
		return WHILE_STATE;
	}
	else if (strstr(state, "do") == state) {
		return DO_STATE;
	}
	else if (strstr(state, "switch") == state) {
		return SWITCH_STATE;
	}
	else if (strstr(state, "case") == state) {
		return CASE_STATE;
	}
	else if (strstr(state, "default") == state) {
		return DEFAULT_STATE;
	}
	else if (strstr(state, "break") == state) {
		return BREAK_STATE;
	}
	else if (strstr(state, "continue") == state) {
		return CONTINUE_STATE;
	}
	else if (strstr(state, "goto") == state) {
		return GOTO_STATE;
	}
	else if (strstr(state, "return") == state) {
		return RETURN_STATE;
	}
	else if (strstr(state, "{") == state) {
		return LEFT_BRACE_STATE;
	}
	else if (strstr(state, "}") == state) {
		return RIGHT_BRACE_STATE;
	}
	else if (strstr(state, "\n") == state) {
		return NEWLINE_STATE;
	}
	else if (strstr(state, "LAB") != NULL) {
		return LAB_STATE;
	}
	else {
		return NORMAL_STATE;
	}
}

int CheckToken(char* state)
{
	if (strstr(state, "if") == state) {
		return INSTRUCTION_JUMP;
	}
	else if (strstr(state, "else if") == state) {
		return INSTRUCTION_JUMP;
	}
	else if (strstr(state, "else") == state) {
		return INSTRUCTION_JUMP;
	}
	else if (strstr(state, "for") == state) {
		return INSTRUCTION_JUMP;
	}
	else if (strstr(state, "while") == state) {
		return INSTRUCTION_JUMP;
	}
	else if (strstr(state, "do") == state) {
		return INSTRUCTION_JUMP;
	}
	else if (strstr(state, "switch") == state) {
		return INSTRUCTION_JUMP;
	}
	else if (strstr(state, "case") == state) {
		return INSTRUCTION_JUMP;
	}
	else if (strstr(state, "default") == state) {
		return INSTRUCTION_JUMP;
	}
	else if (strstr(state, "break") == state) {
		return INSTRUCTION_JUMP;
	}
	else if (strstr(state, "continue") == state) {
		return INSTRUCTION_JUMP;
	}
	else if (strstr(state, "goto") == state) {
		return INSTRUCTION_JUMP;
	}
	else if (strstr(state, "return") == state) {
		return INSTRUCTION_RETURN;
	}
	else if (strstr(state, "CAL_0") == state) {
		return INSTRUCTION_ARITHMETICAL;
	}
	else if (strstr(state, "CAL_1") == state) {
		return INSTRUCTION_LOGICAL;
	}
	else if (strstr(state, "CAL_2") == state) {
		return INSTRUCTION_LOGICAL;
	}
	else if (strstr(state, "CAL_3") == state) {
		return INSTRUCTION_ARITHMETICAL;
	}
	else if (strstr(state, "CAL_4") == state) {
		return INSTRUCTION_LOGICAL;
	}
	else if (strstr(state, "CAL_5") == state) {
		return INSTRUCTION_TRANSFER_ARITHMETICAL;
	}
	else if (strstr(state, "CAL_6") == state) {
		return INSTRUCTION_TRANSFER_LOGICAL;
	}
	else if (strstr(state, "CAL_7") == state) {
		return INSTRUCTION_TRANSFER;
	}
	else if (strstr(state, "CAL_8") == state) {
		return INSTRUCTION_JUMP;
	}
	else if (strstr(state, "(") == state) {
		return INSTRUCTION_LEFT;
	}
	else if (strstr(state, ")") == state) {
		return INSTRUCTION_RIGHT;
	}
	else if (strstr(state, "\n") == state) {
		return -2;
	}
	return -1;
}