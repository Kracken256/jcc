; This is a comment
# This is also a comment
// This is also a comment
/* There are a lot of comments */

;! This is a TODO
## This is a known bug
;, This is just a silly remark
; just a plain comment

;, So what is the deal, huh?
;! Implement Kernel Memory Allocator
## Segmentation Fault on line 42, when gEnTTable.psreg == 0x0
## Integer Overflow on line 69, when gEnTTable.rsiBe > 0x7fffffff && xInc > 0x7fffffff-1
;! Start doing fuzzing

; Naming mangled template types. Template arguments are part of the TypeName
;  ex:
;  struct Point{N: int} {
;      x: int[N]
;  }
;  Point1 for 1 dim, Point2 for 2 dim, Point999999 for 999999 dim, etc.
; No more C++ strange syntax.
; ex2:
;  struct Tensor{D: int[]}[tname: type] {
;      @:optimize("native-aligned, deep-cache")
;      @:default("off")
;      ;
;      ; '@' = 'parrent namespace'
;      ; '\'' = literal
;      ; '##' = 'concat'
;      ; ''a',b' = 'expend argument b with separator a'
;      ; 'D' = 'template argument'
;      ; 'tname' = 'template argument'

; metacode: evalulate following statements (until next expression) if expression is true.

;#[statics.platform == "mainline"]
;#[TENSOR_PLATFORM_MAINLINE == 1]
;      @:mangle("@'::Tensor'##'S4'##'_',D##'_'##tname")
;#[statics.platform != "mainline"]
;#[error "Unsupported platform"]
;
;      x: tname D[..D[]]
;  } 
; Usage:
; let c: Tensor3_2_4[Point4] ; Quantum physics math library could use this
; const s = sizeof(c)

subsystem App {
    struct Color {
        r: long = 0
        g: long = 0
        b: long = 0
        a: int = 0xff
    }

    struct Point {
        x: int = 0
        y: int = 0
        z: int = 0
    }

    struct Line {
        start: Point
        end: Point
        color: Color
    }

    struct Rect {
        tl: Point
        br: Point
        color: Color
    }

    struct Circle {
        center: Point
        r: int = 0
        color: Color
    }

    struct Text {
        tl: Point
        text: string = "Your \ntext here"
        color: Color
    }
}

subsystem Math {
    subsystem LinearAlgebra {
        struct VectorF{D: int} {
            x: float[D]

            dot: const (b: VectorF{D}): float {
                var result: float = 0.0
                for i in 0..D {
                    result += x[i!] * b.x[i!]
                }
                return result
            }

            norm: () {
                let mag: float = 0.0
                for i in 0..D {
                    mag += x[i!] * x[i!]
                }
                
                retz msg // return if mag == 0

                mag = sqrt(mag)

                for i in 0..D {
                    x[i!] /= mag
                }
            }
        }

        struct VectorI{D: int} {
            x: int[D]
        }

        struct MatrixF{R: int}_{C: int} {
            x: float[R][C]
        }

        struct MatrixI{R: int}_{C: int} {
            x: int[R][C]
        }
    }
}

import Math.LinearAlgebra as .

func Main(args: string[]): int {
    let a: MatrixF4_4

    ; return value, operation, do-if-failed:
    ;  C:
    ;  if (operation != 0) {
    ;      do-if-failed
    ;      return value
    ;  }
    retnz 1, step_1(), print("step 1 failed")
    retnz 2, step_2(), print("step 2 failed")
    retnz 3, step_3(), print("step 3 failed")
    retnz 4, step_4(), print("step 4 failed")
    retnz 5, step_5(), print("step 5 failed")

    return 0
}