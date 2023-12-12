#[VERSION_MIN "1.0"]
#[VERSION_MAX "1.0"]
#[VERSION_RECOMMENDED "1.0"]

#[PROJECT_NAME "Language J"]
#[PROJECT_DATE "2023-12-11"]

#include <std/math>
#include <std/types>
#include <std/containers>
#include <std/io/console>

namemap std::math m;
namemap std::container ::;

namespace examples
{
    /// @brief A meta class can modify the behavior a codebase
    /// @note This is a comment
    export metaclass sealed MetaClass
    {
#[ORG_UNIT_TESTING_STATUS "Not tested"]
        export static vec<LexTok> meta(vec<LexTok> tokens)
        {
            // replace all instances of "Hello, world!" with "Hello, J!"
            for (uintn i = 0; i < tokens.size(); i++)
            {
                if (tokens[i].type == LexTok::STRING && tokens[i].value == "Hello, world!")
                {
                    tokens[i].value = "Hello, J!";
                };
            };
            return tokens;
        };
    };

    class class_a
    {
    public
        default constructor();
    public
        static void hello();
    };

    class class_b
    {
    public
        default constructor();
    private
        static void world();
    };

    class class_c : class_a, class_b
    {
    public
        default constructor();

    public
        hello();
        // Claim a private symbol from class_b
    public
        claim world();
    }

    // Change you mind? You don't want to make a symbol global?

    seal class MetaClass;

    // unseal it
    unseal class MetaClass;

    // Instead of using strange types for 'opaque' objects. Just call it a `thing`.
    thing allocate_slot(str req, uuid_t uuid)
    {
        b128_t id; // J-Lang likes bitfields

        return id;
    }

    metatype cstr
    {
    public
        constructor(const char *str)
        {
            // this is a preprocessing step

            this.ephemeral.addrsize = meta.build.arch.addrsize;

            // allocate a slot for the string
            this.storage.alloc(0, addrsize);
        }

        cstr operator=(const char *str)
        {
            // this will be done at compile time
            // no runtime overhead

            this.storage.move(0, str, this.ephemeral.addrsize);
        }
    }
};

// we export a constant c-string symbol using the C ABI. The JString is implicitly converted to a c-string using metatypes!! (no runtime overhead)
// programming is awesome!

#[ABI "C"]
export const cstr VERSION_MIN = "1.0";

void main(vec<str> args, map<str, str> env)
{
    auto x, y, z, w;
    bitn z, w;

    x = 0b10101010;
    y = 0o9010;

    z = x & y;
    w = x | y;

    MetaClass::meta(
        // We can write code and stuff
        // our class will modify it

        str s = "Hello, world!";
        std::io::console::printf("%s\n", s););

    z <<= 1;
    w >>= 2;

    std::io::console::printf("x = %d, y = %d\n", x, y);
    std::io::console::printf("z = %d, w = %d\n", z, w);
}