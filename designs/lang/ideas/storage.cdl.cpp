#[VERSION_MIN "1.0"]
#[VERSION_MAX "1.0"]
#[VERSION_RECOMMENDED "1.0"]

#[PROJECT_NAME "Language CDL"]
#[PROJECT_DATE "2023-12-11"]

#include <statics/math>
#include <boolean>

namespace storage::volatile
{
    /// @brief SR latch
    #[EXPORT_ABI "C"]
    #[CC_OPTIMIZE 0]
    export module srlatch(set:line, en:line, reset:line)(q:line, qn:line)
    {
        tube S1, R1;

        on(reset)
        {
            S1 = 0;
            R1 = 1;
        }

        and(S1, en, set);
        and(R1, en, reset);
        nor(out, S1, q);
        nor(q, R1, qn);
    }

    @ /// @brief {{N}}-bit register
        export module template <int N>
        r(set : line<N>, en : line, reset : line)(dat : line<N>)
    {
        srlatch bits[N];

        on(reset)
        {
            for (i, 0, N)
            {
                bits[i].reset = 1;
            }
        }

        on(en)
        {
            for (i, 0, N)
            {
                bits[i].en = 1;
            }

            for (i, 0, N)
            {
                bits[i].set = set[i];
            }
        }

        for (i, 0, N)
        {
            dat[i] = bits[i].q;
        }
    }

    // lets define 512 types of registers for sizes 1-512 inclusive
    metafor(i, 1, 512)
    {
        @ /// @brief {{i}}-bit register primitive
            typedef r<{{i}}>
                r{{i}};
    }

    @ /// @brief {{N}}x{{B}} SRAM
        export module template <int N, int W>
        static_ram_generic(datin : line<W>, addr : line{{statics.math.log2(N)}}, writable : line, enable : line, reset : line)(dat : line<N>) : constraints(N > 0 && W > 0 && statics.math.ispow2(N))
    {
        r<W> regs[N];

        on(reset)
        {
            for (i, 0, N)
            {
                regs[i].reset = 1;
            }
        }

        on(enable)
        {
            regs[addr].en = 1;

            on(writable)
            {
                regs[addr].set = datin;
            }

            dat = regs[addr].dat;
        }
    }
}
