struct FVec(N: int) {
    x: float[N];

    dot: (b: FVec<N>): float;
    norm: ();
    mag: (): float;
    sum(): float;

    static zeros: FVec<N>;
    static ones: FVec<N>;
    static new: (x: FVec<N>): FVec<N>;
    static new: (x: float[N]): FVec<N>;
    static new: (...[x{N}: float]): FVec<N>;
}

export gMyGlobal: FVec4 = FVec4::new(1, 2, 3, 4);

func KernelInitialize<ARCH_NAME: {X86, AMD64}>(): bool {
    return true;
}
; implicitly created by compiler
func KernelInitializeX86(): bool {
    return KernelInitialize<KernelInitialize::ARCH_NAME::X86>();
}
func KernelInitializeAMD64(): bool {
    return KernelInitialize<KernelInitialize::ARCH_NAME::AMD64>();
}

; can be called as
KernelInitializeAMD64();
KernelInitializeX86();

enum syscall_name {
    read = 0,
    write = 1,
    open = 2,
    close = 3,
}

func sys_<name: syscall_name><bits: [,64]>(...[args: qword]): qword {
    abortif bits != 64;
    let out: qword;

    asm (
        mov rax, <name>;
        mov rdi, <args[0]>;
        mov rsi, <args[1]>;
        mov rdx, <args[2]>;
        syscall;
        mov <out>, rax;
    );

    return out;
}

sys_read(0, 0, 0);
sys_write(0, 0, 0);
sys_open(0, 0, 0);
sys_close(0);