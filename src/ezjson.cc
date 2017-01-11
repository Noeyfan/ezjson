#include "ezjson.h"
#include <iostream>
#include <cassert>

void parse_json_whitespace(const char* json) {
    // just consume ws
    while(json && *json == ' ') {
	json++;
    }
}

parse_status parse_json_value_null(json_value* v, const char* json) {
    parse_status result = ERROR;
    if (*json == 'n' &&
	*(json+1) == 'u' &&
	*(json+2) == 'l' &&
	*(json+3) == 'l') {
	result = OK;
	v->type = JSON_NULL;
    }

    return result;
}

parse_status parse_json_value_boolean(json_value* v, const char* json) {
    parse_status result = ERROR;
    if (
	*json == 'f' &&
	*(json+1) == 'a' &&
	*(json+2) == 'l' &&
	*(json+3) == 's' &&
	*(json+4) == 'e' ) {
	result = OK;
	v->value = false;
	v->type = JSON_BOOLEAN;
    }

    if (*json == 't' &&
	*(json+1) == 'r' &&
	*(json+2) == 'u' &&
	*(json+3) == 'e' ) {
	result = OK;
	v->value = true;
	v->type = JSON_BOOLEAN;
    }

    return result;
}

int i = 0;

parse_status parse_json_value(json_value* v, const char* json) {
    assert(json);
    switch(*json) {
      case 'f':
      case 't':
	return parse_json_value_boolean(v, json);
      case 'n':
	return parse_json_value_null(v, json);
      default:
	return ERROR;
    }
}

json_type get_json_type(const json_value* v) {
    return v->type;
}

int parse_json(json_value* v, const char* json) {
    json_contex jc(json);
    parse_json_whitespace(jc.context);
    parse_status result = parse_json_value(v, jc.context);
    parse_json_whitespace(jc.context);
    return result;
}
