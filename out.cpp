//==================================================================//
// Info: J++ Transpiled Code                                        //
// Type: C++-20                                                     //
// Version: J++-dev                                                 //
// Sources: ["tests/declare_constants.j"]                           //
// Platform: independent                                            //
// Date: 2024-02-15T21:05:43 +0000 GMT                              //
// Copyright (C) 2023 Wesley C. Jones. All rights reserved.         //
//==================================================================//

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <cstring>
#include <iostream>

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

static std::map<typeid_t, _string> g_typenames_mapping = {{0, "_Math::_Constants::CReal"}};
static std::map<_string, typeid_t> g_typenames_mapping_reverse = {{"_Math::_Constants::CReal", 0}};
static std::map<std::string, _uintn> g_builtin_sizes = {{"_bool", 1}, {"_byte", 1}, {"_char", 1}, {"_word", 2}, {"_short", 2}, {"_dword", 4}, {"_int", 4}, {"_float", 4}, {"_double", 8}, {"_qword", 8}, {"_long", 8}, {"_string", 8}, {"_routine", 8}, {"_address", 8}};
struct ReflectiveEntry {
    _string field_name;
    _string type;
    _uintn count;
};

static std::map<typeid_t, std::vector<ReflectiveEntry>> g_reflective_entries = {{0, {{"_real", "_double", 1}, {"_imag", "_double", 1}}}};

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
// File: "tests/declare_constants.j" //
//==================================================================//

/* [] */
namespace _Math
{
    /* [] */
    namespace _Constants
    {
        const _double _PI = 3.14159265358979323846;
        const _double _E = 2.71828182845904523536;
        /* Begin Structure _CReal */
        class _CReal : public StructGeneric<0>
        {
        public:
            _CReal()
            {
                /* auto-generated attributes */
                this->_set("_index", "_real:_double=0,_imag:_double=0,");
                this->_set("_index_names", "_real,_imag,");
                this->_set("_index_types", "_double=0,_double=0,");
            }

            _double _real = 0;
            _double _imag = 0;
        };
        constexpr auto j__CReal_size = sizeof(_CReal);
        /* End Structure _CReal */

        const _CReal _I;
    }

}

//==================================================================//
// EOF: "tests/declare_constants.j"                                 //
//==================================================================//

//==================================================================//
// EOF: J++ Transpiled Code                                         //
// SHA256:                                                          //
// 3da5d72b5191e7e342d916f5c80ac65961e758a16ad81c1ab5bde78855594f4e //
//==================================================================//
