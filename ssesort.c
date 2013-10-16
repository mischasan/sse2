// Copyright (C) 2009-2013 Mischa Sandberg <mischasan@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License Version 2 as
// published by the Free Software Foundation.  You may not use, modify or
// distribute this program under any other version of the GNU General
// Public License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// IF YOU ARE UNABLE TO WORK WITH GPL2, CONTACT ME.
//-------------------------------------------------------------------
#include "sse.h"
#include <stdint.h>

//  "LSB" presumes LSB-first storage of (double).
#define LSB(x) (*(uint8_t*)&(x))

// sserank16d: returns a ranking vector such that keys[rank[i]] <= keys[rank[i+1]].
//  Order-preserving over duplicates. keys[] is sorted (i.e. trashed) as a side-effect.

void
sserank16d(double keys[16], int rank[16])
{
    int     i, dist[16] = {};
    uint8_t lsb[16];

    for (i = 0; i < 16; ++i)
        dist[LSB(keys[i]) & 15]++;

    for (i = 0; i < 15; ++i)
        dist[i + 1] += dist[i];

    for (i = 0; i < 16; ++i) {
        lsb[i] = LSB(keys[i]);
        LSB(keys[i]) = (lsb[i] & ~15) | dist[lsb[i] & 15]++;
    }

    ssesort16d(keys);

    // Populate rank and restore low byte of each key:
    for (i = 0; i < 16; ++i)
        LSB(keys[i]) = lsb[rank[i] = LSB(keys[i]) & 15];
}

// Bitonic sort:    24 x MINMAX
// Odd-even merge: 13 x MINMAX

