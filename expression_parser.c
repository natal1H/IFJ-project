//--------------INCLUDE--------------
#include "expression_parser.h"
#include "scanner.h"
#include "stringlib.h"
# include <stdio.h>
#include <limits.h>
#include <asm/errno.h>
#include <errno.h>

/*
 * Vyuziva enum token_type zo skenera
 *
 *  Zakladna tabulka:
 *          |            |            |              Nasledujuci token(nad nimi su ID z enum token_type)            |
 *          | Podmienka/ |Moze skoncit|  7     9     8     10    17   18     2     3     4     11   12    13     14    15    16 |
 *          |  Pocitadlo |   vyraz?   |  +     -     *     /     (     )    INT  FLOAT STRING  <    <=     >     >=    ==    != | token   ID
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  0  |  0  |  0  |  0  |  1  |  0  |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  +      7
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  0  |  0  |  0  |  0  |  1  |  0  |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  -      9
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  0  |  0  |  0  |  0  |  1  |  0  |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  *      8
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  0  |  0  |  0  |  0  |  1  |  0  |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  /      10
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  ?  |  1  |  0  |  0  |  1  |  1  |  1  |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  (      17
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  0  |  0  |  0  |  0  |  0  |  1  |  1  |  1  |  1  |  1  |  1  |  1  |  1  |  1  |  1  |  )      18
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------------
 *          |            |            |  1  |  1  |  1  |  1  |  0  |  1  |  0  |  0  |  0  |  1  |  1  |  1  |  1  |  1  |  1  |  INT    2
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------  ID
 *          |            |            |  1  |  1  |  1  |  1  |  1  |  1  |  0  |  0  |  0  |  1  |  1  |  1  |  1  |  1  |  1  |  FLOAT  3
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------------
 *          |            |            |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  ?  |  ?  |  ?  |  ?  |  1  |  1  |  STRING 4
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  ?P |  1 |  0  |  0  |   0  |  1P |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  <      11
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  ?  |  1P |  0  |  0  |  0  |  1P |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  <=     12
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------+-----+------------
 *          |            |            |  ?  |  1P |  0  |  0  |  0  |  1P |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  >      13
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  ?  |  1P |  0  |  0  |  0  |  1P |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  >=     14
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  ?  |  1P |  0  |  0  |  0  |  1P |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  ==     15
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *          |            |            |  ?  |  1P |  0  |  0  |  0  |  1P |  1  |  1  |  0  |  0  |  0  |  0  |  0  |  0  |  0  |  !=     16
 *          +------------+------------+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+------------
 *
 *  Legenda:
 *  1  =>  Pravda
 *  1P =>  Pravda s podmienkou
 *  0  =>  Nepravda
 */

#define SUCCESSFUL_END 23
#define COUNTER_OF_TOKEN 22


long long syntax_table[22][24]= {
        //    0      1    2      3      4      5      6      7     8     9    10    11    12    13    14    15    16    17    18    19    20      21   22  23   |
        // NO_TYPE  ID   INT  FLOAT  STRING  KWORD  ASSIGN  +     *     -     /     <    =<     >    >=    ==    !=     (     )   COMMA  EOL     EOF   P   S    | Nazov vstupneho tokenu   ID_cislo
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,    0  , 0 , 0,   }, //NO_TYPE,             //0
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  0  ,  1  ,  0  ,  1  ,    1  , 0 , 1,   }, //IDENTIFIER,          //1
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  0  ,  1  ,  0  ,  1  ,    0  , 0 , 1,   }, //INTEGER,             //2
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  0  ,  1  ,  0  ,  1  ,    0  , 0 , 1,   }, //FLOAT,               //3
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  1  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  1  ,  0  ,  1  ,  0  ,  1  ,    0  , 0 , 1,   }, //STRING,              //4
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,    0  , 0 , 0,   }, //KEYWORD,             //5
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,    0  , 0 , 0,   }, //ASSIGN,              //6
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,   }, //ADDITION,            //7
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,   }, //MULTIPLICATION,      //8
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,   }, //SUBTRACTION,         //9
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,   }, //DIVISION,            //10
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,   }, //LESS,                //11
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,   }, //LESS_OR_EQUALS,      //12
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,   }, //GREATER,             //13
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,   }, //GREATER_OR_EQUALS,   //14
        {    0   ,  1  ,  1  ,  1   ,   1  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,   }, //EQUALS,              //15
        {    0   ,  1  ,  1  ,  1   ,   1  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,   }, //NOT_EQUALS,          //16
        {    0   ,  1  ,  1  ,  1   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  1  ,  0  ,  0  ,  0  ,    0  , 0 , 0,   }, //LEFT_ROUND_BRACKET,  //17
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  1  ,  0  ,  1  ,  0  ,  1  ,    0  , 0 , 1,   }, //RIGHT_ROUND_BRACKET, //18
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0 ,   0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,    0  , 0 , 0,   }, //COMMA,               //19
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,    1  , 0 , 1,   }, //EOL,                 //20
        {    0   ,  0  ,  0  ,  0   ,   0  ,  0   ,  0   ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,  0  ,    0  , 0 , 1,   }, //TYPE_EOF,            //21
                                                                                                                                                   //  ^   ^
                                                                                                                                                   //  |   |
                                                                                                                                                   //  |   +Moze skoncit vyraz?
                                                                                                                                                   //  |
                                                                                                                                                   //  + Pocet prijatych(napr pocitanie zatvoriek)
};

