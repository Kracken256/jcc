//==================================================================//
// Type: J++ Transpiled Code                                        //
// Date: Sun May 18 12:58:06 4461681 HST                               //
//==================================================================//

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <map>

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
            if (g_typenames_mapping.find(m_typeid) == g_typenames_mapping.end())
                return "";
            return g_typenames_mapping[m_typeid];
        }

        bool _has(_jxx::string name)
        {
            return m_attributes[m_typeid].find(name) != m_attributes[m_typeid].end();
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
                return "";
            return (_jxx::string)m_attributes[m_typeid][name];
        }

        long _getint(_jxx::string name)
        {
            if (!this->_has(name))
                return 0;
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
// File: /tmp/filetjiW1G.cpp                                        //
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

    int _main()
    {
        return 0;
    }

}


//==================================================================//
// EOF: /tmp/filetjiW1G.cpp                                         //
//==================================================================//


int main(int argc, char **argv)
{
    return _jxx::_main();
}

//==================================================================//
// EOF: J++ Transpiled Code                                         //
// SHA256:                                                          //
// c86ea2b9ff13a4e3c3668335ade46466ec865db15746c4912f7f4f209b1ff0bd //
//==================================================================//
