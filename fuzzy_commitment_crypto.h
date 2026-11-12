#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

constexpr std::size_t kFuzzyCommitmentSha256Bytes = 32;

std::array<std::uint8_t, kFuzzyCommitmentSha256Bytes> FuzzyCommitmentSha256(
    const std::uint8_t* data,
    std::size_t size);

bool FuzzyCommitmentConstantTimeDigestEqual(
    const std::array<std::uint8_t, kFuzzyCommitmentSha256Bytes>& left,
    const std::array<std::uint8_t, kFuzzyCommitmentSha256Bytes>& right);
