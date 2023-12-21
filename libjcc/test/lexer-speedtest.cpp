#include "compile.hpp"
#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <openssl/md5.h>

using namespace jcc;

static std::string program = "LyoqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKgoqIEZpbGVuYW1lOiAgIHNoYTI1Ni5jCiogQXV0aG9yOiAgICAgQnJhZCBDb250ZSAoYnJhZCBBVCBicmFkY29udGUuY29tKQoqIENvcHlyaWdodDoKKiBEaXNjbGFpbWVyOiBUaGlzIGNvZGUgaXMgcHJlc2VudGVkICJhcyBpcyIgd2l0aG91dCBhbnkgZ3VhcmFudGVlcy4KKiBEZXRhaWxzOiAgICBJbXBsZW1lbnRhdGlvbiBvZiB0aGUgU0hBLTI1NiBoYXNoaW5nIGFsZ29yaXRobS4KICAgICAgICAgICAgICBTSEEtMjU2IGlzIG9uZSBvZiB0aGUgdGhyZWUgYWxnb3JpdGhtcyBpbiB0aGUgU0hBMgogICAgICAgICAgICAgIHNwZWNpZmljYXRpb24uIFRoZSBvdGhlcnMsIFNIQS0zODQgYW5kIFNIQS01MTIsIGFyZSBub3QKICAgICAgICAgICAgICBvZmZlcmVkIGluIHRoaXMgaW1wbGVtZW50YXRpb24uCiAgICAgICAgICAgICAgQWxnb3JpdGhtIHNwZWNpZmljYXRpb24gY2FuIGJlIGZvdW5kIGhlcmU6CiAgICAgICAgICAgICAgICogaHR0cDovL2NzcmMubmlzdC5nb3YvcHVibGljYXRpb25zL2ZpcHMvZmlwczE4MC0yL2ZpcHMxODAtMndpdGhjaGFuZ2Vub3RpY2UucGRmCiAgICAgICAgICAgICAgVGhpcyBpbXBsZW1lbnRhdGlvbiB1c2VzIGxpdHRsZSBlbmRpYW4gYnl0ZSBvcmRlci4KKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqLwoKLyoqKioqKioqKioqKioqKioqKioqKioqKioqKiBIRUFERVIgRklMRVMgKioqKioqKioqKioqKioqKioqKioqKioqKioqLwoKI2lmICFkZWZpbmVkKF9fbGludXhfXykKCiNpbmNsdWRlIDxzdGRsaWIuaD4KI2luY2x1ZGUgPG1lbW9yeS5oPgojaW5jbHVkZSAic2hhMjU2LmgiCgovKioqKioqKioqKioqKioqKioqKioqKioqKioqKioqIE1BQ1JPUyAqKioqKioqKioqKioqKioqKioqKioqKioqKioqKiovCiNkZWZpbmUgUk9UTEVGVChhLCBiKSAoKChhKSA8PCAoYikpIHwgKChhKSA+PiAoMzIgLSAoYikpKSkKI2RlZmluZSBST1RSSUdIVChhLCBiKSAoKChhKSA+PiAoYikpIHwgKChhKSA8PCAoMzIgLSAoYikpKSkKCiNkZWZpbmUgQ0goeCwgeSwgeikgKCgoeCkgJiAoeSkpIF4gKH4oeCkgJiAoeikpKQojZGVmaW5lIE1BSih4LCB5LCB6KSAoKCh4KSAmICh5KSkgXiAoKHgpICYgKHopKSBeICgoeSkgJiAoeikpKQojZGVmaW5lIEVQMCh4KSAoUk9UUklHSFQoeCwgMikgXiBST1RSSUdIVCh4LCAxMykgXiBST1RSSUdIVCh4LCAyMikpCiNkZWZpbmUgRVAxKHgpIChST1RSSUdIVCh4LCA2KSBeIFJPVFJJR0hUKHgsIDExKSBeIFJPVFJJR0hUKHgsIDI1KSkKI2RlZmluZSBTSUcwKHgpIChST1RSSUdIVCh4LCA3KSBeIFJPVFJJR0hUKHgsIDE4KSBeICgoeCkgPj4gMykpCiNkZWZpbmUgU0lHMSh4KSAoUk9UUklHSFQoeCwgMTcpIF4gUk9UUklHSFQoeCwgMTkpIF4gKCh4KSA+PiAxMCkpCgovKioqKioqKioqKioqKioqKioqKioqKioqKioqKiBWQVJJQUJMRVMgKioqKioqKioqKioqKioqKioqKioqKioqKioqKiovCnN0YXRpYyBjb25zdCBXT1JEIGtbNjRdID0gewogICAgMHg0MjhhMmY5OCwgMHg3MTM3NDQ5MSwgMHhiNWMwZmJjZiwgMHhlOWI1ZGJhNSwgMHgzOTU2YzI1YiwgMHg1OWYxMTFmMSwgMHg5MjNmODJhNCwgMHhhYjFjNWVkNSwKICAgIDB4ZDgwN2FhOTgsIDB4MTI4MzViMDEsIDB4MjQzMTg1YmUsIDB4NTUwYzdkYzMsIDB4NzJiZTVkNzQsIDB4ODBkZWIxZmUsIDB4OWJkYzA2YTcsIDB4YzE5YmYxNzQsCiAgICAweGU0OWI2OWMxLCAweGVmYmU0Nzg2LCAweDBmYzE5ZGM2LCAweDI0MGNhMWNjLCAweDJkZTkyYzZmLCAweDRhNzQ4NGFhLCAweDVjYjBhOWRjLCAweDc2Zjk4OGRhLAogICAgMHg5ODNlNTE1MiwgMHhhODMxYzY2ZCwgMHhiMDAzMjdjOCwgMHhiZjU5N2ZjNywgMHhjNmUwMGJmMywgMHhkNWE3OTE0NywgMHgwNmNhNjM1MSwgMHgxNDI5Mjk2NywKICAgIDB4MjdiNzBhODUsIDB4MmUxYjIxMzgsIDB4NGQyYzZkZmMsIDB4NTMzODBkMTMsIDB4NjUwYTczNTQsIDB4NzY2YTBhYmIsIDB4ODFjMmM5MmUsIDB4OTI3MjJjODUsCiAgICAweGEyYmZlOGExLCAweGE4MWE2NjRiLCAweGMyNGI4YjcwLCAweGM3NmM1MWEzLCAweGQxOTJlODE5LCAweGQ2OTkwNjI0LCAweGY0MGUzNTg1LCAweDEwNmFhMDcwLAogICAgMHgxOWE0YzExNiwgMHgxZTM3NmMwOCwgMHgyNzQ4Nzc0YywgMHgzNGIwYmNiNSwgMHgzOTFjMGNiMywgMHg0ZWQ4YWE0YSwgMHg1YjljY2E0ZiwgMHg2ODJlNmZmMywKICAgIDB4NzQ4ZjgyZWUsIDB4NzhhNTYzNmYsIDB4ODRjODc4MTQsIDB4OGNjNzAyMDgsIDB4OTBiZWZmZmEsIDB4YTQ1MDZjZWIsIDB4YmVmOWEzZjcsIDB4YzY3MTc4ZjJ9OwoKLyoqKioqKioqKioqKioqKioqKioqKioqIEZVTkNUSU9OIERFRklOSVRJT05TICoqKioqKioqKioqKioqKioqKioqKioqLwp2b2lkIHNoYTI1Nl90cmFuc2Zvcm0oU0hBMjU2X0NUWCAqY3R4LCBjb25zdCBCWVRFIGRhdGFbXSkKewogICAgV09SRCBhLCBiLCBjLCBkLCBlLCBmLCBnLCBoLCBpLCBqLCB0MSwgdDIsIG1bNjRdOwoKICAgIGZvciAoaSA9IDAsIGogPSAwOyBpIDwgMTY7ICsraSwgaiArPSA0KQogICAgICAgIG1baV0gPSAoZGF0YVtqXSA8PCAyNCkgfCAoZGF0YVtqICsgMV0gPDwgMTYpIHwgKGRhdGFbaiArIDJdIDw8IDgpIHwgKGRhdGFbaiArIDNdKTsKICAgIGZvciAoOyBpIDwgNjQ7ICsraSkKICAgICAgICBtW2ldID0gU0lHMShtW2kgLSAyXSkgKyBtW2kgLSA3XSArIFNJRzAobVtpIC0gMTVdKSArIG1baSAtIDE2XTsKCiAgICBhID0gY3R4LT5zdGF0ZVswXTsKICAgIGIgPSBjdHgtPnN0YXRlWzFdOwogICAgYyA9IGN0eC0+c3RhdGVbMl07CiAgICBkID0gY3R4LT5zdGF0ZVszXTsKICAgIGUgPSBjdHgtPnN0YXRlWzRdOwogICAgZiA9IGN0eC0+c3RhdGVbNV07CiAgICBnID0gY3R4LT5zdGF0ZVs2XTsKICAgIGggPSBjdHgtPnN0YXRlWzddOwoKICAgIGZvciAoaSA9IDA7IGkgPCA2NDsgKytpKQogICAgewogICAgICAgIHQxID0gaCArIEVQMShlKSArIENIKGUsIGYsIGcpICsga1tpXSArIG1baV07CiAgICAgICAgdDIgPSBFUDAoYSkgKyBNQUooYSwgYiwgYyk7CiAgICAgICAgaCA9IGc7CiAgICAgICAgZyA9IGY7CiAgICAgICAgZiA9IGU7CiAgICAgICAgZSA9IGQgKyB0MTsKICAgICAgICBkID0gYzsKICAgICAgICBjID0gYjsKICAgICAgICBiID0gYTsKICAgICAgICBhID0gdDEgKyB0MjsKICAgIH0KCiAgICBjdHgtPnN0YXRlWzBdICs9IGE7CiAgICBjdHgtPnN0YXRlWzFdICs9IGI7CiAgICBjdHgtPnN0YXRlWzJdICs9IGM7CiAgICBjdHgtPnN0YXRlWzNdICs9IGQ7CiAgICBjdHgtPnN0YXRlWzRdICs9IGU7CiAgICBjdHgtPnN0YXRlWzVdICs9IGY7CiAgICBjdHgtPnN0YXRlWzZdICs9IGc7CiAgICBjdHgtPnN0YXRlWzddICs9IGg7Cn0KCnZvaWQgc2hhMjU2X2luaXQoU0hBMjU2X0NUWCAqY3R4KQp7CiAgICBjdHgtPmRhdGFsZW4gPSAwOwogICAgY3R4LT5iaXRsZW4gPSAwOwogICAgY3R4LT5zdGF0ZVswXSA9IDB4NmEwOWU2Njc7CiAgICBjdHgtPnN0YXRlWzFdID0gMHhiYjY3YWU4NTsKICAgIGN0eC0+c3RhdGVbMl0gPSAweDNjNmVmMzcyOwogICAgY3R4LT5zdGF0ZVszXSA9IDB4YTU0ZmY1M2E7CiAgICBjdHgtPnN0YXRlWzRdID0gMHg1MTBlNTI3ZjsKICAgIGN0eC0+c3RhdGVbNV0gPSAweDliMDU2ODhjOwogICAgY3R4LT5zdGF0ZVs2XSA9IDB4MWY4M2Q5YWI7CiAgICBjdHgtPnN0YXRlWzddID0gMHg1YmUwY2QxOTsKfQoKdm9pZCBzaGEyNTZfdXBkYXRlKFNIQTI1Nl9DVFggKmN0eCwgY29uc3QgQllURSBkYXRhW10sIHNpemVfdCBsZW4pCnsKICAgIFdPUkQgaTsKCiAgICBmb3IgKGkgPSAwOyBpIDwgbGVuOyArK2kpCiAgICB7CiAgICAgICAgY3R4LT5kYXRhW2N0eC0+ZGF0YWxlbl0gPSBkYXRhW2ldOwogICAgICAgIGN0eC0+ZGF0YWxlbisrOwogICAgICAgIGlmIChjdHgtPmRhdGFsZW4gPT0gNjQpCiAgICAgICAgewogICAgICAgICAgICBzaGEyNTZfdHJhbnNmb3JtKGN0eCwgY3R4LT5kYXRhKTsKICAgICAgICAgICAgY3R4LT5iaXRsZW4gKz0gNTEyOwogICAgICAgICAgICBjdHgtPmRhdGFsZW4gPSAwOwogICAgICAgIH0KICAgIH0KfQoKdm9pZCBzaGEyNTZfZmluYWwoU0hBMjU2X0NUWCAqY3R4LCBCWVRFIGhhc2hbXSkKewogICAgV09SRCBpOwoKICAgIGkgPSBjdHgtPmRhdGFsZW47CgogICAgLy8gUGFkIHdoYXRldmVyIGRhdGEgaXMgbGVmdCBpbiB0aGUgYnVmZmVyLgogICAgaWYgKGN0eC0+ZGF0YWxlbiA8IDU2KQogICAgewogICAgICAgIGN0eC0+ZGF0YVtpKytdID0gMHg4MDsKICAgICAgICB3aGlsZSAoaSA8IDU2KQogICAgICAgICAgICBjdHgtPmRhdGFbaSsrXSA9IDB4MDA7CiAgICB9CiAgICBlbHNlCiAgICB7CiAgICAgICAgY3R4LT5kYXRhW2krK10gPSAweDgwOwogICAgICAgIHdoaWxlIChpIDwgNjQpCiAgICAgICAgICAgIGN0eC0+ZGF0YVtpKytdID0gMHgwMDsKICAgICAgICBzaGEyNTZfdHJhbnNmb3JtKGN0eCwgY3R4LT5kYXRhKTsKICAgICAgICBtZW1zZXQoY3R4LT5kYXRhLCAwLCA1Nik7CiAgICB9CgogICAgLy8gQXBwZW5kIHRvIHRoZSBwYWRkaW5nIHRoZSB0b3RhbCBtZXNzYWdlJ3MgbGVuZ3RoIGluIGJpdHMgYW5kIHRyYW5zZm9ybS4KICAgIGN0eC0+Yml0bGVuICs9IGN0eC0+ZGF0YWxlbiAqIDg7CiAgICBjdHgtPmRhdGFbNjNdID0gY3R4LT5iaXRsZW47CiAgICBjdHgtPmRhdGFbNjJdID0gY3R4LT5iaXRsZW4gPj4gODsKICAgIGN0eC0+ZGF0YVs2MV0gPSBjdHgtPmJpdGxlbiA+PiAxNjsKICAgIGN0eC0+ZGF0YVs2MF0gPSBjdHgtPmJpdGxlbiA+PiAyNDsKICAgIGN0eC0+ZGF0YVs1OV0gPSBjdHgtPmJpdGxlbiA+PiAzMjsKICAgIGN0eC0+ZGF0YVs1OF0gPSBjdHgtPmJpdGxlbiA+PiA0MDsKICAgIGN0eC0+ZGF0YVs1N10gPSBjdHgtPmJpdGxlbiA+PiA0ODsKICAgIGN0eC0+ZGF0YVs1Nl0gPSBjdHgtPmJpdGxlbiA+PiA1NjsKICAgIHNoYTI1Nl90cmFuc2Zvcm0oY3R4LCBjdHgtPmRhdGEpOwoKICAgIC8vIFNpbmNlIHRoaXMgaW1wbGVtZW50YXRpb24gdXNlcyBsaXR0bGUgZW5kaWFuIGJ5dGUgb3JkZXJpbmcgYW5kIFNIQSB1c2VzIGJpZyBlbmRpYW4sCiAgICAvLyByZXZlcnNlIGFsbCB0aGUgYnl0ZXMgd2hlbiBjb3B5aW5nIHRoZSBmaW5hbCBzdGF0ZSB0byB0aGUgb3V0cHV0IGhhc2guCiAgICBmb3IgKGkgPSAwOyBpIDwgNDsgKytpKQogICAgewogICAgICAgIGhhc2hbaV0gPSAoY3R4LT5zdGF0ZVswXSA+PiAoMjQgLSBpICogOCkpICYgMHgwMDAwMDBmZjsKICAgICAgICBoYXNoW2kgKyA0XSA9IChjdHgtPnN0YXRlWzFdID4+ICgyNCAtIGkgKiA4KSkgJiAweDAwMDAwMGZmOwogICAgICAgIGhhc2hbaSArIDhdID0gKGN0eC0+c3RhdGVbMl0gPj4gKDI0IC0gaSAqIDgpKSAmIDB4MDAwMDAwZmY7CiAgICAgICAgaGFzaFtpICsgMTJdID0gKGN0eC0+c3RhdGVbM10gPj4gKDI0IC0gaSAqIDgpKSAmIDB4MDAwMDAwZmY7CiAgICAgICAgaGFzaFtpICsgMTZdID0gKGN0eC0+c3RhdGVbNF0gPj4gKDI0IC0gaSAqIDgpKSAmIDB4MDAwMDAwZmY7CiAgICAgICAgaGFzaFtpICsgMjBdID0gKGN0eC0+c3RhdGVbNV0gPj4gKDI0IC0gaSAqIDgpKSAmIDB4MDAwMDAwZmY7CiAgICAgICAgaGFzaFtpICsgMjRdID0gKGN0eC0+c3RhdGVbNl0gPj4gKDI0IC0gaSAqIDgpKSAmIDB4MDAwMDAwZmY7CiAgICAgICAgaGFzaFtpICsgMjhdID0gKGN0eC0+c3RhdGVbN10gPj4gKDI0IC0gaSAqIDgpKSAmIDB4MDAwMDAwZmY7CiAgICB9Cn0KCiNlbmRpZiAvLyAhZGVmaW5lZChfX2xpbnV4X18pCg==";

