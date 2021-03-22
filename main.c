#include<FuncDef.h>

int BitdataAnalyze(char* DirData, char* ProgramaName, int sign);
int CdataAnalyze(char* DirData, char* ProgramaName);

int main(int argc, char** argv)
{
	int i = 0;

	/*Deal with paraments*/
	if (argc < 4) {
		printf("main:The paraments are wrong!");
		return 0;
	}
	else {
		for (i = 3; i < argc; i++) {
			if (strcmp(argv[i], "-x86-O0") == 0)
				BitdataAnalyze(argv[1], argv[2], 0);
			else if (strcmp(argv[i], "-x86-O1") == 0)
				BitdataAnalyze(argv[1], argv[2], 1);
			else if (strcmp(argv[i], "-x86-O2") == 0)
				BitdataAnalyze(argv[1], argv[2], 2);
			else if (strcmp(argv[i], "-x86-O3") == 0)
				BitdataAnalyze(argv[1], argv[2], 3);
			else if (strcmp(argv[i], "-mips-O0") == 0)
				BitdataAnalyze(argv[1], argv[2], 4);
			else if (strcmp(argv[i], "-mips-O1") == 0)
				BitdataAnalyze(argv[1], argv[2], 5);
			else if (strcmp(argv[i], "-mips-O2") == 0)
				BitdataAnalyze(argv[1], argv[2], 6);
			else if (strcmp(argv[i], "-mips-O3") == 0)
				BitdataAnalyze(argv[1], argv[2], 7);
			else if (strcmp(argv[i], "-c") == 0)
				CdataAnalyze(argv[1], argv[2]);
		}
	}

	return 0;
}


/*------------------------------Deal with Bit data------------------------------------*/

