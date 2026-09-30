#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <sys\stat.h>
#include <fcntl.h>

#define INPUT_FILE_NAME  "poem.txt"   // входной файл
#define OUTPUT_FILE_NAME "output.txt" // выходной файл

// коды ошибок
enum ERRORS     
{
    FUNK_OK = 0,  // нет ошибки
    ERRORS_FOPEN,   // ошибка открытия файла
    ERRORS_FREAD,   // ошибка чтения файла
    ERRORS_FWRITE,  // ошибка записи в файл
    ERRORS_SIZE,    // ошибка размера данных
    ERRORS_GETMEM,  // ошибка выделения памяти
    ERRORS_GETFDATA // ошибка заполнения структуры с данными о файле
};

// параметры строки
struct str_el
{
    size_t len; // длина строки
    char*  str; // указатель на строку
};


ERRORS search_file_size(size_t* file_size);
ERRORS read_from_file(void** buffer, size_t* num_symb, size_t* num_lines);
ERRORS count_lines(char* buffer, size_t num_symb, size_t* num_lines);
ERRORS fill_ind_array(str_el** ind, char *buf, size_t num_symb, size_t num_lines);
size_t strlen_my(const char* const str);

ERRORS bubble_sort(void* array, const size_t num_elems, const size_t size_el, int (*comp)(void* prev_num, void* next_num, const size_t size_el));
int compare_char_up(void* prev_str, void* next_str, const size_t size_el);
int compare_char_down(void* prev_str, void* next_str, const size_t size_el);
int strcmp_my(char* str1, char* str2);
char* eat_not_symb(char** str);
int strcmp_reverse(char* str1, char* str2);
int eat_not_symb_reverse(char* str, int* i);
ERRORS swap(void* str1, void* str2, const size_t size_el);

ERRORS write_to_file (str_el* ind, const size_t num_lines);
ERRORS write_buffer_to_file(char* buf, const size_t num_symb);

int free_mem(char** ind, size_t max_els);



int main()
{
    size_t num_symb = 0;  // количество символов в файле
    void* buffer;         // буфер данных файла
    size_t num_lines = 0; // количество линий в файле
    str_el* ind;          // массив указателей на строки

    // чтение из файла
    ERRORS result = read_from_file(&buffer, &num_symb, &num_lines);            
    if(result != FUNK_OK) return result;

    // заполнение массива указателей
    fill_ind_array(&ind, (char*)buffer, num_symb, num_lines);

    // сортировка по алфавиту с начала строки
    bubble_sort(ind, num_lines, sizeof(str_el), compare_char_up);

    // вывод строк, отсортированных в алфавитном порядке с начала
    result = write_to_file((str_el*)ind, num_lines);
    if(result != FUNK_OK) return result;
    
    // сортировка по алфавиту по концу
    bubble_sort(ind, num_lines, sizeof(str_el), compare_char_down);
    
    //вывод в алфавитном порядке(строки отсортированы по концу)
    result = write_to_file((str_el*)ind, num_lines);
    if(result != FUNK_OK) return result;
    
    // вывод оригинала
    result = write_buffer_to_file((char*)buffer, num_symb); // сортировку указателей по возрастанию
    if(result != FUNK_OK) return result;

    // очистка памяти
    free(ind);
    free(buffer);

    // индикатор окончания программы
    printf("Done\n");

    return 0;
}


// нахождение размера файла
ERRORS search_file_size(size_t* file_size)
{
    assert(file_size != NULL);

    // считывание информации о файле
    struct stat buff;
    if (stat(INPUT_FILE_NAME, &buff) != 0)
    {
        printf("Read file data error\n");
        return ERRORS_GETFDATA;
    }

    // размер файла
    *file_size = (size_t)buff.st_size;

    return FUNK_OK;
}


// считывание текста из файла
ERRORS read_from_file(void** buffer, size_t* num_symb, size_t* num_lines)
{
    // очистка файла для записи
    FILE* clean = fopen(OUTPUT_FILE_NAME, "w");
    fclose(clean);

    // нахождение размера файла(количества байт)
    ERRORS result = search_file_size(num_symb); // открытие файла отдельно
    if(result != FUNK_OK) return result;
    
    *buffer = calloc(*num_symb, sizeof(char));
    int fd = open(INPUT_FILE_NAME, O_BINARY);
    if (fd == -1)
    {
        // если файл не открыт
        printf("Open file to read error\n");
        return ERRORS_FOPEN;
    }
    int res = read(fd, *buffer, *num_symb);
    if (res == -1)
    {
        // если файл не открыт
        printf("Open file to read error\n");
        return ERRORS_FOPEN;
    }

    *num_symb = (size_t)res;

    result = count_lines((char*)buffer, *num_symb, num_lines);
    if(result != FUNK_OK) return result;

    close(fd);
    return FUNK_OK;
}



// подсчёт количества строк
ERRORS count_lines(char* buffer, size_t num_symb, size_t* num_lines)
{
    (*num_lines) = 0;
    for (size_t i = 0; i < num_symb; i++)
    {
        if(buffer[i] == '\n')
        {
            (*num_lines)++;
        }
    }
    
    return FUNK_OK;
}



