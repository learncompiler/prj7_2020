grammar mdverif;

import CommonLex;

program : function* EOF;

function
    : type Identifier '(' ')' compound_statement
    ;

type
    : 'int'
    ;

compound_statement
    : '{' block_item* '}'
    ;

block_item
    : statement
    | declaration
    ;

statement
    : 'return' expression ';'
    | expression? ';'
    | 'if' '(' expression ')' statement ('else' statement)?
    | compound_statement
    | 'for' '(' expression? ';' expression? ';' expression? ')' statement
    | 'for' '(' declaration expression? ';' expression? ')' statement
    | 'while' '(' expression ')' statement
    | 'do' statement 'while' '(' expression ')' ';'
    | 'break' ';'
    | 'continue' ';'
    ;

declaration
    : type Identifier ('=' expression)? ';'
    ;

expression
    : assignment
    ;

assignment
    : conditional
    | Identifier '=' expression
    ;

conditional
    : logical_or
    | logical_or '?' expression ':' conditional
    ;

logical_or
    : logical_and
    | logical_or '||' logical_and
    ;

logical_and
    : equality
    | logical_and '&&' equality
    ;

equality
    : relational
    | equality ('=='|'!=') relational
    ;

relational
    : additive
    | relational ('<'|'>'|'<='|'>=') additive
    ;

additive
    : multiplicative
    | additive ('+'|'-') multiplicative
    ;

multiplicative
    : unary
    | multiplicative ('*'|'/'|'%') unary
    ;

unary
    : primary
    | ('-'|'~'|'!') unary
    ;

primary
    : Integer
    | '(' expression ')'
    | Identifier
    ;