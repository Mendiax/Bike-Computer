%{

#define YY_NO_UNPUT

#include <parser.hh>
#include <cstdint>
#include <cstdio>
#include <string>

uint64_t lines = 1;

static int createToken(int token);
static int createValueToken(int token);
static int createVariableToken(int token);

%}

%x COMMENT

%%

\(                          {   BEGIN(COMMENT);                         }

<COMMENT>{
    \)                      {   BEGIN(0);                               }
    \n                      {   ++lines;                                }
    [^\(]|[^\)]|[^\n]       {                                           }
}


[[:blank:]]+
^[[:blank:]]*\n             {   ++lines;                                }

\n                          {   ++lines;                                }

\;                          {   return createToken(LEX_SEMICOLON);            }
\,                          {   return createToken(LEX_COMMA);                }

\[                          {   return createToken(LEX_L_BRACKET);            }
\]                          {   return createToken(LEX_R_BRACKET);            }
\:                          {   return createToken(LEX_ARRAY_RANGE);          }


[_a-z]+                     {   return createVariableToken(LEX_VARIABLE);     }

-?[0-9]+                    {   return createValueToken(LEX_NUM);             }

.                           {   return createToken(LEX_ERROR);                }

%%

static int createToken(int token)
{
    yylval.dtoken.line = lines;
    return token;
}

static int createValueToken(int token)
{
    yylval.number.line = lines;
    yylval.number.val = strtoll(yytext, nullptr, 10);
    return token;
}

static int createVariableToken(int token)
{
    yylval.variable.line = lines;
    yylval.variable.str = new std::string(yytext);
    return token;
}

int yywrap()
{
    return 1;
}