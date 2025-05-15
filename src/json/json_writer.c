#include "json_writer.h"

#include <stdlib.h>
#include <string.h>

static void escape_string(FILE *file, const char *content) {
    fputc('"', file);
    for (const char *p = content; *p; ++p) {
        switch (*p) {
            case '"': fputs("\\\"", file); break;
            case '\\': fputs("\\\\", file); break;
            case '\n': fputs("\\n", file); break;
            case '\r': fputs("\\r", file); break;
            case '\t': fputs("\\t", file); break;
            default: fputc(*p, file);
        }
    }
    fputc('"', file);
}

static void write_indent(const JsonWriter *jw) {
    if (jw->config.style == JSON_FORMAT_COMPACT) return;
    fputc('\n', jw->file);
    for (int i = 0; i < jw->config.indent_level *
        (jw->config.style == JSON_FORMAT_PRETTY_TABS ? 1 : jw->config.indent_size); i++) {
        fputc(jw->config.style == JSON_FORMAT_PRETTY_TABS ? '\t' : ' ', jw->file);
    }
}

static void write_comma(const JsonWriter *jw) {
    if (jw->context == JSON_FORMAT_CONTEXT_AFTER_VALUE) {
        fputs(",", jw->file);
        write_indent(jw);
    }

    if (jw->context == JSON_FORMAT_CONTEXT_START) {
        write_indent(jw);
    }
}

JsonWriter *jw_open(const char *filename) {
    JsonWriter *jw = malloc(sizeof(JsonWriter));
    if (!jw) return NULL;
    jw->filename = strdup(filename);
    jw->file = fopen(filename, "w");
    if (!jw->file) {
        free(jw->filename);
        free(jw);
        return NULL;
    }
    jw->config.style = JSON_FORMAT_COMPACT;
    jw->config.indent_level = 0;
    jw->config.indent_size = 4;
    jw->context = JSON_FORMAT_CONTEXT_NONE;
    return jw;
}

void jw_close(JsonWriter *jw) {
    if (!jw) return;
    fclose(jw->file);
    free(jw->filename);
    free(jw);
}

void jw_key(JsonWriter *jw, const char *content) {
    write_comma(jw);
    escape_string(jw->file, content);

    fputc(':', jw->file);
    if (jw->config.style != JSON_FORMAT_COMPACT) {
        fputc(' ', jw->file);
    }

    jw->context = JSON_FORMAT_CONTEXT_AFTER_KEY;
}

void jw_string(JsonWriter *jw, const char *content) {
    write_comma(jw);
    escape_string(jw->file, content);
    jw->context = JSON_FORMAT_CONTEXT_AFTER_VALUE;
}

void jw_integer(JsonWriter *jw, const int content) {
    write_comma(jw);
    fprintf(jw->file, "%d", content);
    jw->context = JSON_FORMAT_CONTEXT_AFTER_VALUE;
}

void jw_long(JsonWriter *jw, const long long content) {
    write_comma(jw);
    fprintf(jw->file, "%lld", content);
    jw->context = JSON_FORMAT_CONTEXT_AFTER_VALUE;
}

void jw_float(JsonWriter *jw, const float content) {
    write_comma(jw);
    fprintf(jw->file, "%g", content);
    jw->context = JSON_FORMAT_CONTEXT_AFTER_VALUE;
}

void jw_double(JsonWriter *jw, const double content) {
    write_comma(jw);
    fprintf(jw->file, "%g", content);
    jw->context = JSON_FORMAT_CONTEXT_AFTER_VALUE;
}

void jw_bool(JsonWriter *jw, const int content) {
    write_comma(jw);
    fputs(content ? "true" : "false", jw->file);
    jw->context = JSON_FORMAT_CONTEXT_AFTER_VALUE;
}

void jw_null(JsonWriter *jw) {
    write_comma(jw);
    fputs("null", jw->file);
    jw->context = JSON_FORMAT_CONTEXT_AFTER_VALUE;
}

void jw_array_start(JsonWriter *jw) {
    write_comma(jw);
    fputc('[', jw->file);
    jw->config.indent_level++;
    jw->context = JSON_FORMAT_CONTEXT_START;
}

void jw_array_end(JsonWriter *jw) {
    jw->config.indent_level--;
    write_indent(jw);
    fputc(']', jw->file);
    jw->context = JSON_FORMAT_CONTEXT_AFTER_VALUE;
}

void jw_object_start(JsonWriter *jw) {
    write_comma(jw);
    fputc('{', jw->file);
    jw->config.indent_level++;
    jw->context = JSON_FORMAT_CONTEXT_START;
}

void jw_object_end(JsonWriter *jw) {
    jw->config.indent_level--;
    write_indent(jw);
    fputc('}', jw->file);
    jw->context = JSON_FORMAT_CONTEXT_AFTER_VALUE;
}

void jw_style_compact(JsonWriter *jw) {
    jw->config.style = JSON_FORMAT_COMPACT;
}

void jw_style_pretty(JsonWriter *jw, const int indent_size) {
    jw->config.style = JSON_FORMAT_PRETTY;
    jw->config.indent_size = indent_size;
}

void jw_style_pretty_tabs(JsonWriter *jw) {
    jw->config.style = JSON_FORMAT_PRETTY_TABS;
}
