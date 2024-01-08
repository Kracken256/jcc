//==================================================================//
// Type: J++ Transpiled Code                                        //
// Date: Fri Apr 13 17:15:56 4461545 HST                               //
//==================================================================//

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <iostream>
#include <cstdlib>

namespace _jxx
{
    typedef bool _bit;
    typedef int8_t _char;
    typedef uint8_t _byte;
    typedef uint16_t _short;
    typedef uint16_t _word;
    typedef int32_t _int;
    typedef uint32_t _dword;
    typedef int64_t _long;
    typedef uint64_t _qword;
    typedef float _float;
    typedef double _double;
    typedef ssize_t _intn;
    typedef size_t _uintn;
    typedef uintptr_t _address;
    typedef const char *_string;
    typedef void *_routine;
#define _null nullptr
#define _void void
#define _vector std::vector
};

namespace _j {
    /* Begin Common Sink functions */

#ifdef __linux__
#include <execinfo.h>

    // no warn unused
    [[gnu::used]] [[noreturn]] static void _panic(_jxx::_string message)
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
    [[gnu::used]] [[noreturn]] static void _panic(_jxx::_string message)
    {
        std::cerr << "PANIC: " << message << std::endl;
        abort();

        while (true) {}
    }  
#endif
    
    
    /* Begin Generic Structure Base Class */
    typedef _jxx::_qword typeid_t;

    static std::map<typeid_t, _jxx::_string> g_typenames_mapping = {{0, "__jxx::::_struct_A"}, {1, "__jxx::_ns_1::::_struct_B"}, {2, "__jxx::_ns_2::::_struct_C"}, {3, "__jxx::_ns_2::_ns_3::::_struct_D"}};
    static std::map<_jxx::_string, typeid_t> g_typenames_mapping_reverse = {{"__jxx::::_struct_A", 0}, {"__jxx::_ns_1::::_struct_B", 1}, {"__jxx::_ns_2::::_struct_C", 2}, {"__jxx::_ns_2::_ns_3::::_struct_D", 3}};
    static std::map<std::string, _jxx::_uintn> g_builtin_sizes = {{"_bit", 1}, {"_byte", 1}, {"_char", 1}, {"_word", 2}, {"_short", 2}, {"_dword", 4}, {"_int", 4}, {"_float", 4}, {"_double", 8}, {"_qword", 8}, {"_long", 8}, {"_string", 8}, {"_routine", 8}, {"_address", 8}};
    struct ReflectiveEntry {
        _jxx::_string field_name;
        _jxx::_string type;
        _jxx::_uintn count;
    };

    static std::map<typeid_t, std::vector<ReflectiveEntry>> g_reflective_entries = {{0, {{"_a", "_int", 1}}}, {1, {{"_b", "_int", 1}}}, {2, {{"_a", "_struct_A", 1}, {"_b", "_ns_1::_struct_B", 1}}}, {3, {{"_a", "_struct_C", 1}, {"_b", "_struct_A", 1}, {"_c", "_ns_1::_struct_B", 1}}}};

    template <typeid_t T>
    class StructGeneric
    {
    protected:
        static const typeid_t m_typeid = T;
        static std::map<typeid_t, std::map<_jxx::_string, const void *>> m_attributes;

    public:
        inline typeid_t _typeid() const
        {
            return m_typeid;
        }

        inline _jxx::_string _typename() const
        {
            return g_typenames_mapping.at(m_typeid);
        }

        inline bool _has(_jxx::_string name) const
        {
            return m_attributes.at(m_typeid).find(name) != m_attributes.at(m_typeid).end();
        }

        bool _hasfield(_jxx::_string name) const
        {
            _jxx::_string index_names = this->_get("_index_names");
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

                if (strncmp(index_names, name, strlen(name)) == 0)
                {
                    return true;
                }

                index_names++;
            }

            return false;
        }

        inline void _set(_jxx::_string name, _jxx::_string value) const
        {
            m_attributes[m_typeid][name] = (void *)value;
        }

        inline void _set(_jxx::_string name, long value) const
        {
            m_attributes[m_typeid][name] = (void *)value;
        }

