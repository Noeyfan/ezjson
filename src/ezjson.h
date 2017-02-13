#ifndef __EZJSON_H__
#define __EZJSON_H__

#include <cstring>
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
    union {
	struct {json_value* e; size_t size; }a;
	struct {char* s; size_t len; }s;
	bool bvalue;
	double number;
    };
};

struct json_context {
    json_context(const char* rhs) : context(rhs) { }
    const char* context;
};

int parse_json(json_value* v, const char* json);
json_type get_json_type(const json_value* v);

#endif
