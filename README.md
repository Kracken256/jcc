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

J++ has a more modern syntax. Here are some examples of what the language will look like:

```c++
struct Point {
    x: int = 0
    y: int = 0
}
```

```c++
// Create a struct with attributes for runtime reflection.
// Instead of namespaces, J++ uses subsystems
// A subsystem is a namespace with optional 'tags' that represent dependencies.
// This allows for creating directed-acyclic-graphs of source code automatically.
// And helps plan out changes to the codebase without breaking things.
subsystem Std::Types: Std::Core::Exception {
    struct uuid_t {
        @:serial_format('hex')
        @:endian('big')
        @:serial_suffix('-')
        a: dword = 0

        @:serial_format('hex')
        @:endian('big')
        @:serial_suffix('-')
        b: word = 0

        @:serial_format('hex')
        @:endian('big')
        @:serial_suffix('-')
        c: word = 0

        @:serial_format('hex')
        @:endian('big')
        @:serial_suffix('-')
        d: word = 0

        @:serial_format('hex')
        @:endian('big')
        e: dword = 0
        @:serial_format('hex')
        @:endian('big')
        f: word = 0;

        new: (str:string): uuid_t {
            let uuid: uuid_t = uuid_t()
            uuid.parse(str)
            return uuid
        }

        new: (a: dword, b: word, c: word, d: word, e: dword, f: word): uuid_t {
            return uuid_t(a: a, b: b, c: c, d: d, e: e, f: f)
        }

        parse: (str: string): void {
            let parts: string[] = str.split('-')
            if (parts.length != 5) {
                throw new Exception("Invalid UUID format")
            }

            a = parts[0].hex('big')
            b = parts[1].hex('big')
            c = parts[2].hex('big')
            d = parts[3].hex('big')
            e = parts[4].hex('big')
        }

        raw: (endian: string = 'big'): byte[] {
            let bytes: byte[] = byte[16]
            bytes[0:4] = a.raw(endian)
            bytes[4:6] = b.raw(endian)
            bytes[6:8] = c.raw(endian)
            bytes[8:10] = d.raw(endian)
            bytes[10:16] = e.raw(endian)
            return bytes
        }

        toString: (): string {
            return a.hex('big') + '-' + b.hex('big') + '-' + c.hex('big') + '-' + d.hex('big') + '-' + e.hex('big')
        }
    }
}
```

```c++
import stdio as . // import all exported contents into global scope

func add(a: int, b: int = 80): int {
    return a + b
}

func main() {
    let myAnonymousFunc = (a: int): int {
        let b: int = 10
        return add(a, b)
    }

    let result: int = myAnonymousFunc(20)

    print(result)
}
```

More:

```c++
import stdio as io // import a module and give it an alias

@:require(!!name) // Require that the name parameter is not null or empty
// Define a cast map for exporting this function to C
@:export_cast_map('[string, const char*(($).raw()), string(*)], [uuid_t, byte[16](($).raw()), uuid_t(a:*[0:4], b:*[4:6], c:*[6:8], d:*[8:10], e:*[10:16])]') 
export "C" func uuid5_new(name: string, namespace: uuid_t): uuid_t {
    import crypto // import a module into the current scope

    // The crypto module is mananged and there is no need to free it.
    let hash: string = crypto.sha1(name.raw('big') + namespace.raw('big'))
    
    return uuid_t(hash[0:4].hex('big'), hash[4:6].hex('big'), hash[6:8].hex('big'), hash[8:10].hex('big'), hash[10:16].hex('big'))
}

func main() {
    let uuid: uuid_t = uuid5_new("Hello World", uuid_t::new("6ba7b810-9dad-11d1-80b4-00c04fd430c8"))
    io.print(uuid)

    // code is fun
}
```

---

This project is still is rapid development and is not ready for use yet. I will update this repo with more information as I make progress.