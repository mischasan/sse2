#include <tap.h>
#include "sse.h"

//--------------|---------------------------------------------
// MEMREF and BITMAT wrapper classes inlined for testing:

typedef struct { char const *ptr; size_t len; } MEMREF;
int     refcmp(MEMREF const a, MEMREF const b);

typedef struct bitmat_s BITMAT;

BITMAT *bitmat(int nrows, int ncols);
void    bitmat_destroy(BITMAT *);
int     bitmat_rows(BITMAT const *);
int     bitmat_cols(BITMAT const *);
int     bitmat_get(BITMAT const *, int row, int col);
void    bitmat_set(BITMAT *, int row, int col, int val);
MEMREF  bitmat_ref(BITMAT const *);
BITMAT *bitmat_trans(BITMAT const *);
//--------------|---------------------------------------------

void    prbm(BITMAT const *);
void    dotest(int nrows, int ncols, int *xys);

int main(void)
{
    plan_tests(5);
    setvbuf(stdout, 0, _IOLBF, 0);
    int pts[] = { 1,3, 2,5, 4,0, 8,9, 5,20, 6,23, 10,10, 10,11, 10,12, 15,22, 13,23, -1 };

    dotest(8, 8, pts);
    dotest(8, 24, pts);
    dotest(16, 16, pts);
    dotest(16, 24, pts);
    dotest(24, 24, pts);

    return exit_status();
}

void
dotest(int nrows, int ncols, int *xys)
{
    BITMAT *inp = bitmat(nrows, ncols);
    BITMAT *exp = bitmat(ncols, nrows);

    for (; *xys >= 0; xys += 2) {
        bitmat_set(inp, xys[0] % nrows, xys[1] % ncols, 1);
        bitmat_set(exp, xys[1] % ncols, xys[0] % nrows, 1);
    }

    puts("---- inp"); prbm(inp);
    puts("---- exp"); prbm(exp);

    BITMAT *act = bitmat_trans(inp);
    int     rc = refcmp(bitmat_ref(act), bitmat_ref(exp));

    ok(!rc, "trans %d x %d", nrows, ncols);
    if (rc) {
        puts("expected:");
        prbm(exp);
        puts("actual:");
        prbm(act);
    }

    bitmat_destroy(inp);
    bitmat_destroy(exp);
    bitmat_destroy(act);
}

void
prbm(BITMAT const *bmp)
{
    int     r, c, nrows = bitmat_rows(bmp), ncols = bitmat_cols(bmp);

    printf("col>");
    for (c = 0; c < ncols; ++c) {
        if (!(c & 7)) putchar(' ');
        putchar('0' + c % 10);
    }
    putchar('\n');

    for (r = 0; r < nrows; ++r) {
        printf("%3d ", r);
        for (c = 0; c < ncols; ++c) {
            if (!(c & 7)) putchar(' ');
            putchar("-#"[bitmat_get(bmp, r, c)]);
        }
        putchar('\n');
    }
}

//--------------|---------------------------------------------

#include <string.h>             // memcmp

static inline int
intmin(int a, int b)
{
    return a < b ? a : b;
}

static inline int
nilref(MEMREF const ref)
{
    return !ref.len && !ref.ptr;
}

int
refcmp(MEMREF const a, MEMREF const b)
{
    int     rc, na = nilref(a), nb = nilref(b);

    return na || nb ? na - nb
        : (rc = memcmp(a.ptr, b.ptr, intmin(a.len, b.len))) ? rc
        : (int)a.len - (int)b.len;
}

#include <assert.h>

struct bitmat_s {
    char   *data;
    unsigned nrows, ncols;
};

BITMAT *
bitmat(int nrows, int ncols)
{
    assert(nrows % 8 == 0 && ncols % 8 == 0);
    BITMAT *bmp = malloc(sizeof *bmp);

    *bmp = (BITMAT) {
    calloc(nrows, ncols / 8), nrows, ncols};
    return bmp;
}

void
bitmat_destroy(BITMAT * bmp)
{
    if (bmp)
        free(bmp->data), free(bmp);
}

int
bitmat_get(BITMAT const *bmp, int row, int col)
{
    assert((unsigned)row < bmp->nrows && (unsigned)col < bmp->ncols);
    col += row * bmp->ncols;
    return ! !(bmp->data[col >> 3] & (1 << (col & 7)));
}

void
bitmat_set(BITMAT * bmp, int row, int col, int val)
{
    assert((unsigned)row < bmp->nrows && (unsigned)col < bmp->ncols);
    col += row * bmp->ncols;
    char    bit = 1 << (col & 7), *at = &bmp->data[col >> 3];
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

MEMREF
bitmat_ref(BITMAT const *bmp)
{
    return (MEMREF) {
    (char const *)bmp->data, bmp->nrows * bmp->ncols / 8};
}

BITMAT *
bitmat_trans(BITMAT const *bmp)
{
    BITMAT *ret = bitmat(bmp->ncols, bmp->nrows);

    ssebmx(bmp->data, ret->data, bmp->nrows, bmp->ncols);
    return ret;
}
