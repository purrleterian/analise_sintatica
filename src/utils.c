#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

char *readFile(char *path) {
    FILE *file_ptr = fopen(path, "r");
    if (file_ptr == NULL) {
        printf("Error opening file: %s\n", path);
        return NULL;
    }
    fseek(file_ptr, 0, SEEK_END);
    long file_size = ftell(file_ptr);
    rewind(file_ptr);
    char *buffer = (char *)malloc(sizeof(char) * (file_size + 1));
    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        fclose(file_ptr);
        return NULL;
    }
    fread(buffer, sizeof(char), file_size, file_ptr);
    buffer[file_size] = '\0';
    fclose(file_ptr);
    return buffer;
}

char *getKeywordToken(char *str) {
    if (strcasecmp(str, "program") == 0) return "KW_PROGRAM";

    if (strcasecmp(str, "var") == 0) return "KW_VAR";

    if (strcasecmp(str, "integer") == 0) return "KW_INTEGER";

    if (strcasecmp(str, "real") == 0) return "KW_REAL";

    if (strcasecmp(str, "begin") == 0) return "KW_BEGIN";

    if (strcasecmp(str, "end") == 0) return "KW_END";

    if (strcasecmp(str, "if") == 0) return "KW_IF";

    if (strcasecmp(str, "then") == 0) return "KW_THEN";

    if (strcasecmp(str, "else") == 0) return "KW_ELSE";

    if (strcasecmp(str, "while") == 0) return "KW_WHILE";

    if (strcasecmp(str, "do") == 0) return "KW_DO";

    return "ID";
}
