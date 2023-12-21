# J++ Compiler development

I wanted to build a compiler/progamming language for a long time. I have tried to find online videos/blogs/tutorials on how to do it, but they are virtually non-existent. The best I could find was on a fizzbuzz like language, which was not very helpful. I wanted a language that was actually useful and powerful. So I decided to just roll my own from scratch.

I present the J++ programming language.

This compiler has a custom Lexer, Parser, Code Generator, and Preprocessor all written in pure C++. 

The target features of this new language are:
- Functions
- Variables
- static typing
- builtin arbitrary precision integers
- builtin `tensor` type (like numpy arrays)
- Global variables
- namespace support
- Imports/Includes
- structs (packed and unpacked)
- classes
- inheritance
- generics
- Memory safety. (Only allow references no raw pointers. Bounds checking on arrays)
- Garbage collection implemented in the standard library (which I will write in the new language)
- C++ interop or at least C interop
- Anonymouse functions
- turing complete preprocessor that is actually useful. (like more then just string concatenation)
- operator overloading
- operator precedence
- type inference
- control flow and builtin asyncronous programming
- Beautiful syntax that looks elegant and is easy to read
- Powerful enough to get things done

J++ is very similar to C++ in features and was greatly inspired by it. J++ compiles directly to C++ code and enjoys the performance benefits of C++ and existing C++ compilers. This also means that virtually any platform or architecture that C++ compiles to, J++ can compile to as well. This also means that J++ can interop with C++ code and libraries.

J++ has a more modern syntax. Here are some examples:

Define a struct:
There is no need for semi-colons.

```c++
struct Point {
    x: int
    y: int
}
```

Define a class:
Constructors and destructors are defined with the `constructor` and `destructor` keywords, respectively.
Exported members/methods and constructors are public. Non-exported items are private.
This class inherits from the `IDisposable` interface.

```c++
namespace net {
    class client : IDisposable {
        constructor(target: string, port?: int, timeout?: int, protocol?: string) {
            // ...
        }

        destructor() {
            net.close()
        }

        export func write(data: buffer) -> nint {
            // ...
        }

        export func read(data: buffer) -> nint {
            // ...
        }

        export func close() {
            // ...
        }

        socket: dword
    }
}
```

Define a function:
Functions can be made anonymous like in javascript. The `func` keyword is required for named functions.

```c++
import stdio

func add(a: int, b: int) -> int {
    return a + b
}

func main() {
    let myAnonymousFunc = ():int => {
        let a: int = 5
        let b: int = 10
        return add(a, b)
    }

    let result: int = myAnonymousFunc()

    print(result)
}
```


---

This project is still is rapid development and is not ready for use yet. I will update this repo with more information as I make progress.