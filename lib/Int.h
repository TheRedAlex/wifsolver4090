#pragma once
/*
 * This file is part of the BSGS distribution (https://github.com/JeanLucPons/Kangaroo).
 * GPLv3
 */

#ifndef BIGINTH
#define BIGINTH

#include <string>
#include <inttypes.h>

#if defined(_MSC_VER)
// ---------------- MSVC / Windows ----------------
  #include <intrin.h>
  #pragma intrinsic(_umul128)
  #pragma intrinsic(_addcarry_u64)
  #pragma intrinsic(_byteswap_uint64)

  static __forceinline unsigned long long mul_lohi_u64(unsigned long long a,
                                                       unsigned long long b,
                                                       unsigned long long* h) {
      return _umul128(a, b, h);
  }

  static __forceinline unsigned long long shrd64(unsigned long long lo,
                                                 unsigned long long hi,
                                                 unsigned n) {
      if (n == 0)  return lo;
      if (n < 64)  return (lo >> n) | (hi << (64 - n));
      if (n == 64) return hi;
      return (hi >> (n - 64));
  }

  static __forceinline unsigned long long shld64(unsigned long long lo,
                                                 unsigned long long hi,
                                                 unsigned n) {
      if (n == 0)  return hi;
      if (n < 64)  return (hi << n) | (lo >> (64 - n));
      if (n == 64) return lo;
      return (lo << (n - 64));
  }

  static __forceinline unsigned char addcarryx_u64(unsigned char c_in,
                                                   unsigned long long x,
                                                   unsigned long long y,
                                                   unsigned long long* out) {
      return _addcarry_u64(c_in, x, y, out);
  }

  static __forceinline int __builtin_ctzll(unsigned long long x) {
      unsigned long r;
      _BitScanForward64(&r, x);
      return (int)r;
  }

  // уникальные имена для избежания конфликта
  static __forceinline uint64_t my_shiftright128(uint64_t a, uint64_t b, unsigned char n) { return shrd64(a, b, n); }
  static __forceinline uint64_t my_shiftleft128 (uint64_t a, uint64_t b, unsigned char n) { return shld64(a, b, n); }

#else
// ---------------- GCC/Clang ----------------
  static inline uint64_t _umul128(uint64_t a, uint64_t b, uint64_t* h) {
      unsigned __int128 p = (unsigned __int128)a * (unsigned __int128)b;
      *h = (uint64_t)(p >> 64);
      return (uint64_t)p;
  }

  static inline uint64_t my_shiftright128(uint64_t a, uint64_t b, unsigned char n) {
      if (n == 0)  return a;
      if (n < 64)  return (a >> n) | (b << (64 - n));
      if (n == 64) return b;
      return (b >> (n - 64));
  }

  static inline uint64_t my_shiftleft128(uint64_t a, uint64_t b, unsigned char n) {
      if (n == 0)  return b;
      if (n < 64)  return (b << n) | (a >> (64 - n));
      if (n == 64) return a;
      return (a << (n - 64));
  }

  static inline unsigned char _addcarry_u64(unsigned char c,
                                            unsigned long long x,
                                            unsigned long long y,
                                            unsigned long long* out) {
      unsigned __int128 s = (unsigned __int128)x + (unsigned __int128)y + (unsigned __int128)c;
      *out = (uint64_t)s;
      return (unsigned char)(s >> 64);
  }

  static inline unsigned char _subborrow_u64(unsigned char c,
                                             unsigned long long x,
                                             unsigned long long y,
                                             unsigned long long* out) {
      unsigned __int128 d = (unsigned __int128)x - (unsigned __int128)y - (unsigned __int128)c;
      *out = (uint64_t)d;
      return (unsigned char)((d >> 127) & 1);
  }

  #ifndef __has_builtin
    #define __has_builtin(x) 0
  #endif

  #if !__has_builtin(__builtin_bswap64)
    static inline uint64_t __builtin_bswap64(uint64_t v) {
        return  ((v & 0x00000000000000FFull) << 56) |
                ((v & 0x000000000000FF00ull) << 40) |
                ((v & 0x0000000000FF0000ull) << 24) |
                ((v & 0x00000000FF000000ull) <<  8) |
                ((v & 0x000000FF00000000ull) >>  8) |
                ((v & 0x0000FF0000000000ull) >> 24) |
                ((v & 0x00FF000000000000ull) >> 40) |
                ((v & 0xFF00000000000000ull) >> 56);
    }
  #endif

  #define _byteswap_uint64 __builtin_bswap64