std::unordered_map<char, int> base64_map = {
    {'A', 0}, {'B', 1}, {'C', 2}, {'D', 3}, {'E', 4}, {'F', 5}, {'G', 6}, {'H', 7}, {'I', 8}, {'J', 9}, {'K', 10}, {'L', 11}, {'M', 12}, {'N', 13}, {'O', 14}, {'P', 15}, {'Q', 16}, {'R', 17}, {'S', 18}, {'T', 19}, {'U', 20}, {'V', 21}, {'W', 22}, {'X', 23}, {'Y', 24}, {'Z', 25}, {'a', 26}, {'b', 27}, {'c', 28}, {'d', 29}, {'e', 30}, {'f', 31}, {'g', 32}, {'h', 33}, {'i', 34}, {'j', 35}, {'k', 36}, {'l', 37}, {'m', 38}, {'n', 39}, {'o', 40}, {'p', 41}, {'q', 42}, {'r', 43}, {'s', 44}, {'t', 45}, {'u', 46}, {'v', 47}, {'w', 48}, {'x', 49}, {'y', 50}, {'z', 51}, {'0', 52}, {'1', 53}, {'2', 54}, {'3', 55}, {'4', 56}, {'5', 57}, {'6', 58}, {'7', 59}, {'8', 60}, {'9', 61}, {'+', 62}, {'/', 63}};

