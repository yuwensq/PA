// #include "FLOAT.h"
// #include <stdint.h>
// #include <assert.h>
// struct float_
// {
//   uint32_t frac : 23;
//   uint32_t exp : 8;
//   uint32_t sign : 1;
// };
// FLOAT F_mul_F(FLOAT a, FLOAT b)
// {
//   // assert(0);
//   FLOAT res = ((a * b) >> 16);
//   return res;
// }

// FLOAT F_div_F(FLOAT a, FLOAT b)
// {
//   // assert(0);
//   FLOAT res = ((a / b) << 16);
//   return res;
// }

// FLOAT f2F(float a)
// {
//   /* You should figure out how to convert `a' into FLOAT without
//    * introducing x87 floating point instructions. Else you can
//    * not run this code in NEMU before implementing x87 floating
//    * point instructions, which is contrary to our expectation.
//    *
//    * Hint: The bit representation of `a' is already on the
//    * stack. How do you retrieve it to another variable without
//    * performing arithmetic operations on it directly?
//    */
//   struct float_ *f = (struct float_ *)&a;
//   uint32_t res;
//   uint32_t frac;
//   int exp;
//   if ((f->exp & 0xff) == 0xff)
//     assert(0);
//   else if (f->exp == 0)
//   {
//     exp = 1 - 127;
//     frac = (f->frac & 0x7fffff);
//   }
//   else
//   {
//     exp = f->exp - 127;
//     frac = (f->frac & 0x7fffff) | (1 << 23);
//   }
//   if (exp >= 7 && exp < 22)
//     res = frac << (exp - 7);
//   else if (exp < 7 && exp > -32)
//     res = frac >> 7 >> -exp;
//   else
//     assert(0);
//   return (f->sign) ? -res : res;
//   // // assert(0);
//   // const int e_min = 126; // e_min是负的
//   // const int e_max = 127;
//   // unsigned int bits = *(unsigned int *)(void *)(&a);
//   // unsigned int s = ((bits & 0x80000000) >> 31);
//   // unsigned int e = ((bits & 0x7f800000) >> 23);
//   // unsigned int m = (bits & 0x007fffff);
//   // FLOAT res = 0;
//   // if (e == 0)
//   // {
//   //   // 这里，只要e==0，就算m不等于零，用FLOAT也表示不了那么小的数，直接给零吧
//   //   // if (m == 0)
//   //   //   res = 0;
//   //   // else {
//   //   //   res = 0;
//   //   // }
//   //   res = 0;
//   // }
//   // else if (e == 128)
//   // { // 要不是无穷，要不是nan，先不处理吧
//   //   assert(0);
//   // }
//   // else
//   // {
//   //   m = (0x00800000 | m);
//   //   m >>= 7;
//   //   if (e >= 127)
//   //     m <<= (e - 127);
//   //   else
//   //     m >>= (127 - e);
//   //   if (s == 1)
//   //     res = -((int)m);
//   //   else
//   //     res = m;
//   // }
//   // return res;
// }

// FLOAT Fabs(FLOAT a)
// {
//   // assert(0);
//   FLOAT res = (a > 0 ? a : -a);
//   return res;
// }

// /* Functions below are already implemented */

// FLOAT Fsqrt(FLOAT x)
// {
//   FLOAT dt, t = int2F(2);

//   do
//   {
//     dt = F_div_int((F_div_F(x, t) - t), 2);
//     t += dt;
//   } while (Fabs(dt) > f2F(1e-4));

//   return t;
// }

// FLOAT Fpow(FLOAT x, FLOAT y)
// {
//   /* we only compute x^0.333 */
//   FLOAT t2, dt, t = int2F(2);

//   do
//   {
//     t2 = F_mul_F(t, t);
//     dt = (F_div_F(x, t2) - t) / 3;
//     t += dt;
//   } while (Fabs(dt) > f2F(1e-4));

//   return t;
// }
#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>
struct float_
{
  uint32_t frac : 23;
  uint32_t exp : 8;
  uint32_t sign : 1;
};
FLOAT F_mul_F(FLOAT a, FLOAT b)
{
  // assert(0);
  // return 0;
  return ((int64_t)a * (int64_t)b) >> 16;
}

FLOAT F_div_F(FLOAT a, FLOAT b)
{
  // assert(0);
  // return 0;
  assert(b != 0);
  FLOAT x = Fabs(a);
  FLOAT y = Fabs(b);
  FLOAT z = x / y;
  x = x % y;

  for (int i = 0; i < 16; i++)
  {
    x <<= 1;
    z <<= 1;
    if (x >= y)
    {
      x -= y;
      z++;
    }
  }
  if (((a ^ b) & 0x80000000) == 0x80000000)
  {
    z = -z;
  }
  return z;
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

  struct float_ *f = (struct float_ *)&a;
  uint32_t res;
  uint32_t frac;
  int exp;
  if ((f->exp & 0xff) == 0xff)
    assert(0);
  else if (f->exp == 0)
  {
    exp = 1 - 127;
    frac = (f->frac & 0x7fffff);
  }
  else
  {
    exp = f->exp - 127;
    frac = (f->frac & 0x7fffff) | (1 << 23);
  }
  if (exp >= 7 && exp < 22)
    res = frac << (exp - 7);
  else if (exp < 7 && exp > -32)
    res = frac >> 7 >> -exp;
  else
    assert(0);
  return (f->sign) ? -res : res;
}

FLOAT Fabs(FLOAT a)
{
  return (a > 0) ? a : -a;
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