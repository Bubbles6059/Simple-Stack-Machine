#include "bof.h"
#include "instruction.h"
#include "machine_types.h"
#include "string.h"


//Both used to check for the file name and the flag if you were to place it anywhere in the command line. 
bool pFlagChecker(int arraySize, char** stringArray);
char* findFileName(int arraySize, char** stringArray);

int main(int argc, char** argv) {

    BOFFILE boffile = bof_read_open(findFileName(argc, argv));
    if(pFlagChecker(argc, argv)) {
        printf("Found file creating header");
        BOFHeader bofHeader = bof_read_header(boffile);
    }
    return 0;
}

bool pFlagChecker(int arraySize, char** stringArray) {
    for(int i = 0; i < arraySize; i++) {
        if(strcmp(stringArray[i], "-p") == 0)
            return true;
    }
    return false;
}

char* findFileName(int arraySize, char** stringArray) {
    for(int i = 0; i < arraySize; i++) {
        if(strcmp(stringArray[i], "./vm") != 0 && strcmp(stringArray[i], "-p") != 0)
            return stringArray[i];
    }
    return "";
}