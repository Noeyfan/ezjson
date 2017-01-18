#ifndef __EZJSON_H__
#define __EZJSON_H__
enum json_type {
    JSON_NULL,
    JSON_BOOLEAN,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
};

enum parse_status {
    OK,
    ERROR
};

struct json_value {
    json_type type;
    bool value;
};

struct json_context {
    json_context(const char* rhs) : context(rhs) { }
    const char* context;
};

int parse_json(json_value* v, const char* json);
json_type get_json_type(const json_value* v);

#endif
