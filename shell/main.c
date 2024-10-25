#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *filename = malloc(256 * sizeof(char));

    if (filename == NULL) {
        perror("Memory allocation failed");
        return 1;
    }

    printf("Enter file name: ");
    scanf("%s", filename);

    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening file");
        free(filename);
        return 2;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        printf("%s", line);
    }

    fclose(fp);
    free(line);
    free(filename);
    
    return 0;
}