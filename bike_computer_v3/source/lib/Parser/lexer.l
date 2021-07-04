%{

// #define YY_NO_UNPUT
[[maybe_unused]]
static void yyunput (int c, char * yy_bp );

#include "parser_bison.hpp"
// #include <cstdint>
// #include <cstdio>
#include <string>
#include <stdio.h>

#include "traces.h"


#define fileno(x) 0

uint64_t lines = 1;

// todo string for date????

static int create_val_token(int token);
static int create_string_token(int token);

%}

%%

\{                          {   return LEX_L_CBRACKET;                          }
\}                          {   return LEX_R_CBRACKET;                          }


[[:blank:]]+
^[[:blank:]]*\n             {   ++lines;                                        }
\n                          {   ++lines;                                        }

\;                          {   return LEX_SEMICOLON;                           }
\,                          {   return LEX_COMMA;                               }

\[                          {   return LEX_L_BRACKET;                           }
\]                          {   return LEX_R_BRACKET;                           }
\:                          {   return LEX_COLON;                               }

\"hourly\"                  {   return LEX_HOURLY;                               }
\"daily\"                   {   return LEX_DAILY;                               }
\"current_weather\"         {   return LEX_CURRENT;                               }

\"[0-9]+-[0-9]+-[0-9]+T[0-9]+:[0-9]+\"              {   return create_string_token(LEX_DATE_ISO);         }
\"[0-9]+-[0-9]+-[0-9]+\"              {   return create_string_token(LEX_DATE_ISO);         }

-?[0-9\.]+                  {   return create_val_token(LEX_NUM);               }
\"[0-9a-z_]+\"              {   return create_string_token(LEX_STRING);         }

\"[0-9A-Za-z_/°:-]+\"       {  return create_string_token(LEX_STRING);         }


.                           { PRINTF("error: %s\n", yytext);   return LEX_ERROR;                               }

%%

void set_input_string(const char* in) {
  yy_scan_string(in);
}

void end_lexical_scan(void) {
  yy_delete_buffer(YY_CURRENT_BUFFER);
}

static int create_val_token(int token)
{
    yylval.number = std::atof(yytext);
    return token;
}

static int create_string_token(int token)
{
  yylval.str_p = new std::string(yytext);
  //PRINTF("%s\n", yylval.str_p->c_str());
  return token;
}

int yywrap()
{
    return 1;
}