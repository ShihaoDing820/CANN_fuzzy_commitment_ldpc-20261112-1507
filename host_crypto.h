#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

constexpr std::size_t kSha256Bytes = 32;

std::array<std::uint8_t, kSha256Bytes> Sha256(const std::uint8_t* data, std::size_t size);

bool ConstantTimeDigestEqual(
    const std::array<std::uint8_t, kSha256Bytes>& left,
    const std::array<std::uint8_t, kSha256Bytes>& right);
