%{
#include "parser_gpx_bison.hpp"
#include <string>
#include <stdio.h>
#include "traces.h"

#define fileno(x) 0

uint64_t lines = 1;
static int create_val_token(int token);
static int create_string_token(int token);
static std::string buffer;

%}

%x IN_TAG
%x IN_QUOTE
%x IN_CONTENT

%%

"<"                     { BEGIN(IN_TAG); return LEX_TAG_START; }
">"                     { BEGIN(IN_CONTENT); return LEX_TAG_END; }
"/"                     { return LEX_SLASH; }
"="                     { return LEX_EQUALS; }
"\""                    { BEGIN(IN_QUOTE); return LEX_QUOTE; }

[[:blank:]]+           /* ignore whitespace */
\n                     { ++lines; }

<IN_TAG>{
    "gpx"              { return LEX_GPX; }
    "trk"              { return LEX_TRK; }
    "trkseg"           { return LEX_TRKSEG; }
    "trkpt"            { return LEX_TRKPT; }
    "ele"              { return LEX_ELE; }
    "name"             { return LEX_NAME; }
    "lat"              { return LEX_LAT; }
    "lon"              { return LEX_LON; }
    [a-zA-Z0-9:./_-]+  { return create_string_token(LEX_STRING); }
}

<IN_QUOTE>{
    \"                 { BEGIN(IN_TAG); return LEX_QUOTE; }
    -?[0-9]+\.[0-9]+  { return create_val_token(LEX_NUM); }
    [^\n\"]+          { return create_string_token(LEX_CONTENT); }
}

<IN_CONTENT>{
    "<"               { BEGIN(IN_TAG); return LEX_TAG_START; }
    -?[0-9]+\.[0-9]+ { return create_val_token(LEX_NUM); }
    [^<\n[:blank:]]+ { return create_string_token(LEX_CONTENT); }
}

.                     { TRACE_ERROR(TRACE_CORE_0, "Lexer error: unexpected character '%s'\n", yytext); return LEX_ERROR; }

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
  return token;
}

int yywrap()
{
    return 1;
}