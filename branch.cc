#include <iostream>
#include <functional>

#include "sljitLir.h"

static void dump_code(void* code, sljit_uw len)
{
  FILE *fp = fopen("/tmp/slj_dump", "wb");
  if(!fp)
    return;
  fwrite(code, len, 1, fp);
  fclose(fp);
#if defined(SLJIT_CONFIG_X86_64)
  system("objdump -b binary -m l1om -D /tmp/slj_dump");
#elif defined(SLJIT_CONFIG_X86_32)
  system("objdump -b binary -m i386 -D /tmp/slj_dump");
#endif
}

static int branch(long a, long b, long c)
{
  void* code;
  unsigned long len;
  typedef long (SLJIT_FUNC *func3_t)(long a, long b, long c);

  struct sljit_compiler *C = sljit_create_compiler(nullptr, nullptr);
  struct sljit_jump *ret_c;
  struct sljit_jump *out;

  // start a context, here function entry
  sljit_emit_enter(C, 0, SLJIT_ARG1(SW)|SLJIT_ARG2(SW)|SLJIT_ARG3(SW), 3, 3, 0, 0, 0);

  // evaluate a&1 and move to R0
  sljit_emit_op2(C, SLJIT_AND, SLJIT_R0, 0, SLJIT_S0, 0, SLJIT_IMM, 1);
  // compare R0 with 0 - if true jump to ret_c
  ret_c = sljit_emit_cmp(C, SLJIT_EQUAL, SLJIT_R0, 0, SLJIT_IMM, 0);

  // R0 = b
  sljit_emit_op1(C, SLJIT_MOV, SLJIT_RETURN_REG, 0, SLJIT_S1, 0);

  // jump to out
  out = sljit_emit_jump(C, SLJIT_JUMP);

  // label ret_c:
  sljit_set_label(ret_c, sljit_emit_label(C));
  // R0 = c
  sljit_emit_op1(C, SLJIT_MOV, SLJIT_RETURN_REG, 0, SLJIT_S2, 0);

  // label out:
  sljit_set_label(out, sljit_emit_label(C));

  // end of function
  sljit_emit_return(C, SLJIT_MOV, SLJIT_RETURN_REG, 0);

  code = sljit_generate_code(C);
  len = sljit_get_generated_code_size(C);

  std::function<long(long,long,long)> func = (func3_t)code;
  long res = func(a,b,c);
  std::cout << "func return " << res << std::endl;

#ifdef DUMP_SLJIT_CODE
  dump_code(code, len);
#endif // DUMP_SLJIT_CODE

  sljit_free_compiler(C);
  sljit_free_code(code, nullptr);
  return 0;
}

int main()
{
  return branch(4, 5, 6);
}
