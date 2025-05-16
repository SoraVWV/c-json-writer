#include "json_writer.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static void escape_string(FILE *file, const bool escape_unicode, const char *content) {
    if (!content) {
        fputs("null", file);
        return;
    }

    fputc('"', file);

    const unsigned char *p = (const unsigned char *)content;
    while (*p) {
        const unsigned char c = *p;

        if (c < 0x80) {
            switch (c) {
                case '"':  fputs("\\\"", file); break;
                case '\\': fputs("\\\\", file); break;
                case '\n': fputs("\\n", file); break;
                case '\r': fputs("\\r", file); break;
                case '\t': fputs("\\t", file); break;
                default:  fputc(c, file);
            }
            p++;
        } else {
            if (!escape_unicode) {
                fputc(c, file);
                p++;
            } else {
                unsigned code_point = 0;
                int extra_bytes = 0;

                if ((c & 0xE0) == 0xC0) {
                    code_point = (c & 0x1F) << 6;
                    extra_bytes = 1;
                } else if ((c & 0xF0) == 0xE0) {
                    code_point = (c & 0x0F) << 12;
                    extra_bytes = 2;
                } else if ((c & 0xF8) == 0xF0) {
                    code_point = (c & 0x07) << 18;
                    extra_bytes = 3;
                } else {
                    fputs("\\uFFFD", file);
                    p++;
                    continue;
                }

                for (int i = 0; i < extra_bytes; i++) {
                    p++;
                    if (!*p || (*p & 0xC0) != 0x80) {
                        fputs("\\uFFFD", file);
                        break;
                    }
                    code_point |= (*p & 0x3F) << (6 * (extra_bytes - 1 - i));
                }
                p++;

                if (code_point >= 0x10000) {
                    const unsigned high_surrogate = 0xD800 + ((code_point - 0x10000) >> 10);
                    const unsigned low_surrogate = 0xDC00 + ((code_point - 0x10000) & 0x3FF);
                    fprintf(file, "\\u%04X\\u%04X", high_surrogate, low_surrogate);
                } else {
                    fprintf(file, "\\u%04X", code_point);
                }
            }
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
    jw->config.precision_float = -1;
    jw->config.precision_double = -1;
    jw->config.escape_unicode = true;
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
    escape_string(jw->file, jw->config.escape_unicode, content);

    fputc(':', jw->file);
    if (jw->config.style != JSON_FORMAT_COMPACT) {
        fputc(' ', jw->file);
    }

    jw->context = JSON_FORMAT_CONTEXT_AFTER_KEY;
}

void jw_string(JsonWriter *jw, const char *content) {
    write_comma(jw);
    escape_string(jw->file, jw->config.escape_unicode, content);
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
    if (jw->config.precision_float >= 0) {
        fprintf(jw->file, "%.*f", jw->config.precision_float, content);
    } else {
        fprintf(jw->file, "%g", content);
    }
    jw->context = JSON_FORMAT_CONTEXT_AFTER_VALUE;
}

void jw_double(JsonWriter *jw, const double content) {
    write_comma(jw);
    if (jw->config.precision_double >= 0) {
        fprintf(jw->file, "%.*f", jw->config.precision_double, content);
    } else {
        fprintf(jw->file, "%g", content);
    }
    jw->context = JSON_FORMAT_CONTEXT_AFTER_VALUE;
}

void jw_bool(JsonWriter *jw, const bool content) {
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

void jw_stype_precision_float(JsonWriter *jw, const int precision) {
    jw->config.precision_float = precision;
}

void jw_stype_precision_double(JsonWriter *jw, const int precision) {
    jw->config.precision_double = precision;
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

void jw_style_escape_unicode(JsonWriter *jw, const bool escape_unicode) {
    jw->config.escape_unicode = escape_unicode;
}