#endif // _MSC_VER

#define BISIZE 256
#if BISIZE==256
  #define NB64BLOCK 5
  #define NB32BLOCK 10
#elif BISIZE==512
  #define NB64BLOCK 9
  #define NB32BLOCK 18
#else
  #error Unsuported size
#endif

class Int {
public:
    Int();
    Int(int64_t i64);
    Int(uint64_t u64);
    Int(Int* a);

    void Add(uint64_t a);
    void Add(Int* a);
    void Add(Int* a, Int* b);
    void AddOne();
    void Sub(uint64_t a);
    void Sub(Int* a);
    void Sub(Int* a, Int* b);
    void SubOne();
    void Mult(Int* a);
    void Mult(uint64_t a);
    void IMult(int64_t a);
    void Mult(Int* a, uint64_t b);
    void IMult(Int* a, int64_t b);
    void Mult(Int* a, Int* b);
    void IMultAdd(Int* a, int64_t aa, Int* b, int64_t bb);
    void Div(Int* a, Int* mod = NULL);
    void MultModN(Int* a, Int* b, Int* n);
    void Neg();
    void Abs();

    void ShiftR(uint32_t n);
    void ShiftR32Bit();
    void ShiftR64Bit();
    void ShiftL(uint32_t n);
    void ShiftL32Bit();
    void ShiftL64Bit();
    void SwapBit(int bitNumber);

    bool IsGreater(Int* a);
    bool IsGreaterOrEqual(Int* a);
    bool IsLowerOrEqual(Int* a);
    bool IsLower(Int* a);
    bool IsEqual(Int* a);
    bool IsZero();
    bool IsOne();
    bool IsStrictPositive();
    bool IsPositive();
    bool IsNegative();
    bool IsEven();
    bool IsOdd();

    double ToDouble();

    static void SetupField(Int* n, Int* R = NULL, Int* R2 = NULL, Int* R3 = NULL, Int* R4 = NULL);
    static Int* GetR();
    static Int* GetR2();
    static Int* GetR3();
    static Int* GetR4();
    static Int* GetFieldCharacteristic();

    void GCD(Int* a);
    void Mod(Int* n);
    void ModInv();
    void MontgomeryMult(Int* a, Int* b);
    void MontgomeryMult(Int* a);
    void ModAdd(Int* a);
    void ModAdd(Int* a, Int* b);
    void ModAdd(uint64_t a);
    void ModSub(Int* a);
    void ModSub(Int* a, Int* b);
    void ModSub(uint64_t a);
    void ModMul(Int* a, Int* b);
    void ModMul(Int* a);
    void ModSquare(Int* a);
    void ModCube(Int* a);
    void ModDouble();
    void ModExp(Int* e);
    void ModNeg();
    void ModSqrt();
    bool HasSqrt();

    static void InitK1(Int* order);
    void ModMulK1(Int* a, Int* b);
    void ModMulK1(Int* a);
    void ModSquareK1(Int* a);
    void ModMulK1order(Int* a);
    void ModAddK1order(Int* a, Int* b);
    void ModAddK1order(Int* a);
    void ModSubK1order(Int* a);
    void ModNegK1order();
    uint32_t ModPositiveK1();

    int GetSize();
    int GetBitLength();

    void SetInt32(uint32_t value);
    void Set(Int* a);
    void SetBase10(char* value);
    void SetBase16(char* value);
    void SetBaseN(int n, char* charset, char* value);
    void SetByte(int n, unsigned char byte);
    void SetDWord(int n, uint32_t b);
    void SetQWord(int n, uint64_t b);
    void Set32Bytes(unsigned char* bytes);
    void MaskByte(int n);

    uint32_t GetInt32();
    int GetBit(uint32_t n);
    unsigned char GetByte(int n);
    void Get32Bytes(unsigned char* buff);

    std::string GetBase2();
    std::string GetBase10();
    std::string GetBase16();
    std::string GetBaseN(int n, char* charset);
    std::string GetBlockStr();
    std::string GetC64Str(int nbDigit);

