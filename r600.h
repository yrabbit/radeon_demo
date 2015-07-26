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

enum chipset_e {
    CHIPSET_NONE = 0,
    CHIPSET_R600,
    CHIPSET_RV610, CHIPSET_RV620,				// no VC
    CHIPSET_RS780, CHIPSET_M72, CHIPSET_M74, CHIPSET_M82,	// no VC
    CHIPSET_RV630, CHIPSET_RV635, CHIPSET_RV670,
    CHIPSET_RV770,
    CHIPSET_RV710,						// no VC
    CHIPSET_RV730
};

/* CP packet types */
enum {
    RADEON_CP_PACKET0              = 0x00000000,
    RADEON_CP_PACKET1              = 0x40000000,
    RADEON_CP_PACKET2              = 0x80000000,
    RADEON_CP_PACKET3              = 0xC0000000,
};

#define CP_PACKET0(reg, n)      (RADEON_CP_PACKET0 | (((n)-1)<<16) | ((reg)>>2))
#define CP_PACKET3(cmd, n)      (RADEON_CP_PACKET3 | (((n)-1)<<16) | ((cmd)<<8))

/* Some packet3 opcodes */
enum {
	INDEX_TYPE		= 0x2a,
	DRAW_INDEX_AUTO		= 0x2d,
	NUM_INSTANCES		= 0x2f,
	IT_SET_CONTEXT_REG	= 0x69
};

extern int debug;

void mk_packet0(struct radeon_cs *, uint32_t, uint32_t);
void mk_packet3(struct radeon_cs *, uint32_t, uint32_t);

/* read register */
int read_reg(int, uint32_t, uint32_t*);
void wait_reg(int, uint32_t, uint32_t, const char*);

/* tests */
void testCP(int, struct radeon_cs_manager*);
void testCPDMA(int, struct radeon_cs_manager*);
void test2DTri(int, struct radeon_cs_manager *);
#endif

