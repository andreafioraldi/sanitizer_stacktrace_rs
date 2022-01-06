#include "sanitizer_stacktrace.h"

using namespace __sanitizer;

extern "C" void RUST_SANITIZER_STACKTRACE_get_pc_bp(uptr* pc_out, uptr* bp_out) {
    GET_CALLER_PC_BP
    *pc_out = pc;
    *bp_out = bp;
}

extern "C" uptr RUST_SANITIZER_STACKTRACE_get_previous_pc(uptr pc) {
    return StackTrace::GetPreviousInstructionPc(pc);
}

extern "C" BufferedStackTrace* RUST_SANITIZER_STACKTRACE_BufferedStackTrace_new() {
    return new BufferedStackTrace();
}

extern "C" void RUST_SANITIZER_STACKTRACE_BufferedStackTrace_unwind(BufferedStackTrace* self, u32 max_depth, uptr pc, uptr bp, void *context, uptr stack_top, uptr stack_bottom, bool request_fast_unwind) {
    self->Unwind(max_depth, pc, bp, context, stack_top, stack_bottom, request_fast_unwind);
}

extern "C" const uptr* RUST_SANITIZER_STACKTRACE_BufferedStackTrace_trace(BufferedStackTrace* self) {
    return self->trace;
}

extern "C" size_t RUST_SANITIZER_STACKTRACE_BufferedStackTrace_size(BufferedStackTrace* self) {
    return self->size;
}
