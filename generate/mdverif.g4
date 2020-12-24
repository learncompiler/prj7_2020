grammar mdverif;

import CommonLex;

program : function* EOF;

function
    : type Identifier '(' parameter_list ')' condition* compound_statement
    ;

parameter_list
    : (type Identifier (',' type Identifier)*)?
    ;

condition : precondition | postcondition;

precondition : 'requires' expression;

postcondition : 'ensures' expression;

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
    : 'return' expression ';' # stmt1
    | assignment ';' # stmt2
    | ';' # stmt3
    | compound_statement # stmt4
    | 'if' '(' expression ')' statement ('else' statement)? # stmt5
    | 'while' '(' expression ')' loop_spec* statement # stmt6
    ;

loop_spec
    : loop_invariant
    | ranking_function
    ;

loop_invariant : 'invariant' expression;
ranking_function : 'decrease' '(' expression (',' expression)* ')';

declaration
    : type Identifier ('=' expression)? ';'
    ;

assignment
    : Identifier '=' expression
    ;

expression
    : logical_imply
    ;

logical_imply
    : logical_or # logical_imply1
    | logical_or '-->' logical_imply # logical_imply2
    ;

logical_or
    : logical_and # logical_or1
    | logical_or '||' logical_and # logical_or2
    ;

logical_and
    : equality # logical_and1
    | logical_and '&&' equality # logical_and2
    ;

equality
    : relational # equality1
    | equality ('=='|'!=') relational # equality2
    ;

relational
    : additive # relational1
    | relational ('<'|'>'|'<='|'>=') additive # relational2
    ;

additive
    : multiplicative # additive1
    | additive ('+'|'-') multiplicative # additive2
    ;

multiplicative
    : unary # multiplicative1
    | multiplicative ('*'|'/'|'%') unary # multiplicative2
    ;

unary
    : primary # unary1
    | ('-'|'~'|'!') unary # unary2
    ;

primary
    : Integer # primary1
    | '(' expression ')' # primary2
    | Identifier # primary3
    ;