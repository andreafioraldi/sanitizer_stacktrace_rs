# sanitizer_stacktrace_rs
A Rust crate to generate backtraces using the LLVM codebase

## Example

You can get a trace of raw addresses from the current context using

```rust
use sanitizer_stacktrace::*;

let mut st = Stacktrace::new();
st.unwind(STACK_TRACE_MAX);
for addr in st.trace() {
    println!("{:#x}", previous_pc(*addr));
}
```

To get a symbolized trace, use `symbolize`

```rust
use sanitizer_stacktrace::*;

let mut st = Stacktrace::new();
st.unwind(STACK_TRACE_MAX);
st.symbolize(|addr, sym| {
    println!(
        "0x{:x}\t{}",
        addr,
        sym.name().map(|x| x.as_str().unwrap()).unwrap_or("")
    );
});
```

You can also generate a stacktrace startign from specific values of PC and BP, like in a signal handler

```rust
pub unsafe fn signal_handler(_sig: c_int, _info: siginfo_t, ctx: *mut ucontext_t) {
    let mut st = Stacktrace::new();
    st.unwind_from(
        STACK_TRACE_MAX,
        a.uc_mcontext.gregs[REG_RIP as usize] as usize,
        a.uc_mcontext.gregs[REG_RBP as usize] as usize,
        con as *const c_void,
    );
    for addr in st.trace() {
        println!("{:#x}", previous_pc(*addr));
    }

    std::process::exit(1);
}
```
