/*********************************************************************
 * Filename:   sha256.h
 * Author:     Brad Conte (brad AT bradconte.com)
 * Copyright:
 * Disclaimer: This code is presented "as is" without any guarantees.
 * Details:    Defines the API for the corresponding SHA1 implementation.
 * Adopted from: https://github.com/B-Con/crypto-algorithms
 *********************************************************************/

#if defined(_JCC_BACKEND_)
#ifndef SHA256_H
#define SHA256_H

/*************************** HEADER FILES ***************************/
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace jcc::crypto
{
    constexpr int SHA256_BLOCK_SIZE = 32; // SHA256 outputs a 32 byte digest

    struct SHA256_CTX
    {
        uint8_t data[64];
        uint32_t datalen;
        unsigned long long bitlen;
        uint32_t state[8];
    };

    /*********************** FUNCTION DECLARATIONS **********************/
    void sha256_init(SHA256_CTX &ctx);
    void sha256_update(SHA256_CTX &ctx, const void *data, size_t len);
    void sha256_final(SHA256_CTX &ctx, uint8_t *hash);

    void sha256(const void *data, size_t len, uint8_t *hash);

    /// @brief Return SHA256 hash as string (raw)
    /// @param data Data to hash
    /// @param len Length of data
    /// @return This is not encoded and is raw bytes.
    std::string sha256(const void *data, size_t len);

    /// @brief Return SHA256 hash as string (raw)
    /// @param data Data to hash
    /// @return This is not encoded and is raw bytes.
    std::string sha256(const std::string &data);

    /// @brief Return SHA256 hash as string (raw)
    /// @param data Data to hash
    /// @param Hash This is not encoded and is raw bytes.
    std::string sha256(const std::vector<uint8_t> &data);
}

#endif // SHA256_H

#endif // defined(_JCC_BACKEND_)