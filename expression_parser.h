#ifndef EXPRESSION_PARSER_EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_EXPRESSION_PARSER_H

#include "scanner.h"

#define table_size 25
#define MAXSTACK 50                      /* maximální počet prvků v zásobníku */

typedef enum precedence_table {
    unary_pTable,               //0
    not_pTable,                 //1
    multiplication_pTable,      //2
    subtraction_pTable,         //3
    division_pTable,            //4
    mod_pTable,                 //5
    and_pTable,                 //6
    addition_pTable,            //7
    substraction_pTable,        //8
    or_pTable,                  //9
    xor_pTable,                 //10
    equals_pTable,              //11
    not_equals_pTable,          //12
    less_pTable,                //13
    less_or_equals_pTable,      //14
    greater_pTable,             //15
    grater_or_equals_pTable,    //16
    in_pTable,                  //17
    left_round_bracket_pTable,  //18
    right_round_bracket_pTable, //19
    id_pTable,                  //20
    function_pTable,            //21
    array_pTable,               //22
    comma_pTable,               //23
    dollar_pTable,              //24
    no_operation_pTable         //25
} pTable;



//typedef struct expression{
//    Token token;
//    struct expression *prev;
//    struct expression *next;
//} *expressionPtr;
//
//typedef struct expressionArray{
//    struct expression First;
//    struct expression Last;
//    struct expression Act;
//}expressionArrayPtr;


typedef struct tDLElem {                 /* prvek dvousměrně vázaného seznamu */
    struct token Token;                                            /* užitečná data */
    struct tDLElem *prev;          /* ukazatel na předchozí prvek seznamu */
    struct tDLElem *next;        /* ukazatel na následující prvek seznamu */
} *tDLElemPtr;

typedef struct {                                  /* dvousměrně vázaný seznam */
    tDLElemPtr First;                      /* ukazatel na první prvek seznamu */
    tDLElemPtr Act;                     /* ukazatel na aktuální prvek seznamu */
    tDLElemPtr SyntaxChecker;                     /* ukazatel na aktuální prvek seznamu */
    tDLElemPtr Symbol;                     /* ukazatel na aktuální prvek seznamu */
    tDLElemPtr Last;                    /* ukazatel na posledni prvek seznamu */
} tDLList;

typedef struct	{                          /* zásobník hodnot typu tBTNodePtr */
    struct token stack[MAXSTACK];
    int top;
} tStackP;

typedef struct	{                          /* zásobník hodnot typu tBTNodePtr */
    long stack[MAXSTACK];
    int top;
} tStackINT;

//typedef struct	{                          /* zásobník hodnot typu tBTNodePtr */
//    char stack[MAXSTACK];
//    int top;
//} tStackP;

//------PRECEDENCE_TABLE_ENUM--------







/**
 * @name Precendecna tabulka priorit
 * @return "<" => nižšia priorita
 *         ">" => vyššia priorita
 *         "#" => bez priority
 */




#endif //EXPRESSION_PARSER_EXPRESSION_PARSER_H
