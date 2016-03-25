#include "assetload.h"
#include <stdio.h>

long int get_filesize(const char* fn)
{
    /* Filesize in bytes */
    long int size = -1;

    /* Try open the file */
    FILE* f = 0;
    f = fopen(fn, "rb");
    if (!f)
        return -1;

    /* Seek to its end */
    fseek(f, 0, SEEK_END);

    /* Tell the size of the current pos */
    size = ftell(f);

    /* Close the file handle and return size */
    fclose(f);
    return size;
}

int read_file_to_mem(const char* filename, unsigned char* buf, long buf_sz)
{
    /* Try open the file */
    FILE* f = 0;
    f = fopen(filename, "rb");
    if (!f)
        return 0;

    /* Read the file in single IO operation */
    fread(buf, 1, buf_sz, f);

    /* Close the file handle and return success code */
    fclose(f);
    return 1;
}