ERRORS fill_ind_array(str_el** ind, char *buf, size_t num_symb, size_t num_lines)
{
    *ind = (str_el*)calloc(num_lines, sizeof(str_el));
    
    // защита: если буфер пустой или строк не требуется, ничего не делаем
    if (num_symb == 0 || num_lines == 0 || buf == NULL || ind == NULL) {
        return FUNK_OK; 
    }
 
    // первая строка всегда начинается с самого начала буфера
    (*ind)[0].str = &buf[0];

    // безопасно идем по всему буферу до конца
    size_t j = 1;
    for (size_t i = 1; i < num_symb - 1; i++)
    {
        if (buf[i] == '\n')
        {   
            (*ind)[j].str = &buf[i + 1];
            (*ind)[j - 1].len = (*ind)[j].str - (*ind)[j - 1].str;
            j++;
        }
    }
    
    return FUNK_OK;
}


// сортировка пузырьком
ERRORS bubble_sort(void* array, const size_t num_elems, const size_t size_el, int (*comp)(void* prev_num, void* next_num, const size_t size_el))
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
                              (void**)((uintptr_t)array + (i + 1) * size_el),
                              size_el);
        }
        if (num_swaps == 0) break;
    }

    return FUNK_OK;
}


// обмен строк по условию
int compare_char_up(void* prev_str, void* next_str, const size_t size_el)
{
    char* a = ((str_el*)prev_str)->str;
    char* b = ((str_el*)next_str)->str;
    if (strcmp_my(a, b) > 0)
    {
        swap(prev_str, next_str, size_el);
        return 1;
    }

    return 0;
}

int compare_char_down(void* prev_str, void* next_str, const size_t size_el)
{
    char* a = ((str_el*)prev_str)->str;
    char* b = ((str_el*)next_str)->str;    
    if (strcmp_reverse(a, b) > 0)
    {
        swap(prev_str, next_str, size_el);
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
    while(*str1 != '\n' && *str2 != '\n')
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
    while(((**str) != '\n') && (!isalpha(**str)))
    {
        (*str)++;
    }

    return *str;
}



int strcmp_reverse(char* str1, char* str2)
{
    // проверка входных параметров
    assert(str1 != NULL);
    assert(str2 != NULL);

    // перебор символов
    int str1_ind = strlen_my(str1);
    int str2_ind = strlen_my(str2);
    
    while(str1_ind >= 0 && str2_ind >= 0)
    {
        eat_not_symb_reverse(str1, &str1_ind);
        eat_not_symb_reverse(str2, &str2_ind);

        if (tolower(str1[str1_ind]) != tolower(str2[str2_ind]))
        {
            return tolower(str1[str1_ind]) - tolower(str2[str2_ind]);
        }

        str1_ind--;
        str2_ind--;
    }

    return 0;
}



size_t strlen_my(const char* const str)
{
    assert(str != NULL);

    size_t length = 0;
    for (size_t i = 0; str[i] != '\n'; i++)
    {
        length++;
    }

    return length;
}



int eat_not_symb_reverse(char* str, int* i)
{
    while((*i) >= 0 && !isalpha(str[*i]))
    {
        (*i)--;
    }

    return 0;
}


// обмен указателей на строки
ERRORS swap(void* str1, void* str2, const size_t size_el)
{
    // проверка входных параметров
    assert(str1 != NULL);
    assert(str2 != NULL);
    
    //обмен
    str_el temp;
    memcpy(&temp, str1, size_el);
    memcpy(str1, str2, size_el);
    memcpy(str2, &temp, size_el);

    return FUNK_OK;
}


//запись текста в файл
ERRORS write_to_file(str_el* ind, const size_t num_lines)
{
    // проверка входных данных
    assert(ind != NULL);
    assert(num_lines != 0);
    if (num_lines == 0) return ERRORS_SIZE;

    int fd = open(OUTPUT_FILE_NAME, O_BINARY | O_WRONLY | O_APPEND);
    if (fd == -1)
    {
        // если файл не открыт
        printf("Open file to read error\n");
        return ERRORS_FOPEN;
    }
    
    // вывод данных в файл
    for (size_t i = 0; i < num_lines; i++)
    {
        write(fd, ind[i].str, ind[i].len);
    }

    write(fd, "\n\n=======================================================\n\n", 60);

    close(fd);

    return FUNK_OK;
}



ERRORS write_buffer_to_file(char* buf, const size_t num_symb)
{
    assert(buf != NULL);
    if(num_symb == 0) return FUNK_OK;
    
    // открытие файла
    int fd = open(OUTPUT_FILE_NAME, O_BINARY | O_WRONLY | O_APPEND);
    if (fd == -1)
    {
        // если файл не открыт
        printf("Open file to read error\n");
        return ERRORS_FOPEN;
    }
    
    // вывод данных в файл
    write(fd, buf, num_symb);

    close(fd);
    return FUNK_OK;
}