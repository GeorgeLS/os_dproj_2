#ifndef EXERCISE_II__COMMON_H_
#define EXERCISE_II__COMMON_H_

#include <cstdint>
#define global static
#define local static
#define internal static

#define DISALLOW_COPY(Typename) \
	Typename(const Typename &rhs) = delete; \
	Typename& operator=(const Typename &rhs) = delete;

#define DISALLOW_MOVE(Typename) \
	Typename(Typename &&rhs) = delete; \
	Typename& operator=(Typename &&rhs) = delete;

#define DISALLOW_COPY_AND_MOVE(Typename) \
	DISALLOW_COPY(Typename) \
	DISALLOW_MOVE(Typename)

using byte = uint8_t;
using u8 = uint8_t;
using i8 = int8_t ;
using u16 = uint16_t;
using i16 = int16_t;
using u32 = uint32_t;
using i32 = int32_t;
using u64 = uint64_t;
using i64 = int64_t;
using f32 = float;
using f64 = double;

#endif //EXERCISE_II__COMMON_H_
