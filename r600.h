/*
 * RadeonHD R6xx, R7xx DRI driver
 *
 * Copyright (C) 2008-2009  Matthias Hopf
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef _R600_H
#define _R600_H

/* CP packet types */
enum {
    RADEON_CP_PACKET0              = 0x00000000,
    RADEON_CP_PACKET1              = 0x40000000,
    RADEON_CP_PACKET2              = 0x80000000,
    RADEON_CP_PACKET3              = 0xC0000000,
};

#define CP_PACKET0(reg, n)      (RADEON_CP_PACKET0 | (((n)-1)<<16) | ((reg)>>2))
#define CP_PACKET3(cmd, n)      (RADEON_CP_PACKET3 | (((n)-1)<<16) | ((cmd)<<8))

#endif

