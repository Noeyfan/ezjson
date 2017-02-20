#ifndef ANY_STACK_H_
#define ANY_STACK_H_

#ifndef _STACK_SIZE
#define _STACK_SIZE 2
#endif

#include <cstdlib>
#include <cassert>
class any_stack {
// use malloc in this stack, TODO replace with user defined Allocator
private:
    char* stack_bottom_;
    char* stack_top_;
    size_t stack_cap_;

    template<typename T>
      void expand() {
	  if (!stack_bottom_) {
	      // create new
	      stack_cap_ = _STACK_SIZE;
	      stack_bottom_ = stack_top_ = (char*)malloc(stack_cap_);
	  }else {
	      stack_cap_ *= 1.5;
	      char* new_bottom = (char*)realloc(stack_bottom_, stack_cap_);
	      assert(new_bottom); // if new_bottom is null, then realloc failed
	      stack_top_ = new_bottom + (stack_top_ - stack_bottom_);
	      stack_bottom_ = new_bottom;
	  }
      }
public:
    any_stack()
      : stack_bottom_(0), stack_top_(0), stack_cap_(0) { }

    ~any_stack() {
	free(stack_bottom_);
    }

    template<typename T>
      T* push(size_t count = 1) {
	  while (stack_top_ + sizeof(T) * count >= stack_bottom_ + stack_cap_) {
	      expand<T>();
	  }
	  char* p_ = stack_top_;
	  stack_top_ += sizeof(T) * count;
	  return (T*)p_;
      }

    template<typename T>
      T* pop(size_t count) {
	  assert(stack_top_ != stack_bottom_);
	  stack_top_ -= sizeof(T) * count;
	  return (T*)stack_top_;
      }

    template<typename T>
      T* top() {
	  return stack_top_;
      }

    template<typename T>
      T* bottom() {
	  return (T*)stack_bottom_;
      }

    bool empty() {
	return stack_top_ == stack_bottom_;
    }
};

#endif
