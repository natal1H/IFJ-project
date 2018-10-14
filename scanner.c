#include "scanner.h"

#define scanner_is_alpha(c) ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
#define scanner_is_number(c) (c >= '0' && c <= '9')


int scanner_initialize() {
  // Inicializuj read_string, do ktorého sa budú pri načítaní tokenov ukladať znaky
  read_string = tstring_struct_initialize();

  if (read_string == NULL) {
    return -1; // TODO: return actual error code
  }
  else {
    return 0;
  }
}

Token *token_initialize() {
  // Alokuj miesto pre štruktúru token
  Token *token = (Token *) malloc(sizeof(Token));
  if (token == NULL) {
    return NULL;
  }

  token->attribute = NULL;
  token->type = NO_TYPE;

  return token;
}

void token_free(Token *token) {
  free(token->attribute);
  free(token);
}

bool is_keyword(char *str) {
  if (strcmp(str, "def") == 0)
    return true;
  else if (strcmp(str, "do") == 0)
    return true;
  else if (strcmp(str, "else") == 0)
    return true;
  else if (strcmp(str, "end") == 0)
    return true;
  else if (strcmp(str, "if") == 0)
    return true;
  else if (strcmp(str, "not") == 0)
    return true;
  else if (strcmp(str, "nil") == 0)
    return true;
  else if (strcmp(str, "then") == 0)
    return true;
  else if (strcmp(str, "while") == 0)
    return true;
  else
    return false;
}

Keyword get_keyword_type(char *str) {
  if (strcmp(str, "def") == 0)
    return KEYWORD_DEF;
  else if (strcmp(str, "do") == 0)
    return KEYWORD_DO;
  else if (strcmp(str, "else") == 0)
    return KEYWORD_ELSE;
  else if (strcmp(str, "end") == 0)
    return KEYWORD_END;
  else if (strcmp(str, "if") == 0)
    return KEYWORD_IF;
  else if (strcmp(str, "not") == 0)
    return KEYWORD_NOT;
  else if (strcmp(str, "nil") == 0)
    return KEYWORD_NIL;
  else if (strcmp(str, "then") == 0)
    return KEYWORD_THEN;
  else if (strcmp(str, "while") == 0)
    return KEYWORD_WHILE;
  else
    return -1;
}

int token_set_type_attribute(Token *token, Token_Type type, char *attribute) {
  // Token nesmie byť neinicializovaný
  if (token == NULL) {
    return -1; // TODO: return actual error code
  }
  // Nastav token type
  token->type = type;

  // Nastav attribute - treba alokovať miesto pre string
  token->attribute = (char *) malloc(sizeof(char) * strlen(attribute));
  if (token->attribute == NULL) {
    return -1; // TODO: return actual error code
  }

  // Skopírovať string do token->attribute
  strcpy(token->attribute, attribute);

  return 0;
}

void print_token(Token *token) {
  printf("Token type: %d\n", token->type);
  printf("Token attribute: %s\n", token->attribute);
}

