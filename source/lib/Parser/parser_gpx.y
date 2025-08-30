%{
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>
#include "traces.h"

static void yyerror(const char *msg);
static int yyparse(void);
int yylex(void);

extern FILE *yyin;

static bool grammarError = false;

struct GpxPoint {
    double lat;
    double lon;
    double elevation;
};

struct GpxTrack {
    std::string name;
    std::vector<GpxPoint> points;
};

static GpxTrack* current_track = nullptr;
static GpxPoint current_point;

// Buffer state for chunk processing
static std::string partial_content;
static bool in_trkpt = false;
static bool in_ele = false;

%}

%code requires {
    #include <string>
    #include <vector>

    struct GpxPoint {
        double lat;
        double lon;
        double elevation;
    };

    struct GpxTrack {
        std::string name;
        std::vector<GpxPoint> points;
    };
}

%union {
    double number;
    std::string* str_p;
    GpxPoint* point_p;
    GpxTrack* track_p;
}

%token  LEX_TAG_START LEX_TAG_END LEX_SLASH LEX_EQUALS LEX_QUOTE
%token  LEX_GPX LEX_TRK LEX_TRKSEG LEX_TRKPT LEX_ELE LEX_NAME
%token  <number> LEX_LAT LEX_LON LEX_NUM
%token  <str_p> LEX_STRING LEX_CONTENT

%type   <number> number
%type   <str_p> string_value
%type   <point_p> trkpt
%type   <track_p> track_segment track

%%

gpx: gpx_tag
     | gpx_fragment
;

gpx_tag: start_gpx track_list end_gpx
;

gpx_fragment: track_segment
            | trkpt
            | point_data
;

start_gpx: LEX_TAG_START LEX_GPX attributes LEX_TAG_END
        {
            if (!current_track) {
                current_track = new GpxTrack();
            }
        }
;

end_gpx: LEX_TAG_START LEX_SLASH LEX_GPX LEX_TAG_END
;

track_list: track
          | track_list track
;

track: start_trk track_segments end_trk
     {
        $$ = current_track;
     }
;

start_trk: LEX_TAG_START LEX_TRK attributes LEX_TAG_END
;

end_trk: LEX_TAG_START LEX_SLASH LEX_TRK LEX_TAG_END
;

track_segments: track_segment
              | track_segments track_segment
;

track_segment: trkseg_start trkpts trkseg_end
            {
                $$ = current_track;
            }
;

trkseg_start: LEX_TAG_START LEX_TRKSEG attributes LEX_TAG_END
;

trkseg_end: LEX_TAG_START LEX_SLASH LEX_TRKSEG LEX_TAG_END
;

trkpts: trkpt
      | trkpts trkpt
;

trkpt: start_trkpt point_data end_trkpt
     {
        $$ = new GpxPoint(current_point);
        if (current_track) {
            current_track->points.push_back(current_point);
        }
        current_point = GpxPoint(); // Reset for next point
     }
;

start_trkpt: LEX_TAG_START LEX_TRKPT attributes LEX_TAG_END
          {
            in_trkpt = true;
          }
;

end_trkpt: LEX_TAG_START LEX_SLASH LEX_TRKPT LEX_TAG_END
         {
            in_trkpt = false;
         }
;

point_data: /* empty */
          | point_data coordinate_attr
          | point_data elevation_element
;

coordinate_attr: LEX_LAT LEX_EQUALS LEX_QUOTE number LEX_QUOTE
               {
                   current_point.lat = $4;
               }
             | LEX_LON LEX_EQUALS LEX_QUOTE number LEX_QUOTE
               {
                   current_point.lon = $4;
               }
;

elevation_element: start_ele LEX_NUM end_ele
                {
                    current_point.elevation = $2;
                }
;

start_ele: LEX_TAG_START LEX_ELE LEX_TAG_END
         {
             in_ele = true;
         }
;

end_ele: LEX_TAG_START LEX_SLASH LEX_ELE LEX_TAG_END
       {
           in_ele = false;
       }
;

attributes: /* empty */
         | attributes attribute
;

