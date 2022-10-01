%{
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdarg>
#include <cinttypes>
#include <string>
#include <sstream>
#include <cstring>
#include "parser.hpp"

#include <unordered_map>


static void yyerror(const char *msg);
static int yyparse(void);
int yylex(void);

extern FILE *yyin;

static bool grammarError = false;

union Value
{
    std::string* string;
    float number;
};

static ForecastS* new_forecast = 0;

enum class DataState
{
    DEFAULT,
    HOURLY,
    DAILY,
    CURRENT
};

static DataState current_state = DataState::DEFAULT;
static std::unordered_map<std::string, void*> forecast_offsets;


%}

%code requires
{
    #include <cstdint>
    #include <string>
    #include <cstring>
    #include "parser.hpp"


    struct TimeArr
    {
        TimeIso8601S* array_p;
        size_t last_id;
    };

    struct FloatArr
    {
        float* array_p;
        size_t last_id;
    };

}

%union
{
    float number;
    std::string* str_p;
    TimeIso8601S* time_p;
    TimeArr* time_arr;
    FloatArr* float_arr;

}

%token  LEX_STRING LEX_DATE_ISO LEX_NUM
        LEX_L_BRACKET LEX_R_BRACKET
        LEX_L_CBRACKET LEX_R_CBRACKET
        LEX_COMMA LEX_SEMICOLON
        LEX_COLON
        LEX_ERROR
        LEX_HOURLY LEX_DAILY LEX_CURRENT

%type <number> LEX_NUM float_num
%type <str_p> string LEX_STRING LEX_DATE_ISO

%type <time_p> time

%type <time_arr> array_time value_array_time

%type <float_arr> array_float value_array_float




%%

program:
    LEX_L_CBRACKET declarations LEX_R_CBRACKET{
        std::cout << "finished" << std::endl;
    }
    | LEX_ERROR {}
;

declarations:
    declarations LEX_COMMA assign
    {

    }
    | assign
    {

    }

assign:
    string LEX_COLON float_num
    {
        //std::cout << "assigning value float_num " << std::endl;
        switch(current_state)
        {
            case DataState::DEFAULT:
            case DataState::CURRENT:
            {
                if(forecast_offsets.find(*$1) != forecast_offsets.end())
                {
                    float* float_p = (float*)forecast_offsets.at(*$1);
                    *float_p = $3;
                }
            }
            case DataState::HOURLY:
            case DataState::DAILY: // TODO idk
            break;
        }
        delete $1;
    }
    | string LEX_COLON time
    {
        // we dont need any of this
        delete $1;
    }
    | string LEX_COLON string
    {
        // we dont need any of this
        // std::cout << *$1 << "assigning " << *$3 << std::endl;
        delete $1;
        delete $3;
    }
    | string LEX_COLON value_array_string
    {
        // we dont need any of this
        delete $1;
    }
    | string LEX_COLON value_array_float
    {
        FloatArr* new_array = $3;
        std::string* var_name = $1;
        if(forecast_offsets.find(*var_name) != forecast_offsets.end())
        {
            //std::cout << "assigning " << var_name->c_str() << " array float " << new_array->last_id << std::endl;

            float** structure_array_p = (float**)forecast_offsets.at(*var_name);
            *structure_array_p = new_array->array_p;
        }

        delete new_array;
        delete var_name;
    }
    | string LEX_COLON value_array_time
    {
        TimeArr* new_array = $3;
        std::string* var_name = $1;

        //std::cout << "assigning value array time " << new_array->last_id << std::endl;
        // for(size_t i = 0; i < new_array->last_id; i++)
        // {
        //     std::cout << new_array->array_p[i].hour << " ";
        // }
        // std::cout << std::endl;
        switch(current_state)
        {
            case DataState::DAILY:
                {
                    TimeIso8601S** structure_array_p = (TimeIso8601S**)forecast_offsets.at(*var_name);
                    *structure_array_p = new_array->array_p;
                }
                break;
            default:
                delete[] new_array->array_p;
        }
        delete new_array;
        delete var_name;

    }
    | hourly LEX_COLON LEX_L_CBRACKET declarations LEX_R_CBRACKET
    {
        // std::cout << "[INFO] restore state " << (int)current_state << std::endl;
        current_state = DataState::DEFAULT;

    }
    | daily LEX_COLON LEX_L_CBRACKET declarations LEX_R_CBRACKET
    {
        // std::cout << "[INFO] restore state " << (int)current_state << std::endl;
        current_state = DataState::DEFAULT;

    }
    | current LEX_COLON LEX_L_CBRACKET declarations LEX_R_CBRACKET
    {
        // std::cout << "[INFO] restore state " << (int)current_state << std::endl;
        current_state = DataState::DEFAULT;

    }
    | string LEX_COLON LEX_L_CBRACKET declarations LEX_R_CBRACKET
    {
        // std::cout << "[INFO] restore state " << (int)current_state << std::endl;
        current_state = DataState::DEFAULT;
        delete $1;
    }
;

hourly:
    LEX_HOURLY
    {
        current_state = DataState::HOURLY;
        // std::cout << "[INFO] Set current_state " << (int)current_state << std::endl;
    }
