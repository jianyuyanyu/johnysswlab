#include "fix16.h"
#include "utils.h"
#include <iostream>

fix16_t fix16_mul(fix16_t inArg0, fix16_t inArg1)
{
	int64_t product = (int64_t)inArg0 * inArg1;
	
	#ifndef FIXMATH_NO_OVERFLOW
	// The upper 17 bits should all be the same (the sign).
	uint32_t upper = (product >> 47);
	#endif
	
	if (product < 0)
	{
		#ifndef FIXMATH_NO_OVERFLOW
		if (~upper)
				return fix16_overflow;
		#endif
		
		#ifndef FIXMATH_NO_ROUNDING
		// This adjustment is required in order to round -1/2 correctly
		product--;
		#endif
	}
	else
	{
		#ifndef FIXMATH_NO_OVERFLOW
		if (upper)
				return fix16_overflow;
		#endif
	}
	
	#ifdef FIXMATH_NO_ROUNDING
	return product >> 16;
	#else
	fix16_t result = product >> 16;
	result += (product & 0x8000) >> 15;
	
	return result;
	#endif
}

#ifdef __GNUC__
// Count leading zeros, using processor-specific instruction if available.
#define clz(x) (__builtin_clzl(x) - (8 * sizeof(long) - 32))
#else
static uint8_t clz(uint32_t x)
{
	uint8_t result = 0;
	if (x == 0) return 32;
	while (!(x & 0xF0000000)) { result += 4; x <<= 4; }
	while (!(x & 0x80000000)) { result += 1; x <<= 1; }
	return result;
}
#endif

fix16_t fix16_div(fix16_t a, fix16_t b)
{
	// This uses a hardware 32/32 bit division multiple times, until we have
	// computed all the bits in (a<<17)/b. Usually this takes 1-3 iterations.
	
	if (b == 0)
			return fix16_minimum;
	
	uint32_t remainder = (a >= 0) ? a : (-a);
	uint32_t divider = (b >= 0) ? b : (-b);
	uint32_t quotient = 0;
	int bit_pos = 17;
	
	// Kick-start the division a bit.
	// This improves speed in the worst-case scenarios where N and D are large
	// It gets a lower estimate for the result by N/(D >> 17 + 1).
	if (divider & 0xFFF00000)
	{
		uint32_t shifted_div = ((divider >> 17) + 1);
		quotient = remainder / shifted_div;
		remainder -= ((uint64_t)quotient * divider) >> 17;
	}
	
	// If the divider is divisible by 2^n, take advantage of it.
	while (!(divider & 0xF) && bit_pos >= 4)
	{
		divider >>= 4;
		bit_pos -= 4;
	}
	
	while (remainder && bit_pos >= 0)
	{
		// Shift remainder as much as we can without overflowing
		int shift = clz(remainder);
		if (shift > bit_pos) shift = bit_pos;
		remainder <<= shift;
		bit_pos -= shift;
		
		uint32_t div = remainder / divider;
		remainder = remainder % divider;
		quotient += div << bit_pos;

		#ifndef FIXMATH_NO_OVERFLOW
		if (div & ~(0xFFFFFFFF >> bit_pos))
				return fix16_overflow;
		#endif
		
		remainder <<= 1;
		bit_pos--;
	}
	
	#ifndef FIXMATH_NO_ROUNDING
	// Quotient is always positive so rounding is easy
	quotient++;
	#endif
	
	fix16_t result = quotient >> 1;
	
	// Figure out the sign of the result
	if ((a ^ b) & 0x80000000)
	{
		#ifndef FIXMATH_NO_OVERFLOW
		if (result == fix16_minimum)
				return fix16_overflow;
		#endif
		
		result = -result;
	}
	
	return result;
}

fix16_t calculate_average(const std::vector<fix16_t>& v) {
    fix16_t current_average = v[0];
    fix16_t current_count = fix16_from_int(1);
    fix16_t next_count;

    for (int i = 0; i < v.size(); i++) {
        next_count = current_count + fix16_from_int(1);
        current_average = fix16_mul(current_average, fix16_div(current_count, next_count)) + fix16_div(v[i], next_count);
        current_count = next_count;
    }

    return current_average;
}

float calculate_average(const std::vector<float>& v) {
    float current_average = v[0];
    float current_count = 1.0;
    float next_count;

    for (int i = 1; i < v.size(); i++) {
        next_count = current_count + 1.0;
        current_average = current_average * (current_count / next_count) + v[i] / next_count;
        current_count = next_count;
    }

    return current_average;
}

float fast_calculate_average(const std::vector<float> &v) {
    float old_average = v[0];
    float old_count = 1.0

    int i = 1;
    while (i < v.size()) { 
        float current_sum = 0.0;
        float current_count = 0.0;
        while (current_sum < std::numeric_limits<float>::max() / 1000.0) {
            current_sum += v[i];
            current_count += 1.0;
            i++;
        }


        i++;
    }

}

int main(int argc, const char* argv[]) {
    const int arr_len = 10 * 1024 * 1024;
    std::vector<fix16_t> v = create_random_array<fix16_t>(arr_len, 0, arr_len);

    fix16_t r = calculate_average(v);

    std::cout << "Average is " << fix16_to_float(r) << std::endl;

    return 0;
}