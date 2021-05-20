#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

char *readFile(char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL)
        return NULL;

    if (fseek(file, 0, SEEK_END) != 0)
        return NULL;

    long fileSize = ftell(file);
    if (fileSize == -1L)
        return NULL;

    if (fseek(file, 0, SEEK_SET) != 0)
        return NULL;

    char *content = malloc(fileSize + 1);
    int c;
    long i = 0;

    while ((c = fgetc(file)) != EOF) {
        content[i++] = (char) c;
    }

    content[fileSize] = 0;

    fclose(file);

    return content;
}
