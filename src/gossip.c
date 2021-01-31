#include <string.h>
#include <stdio.h>
#include "virtual_machine.h"


int main(int argc, char* argv[])
{
    FILE* fp = fopen(argv[1], "rb");

    uint8_t metadata[12]; 
    fread(metadata, sizeof(uint8_t), 12, fp);

    int mf_i = 0;
    int cp_i = 0;
    int cp_size = 0;


    mf_i = (mf_i | metadata[3]) << 24 | (mf_i | metadata[2]) << 16 | (mf_i | metadata[1]) << 8 | (mf_i | metadata[0]); // relative
    cp_i = (cp_i | metadata[7]) << 24 | (cp_i | metadata[6]) << 16 | (cp_i | metadata[5]) << 8 | (cp_i | metadata[4]); // absolute
    cp_size = (cp_size | metadata[11]) << 24 | (cp_size | metadata[10]) << 16 | (cp_size | metadata[9]) << 8 | (cp_size | metadata[8]);

    uint8_t program[cp_i - 12];
    fread(program, sizeof(uint8_t), cp_i - 12, fp);

    uint8_t const_pool[cp_size];
    fread(const_pool, sizeof(uint8_t), cp_size, fp);

    fclose(fp);

    virt_mach* vm = construct_vm( program );
    execute_bytec (vm, mf_i, const_pool);

    return 0;
}

