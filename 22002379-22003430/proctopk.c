/*
 * Usage: proctopk <K> <outfile> <N> <infile1> .... <infileN>
 * N child processes will be created to process N input files.
 */

#include <stdio.h>

char *mem = "mem";
char *files[] = {};

int main(char **argv, int argc)
{
    if (argc < 5)
    {
        printf("Usage: Usage: proctopk <K> <outfile> <N> <infile1> .... <infileN>");
    }
}