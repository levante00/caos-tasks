#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ieee754_clf.h"

float_class_t classify(double x) {
  int const kExponentBitNumber = 11;
  union {
    double d;
    uint64_t bytes;
  } u;
  u.d = x;

  int sign;
  int mantisa_bits_sum = 0;
  int exponent_bits_sum = 0;
  int index = 0;
  int j = 0;
  int z = 0;
  
  for (uint64_t i = (unsigned long)1 << 63; i > 0; i >>= 1) {
    if (index == 0) {
      sign = (u.bytes & i) != 0;
    } else if (index <= kExponentBitNumber) {
      exponent_bits_sum += (u.bytes & i) != 0;
      j++;
    } else {
      mantisa_bits_sum += (u.bytes & i) != 0;
      z++;
    }
    index++;
  }

  if (sign == 0 && mantisa_bits_sum == 0 && exponent_bits_sum == 0) {
    return Zero;
  } else if (sign == 1 && mantisa_bits_sum == 0 && exponent_bits_sum == 0) {
    return MinusZero;
  } else if (sign == 0 && mantisa_bits_sum == 0 &&
             exponent_bits_sum == kExponentBitNumber) {
    return Inf;
  } else if (sign == 1 && mantisa_bits_sum == 0 &&
             exponent_bits_sum == kExponentBitNumber) {
    return MinusInf;
  } else if (exponent_bits_sum == kExponentBitNumber) {
    return NaN;
  } else if (sign == 0 && exponent_bits_sum == 0) {
    return Denormal;
  } else if (sign == 1 && exponent_bits_sum == 0) {
    return MinusDenormal;
  } else if (sign == 0) {
    return Regular;
  } else if (sign == 1) {
    return MinusRegular;
  }

  return Zero;
}