
namespace App {
    struct Color {
        r: long = 0
        g: long = 0
        b: long:34 = 0
        a: int = 0xff
    }

    struct Point {
        x: int = 0
        y: int = 0
    }

    struct Line {
        start: Point
        end: Point
        color: Color
    }

    struct Rect {
        x: int = 0
        y: int = 0
        w: int = 0
        h: int = 0
        color: Color
    }

    struct Circle {
        x: int = 0
        y: int = 0
        r: int = 0
        color: Color
    }

    struct Text {
        x: int = 0
        y: int = 0
        text: string = "Your text here"
        color: Color
    }
}