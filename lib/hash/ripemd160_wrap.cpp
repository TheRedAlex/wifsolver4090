#include <cstdint>
#include <cstring>

extern "C" void ripemd160sse_32(const unsigned char* sh0, const unsigned char* sh1,
                                const unsigned char* sh2, const unsigned char* sh3,
                                unsigned char* h0, unsigned char* h1,
                                unsigned char* h2, unsigned char* h3);

// input: 32 ????? SHA-256, output: 20 ????
extern "C" void ripemd160_32(const unsigned char* sh, unsigned char* out) {
    alignas(16) unsigned char zero[64] = {0};
    unsigned char drop1[20], drop2[20], drop3[20];
    ripemd160sse_32(sh, zero, zero, zero, out, drop1, drop2, drop3);
}
