#include <iostream>
#include <functional>

#include "sljitLir.h"

long ackermann(long m, long n)
{
  if(m==0)
    return n+1;
  else if(n==0)
    return ackermann(m-1,1);
  else
    return ackermann(m-1,ackermann(m,n-1));
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

long generate_ackermann(long m, long n)
{
  void* code;
  unsigned long len;
  typedef long (SLJIT_FUNC *func2_t)(long m, long n);

  struct sljit_compiler *C = sljit_create_compiler(nullptr, nullptr);
	struct sljit_label *entry;
  struct sljit_jump *jump;
  struct sljit_jump *L2;
  struct sljit_jump *L3;
  struct sljit_jump *L4;

  // >>>
  entry = sljit_emit_label(C);
  // start context, here function entry
  sljit_emit_enter(C, 0, SLJIT_ARG1(SW)|SLJIT_ARG2(SW), 3, 2, 0, 0, 0);

  L2 = sljit_emit_cmp(C, SLJIT_NOT_EQUAL, SLJIT_S0, 0, SLJIT_IMM, 0);
  sljit_emit_op2(C, SLJIT_ADD, SLJIT_RETURN_REG, 0, SLJIT_S1, 0, SLJIT_IMM, 1);
  L3 = sljit_emit_jump(C, SLJIT_JUMP);

  // label L2:
  sljit_set_label(L2, sljit_emit_label(C));
  L4 = sljit_emit_cmp(C, SLJIT_NOT_EQUAL, SLJIT_S1, 0, SLJIT_IMM, 0);
  sljit_emit_op2(C, SLJIT_SUB, SLJIT_R0, 0, SLJIT_S0, 0, SLJIT_IMM, 1);
  sljit_emit_op1(C, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, 1);
  jump = sljit_emit_call(C, SLJIT_CALL, SLJIT_RET(SW)|SLJIT_ARG1(SW)|SLJIT_ARG2(SW));
  sljit_set_label(jump, entry);
  sljit_emit_return(C, SLJIT_MOV, SLJIT_RETURN_REG, 0); // <<<

  // label L4:
  sljit_set_label(L4, sljit_emit_label(C));
  sljit_emit_op1(C, SLJIT_MOV, SLJIT_R0, 0, SLJIT_S0, 0);
  sljit_emit_op2(C, SLJIT_SUB, SLJIT_R1, 0, SLJIT_S1, 0, SLJIT_IMM, 1);
  jump = sljit_emit_call(C, SLJIT_CALL, SLJIT_RET(SW)|SLJIT_ARG1(SW)|SLJIT_ARG2(SW));
  sljit_set_label(jump, entry);
  sljit_emit_op1(C, SLJIT_MOV, SLJIT_R1, 0, SLJIT_RETURN_REG, 0); // <<<
  sljit_emit_op2(C, SLJIT_SUB, SLJIT_R0, 0, SLJIT_S0, 0, SLJIT_IMM, 1);
  jump = sljit_emit_call(C, SLJIT_CALL, SLJIT_RET(SW)|SLJIT_ARG1(SW)|SLJIT_ARG2(SW));
  sljit_set_label(jump, entry);

  // label L3:
  sljit_set_label(L3, sljit_emit_label(C));
  // end of function
  sljit_emit_return(C, SLJIT_MOV, SLJIT_RETURN_REG, 0);
  // <<<

  code = sljit_generate_code(C);
  len = sljit_get_generated_code_size(C);

  std::function<long(long,long)> func = (func2_t)code;
  long res = func(m,n);
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
  generate_ackermann(0,3);
  std::cout << "Ack(" << 0 << "," << 3 << ") = " << ackermann(0,3) << std::endl;
  generate_ackermann(3,0);
  std::cout << "Ack(" << 3 << "," << 0 << ") = " << ackermann(3,0) << std::endl;
  generate_ackermann(3,4);
  std::cout << "Ack(" << 3 << "," << 4 << ") = " << ackermann(3,4) << std::endl;
  return 0;
}
