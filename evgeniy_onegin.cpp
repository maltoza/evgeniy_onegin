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

#define DELETE_NOT_ALPHAS false
#define DONT_DELETE_NOT_ALPHAS true

// коды ошибок
enum ERRORS     
{
    ERRORS_OK = 0, // нет ошибки
    ERRORS_FOPEN,  // ошибка открытия файла
    ERRORS_FREAD,  // ошибка чтения файла
    ERRORS_FWRITE, // ошибка записи в файл
    ERRORS_SIZE,   // ошибка размера данных
    ERRORS_GETMEM  // ошибка выделения памяти
};

//поменять порядок
ERRORS read_from_file(char* (*ind_main)[MAX_NUM_LINES], size_t* num_lines);
ERRORS write_to_file (const char* const (*ind)[MAX_NUM_LINES], const size_t num_lines, bool flag);
bool is_alphas_in(const char* str);
int free_values(char* ind[], const size_t num_lines);
int free_ptrs(char* ind[], const size_t num_lines);
ERRORS bubble_sort(void* array, const size_t num_elems, const size_t size_el, int (*comp)(void** prev_num, void** next_num));
int compare_char_up(void** prev_str, void** next_str);
int compare_char_down(void** prev_str, void** next_str);
char* reverse(char** str);
int strcmp_my(char* str1, char* str2);
char* eat_not_symb(char** str);
ERRORS swap(void** str1, void** str2);


int main()
{
    // очистка файла для записи
    FILE* clean = fopen(OUTPUT_FILE_NAME, "w");
    fclose(clean);

    char* ind_main[MAX_NUM_LINES] = {};
    char* ind_copy[MAX_NUM_LINES] = {};
    size_t num_lines = 0;

    // считывание строк из файла
    ERRORS result = read_from_file(&ind_main, &num_lines);
    if(result != ERRORS_OK)
    {
        return result;
    }

    // копирование во второй файл
    memcpy((void*)ind_copy, (void*)ind_main, num_lines * sizeof(ind_main[0]));
    
    // вывод строк в файл
    result = write_to_file((const char* const (*)[MAX_NUM_LINES])&ind_main, num_lines, DONT_DELETE_NOT_ALPHAS);
    if(result != ERRORS_OK)
    {
        return result;
    }

    // сортировка по алфавиту с начала строки
    bubble_sort((void*)ind_main, num_lines, sizeof(char*), compare_char_up);

    // вывод строк, отсортированных в алфавитном порядке с начала
    result = write_to_file((const char* (*)[MAX_NUM_LINES])&ind_main, num_lines, DELETE_NOT_ALPHAS);
    if(result != ERRORS_OK)
    {
        return result;
    }
    
    // сортировка по алфавиту по концу
    bubble_sort((void*)ind_main, num_lines, sizeof(char*), compare_char_down);
    
    //вывод в алфавитном порядке(строки отсортированы по концу)
    result = write_to_file((const char* (*)[MAX_NUM_LINES])&ind_main, num_lines, DELETE_NOT_ALPHAS);
    if(result != ERRORS_OK)
    {
        return result;
    }
    
    // вывод оригинала
    result = write_to_file((const char* const (*)[MAX_NUM_LINES])&ind_copy, num_lines, DONT_DELETE_NOT_ALPHAS);
    if(result != ERRORS_OK)
    {
        return result;
    }

    // очистка памяти
    free_values(ind_main, num_lines);
    free_ptrs(ind_main, num_lines);
    free_ptrs(ind_copy, num_lines);
    

    // индикатор окончания программы
    printf("Done\n");

    return 0;
}

ERRORS read_from_file(char* (*ind_main)[MAX_NUM_LINES], size_t* num_lines)
{
    // проверка входных парамеров
    assert(ind_main != NULL);
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
        (*ind_main)[*num_lines] = strdup(buf);
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
ERRORS write_to_file(const char* const (*ind)[MAX_NUM_LINES], const size_t num_lines, bool flag)
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
        if (flag || is_alphas_in((*ind)[i]))
        {
            fprintf(file, "%s", (*ind)[i]);
        }
    }
    fprintf(file, "\n\n=======================================================================================\n\n");

    fclose(file);
    return ERRORS_OK;
}


// проверка на наличие букв в строке
bool is_alphas_in(const char* str)
{
    // проверка входных параметров
    assert(str != NULL);

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (isalpha(str[i]))
        {
            return true;
        }
    }

    return false;
}

// очистка памяти
int free_values(char* ind[], const size_t num_lines)
{
    // проверка входных данных
    assert(ind != NULL);

    // очистка элементов массива
    for (size_t i = 0; i < num_lines; i++)
    {
        free(ind[i]);
    }

    return 0;
}

int free_ptrs(char* ind[], const size_t num_lines)
{
    // проверка входных данных
    assert(ind != NULL);

    // очистка элементов массива
    for (size_t i = 0; i < num_lines; i++)
    {
        ind[i] = NULL;
    }

    return 0;
}


// сортировка пузырьком
ERRORS bubble_sort(void* array, const size_t num_elems, const size_t size_el, int (*comp)(void** prev_num, void** next_num))
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

int compare_char_down(void** prev_str, void** next_str)
{
    if (strcmp_my(reverse((char**)(prev_str)), reverse((char**)(next_str))) > 0)
    {
        swap(prev_str, next_str);
        return 1;
    }

    return 0; 
}

char* reverse(char** str)
{
    assert(str != NULL);

    size_t len_str = strlen(*str) - 1;

    for (size_t i = 0; i < len_str; i++)
    {
        char temp = (*str)[i];
        (*str)[i] = (*str)[len_str - i - 1];
        (*str)[len_str - i - 1] = temp;
    }

    return *str;
}

// сравнение строк
int strcmp_my(char* str1, char* str2)
{
    // проверка входных параметров
    assert(str1 != NULL);
    assert(str2 != NULL);

    // перебор символов
    while(str1 != '\0' && str2 != '\0')
    {
        eat_not_symb(&str1);
        eat_not_symb(&str2);
        if (tolower(*str1) != tolower(*str2))
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
ERRORS swap(void** str1, void** str2)
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
