#include "ezjson.h"
#include <iostream>
#include <cassert>

void parse_json_whitespace(json_context* jscontext) {
    // just consume ws
    const char* json = jscontext->context;
    while(json && *json == ' ') {
	json++;
    }
    jscontext->context = json;
}

parse_status parse_json_value_null(json_value* v, json_context* jc) {
    parse_status result = ERROR;
    const char* json = jc->context;
    if (*json == 'n' &&
	*(json+1) == 'u' &&
	*(json+2) == 'l' &&
	*(json+3) == 'l') {
	result = OK;
	v->type = JSON_NULL;
	json += 4;
    }

    jc->context = json;
    return result;
}

parse_status parse_json_value_boolean(json_value* v, json_context* jc) {
    parse_status result = ERROR;
    const char* json = jc->context;
    if (
	*json == 'f' &&
	*(json+1) == 'a' &&
	*(json+2) == 'l' &&
	*(json+3) == 's' &&
	*(json+4) == 'e' ) {
	result = OK;
	v->value = false;
	v->type = JSON_BOOLEAN;
	json += 5;
    }

    if (*json == 't' &&
	*(json+1) == 'r' &&
	*(json+2) == 'u' &&
	*(json+3) == 'e' ) {
	result = OK;
	v->value = true;
	v->type = JSON_BOOLEAN;
	json += 4;
    }

    jc->context = json;
    return result;
}

int i = 0;

parse_status parse_json_value(json_value* v, json_context* jc) {
    assert(jc->context);
    switch(*(jc->context)) {
      case 'f':
      case 't':
	return parse_json_value_boolean(v, jc);
      case 'n':
	return parse_json_value_null(v, jc);
      default:
	return ERROR;
    }
}

json_type get_json_type(const json_value* v) {
    return v->type;
}

int parse_json(json_value* v, const char* json) {
    json_context jc(json);
    parse_json_whitespace(&jc);
    parse_status result = parse_json_value(v, &jc);
    parse_json_whitespace(&jc);
    if (jc.context && *(jc.context) != '\0') result = ERROR;
    return result;
}
