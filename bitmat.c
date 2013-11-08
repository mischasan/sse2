//--------------|---------------------------------------------
// XXX modify bitmat definition to allow dims other than 8n
//  as long as each row was stored as an int number of bytes.
//  Transpose would have to not store excess (empty)
//  column bits as excess rows.

#include "bitmat.h"
#include <assert.h>
#include <stdlib.h>             // malloc
#include <string.h>             // memcmp
#include "sse.h"

// If bitmat_msb_first == ~0, returns MSBit-First bitmask.
static inline char BBIT(int col)
{ return 1 << ((col ^ -!!bitmat_msb_first) & 7); }

struct bitmat { char *data; unsigned nrows, ncols; };

BITMAT *
bitmat(int nrows, int ncols)
{
    assert(!(nrows % 8 ||ncols % 8));
    BITMAT bm = { calloc(nrows, ncols / 8), nrows, ncols };
    return memcpy(malloc(sizeof bm), &bm, sizeof bm);
}

void
bitmat_destroy(BITMAT * bmp)
{
    if (bmp) free(bmp->data), free(bmp);
}

int
bitmat_get(BITMAT const *bmp, int row, int col)
{
    assert((unsigned)row < bmp->nrows && (unsigned)col < bmp->ncols);
    col += row * bmp->ncols;
    return !!(bmp->data[col >> 3] & BBIT(col));
}

void
bitmat_set(BITMAT * bmp, int row, int col, int val)
{
    assert((unsigned)row < bmp->nrows && (unsigned)col < bmp->ncols);
    col += row * bmp->ncols;
    char bit = BBIT(col), *at = &bmp->data[col >> 3];
    *at = val ? *at | bit : *at & ~bit;
}

int
bitmat_rows(BITMAT const *bmp)
{
    return bmp->nrows;
}

int
bitmat_cols(BITMAT const *bmp)
{
    return bmp->ncols;
}

int
bitmat_cmp(BITMAT const *a, BITMAT const*b)
{
    return a == b ? 0 
        : !a || !b || a->nrows != b->nrows || a->ncols != b->ncols ? -1
        : memcmp(a->data, b->data, a->nrows * a->ncols / 8);
}

char const*
bitmat_rowval(BITMAT const *bmp, int row)
{
    return &bmp->data[row * bmp->ncols / 8];
}

BITMAT *
bitmat_trans(BITMAT const *bmp)
{
    BITMAT *ret = bitmat(bmp->ncols, bmp->nrows);
    (bitmat_msb_first ? ssebmx_m : ssebmx)(bmp->data, ret->data, bmp->nrows, bmp->ncols);
    return ret;
}
