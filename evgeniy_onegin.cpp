#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <ctype.h>

#define MAX_BUF 100
#define MAX_NUM_LINES 400

#define INPUT_FILE_NAME "poem.txt"
#define OUTPUT_FILE_NAME "output.txt"

size_t read_from_file(char** const ind_main, char** const ind_copy);
int print_strings(char** const ind, const size_t num_lines);
int free_mem(char** const ind, const size_t num_lines);
int bubble_sort(void** const data, const size_t num_elems, const size_t size_el, int (*comp)(void* const prev_num, void* const next_num));
int swap(char** value1, char** value2);
int strcmp_my(char* str1, char* str2);
int compare_strings(void* const prev, void* const next);
int skip_not_letters(const char* str);


int main()
{
    FILE* clean = fopen(OUTPUT_FILE_NAME, "w");
    fclose(clean);

    char* ind_main[MAX_NUM_LINES] = {};
    char* ind_copy[MAX_NUM_LINES] = {};
    size_t num_lines = 0;

    if((num_lines = read_from_file(ind_main, ind_copy)) == 3)
    {
        return 1;
    }

    print_strings(ind_main, num_lines);

    bubble_sort((void**)ind_main, num_lines, sizeof(char*), compare_strings);
    print_strings(ind_main, num_lines); //вывод в алфавитном порядке
/*
    sort_strings_end();
    print_strings(); //вывод в алфавитном порядке(строки отсортированы по концу)
    print_strings(); // вывод оригинала
*/
    free_mem(ind_main, num_lines);
    free_mem(ind_copy, num_lines);
    printf("Done");

    return 0;
}

size_t read_from_file(char** const ind_main, char** const ind_copy)
{
    assert(ind_main != NULL);
    assert(ind_copy != NULL);

    FILE* file = fopen (INPUT_FILE_NAME, "r");
    if (file == NULL)
    {
        printf("Open file error");
        return 3;

    }

    char buf[MAX_BUF] = {};
    size_t num_lines = 0;

    while(fgets(buf, MAX_BUF, file) != NULL)
    {
        ind_main[num_lines] = strdup(buf);
        ind_copy[num_lines] = strdup(buf);
        num_lines++;
    }

    fclose(file);
    return num_lines;
}

int print_strings(char** const ind, const size_t num_lines)
{
    assert(ind != NULL);

    FILE* file = fopen(OUTPUT_FILE_NAME, "a+");
    for (size_t i = 0; i < num_lines; i++)
    {
        fprintf(file, "%s", ind[i]);
    }
    fprintf(file, "\n\n=======================================================================================\n\n");
    fclose(file);

    return 0;
}


int free_mem(char** const ind, const size_t num_lines)
{
    assert(ind != NULL);

    for (size_t i = 0; i < num_lines; i++)
    {
        free(ind[i]);
        ind[i] = NULL;
    }

    return 0;
}



int bubble_sort(void** const data, const size_t num_elems, const size_t size_el, int (*comp)(void* const prev_num, void* const next_num))
{
    assert(data != NULL);
    assert(comp != NULL);

    for (size_t n = 0; n < num_elems; n++)
    {
        size_t num_swaps = 0;
        for (size_t i = 0; i < num_elems - n - 1; i++)
        {
            num_swaps += comp((void*)((uintptr_t)(*data) + i * size_el), (void*)((uintptr_t)(*data) + (i + 1) * size_el));
        }
        if (num_swaps == 0)
        {
            break;
        }
    }

    return 0;
}



int compare_strings(void* const prev, void* const next)
{
    assert(prev != NULL);
    assert(next != NULL);

    char* str1 = (char*)prev;
    char* str2 = (char*)next;
    if (strcmp_my(str1, str2) > 0)
    {
        swap(&str1, &str2);
        return 1;
    }

    return 0;
}



int swap(char** value1, char** value2)
{
    assert(value1 != NULL);
    assert(value2 != NULL);

    char* temp = *value1;
   *value1 = *value2;
    *value2 = temp;

    return 0;
}



int strcmp_my(char* str1, char* str2)
{
    assert(str1 != NULL);
    assert(str2 != NULL);

    char* ptr1 = str1;
    char* ptr2 = str2;

    while(*ptr1 != '\0' || *ptr2 != '\0')
    {
        skip_not_letters(ptr1);
        skip_not_letters(ptr2);
        return *ptr1 - *ptr2;
    }

    return 0;
}


int skip_not_letters(const char* str)
{
    assert(str != NULL);

    while (!isalpha(*str))
    {
        str++;
    }

    return 0;
}
