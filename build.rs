// build.rs

use std::env;

fn main() {
    let out_dir = env::var_os("OUT_DIR").unwrap();
    let out_dir = out_dir.to_string_lossy().to_string();

    cc::Build::new()
        .cpp(true)
        .file("cpp/sanitizer_common.cpp")
        .file("cpp/sanitizer_linux.cpp")
        .file("cpp/sanitizer_mac.cpp")
        .file("cpp/sanitizer_stacktrace.cpp")
        .file("cpp/sanitizer_stacktrace_libcdep.cpp")
        .file("cpp/sanitizer_unwind_linux_libcdep.cpp")
        .file("cpp/sanitizer_win.cpp")
        .file("cpp/sanitizer_unwind_win.cpp")
        .file("cpp/bindings.cpp")
        .compile("sanitizer_stacktrace_cpp");

    println!("cargo:rustc-link-search=native={}", &out_dir);

    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-changed=cpp/sanitizer_common.cpp");
    println!("cargo:rerun-if-changed=cpp/sanitizer_linux.cpp");
    println!("cargo:rerun-if-changed=cpp/sanitizer_mac.cpp");
    println!("cargo:rerun-if-changed=cpp/sanitizer_stacktrace.cpp");
    println!("cargo:rerun-if-changed=cpp/sanitizer_stacktrace_libcdep.cpp");
    println!("cargo:rerun-if-changed=cpp/sanitizer_unwind_linux_libcdep.cpp");
    println!("cargo:rerun-if-changed=cpp/sanitizer_win.cpp");
    println!("cargo:rerun-if-changed=cpp/sanitizer_unwind_win.cpp");
    println!("cargo:rerun-if-changed=cpp/bindings.cpp");
}
