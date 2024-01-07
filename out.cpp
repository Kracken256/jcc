//==================================================================//
// Type: J++ Transpiled Code                                        //
// Date: Thu Apr 12 08:17:34 4461545 HST                               //
//==================================================================//

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <iostream>
#include <cstdlib>

namespace _jxx {
    typedef bool bit;
    typedef uint8_t byte;
    typedef uint16_t word;
    typedef uint32_t dword;
    typedef uint64_t qword;
    typedef int nint;
    typedef unsigned int nuint;
    typedef int intn;
    typedef unsigned int uintn;
    typedef uintptr_t address;
    typedef const char* string;
    typedef std::vector<uint8_t> buffer;
    typedef void* routine;
    #define null nullptr
    using std::vector;
};

namespace _j {
    /* Begin Common Sink functions */

#ifdef __linux__
#include <execinfo.h>

    [[noreturn]] void _panic(_jxx::string message)
    {
        std::cerr << "PANIC: " << message << std::endl;

        void *array[10];
        size_t size;
        char **strings;

        size = backtrace(array, 10);
        strings = backtrace_symbols(array, size);

        std::cerr << "Begin Backtrace:" << std::endl;
        for (size_t i = 0; i < size; i++)
        {
            std::cerr << strings[i] << std::endl;
        }
        std::cerr << "End Backtrace" << std::endl;

        // Don't free strings, just leak it

        std::_Exit(1);
    }  
#else
    [[noreturn]] void _panic(_jxx::string message)
    {
        std::cerr << "PANIC: " << message << std::endl;
        abort();

        while (true) {}
    }  
#endif
    
    
    /* Begin Generic Structure Base Class */
    typedef _jxx::qword typeid_t;

    static std::map<typeid_t, _jxx::string> g_typenames_mapping = {{0, "_uuid_t"}};

    template <typeid_t T>
    class StructGeneric
    {
    protected:
        static const typeid_t m_typeid = T;
        static std::map<typeid_t, std::map<_jxx::string, const void *>> m_attributes;

    public:
        typeid_t _typeid()
        {
            return m_typeid;
        }

        _jxx::string _typename()
        {
            return g_typenames_mapping[m_typeid];
        }

        bool _has(_jxx::string name)
        {
            return m_attributes[m_typeid].find(name) != m_attributes[m_typeid].end();
        }

        bool _hasfield(_jxx::string name)
        {
            _jxx::string index_names = this->_get("_index_names");
            int state = 0;

            while (*index_names)
            {
                // ',' reset
                if (*index_names == ',')
                {
                    state = 0;
                    index_names++;
                    continue;
                }

                if (strcmp(index_names, name) == 0)
                {
                    return true;
                }

                index_names++;
            }

            return false;
        }

        void _set(_jxx::string name, _jxx::string value)
        {
            m_attributes[m_typeid][name] = value;
        }

        void _set(_jxx::string name, long value)
        {
            m_attributes[m_typeid][name] = (void *)value;
        }

        _jxx::string _get(_jxx::string name)
        {
            if (!this->_has(name))
                _panic("meta _get() failed: Attribute not found");
            return (_jxx::string)m_attributes[m_typeid][name];
        }

        long _getint(_jxx::string name)
        {
            if (!this->_has(name))
                _panic("meta _getint() failed: Attribute not found");
            return (_jxx::qword)m_attributes[m_typeid][name];
        }
    };

    template <typeid_t T>
    std::map<typeid_t, std::map<_jxx::string, const void *>> StructGeneric<T>::m_attributes;
    template <typeid_t T>
    const typeid_t StructGeneric<T>::m_typeid;
    /* End Generic Structure Base Class */
};

//==================================================================//
// File: /tmp/fileRez0gk.cpp                                        //
//==================================================================//

namespace _jxx
{
    /* Begin Structure _uuid_t */
    class _uuid_t : public _j::StructGeneric<0>
    {
    public:
        _uuid_t()
        {
            /* attributes for field _a */
            this->_set("_a__serial_format", "hex");
            this->_set("_a__endian", "big");
            this->_set("_a__serial_suffix", "-");

            /* attributes for field _b */
            this->_set("_b__serial_format", "hex");
            this->_set("_b__endian", "big");
            this->_set("_b__serial_suffix", "-");

            /* attributes for field _c */
            this->_set("_c__serial_format", "hex");
            this->_set("_c__endian", "big");
            this->_set("_c__serial_suffix", "-");

            /* attributes for field _d */
            this->_set("_d__serial_format", "hex");
            this->_set("_d__endian", "big");
            this->_set("_d__serial_suffix", "-");

            /* attributes for field _e */
            this->_set("_e__serial_format", "hex");
            this->_set("_e__endian", "big");

            /* attributes for field _f */
            this->_set("_f__serial_format", "hex");
            this->_set("_f__endian", "big");

            /* auto-generated attributes */
            this->_set("_index", "_a:dword=0,_b:word=0,_c:word=0,_d:word=0,_e:dword=0,_f:word=0");
            this->_set("_index_names", "_a,_b,_c,_d,_e,_f");
            this->_set("_index_types", "dword=0,word=0,word=0,word=0,dword=0,word=0");
        }

        dword _a = 0;
        word _b = 0;
        word _c = 0;
        word _d = 0;
        dword _e = 0;
        word _f = 0;
    };
    constexpr auto j__uuid_t_size = sizeof(_uuid_t);
    /* End Structure _uuid_t */

    
#define UUID_SIZE 36

    int _main()
    {
        char uuid[UUID_SIZE];
        printf("Hello, world!\n");
        printf("UUID_SIZE: %d\n", UUID_SIZE);
        return 0;
    }

}


//==================================================================//
// EOF: /tmp/fileRez0gk.cpp                                         //
//==================================================================//


int main(int argc, char **argv)
{
    return _jxx::_main();
}

//==================================================================//
// EOF: J++ Transpiled Code                                         //
// SHA256:                                                          //
// c5024914e4ddfefa47df515ab0ed056d6a7dc7012f6f3371ca7bb3652175d3a2 //
//==================================================================//