void
ssesort16d(double keys[16])
{
    __m128d t, temp[8];

    // MINMAX(a, b): [a, b] := [min(a, b), max(a, b)]
#   define MINMAX(a, b) \
        temp[a] = _mm_min_pd(t = LOAD(a), LOAD(b)), \
        temp[b] = _mm_max_pd(t, LOAD(b))

    // Initially load from keys[]
#   define LOAD(i) _mm_loadu_pd(keys + 2*(i))
    // Bitonic step #1:
    MINMAX(0, 1); MINMAX(2, 3); MINMAX(4, 5); MINMAX(6, 7);

    // Switch to loading (aligned) from temp[]
#   undef  LOAD
#   define LOAD(i) temp[i]

    // Bitonic step #2:
    MINMAX(0, 3); MINMAX(1, 2); MINMAX(4, 7); MINMAX(5, 6);
    MINMAX(0, 1); MINMAX(2, 3); MINMAX(4, 5); MINMAX(6, 7);

    // Bitonic step #3:
    MINMAX(0, 7); MINMAX(1, 6); MINMAX(2, 5); MINMAX(3, 4);
    MINMAX(0, 2); MINMAX(1, 3); MINMAX(4, 6); MINMAX(5, 7);
    MINMAX(0, 1); MINMAX(2, 3); MINMAX(4, 5); MINMAX(6, 7);

    // Linear merge of (temp.d[0,2,4,6], temp.d[1,3,5,7]) => keys:
#if 1
    double a, *tp, *zp = keys, *ap = (double *)temp, *bp = ap + 1, b = *bp;

    do {
        a = *ap;
        if (a > b) {
            tp = ap, ap = bp, bp = tp;
            *zp++ = b;
            b = a;
        } else {
            *zp++ = a;
        }
    } while ((ap += 2) < (double *)temp + 16);

    do *zp++ = *bp;
    while ((bp += 2) < (double *)temp + 16);
#else
    // Odd-even merge => keys
    // This is included as a curiosity only; the linear merge is faster.
    double  d[4];

#   define LOADLO(x,d) (temp[x] = _mm_loadl_pd(temp[x], &(d)))
#   define SAVELO(d,x) _mm_storel_pd(&(d), temp[x])
#   define SAVEHI(d,x) _mm_storeh_pd(&(d), temp[x])
    // I have no idea why SSE2 calls it UNpack.
#   define PACKHI(a,b) (temp[a] = (__m128d)_mm_unpackhi_epi64((XMM)temp[a], (XMM)temp[b]))
#   define PACKLO(a,b) (temp[a] = (__m128d)_mm_unpacklo_epi64((XMM)temp[a], (XMM)temp[b]))

    // At the end of the bitonic sort, registers (X0..X7)
    // hold two sorted 8-element seqs: [0..7] and [8..f].
    // These are represented (in comments below) as
    //  e.g. '19' for (lo=1,hi=9)

    // MINMAX ops need values in different seqs to be
    // in different regs; or to put it another way, (lo) and
    // (hi) of each reg must come from the same seq.
    // MINMAX steps before the final step need adjacent pairs of
    // each seq be in the same reg (e.g 01,23,.., 89,ab,..)

    // Chhugani(sic) et al ("Effic Impl of Sort on Multi-Core
    // SIMD CPU Arch" ?2009?) imply 2 shuffles per minmax for
    // bitonic sort. The following has 26 shuffle ops for 13
    // minmaxes (39 ops).

    // In the following, '_' stands for a don't-care position.
    // X0 X1 X2 X3 X4 X5 X6 X7 
    // 08 19 2a 3b 4c 5d 6e 7f
    // ops=19 for SHUFFLE#1

    SAVEHI(d[0], 0);            // 0_ 19 2a 3b 4c 5d 6e 7f
    SAVEHI(d[1], 2);            // 0_ 19 2_ 3b 4c 5d 6e 7f
    SAVEHI(d[2], 4);            // 0_ 19 2_ 3b 4_ 5d 6e 7f
    SAVEHI(d[3], 6);            // 0_ 19 2_ 3b 4_ 5d 6_ 7f

    PACKLO(0, 1);               // 01 _9 2_ 3b 4_ 5d 6_ 7f
    PACKLO(2, 3);               // 01 _9 23 _b 4_ 5d 6_ 7f
    PACKLO(4, 5);               // 01 _9 23 _b 45 _d 6_ 7f
    PACKLO(6, 7);               // 01 _9 23 _b 45 _d 67 _f

    LOADLO(1, d[0]);            // 01 89 23 _b 45 _d 67 _f
    LOADLO(3, d[1]);            // 01 89 23 ab 45 _d 67 _f
    LOADLO(5, d[2]);            // 01 89 23 ab 45 cd 67 _f
    LOADLO(7, d[3]);            // 01 89 23 ab 45 cd 67 ef

    // ops=29 for (MINMAX#1 + SAVE + MINMAX#2):

    // MINMAX#1
    MINMAX(0, 1);               // 01:89
    MINMAX(2, 3);               // 23:ab
    MINMAX(4, 5);               // 45:cd
    MINMAX(6, 7);               // 67:ef

    // The min of the first of each series (0:8) is the min of
    //  the final result. Likewise the max of the last of each
    //  (7:f).
    // SAVE keys[0,15]
    SAVELO(keys[0], 0);         // _1 89 23 ab 45 cd 67 ef
    SAVEHI(keys[15], 7);        // _1 89 23 ab 45 cd 67 e_

    // MINMAX#2
    MINMAX(4, 1);               // 45:89
    MINMAX(6, 3);               // 67:ab
    MINMAX(2, 4);               // 23:45
    MINMAX(6, 1);               // 67:89
    MINMAX(3, 5);               // ab:cd

    // ops=7 for SHUFFLE#2:
    PACKHI(0, 2);               // 13 89 2_ ab 45 cd 67 e_
    PACKLO(2, 4);               // 13 89 24 ab _5 cd 67 e_
    PACKHI(4, 6);               // 13 89 24 ab 57 cd 6_ e_
    PACKLO(6, 1);               // 13 _9 24 ab 57 cd 68 e_
    PACKHI(1, 3);               // 13 9b 24 a_ 57 cd 68 e_
    PACKLO(3, 5);               // 13 9b 24 ac 57 _d 68 e_
    PACKHI(5, 5);               // 13 9b 24 ac 57 d_ 68 e_

    // ops=26 for (MINMAX#3 + SAVE keys[1..14])

    // MINMAX#3
    MINMAX(0, 2);               // 13:24
    MINMAX(4, 6);               // 57:68
    MINMAX(1, 3);               // 9b:ac
    MINMAX(5, 7);               // d_:e_ only lo part used

    // SAVE->keys[1..14]. Compiler reorders these much:
    SAVELO(keys[1], 0);
    SAVEHI(keys[3], 0);
    SAVELO(keys[2], 2);
    SAVEHI(keys[4], 2);
    SAVELO(keys[5], 4);
    SAVEHI(keys[7], 4);
    SAVELO(keys[6], 6);
    SAVEHI(keys[8], 6);
    SAVELO(keys[9], 1);
    SAVEHI(keys[11], 1);
    SAVELO(keys[10], 3);
    SAVEHI(keys[12], 3);
    SAVELO(keys[13], 5);
    SAVELO(keys[14], 7);
#endif
}
