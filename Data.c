#include<FuncDef.h>
#include<cJSON.h>

int GenerateJson(char* dir, char* filename, int Nodenum)
{
	int i = 0, j = 0, k = 0;
	char *data, buf[MAX_STRING];
	FILE *fpw;
	cJSON *json;
	cJSON *Number;
	cJSON *String;
	cJSON *Arrays;
	cJSON *Array;


	json = cJSON_CreateObject();

	/*Add function name*/
	if ((data = (strstr(filename, ".FunctionBasicBlock"))) != NULL) {
		data[0] = '\0';
	}
	if ((data = (strstr(filename, "--"))) != NULL) {
		data = data + 2;
	}
	String = cJSON_CreateString(data);
	cJSON_AddItemToObject(json, "Funcname", String);

	/*Add Nodenum*/
	Number = cJSON_CreateNumber(Nodenum);
	cJSON_AddItemToObject(json, "Nodenum", Number);

	/*Add Edge*/
	Arrays = cJSON_CreateArray();
	for (i = 0; i < Nodenum; i++) {
		Array = cJSON_CreateArray();
		if (node[i].nextnode >= 0 && node[i].nextnode < Nodenum) {
			Number = cJSON_CreateNumber(node[i].nextnode);
			cJSON_AddItemToArray(Array, Number);
		}
		if (node[i].jumpnode >= 0 && node[i].jumpnode < Nodenum && node[i].jumpnode != node[i].nextnode) {
			Number = cJSON_CreateNumber(node[i].jumpnode);
			cJSON_AddItemToArray(Array, Number);
		}
		cJSON_AddItemToArray(Arrays, Array);
	}
	cJSON_AddItemToObject(json, "CFG", Arrays);

	/*Add Semantic*/
	Arrays = cJSON_CreateArray();
	cJSON_AddItemToObject(json, "PFG", Arrays);

	/*Add Semantic*/
	Arrays = cJSON_CreateArray();
	for (i = 0; i < Nodenum; i++) {
		Array = cJSON_CreateArray();
		for (j = 0; j < 5; j++) {
			Number = cJSON_CreateNumber(node[i].literal[j]);
			cJSON_AddItemToArray(Array, Number);
		}
		cJSON_AddItemToArray(Arrays, Array);
	}
	cJSON_AddItemToObject(json, "Literal", Arrays);

	/*Add Semantic*/
	Arrays = cJSON_CreateArray();
	for (i = 0; i < Nodenum; i++) {
		Array = cJSON_CreateArray();
		for (j = 0; j < MAX_SEQUENCE; j++) {
			if (node[i].semantic[j] == 0)
				break;
			Number = cJSON_CreateNumber(node[i].semantic[j]);
			cJSON_AddItemToArray(Array, Number);
		}
		cJSON_AddItemToArray(Arrays, Array);
	}
	cJSON_AddItemToObject(json, "Semantic", Arrays);

	/*Output JSON data*/
	data = cJSON_Print(json);
	i = 0;
	while (data[i] != '\0') {
		if (data[i] == '\n' || data[i] == '\t')
			data[i] = ' ';
		i++;
	}
	strcpy(buf, dir);
	strcat(buf, "DataC.json");
	if ((fpw = fopen(buf, "a")) == NULL) {
		printf("GenerateJson:error open file %s", buf);
		return -1;
	}
	fwrite(data, sizeof(char), strlen(data), fpw);
	fputc('\n', fpw);
	fclose(fpw);
	cJSON_Delete(json);
	return 0;
}