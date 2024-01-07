//==================================================================//
// Type: J++ Transpiled Code                                        //
// Date: Sat Jan  6 04:59:02 2024 HST                               //
//==================================================================//

#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>

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
typedef const char *string;
typedef std::vector<uint8_t> buffer;
typedef void *routine;
#define null nullptr
using std::vector;

//==================================================================//
// File: /tmp/fileJXycWK.cpp                                        //
//==================================================================//

namespace jxx
{
    typedef qword typeid_t;

    static std::map<typeid_t, string> m_typenames = {
        {0, "TypeA"},
        {1, "TypeB"},
    };

    template <typeid_t T>
    class StructGeneric
    {
    protected:
        static const typeid_t m_typeid = T;
        static std::map<typeid_t, std::map<string, const void *>> m_attributes;

    public:
        typeid_t _typeid()
        {
            return m_typeid;
        }

        string _typename()
        {
            if (m_typenames.find(m_typeid) == m_typenames.end())
                return "";
            return m_typenames[m_typeid];
        }

        bool _has(string name)
        {
            return m_attributes[m_typeid].find(name) != m_attributes[m_typeid].end();
        }

        void _set(string name, string value)
        {
            m_attributes[m_typeid][name] = value;
        }

        void _set(string name, long value)
        {
            m_attributes[m_typeid][name] = (void *)value;
        }

        string _get(string name)
        {
            if (!this->_has(name))
                return "";
            return (string)m_attributes[m_typeid][name];
        }

        long _getint(string name)
        {
            if (!this->_has(name))
                return 0;
            return (qword)m_attributes[m_typeid][name];
        }
    };

    template <typeid_t T>
    std::map<typeid_t, std::map<string, const void *>> StructGeneric<T>::m_attributes;
    template <typeid_t T>
    const typeid_t StructGeneric<T>::m_typeid;

    static const char jcc__TypeA_typename[] = "TypeA";
#pragma pack(push, 1)
    class TypeA : public StructGeneric<0>
    {
    public:
        dword a = 0;
        word b = 0;
        word c = 0;
        word d = 0;
        dword e = 0;
        word f = 0;

        TypeA()
        {
            this->_set("testA", "Hello World");
        }
    };
#pragma pack(pop)

    static const char jcc__TypeB_typename[] = "TypeB";
#pragma pack(push, 1)
    class TypeB : public StructGeneric<1>
    {
    public:
        dword a = 0;
        word b = 0;
        word c = 0;
        word d = 0;
        dword e = 0;
        word f = 0;

        TypeB()
        {
            this->_set("testB", 100);
        }
    };
#pragma pack(pop)

}

using namespace jxx;

int main()
{
    TypeA a;
    TypeB b;

    std::cout << "a: " << a._typeid() << std::endl;
    std::cout << "b: " << b._typeid() << std::endl;
    std::cout << "a: " << a._typename() << std::endl;
    std::cout << "b: " << b._typename() << std::endl;

    std::cout << "sizeof(TypeA): " << sizeof(TypeA) << std::endl;
    std::cout << "sizeof(TypeB): " << sizeof(TypeB) << std::endl;

    std::cout << "a.test: " << a._get("testA") << std::endl;
    std::cout << "b.test: " << b._getint("testB") << std::endl;

    a._set("testA", 100);
    b._set("test", 200);

    std::cout << "a.test: " << a._getint("test") << std::endl;
    std::cout << "b.test: " << b._getint("test") << std::endl;

    return 0;
}

//==================================================================//
// EOF: /tmp/fileJXycWK.cpp                                         //
//==================================================================//

//==================================================================//
// EOF: J++ Transpiled Code                                         //
// SHA256:                                                          //
// b1b5e424dcfeb96cab4b94a0e357e96327e25ada53adfcd7da5ab6f91412c305 //
//==================================================================//
