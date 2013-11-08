#include "sse.h"
#include <stdint.h>

#define INP(x,y) inp[(x)*ncols/8 + (y)/8]
#define OUT(x,y) out[(y)*nrows/8 + (x)/8]

void
ssebmx(char const *inp, char *out, int nrows, int ncols)
{
    int rr, cc, i, h;
    union { XMM x; uint8_t b[16]; } tmp;

    // Do the main body in [16 x 8] blocks:
    for (rr = 0; rr <= nrows - 16; rr += 16)
        for (cc = 0; cc < ncols; cc += 8) {
            for (i = 0; i < 16; ++i)
                tmp.b[i] = INP(rr + i, cc);
            for (i = 8; i--; tmp.x = xm_shl64(tmp.x, 1))
                *(uint16_t*)&OUT(rr, cc + i) = _mm_movemask_epi8(tmp.x);
        }

    if (rr == nrows) return;

    // The remainder is a block of [8 x 16n+8] bits (n >= 0).

    //  Do the [8 x 16] blocks:
    for (cc = 0; cc <= ncols - 16; cc += 16) {
        for (i = 8; i--;)
            tmp.b[i] = h = *(uint16_t const*)&INP(rr + i, cc),
            tmp.b[i + 8] = h >> 8;
        for (i = 8; i--; tmp.x = xm_shl64(tmp.x, 1))
            OUT(rr, cc + i) = h = _mm_movemask_epi8(tmp.x),
            OUT(rr, cc + i + 8) = h >> 8;
    }

    if (cc == ncols) return;

    //  Do the remaining [8 x 8] block:
    for (i = 8; i--;)
        tmp.b[i] = INP(rr + i, cc);
    for (i = 8; i--; tmp.x = xm_shl64(tmp.x, 1))
        OUT(rr, cc + i) = _mm_movemask_epi8(tmp.x);
}

// ssebmx when bits of each byte are numbered MSB-first
void
ssebmx_m(char const *inp, char *out, int nrows, int ncols)
{
    int rr, cc, i, h;
    union { XMM x; uint8_t b[16]; } tmp;

    // Do the main body in [16 x 8] blocks:
    for (rr = 0; rr <= nrows - 16; rr += 16)
        for (cc = 0; cc < ncols; cc += 8) {
            for (i = 0; i < 16; ++i)
                tmp.b[(i ^ 7)] = INP(rr + i, cc);
            for (i = 0; i < 8; ++i, tmp.x = xm_shl64(tmp.x, 1))
                *(uint16_t*)&OUT(rr, cc + i) = _mm_movemask_epi8(tmp.x);
        }

    if (rr == nrows) return;

    // The remainder is a block of [8 x 16n+8] bits (n >= 0).

    //  Do the [8 x 16] blocks:
    for (cc = 0; cc <= ncols - 16; cc += 16) {
        for (i = 8; i--;)
            tmp.b[(i ^ 7)] = h = *(uint16_t const*)&INP(rr + i, cc),
            tmp.b[(i ^ 7) + 8] = h >> 8;
        for (i = 0; i < 8; ++i, tmp.x = xm_shl64(tmp.x, 1))
            OUT(rr, cc + i) = h = _mm_movemask_epi8(tmp.x),
            OUT(rr, cc + i + 8) = h >> 8;
    }

    if (cc == ncols) return;

    //  Do the remaining [8 x 8] block:
    for (i = 8; i--;)
        tmp.b[i] = INP(rr + (i ^ 7), cc);
    for (i = 0; i < 8; ++i, tmp.x = xm_shl64(tmp.x, 1))
        OUT(rr, cc + i) = _mm_movemask_epi8(tmp.x);
}
