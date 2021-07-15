#include <iostream>
#include <functional>

#include "sljitLir.h"

int fibonacci(int n)
{
  // std::cout << "|input n = " << n << "|" << std::flush;
  if(n <= 2)
    return 1;
  else
    return fibonacci(n-1) + fibonacci(n-2);
}

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

int generate_fibonacci(int n)
{
  // std::cout << "|input n = " << n << "|" << std::flush;
  void* code;
  unsigned long len;
  typedef int (SLJIT_FUNC *func1_t)(int n);

  struct sljit_compiler *C = sljit_create_compiler(nullptr, nullptr);
  struct sljit_label *entry;
  struct sljit_jump *end;
  struct sljit_jump *jump;
  struct sljit_jump *L2;
  struct sljit_jump *L3;

  // >>>
  entry = sljit_emit_label(C);
  // start context, here function entry
  sljit_emit_enter(C, 0, SLJIT_ARG1(SW), 3, 2, 0, 0, 0);

  L2 = sljit_emit_cmp(C, SLJIT_GREATER, SLJIT_S0, 0, SLJIT_IMM, 2);
  sljit_emit_op1(C, SLJIT_MOV, SLJIT_RETURN_REG, 0, SLJIT_IMM, 1);
  L3 = sljit_emit_jump(C, SLJIT_JUMP);

  // label L2:
  sljit_set_label(L2, sljit_emit_label(C));
  sljit_emit_op2(C, SLJIT_SUB, SLJIT_R0, 0, SLJIT_S0, 0, SLJIT_IMM, 1);
  jump = sljit_emit_call(C, SLJIT_CALL, SLJIT_RET(SW)|SLJIT_ARG1(SW));
  sljit_set_label(jump, entry);
  sljit_emit_op1(C, SLJIT_MOV, SLJIT_S1, 0, SLJIT_R0, 0);
  sljit_emit_op2(C, SLJIT_SUB, SLJIT_R0, 0, SLJIT_S0, 0, SLJIT_IMM, 2);
  jump = sljit_emit_call(C, SLJIT_CALL, SLJIT_RET(SW)|SLJIT_ARG1(SW));
  sljit_set_label(jump, entry);
  sljit_emit_op2(C, SLJIT_ADD, SLJIT_RETURN_REG, 0, SLJIT_RETURN_REG, 0, SLJIT_S1, 0);

  // label L3:
  sljit_set_label(L3, sljit_emit_label(C));
  // end of function
  sljit_emit_return(C, SLJIT_MOV, SLJIT_RETURN_REG, 0);

#if 0
  end = sljit_emit_cmp(C, SLJIT_LESS_EQUAL, SLJIT_S0, 0, SLJIT_IMM, 2);

  // sljit_emit_op2(C, SLJIT_ADD, SLJIT_R0, 0, SLJIT_S0, 0, SLJIT_IMM, 1);
  // sljit_emit_op1(C, SLJIT_MOV, SLJIT_RETURN_REG, 0, SLJIT_R0, 0);
  // TODO Marian: implement recursive call here
  sljit_emit_op2(C, SLJIT_SUB, SLJIT_R0, 0, SLJIT_S0, 0, SLJIT_IMM, 1);
  jump = sljit_emit_call(C, SLJIT_CALL, SLJIT_RET(SW)|SLJIT_ARG1(SW));
  sljit_set_label(jump, entry);
  sljit_emit_op1(C, SLJIT_MOV, SLJIT_R1, 0, SLJIT_RETURN_REG, 0);
  sljit_emit_op2(C, SLJIT_SUB, SLJIT_R0, 0, SLJIT_S0, 0, SLJIT_IMM, 2);
  jump = sljit_emit_call(C, SLJIT_CALL, SLJIT_RET(SW)|SLJIT_ARG1(SW));
  sljit_set_label(jump, entry);
  sljit_emit_op2(C, SLJIT_ADD, SLJIT_RETURN_REG, 0, SLJIT_RETURN_REG, 0, SLJIT_R1, 0);
  sljit_emit_return(C, SLJIT_MOV, SLJIT_RETURN_REG, 0);

  // label end:
  sljit_set_label(end, sljit_emit_label(C));
  sljit_emit_op1(C, SLJIT_MOV, SLJIT_RETURN_REG, 0, SLJIT_IMM, 1);
  // end of function
  sljit_emit_return(C, SLJIT_MOV, SLJIT_RETURN_REG, 0);
#endif
  // <<<

  code = sljit_generate_code(C);
  len = sljit_get_generated_code_size(C);

  std::function<int(int)> func = (func1_t)code;
  int res = func(n);
  std::cout << "func return " << res << std::endl;

#ifdef DUMP_SLJIT_CODE
  dump_code(code, len);
#endif // DUMP_SLJIT_CODE

  sljit_free_compiler(C);
  sljit_free_code(code, nullptr);
  return res;
}

int main()
{
  // generate_fibonacci(6);
  for(int n=0; n<11; n++)
  {
    std::cout << "---" << std::endl;
    generate_fibonacci(n);
    std::cout << "Fib[" << n << "] = " << fibonacci(n) << std::endl;
  }
  return 0;
}
