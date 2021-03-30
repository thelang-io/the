# Grammar
The syntax is specified using Extended Backus-Naur Form (EBNF) [edited by W3C](https://www.w3.org/TR/xml/#sec-notation).

## Syntax

```txt
Syntax ::= (_ Statement)* _
Statement ::= WS | CallExpr

CallExpr ::= ID _ LPAR _ ArgList _ RPAR

ArgList ::= (Expr (_ COMMA)*)*

Expr ::= Id | Literal
Id ::= ID
Literal ::= STRING_LITERAL

_ ::= (WS)+
```

## Tokens

```txt
COMMA ::= ','

LPAR ::= '('
RPAR ::= ')'

ID ::= [a-zA-Z_$][a-zA-Z0-9_$]+

STRING_LITERAL ::= STRING_LITERAL_DQ | STRING_LITERAL_SQ
STRING_LITERAL_DQ ::= '"' (ANY_CHAR - '"')* '"'
STRING_LITERAL_SQ ::= '\'' (ANY_CHAR - '\'')* '\''

WS ::= (CR | LF | SPACE | TAB)+
```

## Primitives

```txt
ANY_CHAR ::= [#x0-#x10FFFF]+
CR ::= #xD
LF ::= #xA
SPACE ::= #x20
TAB ::= #x9
```
