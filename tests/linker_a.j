struct struct_A {
    a: int
}

namespace ns_1 {
    struct struct_B {
        b: int
    }
}

namespace ns_2 {
    struct struct_C {
        a: struct_A
        b: ns_1::struct_B
    }

    namespace ns_3 {
        struct struct_D {
            a: struct_C
            b: struct_A
            c: ns_1::struct_B
        }
    
    }
}


func hello (a:float = 2.491847018e10.23) {

}