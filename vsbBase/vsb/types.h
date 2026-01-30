//
// Created by Alexander on 30.01.2026.
//

#pragma once
#include <cstdint>

namespace vsb
{
	using i8  = std::int8_t;
	using i16 = std::int16_t;
	using i32 = std::int32_t;
	using i64 = std::int64_t;

	using u8  = std::uint8_t;
	using u16 = std::uint16_t;
	using u32 = std::uint32_t;
	using u64 = std::uint64_t;

	using f32 = float;   // IEEE 754 single - reliably 32-bit in practice
	using f64 = double;  // IEEE 754 double - reliably 64-bit in practice
}


#define USE_VSB_TYPES using vsb::i8, vsb::i16, vsb::i32, vsb::i64, vsb::u8, vsb::u16, vsb::u32, vsb::u64, vsb::f32, vsb::f64;
#define USE_VSB_TYPES_INSIDE using i8 = vsb::i8; using i16 = vsb::i16; using i32 = vsb::i32; using i64 = vsb::i64; using u8 = vsb::u8; using u16 = vsb::u16; using u32 = vsb::u32; using u64 = vsb::u64; using f32 = vsb::f32; using f64 = vsb::f64;