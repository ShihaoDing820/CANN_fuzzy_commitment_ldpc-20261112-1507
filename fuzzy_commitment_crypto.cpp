#include "fuzzy_commitment_crypto.h"

#include <openssl/crypto.h>
#include <openssl/sha.h>

#include <stdexcept>

std::array<std::uint8_t, kFuzzyCommitmentSha256Bytes> FuzzyCommitmentSha256(
    const std::uint8_t* data,
    std::size_t size)
{
    std::array<std::uint8_t, kFuzzyCommitmentSha256Bytes> digest{};
    if (SHA256(data, size, digest.data()) == nullptr) {
        throw std::runtime_error("SHA-256 failed");
    }
    return digest;
}

bool FuzzyCommitmentConstantTimeDigestEqual(
    const std::array<std::uint8_t, kFuzzyCommitmentSha256Bytes>& left,
    const std::array<std::uint8_t, kFuzzyCommitmentSha256Bytes>& right)
{
    return CRYPTO_memcmp(left.data(), right.data(), left.size()) == 0;
}
