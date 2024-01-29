
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

func Main(args: string[]): void {
}