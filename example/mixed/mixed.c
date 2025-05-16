#include "json_writer.h"

int main(void) {
    JsonWriter *jw = jw_open("mixed.json");
    jw_style_pretty_tabs(jw);

    jw_object_start(jw);
    jw_key(jw, "pi");     jw_double(jw, 3.14159);
    jw_key(jw, "valid");  jw_bool(jw, false);
    jw_key(jw, "notes");  jw_null(jw);
    jw_key(jw, "values");
    jw_array_start(jw);
    jw_integer(jw, 10);
    jw_integer(jw, 20);
    jw_integer(jw, 30);
    jw_array_end(jw);
    jw_object_end(jw);

    jw_close(jw);
    return 0;
}