attribute: LEX_STRING LEX_EQUALS LEX_QUOTE string_value LEX_QUOTE
        {
            delete $1;
            delete $4;
        }
;

string_value: LEX_CONTENT
           {
               $$ = $1;
           }
           | LEX_STRING
           {
               $$ = $1;
           }
;

number: LEX_NUM
      {
          $$ = $1;
      }
;

%%

static void yyerror([[maybe_unused]] const char* msg)
{
    grammarError = true;
    TRACE_ERROR(TRACE_CORE_0, "GPX Parse Error: %s\n", msg);
}

/* Interface functions */
extern "C" {
    void set_input_string(const char* in);
    void end_lexical_scan(void);
}

/* Initialize parser state */
void init_gpx_parser() {
    if (current_track) {
        delete current_track;
    }
    current_track = new GpxTrack();
    grammarError = false;
    in_trkpt = false;
    in_ele = false;
    partial_content.clear();
}

/* Parse a chunk of GPX data */
int parse_gpx_chunk(const char* chunk, size_t size) {
    if (!chunk || size == 0) {
        return -1;
    }

    // Append to any existing partial content
    partial_content.append(chunk, size);

    // Try to parse what we have
    set_input_string(partial_content.c_str());
    int rv = yyparse();
    end_lexical_scan();

    if (rv != 0 && !in_trkpt && !in_ele) {
        // If parsing failed but we're not in the middle of a trackpoint or elevation,
        // clear partial content as it may be corrupted
        partial_content.clear();
    }

    return rv;
}

/* Get the parsed track and reset parser state */
GpxTrack* get_parsed_track() {
    if (grammarError || !current_track || current_track->points.empty()) {
        delete current_track;
        current_track = nullptr;
        return nullptr;
    }

    GpxTrack* result = current_track;
    current_track = nullptr;
    return result;
}

/* Reset the parser state */
void reset_gpx_parser() {
    delete current_track;
    current_track = nullptr;
    grammarError = false;
    in_trkpt = false;
    in_ele = false;
    partial_content.clear();
}
    | track_segments track_segment
    ;

track_segment:
    segment_start track_points segment_end
    ;

segment_start:
    LEX_TAG_START LEX_TRKSEG LEX_TAG_END
    ;

segment_end:
    LEX_TAG_START LEX_SLASH LEX_TRKSEG LEX_TAG_END
    ;

track_points:
    track_point
    | track_points track_point
    ;

track_point:
    trkpt_start elevation trkpt_end
    {
        if (current_track) {
            current_track->points.push_back(current_point);
        }
    }
    ;

trkpt_start:
    LEX_TAG_START LEX_TRKPT attributes LEX_TAG_END
    ;

trkpt_end:
    LEX_TAG_START LEX_SLASH LEX_TRKPT LEX_TAG_END
    ;

elevation:
    LEX_TAG_START LEX_ELE LEX_TAG_END number LEX_TAG_START LEX_SLASH LEX_ELE LEX_TAG_END
    {
        current_point.elevation = $4;
    }
    | /* empty */
    {
        current_point.elevation = 0.0;
    }
    ;
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
            case DataState::DAILY:
            break;
        }
        delete $1;
    }
%%

static void yyerror([[maybe_unused]] const char* msg)
{
    grammarError = true;
    TRACE_ERROR(TRACE_CORE_0, "GPX Parse Error: %s\n", msg);
}

/* Declarations */
void set_input_string(const char* in);
void end_lexical_scan(void);

/* This function parses a chunk of GPX data */
int parse_gpx_chunk(const char* chunk, size_t size) {
    set_input_string(chunk);
    int rv = yyparse();
    end_lexical_scan();
    return rv;
}

/* Initialize parser state */
void init_gpx_parser() {
    if (current_track) {
        delete current_track;
    }
    current_track = new GpxTrack();
    grammarError = false;
}

/* Get the parsed track and reset parser state */
GpxTrack* get_parsed_track() {
    if (grammarError || !current_track) {
        delete current_track;
        current_track = nullptr;
        return nullptr;
    }

    GpxTrack* result = current_track;
    current_track = nullptr;
    return result;
}
