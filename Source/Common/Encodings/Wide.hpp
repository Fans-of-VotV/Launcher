#pragma once

#include "Common/Encodings/Base.hpp"
#include "Common/Encodings/Utf16Base.hpp"

template <>
struct Encoding<wchar_t> : Utf16EncodingBase<wchar_t> {};
