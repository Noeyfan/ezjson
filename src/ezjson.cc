#include "ezjson.h"
#include "any_stack.h"
#include <iostream>
#include <cassert>
#include <cctype>

// forward declaration
parse_status parse_json_value(json_value* v, json_context* jc);

// definition
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
	v->bvalue = false;
	v->type = JSON_BOOLEAN;
	json += 5;
    }

    if (*json == 't' &&
	*(json+1) == 'r' &&
	*(json+2) == 'u' &&
	*(json+3) == 'e' ) {
	result = OK;
	v->bvalue = true;
	v->type = JSON_BOOLEAN;
	json += 4;
    }

    jc->context = json;
    return result;
}

#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1T9(ch) ((ch) >= '1' && (ch) <= '9')

parse_status parse_json_number(json_value* v, json_context* jc) {
    const char* json = jc->context;
    if(*json == '-') json++;
    if(!ISDIGIT(*json)) return ERROR;
    if(*json == '0' && ISDIGIT1T9(*(json+1))) return ERROR;
    while(isdigit(*json) ||
	  (*json == '.' && json != jc->context)) {
	json++;
    }
    v->number = std::atof(jc->context);
    v->type = JSON_NUMBER;
    jc->context = json;
    // std::cout << "end: "<<*json << "\n";
    return OK;
}

parse_status parse_json_string(json_value* v, json_context* jc) {
    const char* json = jc->context;
    any_stack as;
    v->s.len = 0;
    for(;;) {
	json++;
	v->s.len++;
	// std::cout << "*json is: " << *json << "\n";
	switch(*json) {
	  case '\"':
	    *as.push<char>() = '\0';
	    v->type = JSON_STRING;
	    v->s.s = (char*)malloc(v->s.len + 1);
	    strcpy(v->s.s, as.bottom<char>());
	    as.pop<char>(v->s.len + 1);
	    jc->context = json + 1;
	    return OK;
	  case '\0':
	    jc->context = json;
	    assert(false);
	    return ERROR;
	  default:
	    *as.push<char>() = *json;
	}
    }
    assert(false);
    return ERROR;
}

parse_status parse_json_array(json_value* v, json_context* jc) {
    json_context tmp_jc(jc->context + 1);
    int size = 0;
    any_stack as;
    for(;;) {
	// std::cout << "context: " << *tmp_jc.context << "\n";
	switch(*tmp_jc.context) {
	  case ']':
	    v->a.e = (json_value*)malloc(size * sizeof(json_value));
	    memcpy(v->a.e, as.bottom<json_value>(), size * sizeof(json_value));
	    v->a.size = size;
	    jc->context = tmp_jc.context + 1;
	    v->type = JSON_ARRAY;
	    return OK;
	  case ',':
	    tmp_jc.context += 1;
	    break;
	  case '\0':
	    assert(false);
	    return ERROR;
	  default:
	    json_value tmp_v;
	    // std::cout << "enter default: " << *tmp_jc.context << "\n";
	    assert(parse_json_value(&tmp_v, &tmp_jc) == OK);
	    *as.push<json_value>() = tmp_v;
	    size++;
	}
    }
    return ERROR;
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
      case '\"':
	return parse_json_string(v, jc);
      case '[':
	return parse_json_array(v, jc);
      default:
	return parse_json_number(v, jc);
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
