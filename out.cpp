//==================================================================//
// Info: J++ Transpiled Code                                        //
// Type: C++-20                                                     //
// Version: J++-dev                                                 //
// Sources: ["tests/complex-struct.j"]                              //
// Platform: independent                                            //
// Date: 2024-01-29T23:42:41 +0000 GMT                              //
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

static std::map<typeid_t, _string> g_typenames_mapping = {{0, "_std::_io::Reader"}, {1, "_libmia::_Format::MIAHeader"}, {2, "_libmia::_Parser::Context"}};
static std::map<_string, typeid_t> g_typenames_mapping_reverse = {{"_std::_io::Reader", 0}, {"_libmia::_Format::MIAHeader", 1}, {"_libmia::_Parser::Context", 2}};
static std::map<std::string, _uintn> g_builtin_sizes = {{"_bool", 1}, {"_byte", 1}, {"_char", 1}, {"_word", 2}, {"_short", 2}, {"_dword", 4}, {"_int", 4}, {"_float", 4}, {"_double", 8}, {"_qword", 8}, {"_long", 8}, {"_string", 8}, {"_routine", 8}, {"_address", 8}};
struct ReflectiveEntry {
    _string field_name;
    _string type;
    _uintn count;
};

static std::map<typeid_t, std::vector<ReflectiveEntry>> g_reflective_entries = {{1, {{"_magic", "_dword", 1}, {"_version", "_word", 1}, {"_progid", "_byte", 16}, {"_camid", "_byte", 16}, {"_quality_index", "_byte", 1}, {"_colormode", "_word", 1}, {"_width", "_dword", 1}, {"_height", "_dword", 1}, {"_span", "_dword", 1}, {"_pxsize", "_byte", 1}, {"_compmode", "_byte", 1}, {"_usize", "_dword", 1}, {"_oid", "_byte", 16}, {"_pgid", "_byte", 16}, {"_timestamp", "_qword", 1}, {"_crc32", "_dword", 1}, {"_reserved", "_byte", 25}}}, {2, {{"_header", "_Format::_MIAHeader", 1}, {"_src", "_std::_io::_Reader", 1}}}};

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
// File: "tests/complex-struct.j" //
//==================================================================//

/* [] */
namespace _std
{
    /* [] */
    namespace _io
    {
        /* Begin Structure _Reader */
        class _Reader : public StructGeneric<0>
        {
        public:
            _Reader()
            {
                /* auto-generated attributes */
                this->_set("_index", "");
                this->_set("_index_names", "");
                this->_set("_index_types", "");
            }

        };
        constexpr auto j__Reader_size = sizeof(_Reader);
        /* End Structure _Reader */

    }

}

/* [] */
namespace _libmia
{
    /* [] */
    namespace _Format
    {
        /* Begin Structure _MIAHeader */
        #pragma pack(push, 1)
        class _MIAHeader : public StructGeneric<1>
        {
        public:
            _MIAHeader()
            {
                /* auto-generated attributes */
                this->_set("_index", "_magic:_dword=0,_version:_word=0,_progid:std::vector<_byte>,_camid:std::vector<_byte>,_quality_index:_byte=0,_colormode:_word=0,_width:_dword=0,_height:_dword=0,_span:_dword=0,_pxsize:_byte=0,_compmode:_byte=0,_usize:_dword=0,_oid:std::vector<_byte>,_pgid:std::vector<_byte>,_timestamp:_qword=0,_crc32:_dword=0,_reserved:std::vector<_byte>,");
                this->_set("_index_names", "_magic,_version,_progid,_camid,_quality_index,_colormode,_width,_height,_span,_pxsize,_compmode,_usize,_oid,_pgid,_timestamp,_crc32,_reserved,");
                this->_set("_index_types", "_dword=0,_word=0,std::vector<_byte>,std::vector<_byte>,_byte=0,_word=0,_dword=0,_dword=0,_dword=0,_byte=0,_byte=0,_dword=0,std::vector<_byte>,std::vector<_byte>,_qword=0,_dword=0,std::vector<_byte>,");
            }

            _dword _magic = 0;
            _word _version = 0;
            _byte _progid[16];
            _byte _camid[16];
            _byte _quality_index = 0;
            _word _colormode = 0;
            _dword _width = 0;
            _dword _height = 0;
            _dword _span = 0;
            _byte _pxsize = 0;
            _byte _compmode = 0;
            _dword _usize = 0;
            _byte _oid[16];
            _byte _pgid[16];
            _qword _timestamp = 0;
            _dword _crc32 = 0;
            _byte _reserved[25];
        };
        #pragma pack(pop)
        constexpr auto j__MIAHeader_size = sizeof(_MIAHeader);
        /* End Structure _MIAHeader */

    }

    /* ["_libmia::_Format", "_std::io"] */
    namespace _Parser
    {
        /* Begin Structure _Context */
        class _Context : public StructGeneric<2>
        {
        public:
            _Context()
            {
                /* auto-generated attributes */
                this->_set("_index", "_header:_Format::_MIAHeader,_src:_std::_io::_Reader,");
                this->_set("_index_names", "_header,_src,");
                this->_set("_index_types", "_Format::_MIAHeader,_std::_io::_Reader,");
            }

            _Format::_MIAHeader _header;
            _std::_io::_Reader _src;
        };
        constexpr auto j__Context_size = sizeof(_Context);
        /* End Structure _Context */

    }

}

//==================================================================//
// EOF: "tests/complex-struct.j"                                    //
//==================================================================//

//==================================================================//
// EOF: J++ Transpiled Code                                         //
// SHA256:                                                          //
// 4b9f51f583293791ce944b56bd4c67ace154e259cba719664187308d0c9f86fe //
//==================================================================//
