#include <stdlib.h>
#include <stdio.h>

#define TSTRING_INITIAL_SIZE 16

typedef struct tstring {
    int allocated;
    int length;
    char *string;
} TString;

int tstring_intialize(TString *tstr);

TString *tstring_struct_initialize();

int tstring_increase_size(TString *tstr);

int tstring_append_char(TString *tstr, char c);

int tstring_clear_string(TString *tstr);

void tstring_free_string(TString *tstr);

void tstring_free_struct(TString *tstr);

void tstring_print_string_info(TString *tstr);
