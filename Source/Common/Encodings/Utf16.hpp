#pragma once

#include "Common/Encodings/Base.hpp"
#include "Common/Encodings/Utf16Base.hpp"

template <>
struct Encoding<char16_t> : Utf16EncodingBase<char16_t> {};