int get_next_token(Token *token) {

  tstring_clear_string(read_string); // str := '';

  TState state = START; // Nastavenie počiatočného stavu v DKA
  int c = getc(stdin);

  do {

    switch(state) {
      case START: // Starting point

        if (scanner_is_alpha(c) || c == (int) '_') {
          // START -> F_ID

          state = F_ID;

          // TODO ošetriť či prebehlo správne pridanie symbolu
          tstring_append_char(read_string, c); // str := symbol
        }
        else if (scanner_is_number(c)) {
          // START -> F_INT

          state = F_INT;

          // TODO ošetriť či prebehlo správne pridanie symbolu
          tstring_append_char(read_string, c); // str := symbol
        }
        else if (c == (int) '=') {
          // START -> F_ASSIGN
          state = F_ASSIGN;

          // TODO ošetriť či prebehlo správne pridanie symbolu
          tstring_append_char(read_string, c); // str := symbol
        }
        else if (c == (int) '+') {
          // START -> F_ADDITION
          //token = token_initialize();
          token_set_type_attribute(token, ADDITION, "");
          return 0;
        }
        else if (c == (int) '*') {
          // START -> F_MULTIPLICATION
          //token = token_initialize();
          token_set_type_attribute(token, MULTIPLICATION, "");
          return 0;
        }
        else if (c == (int) '-') {
          // START -> F_SUBSTRACTION
          //token = token_initialize();
          token_set_type_attribute(token, SUBTRACTION, "");
          return 0;
        }
        else if (c == (int) '/') {
          // START -> F_DIVISION
          //token = token_initialize();
          token_set_type_attribute(token, DIVISION, "");
          return 0;
        }
        else if (c == (int) '<') {
          // START -> F_LESS
          state = F_LESS;

          // TODO ošetriť či prebehlo správne pridanie symbolu
          tstring_append_char(read_string, c); // str := symbol
        }
        else if (c == (int) '>') {
          // START -> F_GREATER
          state = F_GREATER;

          // TODO ošetriť či prebehlo správne pridanie symbolu
          tstring_append_char(read_string, c); // str := symbol
        }

        break;
      case F_ID:

        if (scanner_is_alpha(c) || scanner_is_number(c) || c == (int) '_') {
          // F_ID -> F_ID

          // TODO ošetriť či prebehlo správne pridanie symbolu
          tstring_append_char(read_string, c);
        }
        else {
          // Znak mimo povolených pre identifikátor

          // unget znak
          ungetc(c, stdin);

          // zisti, či ten string je medzi kľúčovými slovami

          if (is_keyword(read_string->string)) {
            token_set_type_attribute(token, KEYWORD, read_string->string);
          }
          else {
            token_set_type_attribute(token, IDENTIFIER, read_string->string);
          }

          return 0;
        }

        break;
      case F_INT:
        if (scanner_is_number(c)) {
          // F_INT -> F_INT
          tstring_append_char(read_string, c);
        }
        else {
          ungetc(c, stdin);
          token_set_type_attribute(token, INTEGER, read_string->string);
          return 0;
        }
        break;
    }

  } while((c = getc(stdin)) != EOF);


  // TODO: pridať tu ošetrenie, že c sa dostalo až na EOF
  // Nech pozrie na koncový stav, ak je v F_nieco, nech vytvorí token s príslušným typom a do attribute nahrá zostatok v read_string

  if (c == EOF) {
      switch (state) {
          case F_ID:
              token_set_type_attribute(token, IDENTIFIER, read_string->string);
              return 0;
          case F_INT:
              token_set_type_attribute(token, INTEGER, read_string->string);
              return 0;
          case F_FLOAT:
              token_set_type_attribute(token, FLOAT, read_string->string);
              return 0;
          case F_STRING:
              token_set_type_attribute(token, STRING, read_string->string);
              return 0;
          case F_ASSIGN:
              token_set_type_attribute(token, ASSIGN, "");
              return 0;
          case F_ADDITION:
              token_set_type_attribute(token, ADDITION, "");
              return 0;
          case F_MULTIPLICATION:
              token_set_type_attribute(token, MULTIPLICATION, "");
              return 0;
          case F_SUBTRACTION:
              token_set_type_attribute(token, SUBTRACTION, "");
              return 0;
          case F_DIVISION:
              token_set_type_attribute(token, DIVISION, "");
              return 0;
          case F_LESS:
              token_set_type_attribute(token, LESS, "");
              return 0;
          case F_GREATER:
              token_set_type_attribute(token, GREATER, "");
              return 0;
      }
  }

  // TODO: temp solution
  token->type = NO_TYPE;
  token->attribute = NULL;
  return EOF;

}


// MAIN - only for temp testing
int main(int argc, char** argv) {
  printf("SCANNER TEST\n");


  // inicializuj scanner najprv cez scanner_initialize()
  if ( scanner_initialize() != 0 ) {
    // chyba pri inicializácii
    printf("Chyba pri inicializácii scannera");
    return -1; // TODO return actual error code
  }


  Token *token =  token_initialize();

  int ret = get_next_token(token);
  while (ret != EOF) {
    printf("RET: %d\n", ret);
    print_token(token);
    token_free(token);
    token = token_initialize();
    ret = get_next_token(token);
  }

  token_free(token);

  // po skončení práce uvoľni miesto po read_string
  tstring_free_struct(read_string);

  return 0;
}

// TODO: = token nie je ešte ošetrený