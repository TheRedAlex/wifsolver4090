#include <cstdint>
#include <cstring>

extern "C" {

typedef struct { uint32_t h[5]; uint64_t len; uint8_t buf[64]; size_t idx; } ripemd160_ctx;

static inline uint32_t rol(uint32_t x, uint32_t n){ return (x<<n)|(x>>(32-n)); }
static inline uint32_t F(uint32_t x,uint32_t y,uint32_t z){ return x^y^z; }
static inline uint32_t G(uint32_t x,uint32_t y,uint32_t z){ return (x&y)|(~x&z); }
static inline uint32_t H(uint32_t x,uint32_t y,uint32_t z){ return (x|~y)^z; }
static inline uint32_t I(uint32_t x,uint32_t y,uint32_t z){ return (x&z)|(y&~z); }
static inline uint32_t J(uint32_t x,uint32_t y,uint32_t z){ return x^(y|~z); }

static void ripemd160_init(ripemd160_ctx* c){
    c->h[0]=0x67452301u; c->h[1]=0xEFCDAB89u; c->h[2]=0x98BADCFEu; c->h[3]=0x10325476u; c->h[4]=0xC3D2E1F0u;
    c->len=0; c->idx=0;
}

static void ripemd160_compress(uint32_t* h, const uint8_t block[64]){
    static const uint32_t KL[5]={0x00000000u,0x5A827999u,0x6ED9EBA1u,0x8F1BBCDCu,0xA953FD4Eu};
    static const uint32_t KR[5]={0x50A28BE6u,0x5C4DD124u,0x6D703EF3u,0x7A6D76E9u,0x00000000u};
    static const uint8_t RL[80]={11,14,15,12,5,8,7,9,11,13,14,15,6,7,9,8,7,6,8,13,11,9,7,15,7,12,15,9,11,7,13,12,11,13,6,7,14,9,13,15,14,8,13,6,5,12,7,5,11,12,14,15,14,15,9,8,9,14,5,6,8,6,5,12,9,15,5,11,6,8,13,12,5,12,13,14,11,8,5,6};
    static const uint8_t RR[80]={8,9,9,11,13,15,15,5,7,7,8,11,14,14,12,6,9,13,15,7,12,8,9,11,7,7,12,7,6,15,13,11,9,7,15,11,8,6,6,14,12,13,5,14,13,13,7,5,15,5,8,11,14,14,6,14,6,9,12,9,12,5,15,8,8,5,12,9,12,5,14,6,8,13,6,5,15,13,11,11};
    static const uint8_t SL[80]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,7,4,13,1,10,6,15,3,12,0,9,5,2,14,11,8,3,10,14,4,9,15,8,1,2,7,0,6,13,11,5,12,1,9,11,10,0,8,12,4,13,3,7,15,14,5,6,2,4,0,5,9,7,12,2,10,14,1,3,8,11,6,15,13};
    static const uint8_t SR[80]={5,14,7,0,9,2,11,4,13,6,15,8,1,10,3,12,6,11,3,7,0,13,5,10,14,15,8,12,4,9,1,2,15,5,1,3,7,14,6,9,11,8,12,2,10,0,4,13,8,6,4,1,3,11,15,0,5,12,2,13,9,7,10,14,12,15,10,4,1,5,8,7,6,2,13,14,0,3,9,11};

    uint32_t X[16];
    for(int i=0;i<16;i++) X[i]=(uint32_t)block[4*i]|((uint32_t)block[4*i+1]<<8)|((uint32_t)block[4*i+2]<<16)|((uint32_t)block[4*i+3]<<24);
    uint32_t al=h[0],bl=h[1],cl=h[2],dl=h[3],el=h[4], ar=h[0],br=h[1],cr=h[2],dr=h[3],er=h[4];

    for(int i=0;i<80;i++){
        uint32_t t;
        if(i<16){ t=((al+ (bl^cl^dl) + X[SL[i]] + KL[0])<<RL[i])|((al+ (bl^cl^dl) + X[SL[i]] + KL[0])>>(32-RL[i])); t+=el; al=el; el=dl; dl=(cl<<10)|(cl>>22); cl=bl; bl=t; }
        else if(i<32){ t=((al+ ((bl&cl)|(~bl&dl)) + X[SL[i]] + KL[1])<<RL[i])|((al+ ((bl&cl)|(~bl&dl)) + X[SL[i]] + KL[1])>>(32-RL[i])); t+=el; al=el; el=dl; dl=(cl<<10)|(cl>>22); cl=bl; bl=t; }
        else if(i<48){ t=((al+ ((bl|~cl)^dl) + X[SL[i]] + KL[2])<<RL[i])|((al+ ((bl|~cl)^dl) + X[SL[i]] + KL[2])>>(32-RL[i])); t+=el; al=el; el=dl; dl=(cl<<10)|(cl>>22); cl=bl; bl=t; }
        else if(i<64){ t=((al+ ((bl&dl)|(cl&~dl)) + X[SL[i]] + KL[3])<<RL[i])|((al+ ((bl&dl)|(cl&~dl)) + X[SL[i]] + KL[3])>>(32-RL[i])); t+=el; al=el; el=dl; dl=(cl<<10)|(cl>>22); cl=bl; bl=t; }
        else         { t=((al+ (bl^(cl|~dl)) + X[SL[i]] + KL[4])<<RL[i])|((al+ (bl^(cl|~dl)) + X[SL[i]] + KL[4])>>(32-RL[i])); t+=el; al=el; el=dl; dl=(cl<<10)|(cl>>22); cl=bl; bl=t; }

        if(i<16){ t=((ar+ (br^(cr|~dr)) + X[SR[i]] + KR[0])<<RR[i])|((ar+ (br^(cr|~dr)) + X[SR[i]] + KR[0])>>(32-RR[i])); t+=er; ar=er; er=dr; dr=(cr<<10)|(cr>>22); cr=br; br=t; }
        else if(i<32){ t=((ar+ ((br&dr)|(cr&~dr)) + X[SR[i]] + KR[1])<<RR[i])|((ar+ ((br&dr)|(cr&~dr)) + X[SR[i]] + KR[1])>>(32-RR[i])); t+=er; ar=er; er=dr; dr=(cr<<10)|(cr>>22); cr=br; br=t; }
        else if(i<48){ t=((ar+ ((br|~cr)^dr) + X[SR[i]] + KR[2])<<RR[i])|((ar+ ((br|~cr)^dr) + X[SR[i]] + KR[2])>>(32-RR[i])); t+=er; ar=er; er=dr; dr=(cr<<10)|(cr>>22); cr=br; br=t; }
        else if(i<64){ t=((ar+ ((br&cr)|(~br&dr)) + X[SR[i]] + KR[3])<<RR[i])|((ar+ ((br&cr)|(~br&dr)) + X[SR[i]] + KR[3])>>(32-RR[i])); t+=er; ar=er; er=dr; dr=(cr<<10)|(cr>>22); cr=br; br=t; }
        else         { t=((ar+ (br^cr^dr) + X[SR[i]] + KR[4])<<RR[i])|((ar+ (br^cr^dr) + X[SR[i]] + KR[4])>>(32-RR[i])); t+=er; ar=er; er=dr; dr=(cr<<10)|(cr>>22); cr=br; br=t; }
    }

    uint32_t t = h[1] + cl + dr;
    h[1] = h[2] + dl + er;
    h[2] = h[3] + el + ar;
    h[3] = h[4] + al + br;
    h[4] = h[0] + bl + cr;
    h[0] = t;
}

