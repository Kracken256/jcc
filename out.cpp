//==================================================================//
// Info: J++ Transpiled Code                                        //
// Type: C++-20                                                     //
// Version: J++-dev                                                 //
// Sources: ["tests/nested-struct.j"]                               //
// Platform: independent                                            //
// Date: 2024-02-11T20:24:51 +0000 GMT                              //
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

static std::map<typeid_t, _string> g_typenames_mapping = {{0, "_App::Color"}, {1, "_App::Point"}, {2, "_App::Line"}, {3, "_App::Rect"}, {4, "_App::Circle"}, {5, "_App::Text"}};
static std::map<_string, typeid_t> g_typenames_mapping_reverse = {{"_App::Color", 0}, {"_App::Point", 1}, {"_App::Line", 2}, {"_App::Rect", 3}, {"_App::Circle", 4}, {"_App::Text", 5}};
static std::map<std::string, _uintn> g_builtin_sizes = {{"_bool", 1}, {"_byte", 1}, {"_char", 1}, {"_word", 2}, {"_short", 2}, {"_dword", 4}, {"_int", 4}, {"_float", 4}, {"_double", 8}, {"_qword", 8}, {"_long", 8}, {"_string", 8}, {"_routine", 8}, {"_address", 8}};
struct ReflectiveEntry {
    _string field_name;
    _string type;
    _uintn count;
};

static std::map<typeid_t, std::vector<ReflectiveEntry>> g_reflective_entries = {{0, {{"_r", "_long", 1}, {"_g", "_long", 1}, {"_b", "_long", 1}, {"_a", "_int", 1}}}, {1, {{"_x", "_int", 1}, {"_y", "_int", 1}, {"_z", "_int", 1}}}, {2, {{"_start", "_Point", 1}, {"_end", "_Point", 1}, {"_color", "_Color", 1}}}, {3, {{"_tl", "_Point", 1}, {"_br", "_Point", 1}, {"_color", "_Color", 1}}}, {4, {{"_center", "_Point", 1}, {"_r", "_int", 1}, {"_color", "_Color", 1}}}, {5, {{"_tl", "_Point", 1}, {"_text", "_string", 1}, {"_color", "_Color", 1}}}};

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
// File: "tests/nested-struct.j" //
//==================================================================//

/* [] */
namespace _App
{
    /* Begin Structure _Color */
    class _Color : public StructGeneric<0>
    {
    public:
        _Color()
        {
            /* auto-generated attributes */
            this->_set("_index", "_r:_long=0,_g:_long=0,_b:_long=0,_a:_int=255,");
            this->_set("_index_names", "_r,_g,_b,_a,");
            this->_set("_index_types", "_long=0,_long=0,_long=0,_int=255,");
        }

        _long _r = 0;
        _long _g = 0;
        _long _b = 0;
        _int _a = 255;
    };
    constexpr auto j__Color_size = sizeof(_Color);
    /* End Structure _Color */

    /* Begin Structure _Point */
    class _Point : public StructGeneric<1>
    {
    public:
        _Point()
        {
            /* auto-generated attributes */
            this->_set("_index", "_x:_int=0,_y:_int=0,_z:_int=0,");
            this->_set("_index_names", "_x,_y,_z,");
            this->_set("_index_types", "_int=0,_int=0,_int=0,");
        }

        _int _x = 0;
        _int _y = 0;
        _int _z = 0;
    };
    constexpr auto j__Point_size = sizeof(_Point);
    /* End Structure _Point */

    /* Begin Structure _Line */
    class _Line : public StructGeneric<2>
    {
    public:
        _Line()
        {
            /* auto-generated attributes */
            this->_set("_index", "_start:_Point,_end:_Point,_color:_Color,");
            this->_set("_index_names", "_start,_end,_color,");
            this->_set("_index_types", "_Point,_Point,_Color,");
        }

        _Point _start;
        _Point _end;
        _Color _color;
    };
    constexpr auto j__Line_size = sizeof(_Line);
    /* End Structure _Line */

    /* Begin Structure _Rect */
    class _Rect : public StructGeneric<3>
    {
    public:
        _Rect()
        {
            /* auto-generated attributes */
            this->_set("_index", "_tl:_Point,_br:_Point,_color:_Color,");
            this->_set("_index_names", "_tl,_br,_color,");
            this->_set("_index_types", "_Point,_Point,_Color,");
        }

        _Point _tl;
        _Point _br;
        _Color _color;
    };
    constexpr auto j__Rect_size = sizeof(_Rect);
    /* End Structure _Rect */

    /* Begin Structure _Circle */
    class _Circle : public StructGeneric<4>
    {
    public:
        _Circle()
        {
            /* auto-generated attributes */
            this->_set("_index", "_center:_Point,_r:_int=0,_color:_Color,");
            this->_set("_index_names", "_center,_r,_color,");
            this->_set("_index_types", "_Point,_int=0,_Color,");
        }

        _Point _center;
        _int _r = 0;
        _Color _color;
    };
    constexpr auto j__Circle_size = sizeof(_Circle);
    /* End Structure _Circle */

    /* Begin Structure _Text */
    class _Text : public StructGeneric<5>
    {
    public:
        _Text()
        {
            /* auto-generated attributes */
            this->_set("_index", "_tl:_Point,_text:_string=\"Your \\ntext here\",_color:_Color,");
            this->_set("_index_names", "_tl,_text,_color,");
            this->_set("_index_types", "_Point,_string=\"Your \\ntext here\",_Color,");
        }

        _Point _tl;
        _string _text = "Your \ntext here";
        _Color _color;
    };
    constexpr auto j__Text_size = sizeof(_Text);
    /* End Structure _Text */

}

_int _Main(const std::vector<_string>& _args)
{
    return 0;
}

//==================================================================//
// EOF: "tests/nested-struct.j"                                     //
//==================================================================//

int main(int argc, char **argv)
{
    std::vector<_string> args(argv, argv + argc);
    return _Main(args);
}

//==================================================================//
// EOF: J++ Transpiled Code                                         //
// SHA256:                                                          //
// 888a54c79a77ded9e69339232309959299a1b61a597b3f375c9342030854ab2c //
//==================================================================//
