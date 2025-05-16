#include "json_writer.h"

int main(void) {
    JsonWriter *jw = jw_open("output.json");
    // default is compact style
    jw_object_start(jw);
    jw_key(jw, "name"); jw_string(jw, "Alice");
    jw_key(jw, "age");  jw_integer(jw, 30);
    jw_key(jw, "active"); jw_bool(jw, true);
    jw_object_end(jw);
    jw_close(jw);
    return 0;
}
