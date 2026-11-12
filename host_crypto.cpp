#include "host_crypto.h"

#include <openssl/crypto.h>
#include <openssl/sha.h>

#include <stdexcept>

std::array<std::uint8_t, kSha256Bytes> Sha256(const std::uint8_t* data, std::size_t size)
{
    std::array<std::uint8_t, kSha256Bytes> digest{};
    if (SHA256(data, size, digest.data()) == nullptr) {
        throw std::runtime_error("SHA-256 failed");
    }
    return digest;
}

bool ConstantTimeDigestEqual(
    const std::array<std::uint8_t, kSha256Bytes>& left,
    const std::array<std::uint8_t, kSha256Bytes>& right)
{
    return CRYPTO_memcmp(left.data(), right.data(), left.size()) == 0;
}
