#include "sse.h"
#include <stdio.h>
#include <string.h> // ffsl

static inline int fls(int x) regargs;
static inline int fls(int x)
{ asm("bsrl %0,%0":"=r"(x):"r"(x)); return x; }

#undef  DO
#define DO_8x8(op,a) DO_8(op,a,0) DO_8(op,a,1) DO_8(op,a,2) DO_8(op,a,3) DO_8(op,a,4) DO_8(op,a,5) DO_8(op,a,6) DO_8(op,a,7)
#define DO_8(op,a,b) DO(op,a,b,0) DO(op,a,b,1) DO(op,a,b,2) DO(op,a,b,3) DO(op,a,b,4) DO(op,a,b,5) DO(op,a,b,6) DO(op,a,b,7)
#define DO(op,a,b,c) case 0##a##b##c: x = xm_##op##_##a##b##c(x); break;

#define xm_shl_000(x) (x)
XMM xm_shl(XMM x, unsigned nbits)
{
    switch (nbits) { DO_8x8(shl,0) DO_8x8(shl,1) default: x = xm_zero(); }
    return x;
}

#define xm_shr_000(x) (x)
XMM
xm_shr(XMM x, unsigned nbits)
{
    switch (nbits) { DO_8x8(shr,0) DO_8x8(shr,1) default: x = xm_zero(); }
    return x;
}

int
xm_ffs(XMM x)
{
    int pos = ffsl(xm_diff(x, xm_zero())) - 1;
    return pos < 0 ? pos : (pos << 3) + ffsl(((unsigned char const*)&x)[pos]) - 1;
}

int
xm_fls(XMM x)
{
    int pos = xm_diff(x, xm_zero());
    if (!pos) return -1;
    pos = fls(pos); // NOTE: FSL and FFSL map to bit pos #'s differently!
    return (pos << 3) + fls(((unsigned char const*)&x)[pos]);
}

char*
xm_dbl(__m128d x, char buf[48])
{
    union { __m128d xm; double b[2]; } a = { x };
    sprintf(buf, "%g,%g", a.b[0], a.b[1]);
    return buf;
}

char*
xm_hex(XMM x, char buf[48])
{
    union { XMM xm; unsigned long long b[2]; } a = { x };
    sprintf(buf, "%016llX%016llX", a.b[1], a.b[0]);
    return buf;
}

char*
xm_str(XMM x, char buf[48])
{
    int i;
    union { XMM xm; unsigned char b[16]; } a = { x };
    char *cp = buf + sprintf(buf, "%02X", a.b[0]);
    for (i = 1; i < 16; ++i) cp += sprintf(cp, "%c%02X", i == 8 ? '-' : ',', a.b[i]);
    return buf;
}
