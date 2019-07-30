#include "exception.h"

jmp_buf _exception_jump_buf;
bool _exception_active = false;
volatile int _exception_code = 0;
