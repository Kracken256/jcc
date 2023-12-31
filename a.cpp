//==================================================================//
// Info: J++ Transpiled Code                                        //
// Type: C++-20                                                     //
// Version: J++-dev                                                 //
// Sources: ["tests/func-declare.j"]                                //
// Platform: independent                                            //
// Date: 2024-01-09T07:30:30 +0000 GMT                              //
// Copyright (C) 2023 Wesley C. Jones. All rights reserved.         //
//==================================================================//

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <iostream>
#include <cstdlib>

typedef bool _bool;
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


/* Begin Common Sink functions */

#ifdef __linux__
#include <execinfo.h>

// no warn unused
[[gnu::used]] [[noreturn]] static void _panic(_string message)
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
[[gnu::used]] [[noreturn]] static void _panic(_string message)
{
    std::cerr << "PANIC: " << message << std::endl;
    abort();

    while (true) {}
}  
#endif


/* Begin Generic Structure Base Class */
typedef _qword typeid_t;

static std::map<typeid_t, _string> g_typenames_mapping = {{0, "__common::::_uuid_t"}};
static std::map<_string, typeid_t> g_typenames_mapping_reverse = {{"__common::::_uuid_t", 0}};
static std::map<std::string, _uintn> g_builtin_sizes = {{"_bool", 1}, {"_byte", 1}, {"_char", 1}, {"_word", 2}, {"_short", 2}, {"_dword", 4}, {"_int", 4}, {"_float", 4}, {"_double", 8}, {"_qword", 8}, {"_long", 8}, {"_string", 8}, {"_routine", 8}, {"_address", 8}};
struct ReflectiveEntry {
    _string field_name;
    _string type;
    _uintn count;
};

static std::map<typeid_t, std::vector<ReflectiveEntry>> g_reflective_entries = {{0, {{"_a", "_dword", 1}, {"_b", "_word", 1}, {"_c", "_word", 1}, {"_d", "_word", 1}, {"_e", "_dword", 1}, {"_f", "_word", 1}}}};

template <typeid_t T>
class StructGeneric
{
protected:
    static const typeid_t m_typeid = T;
    static std::map<typeid_t, std::map<_string, const void *>> m_attributes;

public:
    inline typeid_t _typeid() const
    {
        return m_typeid;
    }

    inline _string _typename() const
    {
        return g_typenames_mapping.at(m_typeid);
    }

    inline bool _has(_string name) const
    {
        return m_attributes.at(m_typeid).find(name) != m_attributes.at(m_typeid).end();
    }

    bool _hasfield(_string name) const
    {
        _string index_names = this->_get("_index_names");
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

    inline void _set(_string name, _string value) const
    {
        m_attributes[m_typeid][name] = (void *)value;
    }

    inline void _set(_string name, long value) const
    {
        m_attributes[m_typeid][name] = (void *)value;
    }

    inline _string _get(_string name) const
    {
        return (_string)m_attributes.at(m_typeid).at(name);
    }

    inline long _getint(_string name) const
    {
        return (_qword)m_attributes.at(m_typeid).at(name);
    }

    static inline _string _gettypename(typeid_t type)
    {
        return g_typenames_mapping.at(type);
    }

    static inline typeid_t _gettypeid(_string name)
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
std::map<typeid_t, std::map<_string, const void *>> StructGeneric<T>::m_attributes;
template <typeid_t T>
const typeid_t StructGeneric<T>::m_typeid;
/* End Generic Structure Base Class */
//==================================================================//
// File: "tests/func-declare.j" //
//==================================================================//

/* [] */
namespace _common
{
    /* Begin Structure _uuid_t */
    #pragma pack(push, 1)
    class _uuid_t : public StructGeneric<0>
    {
    public:
        _uuid_t()
        {
            /* attributes for field _a */
            this->_set("_a_serial_format", "hex");
            this->_set("_a_endian", "big");
            this->_set("_a_serial_suffix", "-");

            /* attributes for field _b */
            this->_set("_b_serial_format", "hex");
            this->_set("_b_endian", "big");
            this->_set("_b_serial_suffix", "-");

            /* attributes for field _c */
            this->_set("_c_serial_format", "hex");
            this->_set("_c_endian", "big");
            this->_set("_c_serial_suffix", "-");

            /* attributes for field _d */
            this->_set("_d_serial_format", "hex");
            this->_set("_d_endian", "big");
            this->_set("_d_serial_suffix", "-");

            /* attributes for field _e */
            this->_set("_e_serial_format", "hex");
            this->_set("_e_endian", "big");

            /* attributes for field _f */
            this->_set("_f_serial_format", "hex");
            this->_set("_f_endian", "big");

            /* auto-generated attributes */
            this->_set("_index", "_a:_dword=0,_b:_word=0,_c:_word=0,_d:_word=0,_e:_dword=0,_f:_word=0,");
            this->_set("_index_names", "_a,_b,_c,_d,_e,_f,");
            this->_set("_index_types", "_dword=0,_word=0,_word=0,_word=0,_dword=0,_word=0,");
        }

        _dword _a = 0;
        _word _b = 0;
        _word _c = 0;
        _word _d = 0;
        _dword _e = 0;
        _word _f = 0;
    };
    #pragma pack(pop)
    constexpr auto j__uuid_t_size = sizeof(_uuid_t);
    /* End Structure _uuid_t */

}

//==================================================================//
// EOF: "tests/func-declare.j"                                      //
//==================================================================//

//==================================================================//
// EOF: J++ Transpiled Code                                         //
// SHA256:                                                          //
// 229940766ade0a2c9029145d18e0961c0db1010f26b83fb7c467be5bbc27681e //
//==================================================================//