std::string base64_decode(const std::string &input)
{
    std::string result;
    result.reserve(input.size() * 3 / 4);

    for (size_t i = 0; i < input.size(); i += 4)
    {
        uint32_t tmp = 0;
        for (size_t j = 0; j < 4; ++j)
        {
            if (input[i + j] != '=')
            {
                tmp |= base64_map[input[i + j]] << (18 - j * 6);
            }
        }
        for (size_t j = 0; j < 3; ++j)
        {
            if (input[i + j + 1] != '=')
            {
                result.push_back(static_cast<char>((tmp >> (16 - j * 8)) & 0xFF));
            }
        }
    }
    return result;
}

bool verify_result(const std::string &result)
{
    // compute md5 of result to check if output has changed from expected
    // this does not imply that the output is correct, just that it is the same as before
    const std::string correct = "ff31328172ac0642c444bb42c9e08450";
    char digest[16];
    std::string hex_digest;

    MD5((unsigned char *)result.c_str(), result.size(), (unsigned char *)&digest);

    for (int i = 0; i < 16; i++)
    {
        char buf[3];
        sprintf(buf, "%02x", (unsigned char)digest[i]);
        hex_digest += buf;
    }

    return hex_digest == correct;
}

int main()
{
    // Start measuring time using the rdtsc instruction
    uint64_t start, end;
    jcc::TokenList list;
    std::vector<uint64_t> times;

    jcc::CompilationUnit unit;

    std::string raw = base64_decode(program);

    for (size_t i = 0; i < 400; i++)
    {
        start = 0;
        end = 0;
        start = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

        // Call the function you want to measure
        list = unit.lex(raw);

        end = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        // Calculate the elapsed clock cycles
        times.push_back(end - start);
    }

    if (!verify_result(list.to_json()))
    {
        std::cout << "Lexer result is \x1b[31;4mnot\x1b[0m correct" << std::endl;
    }
    else
    {
        std::cout << "Lexer result \x1b[32;4mis correct\x1b[0m \n"
                  << std::endl;
    }

    // Calculate the average clock cycles
    uint64_t sum = 0;
    for (auto &time : times)
    {
        sum += time;
    }

    double avg = sum / times.size();
    std::cout << "Total tokens: " << list.size() << std::endl;
    std::cout << "Avg total: " << (uint64_t)avg << " ns" << std::endl;
    std::cout << "Avg compute time: " << avg / (raw.size() * 8) << " ns/bit" << std::endl;

    if (list.size() == 0)
    {
        std::cout << "Lexer failed" << std::endl;
        return 1;
    }

    std::cout << "Avg: " << avg / list.size() << " ns/token" << std::endl;

    return 0;
}