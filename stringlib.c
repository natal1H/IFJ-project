#include "stringlib.h"

int tstring_intialize(TString *tstr) {
  tstr->string = (char *) malloc(sizeof(char) * TSTRING_INITIAL_SIZE);
  if (tstr->string == NULL) {
    return -1; // TODO: return actual error code
  }
  tstr->allocated = TSTRING_INITIAL_SIZE;
  tstr->length = 0;
  tstr->string[0] = '\0';

  return 0; // OK
}

TString *tstring_struct_initialize() {
  //printf("Allocating: %ld, (%ld + %ld + %ld)\n", sizeof(int) + sizeof(int) + sizeof(char *), sizeof(int), sizeof(int), sizeof(char *));
  TString *mystruct = (TString *) malloc(sizeof(int) + sizeof(int) + sizeof(char *));
  if (mystruct == NULL) {
    return NULL;
  }
  if (tstring_intialize(mystruct) != 0) {
    return NULL;
  }

  return mystruct;
}

int tstring_increase_size(TString *tstr) {
  tstr->string = (char *) realloc(tstr->string, tstr->allocated + TSTRING_INITIAL_SIZE); // Zvýši allocated o TSTRING_SIZE
  if (tstr->string == NULL) {
    return -1; // TODO: return actual error code
  }
  tstr->allocated += TSTRING_INITIAL_SIZE;

  return 0; // OK
}

int tstring_append_char(TString *tstr, char c) {
  // Pozri, či je ešte miesto na pridanie znaku
  if (tstr->length + 1 > tstr->allocated - 1) {
    // Treba najskôr zväčšiť alokované miesto
    if (tstring_increase_size(tstr) != 0) {
      // Nastala chyba pri rozširovaní - zlyhala aj táto funkcia
      return -1; // TODO: return actual error code
    }
  }

  tstr->string[tstr->length] = c;
  tstr->length += 1;

  return 0;
}

int tstring_clear_string(TString *tstr) {
  if (tstr->allocated <= 0 && tstr != NULL) {
    // Chyba - nie je inicializované alebo niečo podobné
    return -1; // TODO: return actual error code
  }
  tstr->string[0] = '\0';
  tstr->length = 0;

  return 0;
}

void tstring_free_string(TString *tstr) {
  free(tstr->string);
  tstr->length = 0;
  tstr->allocated = 0;
}

void tstring_free_struct(TString *tstr) {
  tstring_free_string(tstr);
  free(tstr);
}

void tstring_print_string_info(TString *tstr) {
  printf("Allocated: %d\n", tstr->allocated);
  printf("Length: %d\n", tstr->length);
  printf("String: %s\n", tstr->string);
}

