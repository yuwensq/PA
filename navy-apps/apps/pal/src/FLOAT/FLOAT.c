#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>

FLOAT F_mul_F(FLOAT a, FLOAT b)
{
  // assert(0);
  FLOAT res = (((int64_t)a * (int64_t)b) >> 16);
  return res;
}

FLOAT F_div_F(FLOAT a, FLOAT b)
{
  // assert(0);
  int is_neg = ((a < 0) != (b < 0));
  // 这里如果简单的让res = ((a / b) << 16)，会丢失小数部分
  FLOAT res = Fabs(a) / Fabs(b);
  int rem = Fabs(a) % Fabs(b);
  rem <<= 16;
  rem /= b;
  res <<= 16;
  res |= rem;
  if (is_neg)
    res = -res;
  return res;
}

FLOAT f2F(float a)
{
  /* You should figure out how to convert `a' into FLOAT without
   * introducing x87 floating point instructions. Else you can
   * not run this code in NEMU before implementing x87 floating
   * point instructions, which is contrary to our expectation.
   *
   * Hint: The bit representation of `a' is already on the
   * stack. How do you retrieve it to another variable without
   * performing arithmetic operations on it directly?
   */
  // assert(0);
  unsigned int bits = *(unsigned int *)(void *)(&a);
  unsigned int s = ((bits & 0x80000000) >> 31);
  unsigned int e = ((bits & 0x7f800000) >> 23);
  unsigned int m = (bits & 0x007fffff);
  FLOAT res = 0;
  if (e == 0)
  {
    // 这里，只要e==0，就算m不等于零，用FLOAT也表示不了那么小的数，直接给零吧
    // if (m == 0)
    //   res = 0;
    // else {
    //   res = 0;
    // }
    res = 0;
  }
  else if (e == 128)
  { // 要不是无穷，要不是nan，先不处理吧
    assert(0);
  }
  else
  {
    m = (0x00800000 | m);
    m >>= 7;
    if (e >= 127)
      m <<= (e - 127);
    else
      m >>= (127 - e);
    if (s == 1)
      res = -((int)m);
    else
      res = m;
  }
  return res;
}

FLOAT Fabs(FLOAT a)
{
  // assert(0);
  FLOAT res = (a > 0 ? a : -a);
  return res;
}

/* Functions below are already implemented */

FLOAT Fsqrt(FLOAT x)
{
  FLOAT dt, t = int2F(2);

  do
  {
    dt = F_div_int((F_div_F(x, t) - t), 2);
    t += dt;
  } while (Fabs(dt) > f2F(1e-4));

  return t;
}

FLOAT Fpow(FLOAT x, FLOAT y)
{
  /* we only compute x^0.333 */
  FLOAT t2, dt, t = int2F(2);

  do
  {
    t2 = F_mul_F(t, t);
    dt = (F_div_F(x, t2) - t) / 3;
    t += dt;
  } while (Fabs(dt) > f2F(1e-4));

  return t;
}