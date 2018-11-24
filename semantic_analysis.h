#include <stdbool.h>
#include "symtable.h"
#include "error.h"

bool check_if_function_already_defined(tBSTNodePtr global_table, char *function_id);
int function_definition(tBSTNodePtr *global_table, char *function_id);