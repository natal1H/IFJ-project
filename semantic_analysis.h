#include <stdbool.h>
#include <string.h>
#include "symtable.h"
#include "error.h"

// Funkcie na semantickú analýzu v hlavnom parseri
bool check_if_function_already_defined(tBSTNodePtr global_table, char *function_id);
int function_definition(tBSTNodePtr *global_table, char *function_id);
int function_increase_number_param(tBSTNodePtr global_table, char *function_id);
int define_variable(tBSTNodePtr *current_function_root, char *variable_id);
void set_variable_type(tBSTNodePtr *current_function_root, char *variable_id, tDataType type);

// Funkcie na semantickú analýzuz v parseri výrazov
bool is_int(char *str);
bool is_float(char *str);
bool is_nil(char *str);
bool is_variable(tBSTNodePtr current_function_root, char *str);
//bool is_function(char *str);
int check_type_compatibility_aritmetic(tBSTNodePtr *current_function_root, char *id1, char *id2);