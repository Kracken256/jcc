
namespace examples {
    /// @brief Packed structure with properties
    /// @note Members may have attributes. 
    @attribute('packed')
    struct my_bitfield {
        @attribute('le') # use little endian
        let f1: u32:19; # 19 bit field 1
        
        @attribute('le') # use little endian
        let f2: u32:9;  # 9 bit field 2

        @attribute('be') # use big endian
        let f3: u32:4;  /* 4 bit field 3 */

        // Common methods for stdlib support
        overload(std::to_string, (this) => @fmt.packer(this, base=16, sep=''))

        # Member count
        overload(std::count, ()=>3) 
    }

    struct ScreenPosition {
        x: u32 = 1
        y: u32 = 1
    }

    namespace meta::world {
        struct Color {
            r: u8 = 0
            g: u8 = 0
            b: u8 = 0
            a: u8 = 0xff
        }

        struct Point {
            i: i32 = 0
            j: i32 = 0
            k: i32 = 0
        }

        struct Line {
            a: Point
            b: Point
        }

        struct SolidCube {
            x: Point
            n: i32 = 0 
            color: Color = {0, 0, 0xff, 0xff}
        }

        class Construct {
            virtual func is_renderable() => false
            virtual func objsize() -> u32 { return sizeof(*this) }
        }

        class SolidConstruct: Construct {
            virtual func dim() -> group[3, u32] { return {0, 0, 0}}
        }

        class CubeSolidConstruct: SolidConstruct {
            CubeSolidConstruct(shape: SolidCube): m_cube(std::getref(shape))

            overload func dim() -> group[3, u32] { return {m_cube.n, m_cube.n, m_cube.n}}
            overload func is_renderable() => true

        protected:
            m_cube: cref SolidCube;
        }

        interface IContextGeneric {
            func typestr() -> string
            func set(item: any, value:any) -> bool
            func get(item: any) -> (value:any, bool)
        }

        class MetaContext: Object, IContextGeneric
            MetaContext(children: list[cref [Construct]]) {
                m_children = children
            }

            virtual func render2fb(framebuffer: list[Color, %, %]) {
                foreach (child in m_children) {
                    child.render2fb(framebuffer)
                }
            }

            func typestr() => "MetaContext"

            func set(item:string,value:string) {
                props[item] = value

                return true
            }

            func get(item:string) {
                return props.has(item) ? (props[item], true), (nil, false)
            }

        protected:
            m_children: cref [Construct]
            props: map[string,string]
        }

        /// ... yada yada
        /*
            * J++ is somewhere between C++,Swift,GoLang & TypeScript
        */
    }


    /// @brief Send binary data over the 'example' knet protocol
    /// @param buf Binary data to send
    /// @param prot Flags for peer. 'example' knet protocol
    /// @return true if success, else false
    /// @statics message The data sent
    func knet_write(buf: const byte[], prot: u32) -> bool {
        let message: static string

        message.clear()

        # Yep. We have anon functions like JavaScript

        message << '{"content": [' << string::from_array(buf, (b: byte, i: int) => {
            return xcode::hex(b, 2) + (i ? ', ' : '')
        }) << '], "prop":' << prot << '}'

        let client: infer = @net[tcp].connect("23.23.23.23", 80)

        // Works just like a GoLang defer.
        // Execute after the function returns.
        defer client.dispose() # Disconnect

        return client.send(message.raw())
    }
}

# Lets map the namespace examples to the primary space
namemap examples -> .

# Use standand library version 9
namemap std9 -> std

/// @brief Entry point
/// @param argv list of program arguments
/// @param env Map of environment variable
/// @return returns native integer status code
func main(argv: const list[string], env: const map[string, string]) -> nint {
    # Make buffer const
    let buffer: const byte[] = {0x90, 0x12, 0x78, 0x45, 0x00, 0x01, 0b10010011}

    if not knet_write(buffer, 0x89671290) { // extra paranthesis are optional
        @io[console].log("Unable to send buffer to knet server", level=Error, fatal=true)
        return 1
    }

    let encoded: string = [knet_write].message; # Static member accessor

    # Auto resolve io.console name
    /*
    '@' is autoresolve operator here
    'io' is the collection (big namespace)
    'console' is the bucket (related classes and things)
    'println' is a function

    // full expansion
    std::io::console::println(encoded)
    */
    @io[console].println(encoded)

    // by default all builtin types are initialized to 0x0 in memory.
    let bitf: my_bitfield! // Don't set default values of memebers. Could be tiny Optimization

    bitf.f1 = 90
    bitf.f2 = 89
    bitf.f3 = 9

    @io[console].println(bitf) # Will invoke the overloaded to_string

    // we have a nice standard library
    @io[console].asnidraw("J++ is great for getting things done! Lets get started!")

    // sstring is a builtin type
    // the memeber is automatically cleared (and freed) after the variable goes out of scope
    // so any secrets will be 0x0... in hexdump of program member after destruction
    sstring my_secret = env["SECRET_AES_KEY"]

    let cryptor: infer = @crypto[cipher][aes].ctr_cryptor()

    # Setup
    cryptor.setup(my_secret, ivmode="rand-append", kdf='pbkdf2', kdfoptions=['iter':10000])
    // cryptor.build() # run expensive operations

    # Build is run automatically if not ready
    cryptor.encrypt(&bitf, &bitf); # Encrypt the bitf struct abd write back the result

    @io[console].ansidraw('All Hail AES', colors='auto')

    @io[console].println(bitf)

    return 0
}