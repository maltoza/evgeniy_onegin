#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>


#define MAX_BUF       100 // максимальная длина строки
#define MAX_NUM_LINES 10000 // максимально количество линий

#define INPUT_FILE_NAME  "poem.txt"   // входной файл
#define OUTPUT_FILE_NAME "output.txt" // выходной файл

// коды ошибок
enum ERRORS     
{
    ERRORS_OK = 0, // нет ошибки
    ERRORS_FOPEN,  // ошибка открытия файла
    ERRORS_FREAD,  // ошибка чтения файла
    ERRORS_FWRITE, // ошибка записи в файл
    ERRORS_SIZE    // ошибка размера данных
};

enum ERRORS read_from_file(char* (*ind_src)[MAX_NUM_LINES], size_t* num_lines);
enum ERRORS write_to_file (const char* const (*ind)[MAX_NUM_LINES], const size_t num_lines);
int free_mem(char* ind[], const size_t num_lines);
enum ERRORS bubble_sort(void* array, const size_t num_elems, const size_t size_el, int (*comp)(void** prev_num, void** next_num));
int compare_char_up(void** prev_str, void** next_str);
int strcmp_my(char* str1, char* str2);
char* eat_not_symb(char** str);
enum ERRORS swap(void** str1, void** str2);

int main()
{
    // очистка файла для записи
    FILE* clean = fopen(OUTPUT_FILE_NAME, "w");
    fclose(clean);

    char* ind_main[MAX_NUM_LINES] = {};
    char* ind_copy[MAX_NUM_LINES] = {};
    size_t num_lines = 0;

    //считывание строк из файла
    enum ERRORS result = read_from_file(&ind_main, &num_lines);
    if(result != ERRORS_OK)
    {
        return result;
    }

    //вывод строк в файл
    result = write_to_file((const char* const (*)[MAX_NUM_LINES])&ind_main, num_lines);
    if(result != ERRORS_OK)
    {
        return result;
    }

    // сортировка по алфавиту с начала строки (пузырьком)
    bubble_sort((void*)ind_main, num_lines, sizeof(char*), compare_char_up);

    //вывод строк, отсортированных в алфавитном порядке с начала
    result = write_to_file((const char* (*)[MAX_NUM_LINES])&ind_main, num_lines);
    if(result != ERRORS_OK)
    {
        return result;
    }

/*
    sort_strings_end();
    write_to_file(); //вывод в алфавитном порядке(строки отсортированы по концу)
    write_to_file(); // вывод оригинала
*/
    // очистка памяти
    free_mem(ind_main, num_lines);
    free_mem(ind_copy, num_lines);

    // индикатор окончания программы
    printf("Done\n");

    return 0;
}

enum ERRORS read_from_file(char* (*ind_src)[MAX_NUM_LINES], size_t* num_lines)
{
    // проверка входных парамеров
    assert(ind_src != NULL);
    assert(num_lines != NULL);

    // открытие файла для чтения
    FILE* file = fopen(INPUT_FILE_NAME, "r");
    if (file == NULL)
    {
        // если файл не открыт
        printf("Open file to read error\n");
        return ERRORS_FOPEN;
    }

    // чтение построчно из файла и запись указателя на считанную строку в массив
    char buf[MAX_BUF] = {};
    while(fgets(buf, MAX_BUF, file) != NULL)
    {
        (*ind_src)[*num_lines] = strdup(buf);
        (*num_lines)++;
        if ((*num_lines) > MAX_NUM_LINES)
        {
            printf("Read file error");
            fclose(file);
            return ERRORS_FREAD;
        }
    }

    fclose(file);
    return ERRORS_OK;
}

//запись текста в файл
enum ERRORS write_to_file(const char* const (*ind)[MAX_NUM_LINES], const size_t num_lines)
{
    // проверка входных данных
    assert(ind != NULL);
    assert(num_lines != 0);
    if (num_lines == 0)
    {
        return ERRORS_SIZE;
    }

    // открытие файла для записи
    FILE* file = fopen(OUTPUT_FILE_NAME, "a+");
    if (file == NULL)
    {
        // ошибка открытия файла
        printf("Open file to write error");
        return ERRORS_FWRITE;
    }

    // вывод данных в файл
    for (size_t i = 0; i < num_lines; i++)
    {
        if ((*ind)[i] != "\n")
        {
            fprintf(file, "%s", (*ind)[i]);
        }
    }
    fprintf(file, "\n\n=======================================================================================\n\n");

    fclose(file);
    return ERRORS_OK;
}

// очистка памяти
int free_mem(char* ind[], const size_t num_lines)
{
    // проверка входных данных
    assert(ind != NULL);

    // очистка элементов массива
    for (size_t i = 0; i < num_lines; i++)
    {
        free(ind[i]);
        ind[i] = NULL;
    }

    return 0;
}


// сортировка пузырьком
enum ERRORS bubble_sort(void* array, const size_t num_elems, const size_t size_el, int (*comp)(void** prev_num, void** next_num))
{
    // проверка входных параметров
    assert(array != NULL);
    assert(comp != NULL);

    // сортировка
    for (size_t n = 0; n < num_elems; n++)
    {
        size_t num_swaps = 0;
        for (size_t i = 0; i < num_elems - n - 1; i++)
        {
            num_swaps += comp((void**)((uintptr_t)array + i * size_el),
                              (void**)((uintptr_t)array + (i + 1) * size_el));
        }
        if (num_swaps == 0)
        {
            break;
        }
    }

    return ERRORS_OK;
}


// обмен строк по условию
int compare_char_up(void** prev_str, void** next_str)
{
    if (strcmp_my(*(char**)prev_str, *(char**)next_str) > 0)
    {
        swap(prev_str, next_str);
        return 1;
    }

    return 0;
}

// сравнение строк
int strcmp_my(char* str1, char* str2)
{
    // проверка входных параметров
    assert(str1 != NULL);
    assert(str2 != NULL);

    // перебор символов
    while((str1 = eat_not_symb(&str1)) != '\0' && (str2 = eat_not_symb(&str2)) != '\0')
    {
        if (tolower(*str1) != tolower(*str2) && toupper(*str1) != toupper(*str2))
        {
            return tolower(*str1) - tolower(*str2);
        }
        str1++;
        str2++;
    }

    return 0;
}


// устранение не букв
char* eat_not_symb(char** str)
{
    while(!isalpha(**str) && ((**str) != '\0'))
    {
        (*str)++;
    }

    return *str;
}

// обмен указателей на строки
enum ERRORS swap(void** str1, void** str2)
{
    // проверка входных параметров
    assert(str1 != NULL);
    assert(str2 != NULL);
    
    //обмен
    void* temp = *str1;
    *str1 = *str2;
    *str2 = temp;

    return ERRORS_OK;
}