    union {
        uint32_t bits[NB32BLOCK];
        uint64_t bits64[NB64BLOCK];
    };

private:
    void ShiftL32BitAndSub(Int* a, int n);
    uint64_t AddC(Int* a);
    void AddAndShift(Int* a, Int* b, uint64_t cH);
    void Mult(Int* a, uint32_t b);
    int  GetLowestBit();
    void CLEAR();
    void CLEARFF();
};

#define LoadI64(i,i64)    \
i.bits64[0] = i64;        \
i.bits64[1] = i64 >> 63;  \
i.bits64[2] = i.bits64[1];\
i.bits64[3] = i.bits64[1];\
i.bits64[4] = i.bits64[1];

static inline void imm_mul(uint64_t* x, uint64_t y, uint64_t* dst) {
    unsigned char c = 0;
    uint64_t h, carry;
    dst[0] = _umul128(x[0], y, &h); carry = h;
    c = _addcarry_u64(c, _umul128(x[1], y, &h), carry, dst + 1); carry = h;
    c = _addcarry_u64(c, _umul128(x[2], y, &h), carry, dst + 2); carry = h;
    c = _addcarry_u64(c, _umul128(x[3], y, &h), carry, dst + 3); carry = h;
    c = _addcarry_u64(c, _umul128(x[4], y, &h), carry, dst + 4); carry = h;
#if NB64BLOCK > 5
    c = _addcarry_u64(c, _umul128(x[5], y, &h), carry, dst + 5); carry = h;
    c = _addcarry_u64(c, _umul128(x[6], y, &h), carry, dst + 6); carry = h;
    c = _addcarry_u64(c, _umul128(x[7], y, &h), carry, dst + 7); carry = h;
    c = _addcarry_u64(c, _umul128(x[8], y, &h), carry, dst + 8); carry = h;
#endif
}

static inline void imm_umul(uint64_t* x, uint64_t y, uint64_t* dst) {
    unsigned char c = 0;
    uint64_t h, carry;
    dst[0] = _umul128(x[0], y, &h); carry = h;
    c = _addcarry_u64(c, _umul128(x[1], y, &h), carry, dst + 1); carry = h;
    c = _addcarry_u64(c, _umul128(x[2], y, &h), carry, dst + 2); carry = h;
    c = _addcarry_u64(c, _umul128(x[3], y, &h), carry, dst + 3); carry = h;
#if NB64BLOCK > 5
    c = _addcarry_u64(c, _umul128(x[4], y, &h), carry, dst + 4); carry = h;
    c = _addcarry_u64(c, _umul128(x[5], y, &h), carry, dst + 5); carry = h;
    c = _addcarry_u64(c, _umul128(x[6], y, &h), carry, dst + 6); carry = h;
    c = _addcarry_u64(c, _umul128(x[7], y, &h), carry, dst + 7); carry = h;
#endif
    _addcarry_u64(c, 0ULL, carry, dst + (NB64BLOCK - 1));
}

static inline void shiftR(unsigned char n, uint64_t* d) {
    d[0] = my_shiftright128(d[0], d[1], n);
    d[1] = my_shiftright128(d[1], d[2], n);
    d[2] = my_shiftright128(d[2], d[3], n);
    d[3] = my_shiftright128(d[3], d[4], n);
#if NB64BLOCK > 5
    d[4] = my_shiftright128(d[4], d[5], n);
    d[5] = my_shiftright128(d[5], d[6], n);
    d[6] = my_shiftright128(d[6], d[7], n);
    d[7] = my_shiftright128(d[7], d[8], n);
#endif
    d[NB64BLOCK - 1] = ((int64_t)d[NB64BLOCK - 1]) >> n;
}

static inline void shiftL(unsigned char n, uint64_t* d) {
#if NB64BLOCK > 5
    d[8] = my_shiftleft128(d[7], d[8], n);
    d[7] = my_shiftleft128(d[6], d[7], n);
    d[6] = my_shiftleft128(d[5], d[6], n);
    d[5] = my_shiftleft128(d[4], d[5], n);
#endif
    d[4] = my_shiftleft128(d[3], d[4], n);
    d[3] = my_shiftleft128(d[2], d[3], n);
    d[2] = my_shiftleft128(d[1], d[2], n);
    d[1] = my_shiftleft128(d[0], d[1], n);
    d[0] = d[0] << n;
}

#endif // BIGINTH
