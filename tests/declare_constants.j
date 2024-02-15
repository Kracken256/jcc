subsystem Math {
    subsystem Constants {
        const PI: double = 3.14159265358979323846;
        const E: double = 2.71828182845904523536;

        struct CReal {
            real: double = 0;
            imag: double = 0;
        };

        const I: CReal;
    }
}
