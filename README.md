# JSON Writer Library

A small C library for writing well-formed JSON in either compact or pretty-printed formats.

## Features

* Write arrays and objects, nested arbitrarily deep
* Support for strings, integers, floats, booleans, null
* Compact mode (no extra line breaks)
* Pretty mode with configurable spaces or tab indent

## API Overview

Include the header:

```c
#include "json_writer.h"
```

### Configuration & Context

* `JsonWriter *jw_open(const char *filename);`
* `void jw_close(JsonWriter *jw);`

By default, uses **compact** style. Change style before writing any content:

```c
jw_style_pretty(jw, 4);        // 4 spaces indent
jw_style_pretty_tabs(jw);      // tab indent
jw_style_compact(jw);          // no indent/line breaks
```

### Writing Primitives

* `void jw_key(JsonWriter *jw, const char *content);`
* `void jw_string(JsonWriter *jw, const char *content);`
* `void jw_integer(JsonWriter *jw, int content);`
* `void jw_long(JsonWriter *jw, long long content);`
* `void jw_float(JsonWriter *jw, float content);`
* `void jw_double(JsonWriter *jw, double content);`
* `void jw_bool(JsonWriter *jw, int content);`  (*0 = false, non-zero = true*)
* `void jw_null(JsonWriter *jw);`

### Containers

* `void jw_array_start(JsonWriter *jw);`
* `void jw_array_end(JsonWriter *jw);`
* `void jw_object_start(JsonWriter *jw);`
* `void jw_object_end(JsonWriter *jw);`

## Examples

### Example 1: Simple Object (Compact)

```c
#include "json_writer.h"

int main(void) {
    JsonWriter *jw = jw_open("output.json");
    // default is compact
    jw_object_start(jw);
      jw_key(jw, "name"); jw_string(jw, "Alice");
      jw_key(jw, "age");  jw_integer(jw, 30);
      jw_key(jw, "active"); jw_bool(jw, 1);
    jw_object_end(jw);
    jw_close(jw);
    return 0;
}
```

**`output.json` will contain:**

```json
{"name":"Alice","age":30,"active":true}
```

### Example 2: Pretty-Printed Array of Objects

```c
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
```

**`users.json` will contain:**

```json
[
  {
    "id": 1,
    "email": "user1@example.com"
  },
  {
    "id": 2,
    "email": "user2@example.com"
  }
]
```

### Example 3: Mixing Data Types & Nulls

```c
#include "json_writer.h"

int main(void) {
    JsonWriter *jw = jw_open("mixed.json");
    jw_style_pretty_tabs(jw);

    jw_object_start(jw);
      jw_key(jw, "pi");     jw_double(jw, 3.14159);
      jw_key(jw, "valid");  jw_bool(jw, 0);
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
```

**`mixed.json`**

```
{
	"pi": 3.14159,
	"valid": false,
	"notes": null,
	"values": [
		10,
		20,
		30
	]
}
```
