//==================================================================//
// Type: J++ Transpiled Code                                        //
// Date: Fri Nov  8 18:21:00 3005252 HST                               //
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

    static std::map<typeid_t, _jxx::string> g_typenames_mapping = {{0, "_uuid_t"}, {1, "_database_record_t"}, {2, "_database_t"}};
    static std::map<_jxx::string, typeid_t> g_typenames_mapping_reverse = {{"_uuid_t", 0}, {"_database_record_t", 1}, {"_database_t", 2}};
    static std::map<std::string, uint> g_builtin_sizes = {{"bit", 1}, {"byte", 1}, {"char", 1}, {"word", 2}, {"short", 2}, {"dword", 4}, {"int", 4}, {"float", 4}, {"double", 8}, {"qword", 8}, {"long", 8}, {"string", 8}, {"buffer", 8}, {"routine", 8}, {"address", 8}};
    struct ReflectiveEntry {
        _jxx::string field_name;
        _jxx::string type;
        _jxx::uintn count;
    };

    static std::map<typeid_t, std::vector<ReflectiveEntry>> g_reflective_entries = {{0, {{"_a", "dword", 1}, {"_b", "word", 1}, {"_c", "word", 1}, {"_d", "word", 1}, {"_e", "dword", 1}, {"_f", "word", 1}}}, {1, {{"_id", "_uuid_t", 1}, {"_name", "string", 1}, {"_age", "byte", 1}, {"_height", "float", 1}, {"_weight", "float", 1}, {"_is_married", "bit", 1}}}, {2, {{"_records", "_database_record_t", 10}}}};

    template <typeid_t T>
    class StructGeneric
    {
    protected:
        static const typeid_t m_typeid = T;
        static std::map<typeid_t, std::map<_jxx::string, const void *>> m_attributes;

    public:
        inline typeid_t _typeid() const
        {
            return m_typeid;
        }

        inline _jxx::string _typename() const
        {
            return g_typenames_mapping.at(m_typeid);
        }

        inline bool _has(_jxx::string name) const
        {
            return m_attributes.at(m_typeid).find(name) != m_attributes.at(m_typeid).end();
        }

        bool _hasfield(_jxx::string name) const
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

                if (strncmp(index_names, name, strlen(name)) == 0)
                {
                    return true;
                }

                index_names++;
            }

            return false;
        }

        inline void _set(_jxx::string name, _jxx::string value) const
        {
            m_attributes[m_typeid][name] = (void *)value;
        }

        inline void _set(_jxx::string name, long value) const
        {
            m_attributes[m_typeid][name] = (void *)value;
        }

        inline _jxx::string _get(_jxx::string name) const
        {
            return (_jxx::string)m_attributes.at(m_typeid).at(name);
        }

        inline long _getint(_jxx::string name) const
        {
            return (_jxx::qword)m_attributes.at(m_typeid).at(name);
        }

        static inline _jxx::string _gettypename(typeid_t type)
        {
            return g_typenames_mapping.at(type);
        }

        static inline typeid_t _gettypeid(_jxx::string name)
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
    std::map<typeid_t, std::map<_jxx::string, const void *>> StructGeneric<T>::m_attributes;
    template <typeid_t T>
    const typeid_t StructGeneric<T>::m_typeid;
    /* End Generic Structure Base Class */
};

//==================================================================//
// File: /tmp/filevOJgpT.cpp                                        //
//==================================================================//

namespace _jxx
{
    /* Begin Structure _uuid_t */
    #pragma pack(push, 1)
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
            this->_set("_index", "_a:dword=0,_b:word=0,_c:word=0,_d:word=0,_e:dword=0,_f:word=0,");
            this->_set("_index_names", "_a,_b,_c,_d,_e,_f,");
            this->_set("_index_types", "dword=0,word=0,word=0,word=0,dword=0,word=0,");
        }

        dword _a = 0;
        word _b = 0;
        word _c = 0;
        word _d = 0;
        dword _e = 0;
        word _f = 0;
    };
    #pragma pack(pop)
    constexpr auto j__uuid_t_size = sizeof(_uuid_t);
    /* End Structure _uuid_t */

    /* Begin Structure _database_record_t */
    class _database_record_t : public _j::StructGeneric<1>
    {
    public:
        _database_record_t()
        {
            /* auto-generated attributes */
            this->_set("_index", "_id:_uuid_t,_name:string,_age:byte,_height:float,_weight:float,_is_married:bit,");
            this->_set("_index_names", "_id,_name,_age,_height,_weight,_is_married,");
            this->_set("_index_types", "_uuid_t,string,byte,float,float,bit,");
        }

        _uuid_t _id;
        string _name;
        byte _age;
        float _height;
        float _weight;
        bit _is_married;
    };
    constexpr auto j__database_record_t_size = sizeof(_database_record_t);
    /* End Structure _database_record_t */

    /* Begin Structure _database_t */
    #pragma pack(push, 1)
    class _database_t : public _j::StructGeneric<2>
    {
    public:
        _database_t()
        {
            /* auto-generated attributes */
            this->_set("_index", "_records:vector<_database_record_t>,");
            this->_set("_index_names", "_records,");
            this->_set("_index_types", "vector<_database_record_t>,");
        }

        _database_record_t _records[10];
    };
    #pragma pack(pop)
    constexpr auto j__database_t_size = sizeof(_database_t);
    /* End Structure _database_t */

    int _main()
    {
        return 0;
    }

}


//==================================================================//
// EOF: /tmp/filevOJgpT.cpp                                         //
//==================================================================//


int main(int argc, char **argv)
{
    return _jxx::_main();
}

//==================================================================//
// EOF: J++ Transpiled Code                                         //
// SHA256:                                                          //
// af9de765da4c977f490c78afdb4ecb570c0fcfaf8746984998cd453eaf11040e //
//==================================================================//