int BitdataAnalyze(char* DirData,char* ProgramaName,int sign){
	/*Variable declarations*/
	char buff[4* MAX_PATH], bufs[4 * MAX_PATH];
	char dirSource[MAX_PATH], dirDest[MAX_PATH];
	char dirFile[MAX_PATH], dirFunc[MAX_PATH], fileDir[MAX_PATH];
	char idaPath[MAX_PATH] = "C:\\Program Files\\IDA 7.0\\ida64.exe";
	char para[MAX_PATH] = "-A -c -SC:\\Users\\nocan\\Desktop\\MasterThesis\\Code\\PCFG_IDAPython\\main.py ";
	char file[MAX_PATH] = "C:\\Users\\nocan\\Desktop\\MasterThesis\\Code\\PCFG_IDAPython\\Paraments.py";
	WIN32_FIND_DATA FindFileData;
	HANDLE hListFile;
	FILE *fp;

	/*Judge whether have source data and dest dir*/
	strcpy(dirSource, DirData);
	strcat(dirSource, "Source\\");
	strcat(dirSource, ProgramaName);
	char fileType[8][20] = { "\\x86-O0\\" ,"\\x86-O1\\" ,"\\x86-O2\\","\\x86-O3\\" ,
		"\\mips-O0\\","\\mips-O1\\","\\mips-O2\\","\\mips-O3\\", };
	strcat(dirSource, fileType[sign]);
	if (access(dirSource, 0) != 0) {
		printf("BitdataAnalyze:%s\'s x86 or mips data loss\n", ProgramaName);
		return 1;
	}

	strcpy(dirDest, DirData);
	strcat(dirDest, "Dest\\");
	if (access(dirDest, 0) != 0) {
		if (_mkdir(dirDest, NULL) != 0)
			return 1;
	}
	strcat(dirDest, ProgramaName);
	strcat(dirDest, "\\");
	if (access(dirDest, 0) != 0) {
		if (_mkdir(dirDest, NULL) != 0)
			return 1;
	}

	/*Divide paraments to python file*/
	fp = fopen(file, "wb");
	char saveType[8][20] = { "DataX86-O0.json" ,"DataX86-O1.json" ,"DataX86-O2.json","DataX86-O3.json" ,
		"DataMIPS-O0.json","DataMIPS-O1.json","DataMIPS-O2.json","DataMIPS-O3.json", };
	if (sign < 4)
		fputs("SIGN=0\nSAVEDIR=r\'", fp);
	else
		fputs("SIGN=1\nSAVEDIR=r\'", fp);
	strcat(dirDest, saveType[sign]);
	remove(dirDest);
	strcat(dirDest, "\'");
	fputs(dirDest,fp);
	fclose(fp);

	/*Deal with data*/
	strcpy(fileDir, dirSource);
	strcat(fileDir, "*");
	hListFile = FindFirstFileA(fileDir, &FindFileData);
	if (hListFile == INVALID_HANDLE_VALUE) {
		printf("BitdataAnalyze error:%d", GetLastError());
		return 1;
	}
	else {
		do {
			if (!(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) {
				strcpy(buff, idaPath);
				GenerateDir(buff);
				strcpy(fileDir, buff);
				strcat(fileDir, para);
				strcpy(buff, dirSource);
				strcat(buff, FindFileData.cFileName);
				GenerateDir(buff);
				strcat(fileDir, buff);

				printf("BitdataAnalyze:%s\n", fileDir);
				system(fileDir);

				/*Judge whether the file has been deal by IDApro  */
				do {
					if (CheckI64(dirSource) != 0)
						break;
					Sleep(1000);
				} while (1);

			}
		} while (FindNextFileA(hListFile, &FindFileData));
	}
	printf("BitdataAnalyze:%s data has been analyzed\n", fileType[sign]);

	return 0;
}


/*------------------------------Deal with C data------------------------------------*/

int CdataAnalyze(char* DirData, char* ProgramaName) {
	/*Variable declarations*/
	int i = 0, nodeNum = 0;
	char dirSource[MAX_PATH], dirDest[MAX_PATH];
	char dirFunc[MAX_PATH], fileDir[MAX_PATH], destDir[MAX_PATH],fileDest[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	HANDLE hListFile;

	/*Judge whether have source data and dest dir*/
	strcpy(dirSource, DirData);
	strcat(dirSource, "Source\\");
	strcat(dirSource, ProgramaName);
	strcat(dirSource, "\\c\\");
	if (access(dirSource, 0) != 0) {
		printf("CdataAnalyze:%s\'s c data loss\n", ProgramaName);
		return 1;
	}

	strcpy(dirDest, DirData);
	strcat(dirDest, "Dest\\");
	if (access(dirDest, 0) != 0) {
		if (_mkdir(dirDest, NULL) != 0)
			return 1;
	}
	strcat(dirDest, ProgramaName);
	strcat(dirDest, "\\");
	if (access(dirDest, 0) != 0) {
		if (_mkdir(dirDest, NULL) != 0)
			return 1;
	}

	strcpy(fileDest, dirDest);
	strcat(fileDest, "DataC.json");
	remove(fileDest);

	strcpy(dirFunc, dirDest);
	strcat(dirFunc, "Function\\");
	if (access(dirFunc, 0) != 0) {
		if (_mkdir(dirFunc, NULL) != 0)
			return 1;
	}
#if 0
	/*Extract .c file for lexical analysis and basic block division*/
	strcpy(fileDir, dirSource);
	strcat(fileDir, "*");
	hListFile = FindFirstFileA(fileDir, &FindFileData);
	if (hListFile == INVALID_HANDLE_VALUE) {
		printf("CdataAnalyze error:%d", GetLastError());
		return 1;
	}
	else {
		do{
			if (!(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) {
				strcpy(fileDir, dirSource);
				strcat(fileDir, FindFileData.cFileName);
				strcpy(destDir, dirFunc);
				strcat(destDir, FindFileData.cFileName);

				printf("CdataAnalyze:%s\n", FindFileData.cFileName);
				LexicalEntrance(fileDir, destDir);
			}
		} while (FindNextFileA(hListFile, &FindFileData));
	}
#endif
#if 1
	/*Obtain node info from Function files*/
	strcpy(fileDir, dirFunc);
	strcat(fileDir, "*");
	hListFile = FindFirstFileA(fileDir, &FindFileData);
	if (hListFile == INVALID_HANDLE_VALUE) {
		printf("CdataAnalyze error:%d", GetLastError());
		return 1;
	}
	else {
		do {
			if (!(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) {
				strcpy(fileDir, dirFunc);
				strcat(fileDir, FindFileData.cFileName);

				printf("CdataAnalyze:%s\n", FindFileData.cFileName);
				GenerateCFG(fileDir, &nodeNum);
				if (nodeNum <= 1) {
					remove(fileDir);
					if (nodeNum > 0)
						free(node);
					continue;
				}
				GnerateFeature(fileDir, nodeNum);
				GenerateJson(dirDest, fileDir, nodeNum);
				free(node);
			}
		} while (FindNextFileA(hListFile, &FindFileData));
	}
#endif
	printf("CdataAnalyze:C data has been analyzed\n");
	return 0;
}