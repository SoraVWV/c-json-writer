#include "json_writer.h"

int main(void) {
    JsonWriter *jw = jw_open("users.json");
    jw_style_pretty(jw, 2);    // 2 spaces indent

    jw_array_start(jw);

    // User 1
    jw_object_start(jw);
    jw_key(jw, "id");   jw_integer(jw, 1);
    jw_key(jw, "email");jw_string(jw, "user1@example.com");
    jw_object_end(jw);

    // User 2
    jw_object_start(jw);
    jw_key(jw, "id");   jw_integer(jw, 2);
    jw_key(jw, "email");jw_string(jw, "user2@example.com");
    jw_object_end(jw);

    jw_array_end(jw);
    jw_close(jw);
    return 0;
}
