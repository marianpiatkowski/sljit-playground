// compile command: g++-7 -Wall -I../sljit/sljit_src -DSLJIT_CONFIG_AUTO=1 ../sljit/sljit_src/sljitLir.c hello_sljit.cc -o hello_sljit
#include <iostream>

#include "sljitLir.h"

int main()
{
  struct sljit_compiler *C = sljit_create_compiler(nullptr, nullptr);
  sljit_free_compiler(C);
  return 0;
}
