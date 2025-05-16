#ifndef JSON_WRITER_H
#define JSON_WRITER_H
#define JSON_WRITER_VERSION "0.01.1"

#include <stdbool.h>
#include <stdio.h>

typedef enum {
    JSON_FORMAT_CONTEXT_NONE,
    JSON_FORMAT_CONTEXT_START,
    JSON_FORMAT_CONTEXT_AFTER_KEY,
    JSON_FORMAT_CONTEXT_AFTER_VALUE
} JsonFormatContext;

typedef enum {
    JSON_FORMAT_COMPACT,
    JSON_FORMAT_PRETTY,
    JSON_FORMAT_PRETTY_TABS
} JsonFormatStyle;

typedef struct {
    int indent_level;
    int indent_size;
    int precision_float;
    int precision_double;
    JsonFormatStyle style;
    bool escape_unicode;
} JsonFormatConfig;

typedef struct {
    char *filename;
    FILE *file;
    JsonFormatConfig config;
    JsonFormatContext context;
} JsonWriter;

JsonWriter *jw_open(const char *filename);

void jw_close(JsonWriter *jw);

void jw_key(JsonWriter *jw, const char *content);

void jw_string(JsonWriter *jw, const char *content);

void jw_integer(JsonWriter *jw, int content);

void jw_long(JsonWriter *jw, long long content);

void jw_float(JsonWriter *jw, float content);

void jw_double(JsonWriter *jw, double content);

void jw_bool(JsonWriter *jw, bool content);

void jw_null(JsonWriter *jw);

void jw_array_start(JsonWriter *jw);

void jw_array_end(JsonWriter *jw);

void jw_object_start(JsonWriter *jw);

void jw_object_end(JsonWriter *jw);

void jw_stype_precision_float(JsonWriter *jw, int precision);

void jw_stype_precision_double(JsonWriter *jw, int precision);

void jw_style_compact(JsonWriter *jw);

void jw_style_pretty(JsonWriter *jw, int indent_size);

void jw_style_pretty_tabs(JsonWriter *jw);

void jw_style_escape_unicode(JsonWriter *jw, bool escape_unicode);

#endif //JSON_WRITER_H
