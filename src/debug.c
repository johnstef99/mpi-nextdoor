#include "debug.h"

void dd(const char *format, ...) {
  char *debug_env = getenv("DEBUG");
  if (debug_env != NULL && debug_env[0] == '1') {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
  }
}
