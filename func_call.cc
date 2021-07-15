#include <iostream>
#include <functional>

#include "sljitLir.h"

static long SLJIT_FUNC print_num(long a)
{
  std::cout << "a = " << a << std::endl;
  return a+1;
}

// generate code for the following function calls
#if 0
  {
    if((a & 1) == 0)
      return print_num(c);
    return print_num(b);
  }
#endif
static int func_call(long a, long b, long c)
{
  void* code;
  unsigned long len;
  typedef long (SLJIT_FUNC *func3_t)(long a, long b, long c);

  struct sljit_compiler *C = sljit_create_compiler(nullptr, nullptr);
  struct sljit_jump *print_c;
  struct sljit_jump *out;

  sljit_emit_enter(C, 0, SLJIT_ARG1(SW)|SLJIT_ARG2(SW)|SLJIT_ARG3(SW), 3, 3, 0, 0, 0);

  // evaluate a & 1 ----> R0
  sljit_emit_op2(C, SLJIT_AND, SLJIT_R0, 0, SLJIT_S0, 0, SLJIT_IMM, 1);
  // compare R0 with 0 - if true -----> jump print_c  (conditional jump)
  print_c = sljit_emit_cmp(C, SLJIT_EQUAL, SLJIT_R0, 0, SLJIT_IMM, 0);

  // R0 = S1
  sljit_emit_op1(C, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S1, 0);
  // call print_num(R0)
  sljit_emit_icall(C, SLJIT_CALL, SLJIT_RET(SW)|SLJIT_ARG1(SW), SLJIT_IMM, SLJIT_FUNC_OFFSET(print_num));

  // jump to out (unconditional jump)
  out = sljit_emit_jump(C, SLJIT_JUMP);

  // print_c:
  sljit_set_label(print_c, sljit_emit_label(C));
  // R0 = c
  sljit_emit_op1(C, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S2, 0);
  // print_num(R0)
  sljit_emit_icall(C, SLJIT_CALL, SLJIT_RET(SW)|SLJIT_ARG1(SW), SLJIT_IMM, SLJIT_FUNC_OFFSET(print_num));

  // out:
  sljit_set_label(out, sljit_emit_label(C));
  sljit_emit_return(C, SLJIT_MOV, SLJIT_R0, 0);

  code = sljit_generate_code(C);
  len = sljit_get_generated_code_size(C);

  std::function<long(long,long,long)> func = (func3_t)code;
  long res = func(a, b, c);
  std::cout << "func return " << res << std::endl;

  sljit_free_compiler(C);
  sljit_free_code(code, nullptr);
  return 0;
}

int main()
{
  return func_call(4, 5, 6);
}
