#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "arrlist.h"
#include "reader.h"

ArrList readFile(char *file){
	ArrList l = initArrList(4096, sizeof(int8_t));
	FILE *fp = fopen(file, "r");
	uint8_t ins;

	while (fread(&ins, sizeof(int8_t), 1, fp) != 0){
		addArrList(&ins,l);
	}

	//printDiagsArrList(l);
	fclose(fp);

	return l;
}