;
daily:
    LEX_DAILY
    {
        current_state = DataState::DAILY;
        // std::cout << "[INFO] Set current_state " << (int)current_state << std::endl;
    }
;
current:
    LEX_CURRENT
    {
        current_state = DataState::CURRENT;
        // std::cout << "[INFO] Set current_state " << (int)current_state << std::endl;
    }
;
string:
    LEX_STRING
    {
        //std::cout << "read string " << *$1 << std::endl;
        $$ = $1;
    }
;

// ========================================================
//    grammar for creating arrays of strings
// ========================================================
value_array_string:
    LEX_L_BRACKET array_string LEX_R_BRACKET {}
;
array_string:
    array_float LEX_COMMA LEX_STRING {}
    | LEX_STRING {}
;

// ========================================================
//    grammar for creating arrays of floats
// ========================================================
value_array_float:
    LEX_L_BRACKET array_float LEX_R_BRACKET
    {
        //std::cout << "read array float" << std::endl;
        $$ = $2;
    }
;

array_float:
    array_float LEX_COMMA float_num
    {
        FloatArr* new_arr = $1;

        // update array
        new_arr->array_p[new_arr->last_id] = $3;
        new_arr->last_id++;

        $$ = new_arr;
    }
    |
    float_num
    {
        FloatArr* new_arr = new FloatArr();
        new_arr->array_p = 0;
        new_arr->last_id = 0;
        switch(current_state)
        {
            case DataState::HOURLY:
                new_arr->array_p = new float[new_forecast->data_len];
                break;
            case DataState::DAILY:
                new_arr->array_p = new float[new_forecast->data_len / DATA_PER_DAY];
                break;
            default:
            // error ?
                new_arr->array_p = 0;
            break;
        }

        //std::cout << "insert float " << $1 << std::endl;

        // update array
        new_arr->array_p[new_arr->last_id] = $1;
        new_arr->last_id++;

        $$ = new_arr;
    }
;

float_num:
    LEX_NUM
    {
        auto num = $1;
        //std::cout << "read float " << num << std::endl;
        $$ = num;
    }
;

// ========================================================
//    grammar for creating arrays of time/dates
// ========================================================
value_array_time:
    LEX_L_BRACKET array_time LEX_R_BRACKET
    {
        //std::cout << "read array time" << std::endl;
        $$ = $2;
    }
;

array_time:
    array_time LEX_COMMA time
    {
        TimeArr* new_arr = $1;

        // update array
        new_arr->array_p[new_arr->last_id] = *$3;
        delete $3;
        new_arr->last_id++;

        $$ = new_arr;
    }
    |
    time
    {
        TimeArr* new_arr = new TimeArr();
        new_arr->array_p = 0;
        new_arr->last_id = 0;
        switch(current_state)
        {
            case DataState::HOURLY:
                new_arr->array_p = new TimeIso8601S[new_forecast->data_len];
                break;
            case DataState::DAILY:
                new_arr->array_p = new TimeIso8601S[new_forecast->data_len / DATA_PER_DAY];
                break;
            default:
            // error ?
                new_arr->array_p = 0;
            break;
        }

        //std::cout << "insert date " << $1 << std::endl;


        // update array
        new_arr->array_p[new_arr->last_id] = *$1;
        delete $1;
        new_arr->last_id++;

        $$ = new_arr;
    }
;

time:
    LEX_DATE_ISO
    {
        auto time_p = TimeIso8601_from_string(*$1);
        //std::cout << "read date" << *$1 << " -> " << time_p->year << std::endl;
        $$ = time_p;
    }
;


%%

static void yyerror(const char* msg)
{
    grammarError = true;
    std:: cout << "!!!ERROR " << msg << std::endl;
}


/* Declarations */
void set_input_string(const char* in);
void end_lexical_scan(void);

/* This function parses a string */
int parse_string(const char* in) {
  set_input_string(in);
  int rv = yyparse();
  end_lexical_scan();
  return rv;
}

[[nodiscard]]
ForecastS* parse_json(const std::string& json, uint8_t days)
{
    new_forecast = new ForecastS(days);
    create_offsets(forecast_offsets, *new_forecast);
    //std::cout << forecast_offsets.at("\"temperature_2m\"") << std::endl;

    parse_string(json.c_str());

    if(grammarError){
        // TODO
        std::cout << "ERROR in parsing file" << std::endl;
        delete new_forecast;
        new_forecast = 0;
        return NULL;
    }

    /* for(size_t i = 0; i < new_forecast->data_len/DATA_PER_DAY; i++)
    {   if(new_forecast->sunset != 0)
            TimeIso8601_print( new_forecast->sunset[i]);
    }
    std::cout << std::endl;
    for(size_t i = 0; i < new_forecast->data_len; i++)
    {   if(new_forecast->temperature_2m != 0)
            std::cout << new_forecast->temperature_2m[i] << " ";
    }
    std::cout << std::endl;
    for(size_t i = 0; i < new_forecast->data_len; i++)
    {   if(new_forecast->precipitation != 0)
            std::cout << new_forecast->precipitation[i] << " ";
    }
    std::cout << std::endl; */

    ForecastS* ret_forecast = new_forecast;
    new_forecast = NULL;
    return ret_forecast;
}
