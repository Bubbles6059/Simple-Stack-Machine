#include "bof.h"
#include "instruction.h"
int main(int argc, char** argv) {
    BOFFILE boffile = bof_read_open(argv[1]);
    bof_read_header(boffile);
    return 0;
}