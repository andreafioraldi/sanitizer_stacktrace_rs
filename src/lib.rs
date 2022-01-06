use core::ffi::c_void;

pub const STACK_TRACE_MAX: u32 = 255;

type BufferedStackTrace = c_void;

extern "C" {
    fn RUST_SANITIZER_STACKTRACE_get_pc_bp(pc: *mut usize, bp: *mut usize);
    fn RUST_SANITIZER_STACKTRACE_get_previous_pc(pc: usize) -> usize;
    fn RUST_SANITIZER_STACKTRACE_BufferedStackTrace_new() -> *mut BufferedStackTrace;
    fn RUST_SANITIZER_STACKTRACE_BufferedStackTrace_unwind(
        _self: *mut BufferedStackTrace,
        max_depth: u32,
        pc: usize,
        bp: usize,
        context: *const c_void,
        stack_top: usize,
        stack_bottom: usize,
        request_fast_unwind: bool,
    );
    fn RUST_SANITIZER_STACKTRACE_BufferedStackTrace_trace(
        _self: *const BufferedStackTrace,
    ) -> *const usize;
    fn RUST_SANITIZER_STACKTRACE_BufferedStackTrace_size(_self: *const BufferedStackTrace)
        -> usize;
}

pub fn previous_pc(pc: usize) -> usize {
    unsafe { RUST_SANITIZER_STACKTRACE_get_previous_pc(pc) }
}

pub struct Stacktrace {
    ptr: *mut BufferedStackTrace,
}

impl Stacktrace {
    pub fn new() -> Self {
        Self {
            ptr: unsafe { RUST_SANITIZER_STACKTRACE_BufferedStackTrace_new() },
        }
    }

    pub fn unwind_from(&mut self, max_size: u32, pc: usize, bp: usize, context: *const c_void) {
        assert!(max_size <= STACK_TRACE_MAX);
        unsafe {
            RUST_SANITIZER_STACKTRACE_BufferedStackTrace_unwind(
                self.ptr, max_size, pc, bp, context, 0, 0, false,
            );
        }
    }

    pub fn unwind(&mut self, max_size: u32) {
        unsafe {
            let mut pc = 0;
            let mut bp = 0;
            RUST_SANITIZER_STACKTRACE_get_pc_bp(&mut pc, &mut bp);

            self.unwind_from(max_size, pc, bp, core::ptr::null());
        }
    }

    pub fn trace(&self) -> &[usize] {
        unsafe {
            core::slice::from_raw_parts(
                RUST_SANITIZER_STACKTRACE_BufferedStackTrace_trace(self.ptr),
                RUST_SANITIZER_STACKTRACE_BufferedStackTrace_size(self.ptr),
            )
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn c_works() {
        unsafe {
            let mut pc = 0;
            let mut bp = 0;
            RUST_SANITIZER_STACKTRACE_get_pc_bp(&mut pc, &mut bp);

            let st = RUST_SANITIZER_STACKTRACE_BufferedStackTrace_new();
            RUST_SANITIZER_STACKTRACE_BufferedStackTrace_unwind(
                st,
                STACK_TRACE_MAX,
                pc,
                bp,
                core::ptr::null(),
                0,
                0,
                false,
            );
            let trace = core::slice::from_raw_parts(
                RUST_SANITIZER_STACKTRACE_BufferedStackTrace_trace(st),
                RUST_SANITIZER_STACKTRACE_BufferedStackTrace_size(st),
            );
            for t in trace {
                println!("{:#x}", previous_pc(*t));
            }
            assert!(trace.len() > 0);
        }
    }

    #[test]
    fn rust_works() {
        let mut st = Stacktrace::new();
        st.unwind(STACK_TRACE_MAX);
        for t in st.trace() {
            println!("{:#x}", previous_pc(*t));
        }
        assert!(st.trace().len() > 0);
    }
}
