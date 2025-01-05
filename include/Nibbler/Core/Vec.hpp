#pragma once

#include <cstddef>
#include <cstdint>

namespace Nibbler
{

typedef unsigned int	uint_t;
template<int L, typename T> struct vec;

template <class Tp>
struct vec<1, Tp>
{
	vec() = default;
	constexpr vec(Tp x): x(x) {}

	union { Tp x, r, s; };
};

template <class Tp>
struct vec<2, Tp>
{
	vec() = default;
	constexpr vec(Tp x, Tp y): x(x), y(y) {}

	union { Tp x, r, s; };
	union { Tp y, g, t; };
};

template <class Tp>
struct vec<3, Tp>
{
	vec() = default;
	constexpr vec(Tp x, Tp y, Tp z): x(x), y(y), z(z) {}

	union { Tp x, r, s; };
	union { Tp y, g, t; };
	union { Tp z, b, p; };
};

template <class Tp>
struct vec<4, Tp>
{
	vec() = default;
	constexpr vec(Tp x, Tp y, Tp z, Tp w): x(x), y(y), z(z), w(w) {}

	union { Tp x, r, s; };
	union { Tp y, g, t; };
	union { Tp z, b, p; };
	union { Tp w, a, q; };
};

typedef vec<1, bool>		bvec1;
typedef vec<1, float>		vec1;
typedef vec<1, double>		dvec1;
typedef vec<1, int>			ivec1;
typedef vec<1, int8_t>		i8vec1;
typedef vec<1, int16_t>		i16vec1;
typedef vec<1, int32_t>		i32vec1;
typedef vec<1, int64_t>		i64vec1;
typedef vec<1, uint_t>		uvec1;
typedef vec<1, uint8_t>		u8vec1;
typedef vec<1, uint16_t>	u16vec1;
typedef vec<1, uint32_t>	u32vec1;
typedef vec<1, uint64_t>	u64vec1;

typedef vec<2, bool>		bvec2;
typedef vec<2, float>		vec2;
typedef vec<2, double>		dvec2;
typedef vec<2, int>			ivec2;
typedef vec<2, int8_t>		i8vec2;
typedef vec<2, int16_t>		i16vec2;
typedef vec<2, int32_t>		i32vec2;
typedef vec<2, int64_t>		i64vec2;
typedef vec<2, uint_t>		uvec2;
typedef vec<2, uint8_t>		u8vec2;
typedef vec<2, uint16_t>	u16vec2;
typedef vec<2, uint32_t>	u32vec2;
typedef vec<2, uint64_t>	u64vec2;

typedef vec<3, bool>		bvec3;
typedef vec<3, float>		vec3;
typedef vec<3, double>		dvec3;
typedef vec<3, int>			ivec3;
typedef vec<3, int8_t>		i8vec3;
typedef vec<3, int16_t>		i16vec3;
typedef vec<3, int32_t>		i32vec3;
typedef vec<3, int64_t>		i64vec3;
typedef vec<3, uint_t>		uvec3;
typedef vec<3, uint8_t>		u8vec3;
typedef vec<3, uint16_t>	u16vec3;
typedef vec<3, uint32_t>	u32vec3;
typedef vec<3, uint64_t>	u64vec3;

typedef vec<4, bool>		bvec4;
typedef vec<4, float>		vec4;
typedef vec<4, double>		dvec4;
typedef vec<4, int>			ivec4;
typedef vec<4, int8_t>		i8vec4;
typedef vec<4, int16_t>		i16vec4;
typedef vec<4, int32_t>		i32vec4;
typedef vec<4, int64_t>		i64vec4;
typedef vec<4, uint_t>		uvec4;
typedef vec<4, uint8_t>		u8vec4;
typedef vec<4, uint16_t>	u16vec4;
typedef vec<4, uint32_t>	u32vec4;
typedef vec<4, uint64_t>	u64vec4;

} // Nibbler