        inline _jxx::_string _get(_jxx::_string name) const
        {
            return (_jxx::_string)m_attributes.at(m_typeid).at(name);
        }

        inline long _getint(_jxx::_string name) const
        {
            return (_jxx::_qword)m_attributes.at(m_typeid).at(name);
        }

        static inline _jxx::_string _gettypename(typeid_t type)
        {
            return g_typenames_mapping.at(type);
        }

        static inline typeid_t _gettypeid(_jxx::_string name)
        {
            return g_typenames_mapping_reverse.at(name);
        }

        static size_t _sizeof(typeid_t id)
        {
            size_t size = 0;
            for (auto &field_type : g_reflective_entries.at(id))
            {
                if (g_builtin_sizes.contains(field_type.type))
                {
                    size += g_builtin_sizes.at(field_type.type) * field_type.count;
                }
                else
                {
                    size += _sizeof(_gettypeid(field_type.type)) * field_type.count;
                }
            }

            return size;
        }

        inline size_t _sizeof() const
        {
            return _sizeof(m_typeid);
        }
    };

    template <typeid_t T>
    std::map<typeid_t, std::map<_jxx::_string, const void *>> StructGeneric<T>::m_attributes;
    template <typeid_t T>
    const typeid_t StructGeneric<T>::m_typeid;
    /* End Generic Structure Base Class */
};

//==================================================================//
// File: /tmp/fileXpoKFj.cpp                                        //
//==================================================================//

namespace _jxx
{
    /* Begin Structure _struct_A */
    class _struct_A : public _j::StructGeneric<0>
    {
    public:
        _struct_A()
        {
            /* auto-generated attributes */
            this->_set("_index", "_a:_int,");
            this->_set("_index_names", "_a,");
            this->_set("_index_types", "_int,");
        }

        _int _a;
    };
    constexpr auto j__struct_A_size = sizeof(_struct_A);
    /* End Structure _struct_A */

    namespace _ns_1
    {
        /* Begin Structure _struct_B */
        class _struct_B : public _j::StructGeneric<1>
        {
        public:
            _struct_B()
            {
                /* auto-generated attributes */
                this->_set("_index", "_b:_int,");
                this->_set("_index_names", "_b,");
                this->_set("_index_types", "_int,");
            }

            _int _b;
        };
        constexpr auto j__struct_B_size = sizeof(_struct_B);
        /* End Structure _struct_B */

    }

    namespace _ns_2
    {
        /* Begin Structure _struct_C */
        class _struct_C : public _j::StructGeneric<2>
        {
        public:
            _struct_C()
            {
                /* auto-generated attributes */
                this->_set("_index", "_a:_struct_A,_b:_ns_1::_struct_B,");
                this->_set("_index_names", "_a,_b,");
                this->_set("_index_types", "_struct_A,_ns_1::_struct_B,");
            }

            _struct_A _a;
            _ns_1::_struct_B _b;
        };
        constexpr auto j__struct_C_size = sizeof(_struct_C);
        /* End Structure _struct_C */

        namespace _ns_3
        {
            /* Begin Structure _struct_D */
            class _struct_D : public _j::StructGeneric<3>
            {
            public:
                _struct_D()
                {
                    /* auto-generated attributes */
                    this->_set("_index", "_a:_struct_C,_b:_struct_A,_c:_ns_1::_struct_B,");
                    this->_set("_index_names", "_a,_b,_c,");
                    this->_set("_index_types", "_struct_C,_struct_A,_ns_1::_struct_B,");
                }

                _struct_C _a;
                _struct_A _b;
                _ns_1::_struct_B _c;
            };
            constexpr auto j__struct_D_size = sizeof(_struct_D);
            /* End Structure _struct_D */

        }

    }

    _void _hello(const _float& _a = 42317633812.24230194091796875, const _int& _b = 10)
    {
    }

}

//==================================================================//
// EOF: /tmp/fileXpoKFj.cpp                                         //
//==================================================================//

//==================================================================//
// EOF: J++ Transpiled Code                                         //
// SHA256:                                                          //
// 31d971fb8b6aab345ffd5cb898b9dc847b4ce6cd6e6873c833998fc043099847 //
//==================================================================//
