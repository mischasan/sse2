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
#ifndef _SSE_H
#define _SSE_H

#include "sse.h"

#include <stdint.h>
#include <xmmintrin.h>

typedef __m128i XMM;

static inline unsigned
under(unsigned x)
{
    return (x - 1) & ~x;
}

#define xm_fill(c)    _mm_set1_epi8(c)

static inline unsigned
xm_same(XMM a, XMM b)
{
    return _mm_movemask_epi8(_mm_cmpeq_epi8(a, b));
}

static inline unsigned
xm_diff(XMM a, XMM b)
{
    return xm_same(a, b) ^ 0xFFFF;
}

static inline XMM
xm_load(void const *p)
{
    return _mm_load_si128((XMM const *)p);
}

static inline XMM
xm_loud(void const *p)
{
    return (XMM) _mm_loadu_pd((double const *)p);
}

#endif //_SSE_H
