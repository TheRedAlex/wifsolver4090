// Minimal wrapper to provide ripemd160_32() via SSE batch implementation.
#include <cstdint>
#include <cstring>

extern "C" void ripemd160sse_32(const unsigned char* sh0, const unsigned char* sh1,
                                const unsigned char* sh2, const unsigned char* sh3,
                                unsigned char* h0, unsigned char* h1,
                                unsigned char* h2, unsigned char* h3);

// API expected by Secp256K1.cpp: input = 32-byte SHA256, output = 20 bytes
extern "C" void ripemd160_32(const unsigned char* sh, unsigned char* out) {
    alignas(16) unsigned char zero[64] = {0};  // заглушки для батча
    unsigned char drop1[20], drop2[20], drop3[20];
    ripemd160sse_32(sh, zero, zero, zero, out, drop1, drop2, drop3);
}