const char precedenceTable[table_size][table_size]= {
        //    0      1    2     3     4     5     6     7     8    9     10    11     12   13    14    15    16    17    18    19    20      21       22    23    24
        //  unary-  not   *     /    div   mod   and    +     -    or    xor    =     <>    <    <=     >    >=    in     (    )     ID   function  array   ,      $
        { '<'  , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '#' , '>' }, //unary  0
        { '<'  , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // not   1
        { '<'  , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // *     2
        { '<'  , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // /     3
        { '<'  , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // div   4
        { '<'  , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // mod   5
        { '<'  , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // and   6
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // +     7
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // -     8
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // or    9
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // xor   10
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // =     11
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // <>    12
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // <     13
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // <=    14
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // >     15
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // >=    16
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '<' , '>' , '<' ,   '<'   ,  '<' , '>' , '>' }, // in    17
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '=' , '<' ,   '<'   ,  '<' , '=' , '#' }, // (     18
        { '#'  , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '#' , '>' , '#' ,   '#'   ,  '#' , '>' , '>' }, // )     19
        { '#'  , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '>' , '#' , '>' , '#' ,   '#'   ,  '#' , '>' , '>' }, // ID    20
        { '#'  , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '=' , '#' , '#' ,   '#'   ,  '#' , '#' , '#' }, // func  21
        { '#'  , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '#' , '=' , '#' , '#' ,   '#'   ,  '#' , '#' , '#' }, // array 22
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '=' , '<' ,   '<'   ,  '<' , '=' , '#' }, // ,     23
        { '<'  , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '<' , '#' , '<' ,   '<'   ,  '<' , '#' , '#' }, // $     24
};

int TransformTable[22] = {
        no_operation_pTable,       //0
        id_pTable,                 //1
        id_pTable,                 //2
        id_pTable,                 //3
        no_operation_pTable,       //4
        no_operation_pTable,       //5
        no_operation_pTable,       //6
        addition_pTable,           //7
        multiplication_pTable,     //8
        substraction_pTable,       //9
        division_pTable,           //10
        less_pTable,               //11
        less_or_equals_pTable,     //12
        greater_pTable,            //13
        grater_or_equals_pTable,   //14
        equals_pTable,             //15
        not_equals_pTable,         //16
        left_round_bracket_pTable, //17
        right_round_bracket_pTable,//18
        comma_pTable,              //19
        dollar_pTable,             //20
        no_operation_pTable        //21
};


/* -------------------------------------------------------------------------- */

/**
 * Funkcie na pracu s tokenovym zasobnikom 
 */

void InitStackTOKEN (tStackP *S)
/*   ------
** Inicializace zásobníku.
**/
{
    S->top = 0;
}

void PushStackTOKEN (tStackP *S, struct token ptr)
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
{
    /* Při implementaci v poli může dojít k přetečení zásobníku. */
    if (S->top==MAXSTACK)
        printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
    else {
        S->top++;
        S->stack[S->top]=ptr;
    }
}

struct token PopStackTOKEN (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
    /* Operace nad prázdným zásobníkem způsobí chybu. */
    if (S->top==0)  {
        printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
//        return(-1);
    }
    else {
        return (S->stack[S->top--]);
    }
}

bool IsStackEmptyTOKEN (tStackP *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
    return(S->top==0);
}

/* -------------------------------------------------------------------------- */

/**
 * Funkcie na pracu s integerovym zasobnikom
 */

void InitStackINT (tStackINT *S)
/*   ------
** Inicializace zásobníku.
**/
{
    S->top = 0;
}

void PushStackINT (tStackINT *S, long ptr)
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
{
    /* Při implementaci v poli může dojít k přetečení zásobníku. */
    if (S->top==MAXSTACK)
        printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
    else {
        S->top++;
        S->stack[S->top]=ptr;
    }
}

long PopStackINT (tStackINT *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
    /* Operace nad prázdným zásobníkem způsobí chybu. */
    if (S->top==0)  {
        printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
//        return(-1);
    }
    else {
        return (S->stack[S->top--]);
    }
}

bool IsStackEmptyINT (tStackINT *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
    return(S->top==0);
}

/* -------------------------------------------------------------------------- */

/**
 * Funkcie pre pracu s charovym zasobnikom 
 */

void InitStackSTRING (tStackCHAR *S)
/*   ------
** Inicializace zásobníku.
**/
{
    S->top = 0;
}

void PushStackSTRING (tStackCHAR *S, char* ptr)
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
{
    /* Při implementaci v poli může dojít k přetečení zásobníku. */
    if (S->top==MAXSTACK)
        printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
    else {
        S->top++;
        S->stack[S->top]=ptr;
    }
}

char* PopStackSTRING (tStackCHAR *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
    /* Operace nad prázdným zásobníkem způsobí chybu. */
    if (S->top==0)  {
        printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
//        return(-1);
    }
    else {
        return (S->stack[S->top--]);
    }
}

bool IsStackEmptySTRING (tStackINT *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
    return(S->top==0);
}

/* -------------------------------------------------------------------------- */


/**
 * @param Token Vystup z scannera
 * @param L Buffer/Obojsmerny zoznam
 * @note Ulozi cely kontrolovany vyraz do bufferu
 */
void LoadToBuffer(Token *Token, tDLList *ExprList) {

    tDLElemPtr Expr = malloc(sizeof(struct tDLElem)); //Alokovanie prvku
    if (Expr == NULL) { //Ak sa nepodarila alokacia
        fprintf(stderr, "Memory is full");
        return;
    } else {

        if (ExprList->First == NULL) {
            (*Expr).Token = *Token; //inicializacia noveho prvku
            ExprList->First = Expr;        //prvy v zozname ak je prvy prvok
            ExprList->Last = Expr;         //zaroven ako posledny
            ExprList->Act = Expr;
        } else {
            (*Expr).Token = *Token;         //inicializacia noveho prvku
            ExprList->Last->next = Expr;           //pridanie do zoznamu na koniec
            ExprList->Last->next->prev = ExprList->Last;  //obojsmerne previazanie
            ExprList->Last = Expr;                 //nastavenie posledneho prvku
        }
    }
}

/**
 * Pretypuje integer na string
 * @param x Integerova hodnota na pretypovanie
 * @return Identicka hodnota v stringu
 */
char* integer_to_string(int x)
{
    char* buffer = malloc(sizeof(char) * sizeof(int) * 4 + 1);
    if (buffer)
    {
        sprintf(buffer, "%d", x);
    }

    return buffer; // TODO neuvolnena pamat!
}


/**
 *  Pretypuje string na integer
 * @param x String na pretypovanie
 * @return Identicka hodnota ako integer
 */
int string_to_integer(char* x){
    char *ptr;
    long return_value = strtol(x, &ptr, 10); //Pretypovanie char -> int

    //Resetovanie errno pred volanim
    errno = 0;

    //Osetrenie moznych errov pri pretypovani
    if (x == ptr) {
        fprintf(stderr, " token_ID1_in_Integer : %lu  neplatne pretypovanie  (no digits found, 0 returned)\n", return_value);
    }
    else if (errno == ERANGE && return_value == LONG_MIN) {
        fprintf(stderr, " token_ID1_in_Integer : %lu  neplatne pretypovanie   (underflow occurred)\n", return_value);
    }
    else if (errno == ERANGE && return_value == LONG_MAX) {
        fprintf(stderr, " token_ID1_in_Integer : %lu  neplatne pretypovanie   (overflow occurred)\n", return_value);
    }
    else if (errno == EINVAL) {  /* not in all c99 implementations - gcc OK */
        fprintf(stderr, " token_ID1_in_Integer : %lu  neplatne pretypovanie   (base contains unsupported value)\n", return_value);
    }
    else if (errno != 0 && return_value == 0) {
        fprintf(stderr, " token_ID1_in_Integer : %lu  neplatne pretypovanie   (unspecified error occurred)\n", return_value);
    }
    else if (errno == 0 && x && *ptr != 0) {
        fprintf(stderr, " token_ID1_in_Integer : %lu  platne pretypovanie   (but additional characters remain)\n", return_value);
    }
    else if (errno == 0 && x && !*ptr){
    } //Ak je konverzia spravna


    return return_value;
}


/**
 * Vykona operaciu pri vypocitavani postfixovej notacie
 * @param token_ID1 Prvy operand
 * @param token_OP Operacia
 * @param token_ID2 Druhy operand
 * @return Vrati vysledok danej operacie
 */
char* EvaluateNow(char* token_ID1, Token token_OP, char* token_ID2 ){

    switch(token_OP.type){

        case (ADDITION) : {

            int result = 0;

            result = string_to_integer(token_ID1) + string_to_integer(token_ID2); //vypocet int + int
            char* tmp = integer_to_string(result); // pretypovanie int -> *char
            return tmp;

        }

        case (MULTIPLICATION) : {
            int result = 0;

            result = string_to_integer(token_ID1) * string_to_integer(token_ID2); //vypocet int + int
            char* tmp = integer_to_string(result); // pretypovanie int -> *char

            return tmp;
        }

        default:{}
    }
}

/**
 * Vypocita vyraz v postfixovej notacii
 * @param ExprList Obojsmerny pointrovy zoznam s jednotlivymi tokenmi ulozenych v strukture - TODO momentalne nepotebne
 * @param stackPostfix Zasobnik s ulozenou postfixou notaciou
 * @param stackTemp Pomocny zasobnik pre pretocenie poradia v postfixovom zasobniku
 * @param stackOutputINT Vystupny zasobnik v ktorom je pocitana postfixova notacia - TODO momentalne nepouzivane
 * @param stackOutputCHAR Vystupny zasobnik v ktorom je pocitana postfixova notacia v type char!
 * @param last_operation Posledna operacia - momentalne nepotrebne - TODO momentalne nepotrebne
 * @return Vracia vysledok v datovom type long
 */
char* EvaluateFromPostfix(tDLList *ExprList, tStackP *stackPostfix, tStackP *stackCHAR, tStackINT *stackOutputINT , tStackCHAR *stackOutputCHAR, int last_operation) {

    //Obratenie zasobnika, najskor POPnut potrebujeme operandy ktore su na spodku zasobnika
    while(stackPostfix->top > 0){
        PushStackTOKEN(stackCHAR, PopStackTOKEN(stackPostfix));
    }
    *stackPostfix = *stackCHAR; //Prekopirovanie pretoceneho zasobnika na stary zasobnik

    while(stackPostfix->top > 0){
        Token tmp;                    //Pomocny token
        tmp = PopStackTOKEN(stackPostfix); //Zisti hodnotu na vrchole zasobniku

            if(TransformTable[tmp.type] == id_pTable) { //Ak ide o ID
//                char *ptr;  //Buffer k pretypovaniu
//                PushStackINT(stackOutputINT, strtol(tmp.attribute, &ptr, 10));

                //Pushni char do charoveho stacku
                PushStackSTRING(stackOutputCHAR, tmp.attribute);

            } else {
                //Pushni na vysledny charovy stack vysledok
                PushStackSTRING(stackOutputCHAR, EvaluateNow(PopStackSTRING(stackOutputCHAR), tmp, PopStackSTRING(stackOutputCHAR)));
            }
    }
    //Vrat vysledok zo zasobnika
    return PopStackSTRING(stackOutputCHAR);
}

/**
 * Spracuje infixovy vyraz do postfixovej notacie pomocou precedencnej tabulky
 * @param ExprList Obojsmerny pointrovy zoznam s jednotlivymi tokenmi ulozenych v strukture
 * @param stack Pomocny zasobnik kde sa vyhodnocuje priorita operatorov
 * @param stackOutput Vystupny zasobnik s postfixovou notaciou
 * @param last_operation Posledna vykonana operacia - TODO nepotrebne momentane
 * @return Vracia zasobnik s postfixou notaciou
 */
tStackP ParseToPostfix(tDLList *ExprList, tStackP *stack, tStackP *stackOutput, int last_operation) {

    bool UvolnovanieZasobnika = false;
    Token tmp;              //Pomocny token
    tmp.type = EOL;         //Koniec zasobnika ako EOL
    PushStackTOKEN(stack, tmp);  //Informuje o konci zasobnika

//    ExprList->Act = ExprList->First;

    while (ExprList->Act != NULL && ExprList->Act->Token.type != EOL) { //Pokym nie je koniec vyrazu(zoznamu) TODO Token.Next == NULL

                tmp = PopStackTOKEN(stack); //Zisti hodnotu na vrchole zasobnika
                PushStackTOKEN(stack, tmp); //Ale hodnotu ponechaj ulozenu na zasobniku

                if(TransformTable[ExprList->Act->Token.type] == id_pTable){ //Ak je to operand
                    PushStackTOKEN(stackOutput, ExprList->Act->Token);           //Potom push do vysledneho zasobnika
                } else {

                    //Pokym je vacsia priorita
                    while (precedenceTable[TransformTable[tmp.type]][TransformTable[ExprList->Act->Token.type]] == '>') {

                        //Ak sa token na zasobniku nerovna '(' alebo ')'
                        //Zatvorky nepushujeme do vysledneho vyrazu!
                        if(TransformTable[tmp.type] != left_round_bracket_pTable && TransformTable[tmp.type] != right_round_bracket_pTable) {
                            PushStackTOKEN(stackOutput, PopStackTOKEN(stack)); //Prehod token do vysledneho zasobnika
                        } else {
                            PopStackTOKEN(stack);    //Len odober token zo zasobnika, len zatvorky!
                        }

                        tmp = PopStackTOKEN(stack); //Zisti hodnotu na vrchole zasobnika
                        PushStackTOKEN(stack, tmp); //Ale hodnotu ponechaj ulozenu na zasobniku
                        UvolnovanieZasobnika = true;

//                      if(precedenceTable[TransformTable[tmp.type]][TransformTable[ExprList->Act->Token.type]] == '<') {
//                          PushStackTOKEN(stack, ExprList->Act->Token);
//                      }
                    }

                    //Rovnaka priorita
                    if(precedenceTable[TransformTable[tmp.type]][TransformTable[ExprList->Act->Token.type]] == '=' ){
                        PopStackTOKEN(stack);
//                    } else if (1) {
                    } else {
                        PushStackTOKEN(stack, ExprList->Act->Token);
                    }
                }
                UvolnovanieZasobnika = false;
                ExprList->Act = ExprList->Act->next; //Dalsi Token
    }


    while(stack->top > 1){
//        tmp = PopStackTOKEN(stack);
//        if (tmp.type != LEFT_ROUND_BRACKET && tmp.type != RIGHT_ROUND_BRACKET) {
            PushStackTOKEN(stackOutput, PopStackTOKEN(stack));
//        }
    }
//    for(int i = 1; i<=stackOutput->top; i++) {
//        print_token(&(stackOutput)->stack[i]);
//    }

    return *stackOutput;
}

bool FindRule(tDLList *ExprList, bool *SyntaxError) {


// TODO kontrola syntaxe
        bool CouldBeTokenTheLastOne = true;
//        ExprList->Act = ExprList->Act->next;
        do{

            if(ExprList->Act->next != NULL) {
                if (syntax_table[ExprList->Act->Token.type][ExprList->Act->next->Token.type] == false) {
                    *SyntaxError = true;
                }
            }


            //Kontrola zatvoriek
            if(ExprList->Act->Token.type == LEFT_ROUND_BRACKET){
                   syntax_table[LEFT_ROUND_BRACKET][COUNTER_OF_TOKEN] += 1;

            } else if(ExprList->Act->Token.type == RIGHT_ROUND_BRACKET &&
                     (syntax_table[LEFT_ROUND_BRACKET][COUNTER_OF_TOKEN] >= syntax_table[RIGHT_ROUND_BRACKET][COUNTER_OF_TOKEN] ) ){
                         syntax_table[RIGHT_ROUND_BRACKET][COUNTER_OF_TOKEN] += 1;

                  } else if(ExprList->Act->Token.type == RIGHT_ROUND_BRACKET){
                         *SyntaxError = true;
                  } else {
                         syntax_table[ExprList->Act->Token.type][COUNTER_OF_TOKEN] += 1;
                  }


            //Vyhodnotenie pre posledny token
            if( syntax_table[ExprList->Act->Token.type][SUCCESSFUL_END]){
                     CouldBeTokenTheLastOne = true;
            } else { CouldBeTokenTheLastOne = false; }

            ExprList->Act = ExprList->Act->next;
        } while (!((ExprList->Act == NULL) || ExprList->Act->Token.type == EOL || ExprList->Act->Token.type == EOF ));

        //Ak sa nerovna pocet zatvoriek alebo posledny token nemoze byt posledny vo vyraze(napriklad nemoze vyraz koncit s "+")
        if( syntax_table[LEFT_ROUND_BRACKET][COUNTER_OF_TOKEN] != syntax_table[RIGHT_ROUND_BRACKET][COUNTER_OF_TOKEN] ||
            CouldBeTokenTheLastOne == false ||

            //Ak sa miesaju stringy a int/float
            ((syntax_table[STRING][COUNTER_OF_TOKEN] != 0)
             && (syntax_table[INTEGER][COUNTER_OF_TOKEN] +
              syntax_table[FLOAT][COUNTER_OF_TOKEN]) != 0) ||

            //Ak je vo vyraze string a pocet operandov je parny
            //TODO Doplnit podmienku ze identifier je string!
            ((syntax_table[STRING][COUNTER_OF_TOKEN] > 0)
             && ((syntax_table[INTEGER][COUNTER_OF_TOKEN] + syntax_table[FLOAT][COUNTER_OF_TOKEN] + syntax_table[IDENTIFIER][COUNTER_OF_TOKEN]) == 0)
             &&  (syntax_table[STRING][COUNTER_OF_TOKEN]%2) == 0) ||

            //Maximalne jeden porovnavaci token
            (((syntax_table[LESS][COUNTER_OF_TOKEN] + syntax_table[LESS_OR_EQUALS][COUNTER_OF_TOKEN] +
               syntax_table[GREATER][COUNTER_OF_TOKEN] + syntax_table[GREATER_OR_EQUALS][COUNTER_OF_TOKEN] +
               syntax_table[EQUALS][COUNTER_OF_TOKEN] + syntax_table[NOT_EQUALS][COUNTER_OF_TOKEN]) != 0) &&

              (syntax_table[LESS][COUNTER_OF_TOKEN] + syntax_table[LESS_OR_EQUALS][COUNTER_OF_TOKEN] +
               syntax_table[GREATER][COUNTER_OF_TOKEN] + syntax_table[GREATER_OR_EQUALS][COUNTER_OF_TOKEN] +
               syntax_table[EQUALS][COUNTER_OF_TOKEN] + syntax_table[NOT_EQUALS][COUNTER_OF_TOKEN] != 1)) ||

            //Pocet operacii je neparny
            //Sucet operatov musi byt parny ak neexistuje porovnavanie
            !(

                    //Ak nie je pritomne porovnanie
                   (((syntax_table[LESS][COUNTER_OF_TOKEN] + syntax_table[LESS_OR_EQUALS][COUNTER_OF_TOKEN] +
                      syntax_table[GREATER][COUNTER_OF_TOKEN] + syntax_table[GREATER_OR_EQUALS][COUNTER_OF_TOKEN] +
                      syntax_table[EQUALS][COUNTER_OF_TOKEN] + syntax_table[NOT_EQUALS][COUNTER_OF_TOKEN]) == 0)

                            && //musi byt

                                      //Potom je pocet operacii neparny
                                     (
                                      //Priklad 1+2+3
                                     (((syntax_table[ADDITION][COUNTER_OF_TOKEN] + syntax_table[SUBTRACTION][COUNTER_OF_TOKEN] +
                                        syntax_table[MULTIPLICATION][COUNTER_OF_TOKEN] + syntax_table[DIVISION][COUNTER_OF_TOKEN])%2 == 0)&&
                                      ((syntax_table[INTEGER][COUNTER_OF_TOKEN] + syntax_table[FLOAT][COUNTER_OF_TOKEN] + syntax_table[IDENTIFIER][COUNTER_OF_TOKEN])%2 == 1))

                                     || //alebo

                                      //Priklad 1+2+3+4
                                     (((syntax_table[ADDITION][COUNTER_OF_TOKEN] + syntax_table[SUBTRACTION][COUNTER_OF_TOKEN] +
                                        syntax_table[MULTIPLICATION][COUNTER_OF_TOKEN] + syntax_table[DIVISION][COUNTER_OF_TOKEN])%2 == 1)&&
                                      ((syntax_table[INTEGER][COUNTER_OF_TOKEN] + syntax_table[FLOAT][COUNTER_OF_TOKEN] + syntax_table[IDENTIFIER][COUNTER_OF_TOKEN])%2 == 0))
                                     )
                   )

                     || //alebo

                    //Ak je pritomne porovnanie
                   ((((syntax_table[LESS][COUNTER_OF_TOKEN] + syntax_table[LESS_OR_EQUALS][COUNTER_OF_TOKEN] +
                       syntax_table[GREATER][COUNTER_OF_TOKEN] + syntax_table[GREATER_OR_EQUALS][COUNTER_OF_TOKEN] +
                       syntax_table[EQUALS][COUNTER_OF_TOKEN] + syntax_table[NOT_EQUALS][COUNTER_OF_TOKEN]) == 1))

                            && //musi byt

                                    //Sucet operacii je neparny a sucet operandov je neparny
                                   ((((syntax_table[ADDITION][COUNTER_OF_TOKEN] + syntax_table[SUBTRACTION][COUNTER_OF_TOKEN] +                                                                                                                             syntax_table[MULTIPLICATION][COUNTER_OF_TOKEN] + syntax_table[DIVISION][COUNTER_OF_TOKEN])%2 == 1) &&
                                    ((syntax_table[INTEGER][COUNTER_OF_TOKEN] + syntax_table[FLOAT][COUNTER_OF_TOKEN] + syntax_table[IDENTIFIER][COUNTER_OF_TOKEN])%2 == 1))

                                   || //alebo

                                    //Sucet operacii je parny a sucet operandov je parny
                                   (((syntax_table[ADDITION][COUNTER_OF_TOKEN] + syntax_table[SUBTRACTION][COUNTER_OF_TOKEN] +                                                                                                                             syntax_table[MULTIPLICATION][COUNTER_OF_TOKEN] + syntax_table[DIVISION][COUNTER_OF_TOKEN])%2 == 0) &&
                                   ((syntax_table[INTEGER][COUNTER_OF_TOKEN] + syntax_table[FLOAT][COUNTER_OF_TOKEN] + syntax_table[IDENTIFIER][COUNTER_OF_TOKEN])%2 == 0)))
                   )
            )


        ){
            *SyntaxError = true;
        }
    return *SyntaxError;
}

bool MainSyntaxCheck(tDLList *ExprList) {
    if (ExprList == NULL) {
        return false; //Ziadny vyraz TODO osetrit specialny stav
    } else {

        //Kontrola syntaxe vyrazu
        bool syntaxStatus = false;

        //Inicializacia temporary
        bool SyntaxError = false;
        bool *SyntaxErroPtr = &SyntaxError;

        //Kontrolujeme syntax od zaciatku zoznamu
        ExprList->Act = ExprList->First;

        //Kontroluje syntakticke pravidla
        syntaxStatus =  FindRule(ExprList, SyntaxErroPtr);


        int static last_operation = no_operation_pTable; //TODO Nepouzivane

        if(syntaxStatus == ERR_OK) { //Ak je error semanticku cast preskoc

            //Alokacia zasobnika
            tStackP *stack = malloc(sizeof(tStackP));
            tStackP *stackOutput = malloc(sizeof(tStackP));
            tStackINT *stackOutputINT = malloc(sizeof(tStackINT));
            tStackCHAR *stackOutputCHAR = malloc(sizeof(tStackCHAR));

            //Inicializacia zasobnika
            InitStackTOKEN(stack);
            InitStackTOKEN(stackOutput);
            InitStackINT(stackOutputINT);
            InitStackSTRING(stackOutputCHAR);


            //Rozlozenie vyrazu do postfixovej notacie
            ExprList->Act = ExprList->First;
            *stack = ParseToPostfix(ExprList, stack, stackOutput, last_operation);
            //Re-Inicializacia(Vycistenie) zasobnika
            InitStackTOKEN(stackOutput);

            //Vysledok daneho vyrazu TODO Zmenit
            char* vysledok = 0;
            vysledok = EvaluateFromPostfix(ExprList, stack, stackOutput, stackOutputINT, stackOutputCHAR, last_operation);
//            printf("Vysledok: %s\n", vysledok);

            free(stack);
            free(stackOutput);
            free(stackOutputINT);
            free(stackOutputCHAR);
        } else {
            return syntaxStatus; //ERR_SYNTAX return
        }

        return syntaxStatus; //ERR_OK return
    }
}


/**
 * Uvolni pamat v ktorom je nacitany vyraz
 * @param ExprList Zoznam s vyrazom
 */
void FreeBuffer(tDLList *ExprList){
    if(ExprList != NULL){
        if(ExprList->First != NULL){
            do {
                ExprList->Act = ExprList->First->next;
                free(ExprList->First);
                ExprList->First = ExprList->Act;
            }while(ExprList->Act != NULL);
            free(ExprList);
        }
    }
}



int CallExpressionParser(Token *token) {

    //Ulozenie adresy ktoru dostanem pre zapis posledneho tokenu
    Token *SaveMyToken = token;

    // inicializuj scanner najprv cez scanner_initialize()
    int ScannerErrorCheck = 0;
    if ( (ScannerErrorCheck = scanner_initialize()) != 0 ) {
        return ScannerErrorCheck;
    }

    tDLList *ExprArray = malloc(sizeof(tDLList));

    while   (token->type != EOL &&
             token->type != TYPE_EOF &&
             (token->type != KEYWORD && strcmp(token->attribute, "do") != 0 &&
              token->type != KEYWORD && strcmp(token->attribute, "then") != 0)) {

        LoadToBuffer(token, ExprArray);

        ScannerErrorCheck =  get_next_token(token);

        if(ScannerErrorCheck == ERR_SCANNER && token->type != TYPE_EOF){
            return ScannerErrorCheck;
        }

        //Zachovanie posledneho tokena
        SaveMyToken->type = token->type;
        SaveMyToken->attribute = token->attribute;
    }

    bool MainSyntaxStatus = false;
    MainSyntaxStatus = MainSyntaxCheck(ExprArray);

    FreeBuffer(ExprArray);

    //true = error
    if (MainSyntaxStatus == true) {
//        printf("%d", ERR_SYNTAX);
        return ERR_SYNTAX;
    } else {
//        printf("%d", ERR_OK);
        return ERR_OK;
    }
}




//int main(int argc, char *argv[]) {
//
//    freopen("input.txt","r",stdin);
//    int ScannerErrorCheck = 0;
//    if ( (ScannerErrorCheck = scanner_initialize()) != 0 ) {
//        return ScannerErrorCheck;
//    }
//
//    Token *token;
//    token = token_initialize();
//
//    ScannerErrorCheck = get_next_token(token);
//    if(ScannerErrorCheck != 0){
//        return ScannerErrorCheck;
//    }
//
//    CallExpressionParser(token);
//
//    print_token(token);
//
//}
