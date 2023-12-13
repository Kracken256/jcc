#[PROJECT_NAME "Basic code"]
#[PROJECT_DESCRIPTION "Basic code for a J project"]
#[PROJECT_AUTHOR "Wesley Jones"]
#[PROJECT_LICENSE "MIT"]

// export global infer myappname_stuff = "stuff";

namespace runtime
{
    metaclass engine{
        template <meta code[]>
        export void run(){
            @console.log("Running code: {{code}}");
    return;
}
}
;
}

plug void fn_swap_a(void)
{
    // Implementation A
    return;
}

plug void fn_swap_b(void)
{
    // Implementation B
    return;
}

metafunction fn_swap(fn_swap_a, fn_swap_b)
{
    // define runtime condition to choose between fn_swap_a and fn_swap_b
    // when the program is run the text section will be modified. The function will be
    // 'hollowed out' and replaced with a jump to the correct implementation.
    // This achieves true metaprogramming.

    // use the non-zero operator to pick a random implementation
    // and bind it to the name fn_swap
    random.get(0, 1) ? ? () fn_swap_a : () fn_swap_b;
}

namespace app
{
    export void main()
    {
        // prefix @ means automatically resolve imports and namespaces.
        // No need to use the fully qualified name or deal with include files.
        @console.log("Hello, world!. This is a J++ program.");
        @console.log("The PROJECT_NAME is: {{PROJECT_NAME}}");

        // Metamorphosis is a feature that allows you to write god-hacker-tier self-modifying code.
        @runtime::engine::run<"console.log('Hello, world!');">();

        // catch exceptions
        try
        {
        }
        // exceptions types are implicitly caught.
        catch (e : @Exception)
        {
            switch (e.Type)
            {
            case @RuntimeIOException:
                @console.log("Runtime IO exception: {{e.Message}}");
                break;
            case @RuntimeMemoryException:
                @console.log("Runtime memory exception: {{e.Message}}");
                break;
            default:
                raise fault("Exception not handled")
            }
        }
        return;
    }
}