static void ripemd160_update(ripemd160_ctx* c, const uint8_t* data, size_t len){
    c->len += (uint64_t)len * 8u;
    while(len--){
        c->buf[c->idx++] = *data++;
        if(c->idx==64){ ripemd160_compress(c->h, c->buf); c->idx=0; }
    }
}

static void ripemd160_final(ripemd160_ctx* c, uint8_t out[20]){
    size_t i = c->idx; c->buf[i++]=0x80;
    if(i>56){ while(i<64) c->buf[i++]=0; ripemd160_compress(c->h,c->buf); i=0; }
    while(i<56) c->buf[i++]=0;
    for(int j=0;j<8;j++) c->buf[56+j]=(uint8_t)((c->len>>(8*j))&0xFF);
    ripemd160_compress(c->h,c->buf);
    for(int j=0;j<5;j++){ out[4*j+0]=(uint8_t)(c->h[j]&0xFF); out[4*j+1]=(uint8_t)((c->h[j]>>8)&0xFF); out[4*j+2]=(uint8_t)((c->h[j]>>16)&0xFF); out[4*j+3]=(uint8_t)((c->h[j]>>24)&0xFF); }
}

// ????: 32 ????? SHA-256, ?????: 20 ????
void ripemd160_32(const unsigned char* sh, unsigned char* out){
    ripemd160_ctx c; ripemd160_init(&c); ripemd160_update(&c, sh, 32); ripemd160_final(&c, out);
}

} // extern "C"
