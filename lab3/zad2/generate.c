#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

int MAX_INT_IN_MARTIX = 5;
int MIN_INT_IN_MATRIX = 0;

int rand_int(int min, int max)
{
    return min + rand() % (max - min + 1);
}

int main(int argc, char **argv)
{
    int count = atoi(argv[1]);
    int min = atoi(argv[2]);
    int max = atoi(argv[3]);

    char *file_name = (char *)calloc(200, sizeof(char));
    FILE *list = fopen("list", "w+");

    for (int i = 0; i < count; i++)
    {
        char dop[3];
        sprintf(dop, "%d", i);
        strcpy(file_name, "tests/");
        strcat(file_name, dop);
        strcat(file_name, "_A");
        FILE *file_a = fopen(file_name, "w+");
        int a_rows = rand_int(min, max);
        int a_cols = rand_int(min, max);
        int b_cols = rand_int(min, max);
        for (int i = 0; i < a_rows; i++)
        {
            for (int j = 0; j < a_cols; j++)
            {
                int num = rand_int(MIN_INT_IN_MATRIX, MAX_INT_IN_MARTIX);

                fprintf(file_a, "%d", num);
                fprintf(file_a, " ");
            }
            fprintf(file_a, "\n");
        }
        fclose(file_a);

        fprintf(list, "%s ", file_name);
        strcpy(file_name, "tests/");
        strcat(file_name, dop);
        strcat(file_name, "_B");
        FILE *file_b = fopen(file_name, "w+");

        for (int i = 0; i < a_rows; i++)
        {
            for (int j = 0; j < b_cols; j++)
            {
                int num = rand_int(MIN_INT_IN_MATRIX, MAX_INT_IN_MARTIX);

                fprintf(file_b, "%d", num);
                fprintf(file_b, " ");
            }
            fprintf(file_b, "\n");
        }
        fclose(file_b);

        fprintf(list, "%s ", file_name);
        strcpy(file_name, "tests/");
        strcat(file_name, dop);
        strcat(file_name, "_C");
        fprintf(list, "%s\n", file_name);
    }

    return 0;
}
