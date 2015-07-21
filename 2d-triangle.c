/*
 * Test 3d triangle
 */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <xf86drm.h>
#include <libdrm/radeon_cs_gem.h>

#include "r600.h"
#include "r600_reg_auto_r6xx.h"
#include "r600_reg_r6xx.h"
#include "r600_shader.h"

extern FILE* fout;

void
test2DTri(int fd, struct radeon_cs_manager *csm)
{
#define NDW 100
	struct radeon_cs *cs;
	uint32_t vs[30];
	uint32_t ps[2];
	int i, j;

	cs = radeon_cs_create(csm, NDW);
	if (cs == NULL) {
		fprintf(stderr, "%s: can't create CS.", __FUNCTION__);
		goto err;
	}

	/* Setup */
	/* what are these instance steps anyway? */
	mk_packet0(cs, VGT_INSTANCE_STEP_RATE_0, 0);
	mk_packet0(cs, VGT_INSTANCE_STEP_RATE_1, 0);
	mk_packet0(cs, VGT_MIN_VTX_INDX, 0);
	mk_packet0(cs, VGT_INDX_OFFSET, 0);

	/* We'll draw triangles */
	mk_packet0(cs, VGT_PRIMITIVE_TYPE, DI_PT_TRILIST);

	radeon_cs_write_dword(cs, CP_PACKET3(INDEX_TYPE, 1));
	radeon_cs_write_dword(cs, DI_INDEX_SIZE_16_BIT);

	/* Use just one VGT DMA instance */
	radeon_cs_write_dword(cs, CP_PACKET3(NUM_INSTANCES, 1));
	radeon_cs_write_dword(cs, 1);

	/* Vertex shader program */
	i = 0;
	// 0 execute fetch through a vertex cache
	// clause begins at offset 10 qword and consists 2 commands
	vs[i++] = CF_DWORD0(ADDR(10));
	vs[i++] = CF_DWORD1(POP_COUNT(0),
			CF_CONST(0),
			COND(SQ_CF_COND_ACTIVE),
			COUNT(2),
			CALL_COUNT(0),
			END_OF_PROGRAM(0),
			VALID_PIXEL_MODE(0),
			CF_INST(SQ_CF_INST_VTX),
			WHOLE_QUAD_MODE(0),
			BARRIER(1));
	
	// 1  alu clause begins at offset 4
	//    don't lock any cache in first set
	vs[i++] = CF_ALU_DWORD0(ADDR(4),
			KCACHE_BANK0(0),
			KCACHE_BANK1(0),
			KCACHE_MODE0(0));
	//    don't lock any cache in second set
	//    2 64-bit slots in ALU clause
	//    waterfall? ALT_CONSTS=0 looks better
	vs[i++] = CF_ALU_DWORD1(KCACHE_MODE1(0),
			KCACHE_ADDR0(0),
			KCACHE_ADDR1(0),
			COUNT(2),
			USES_WATERFALL(0),
			CF_INST(SQ_CF_INST_ALU),
			WHOLE_QUAD_MODE(0),
			BARRIER(1));

	// 2  export position from GPR(1)
	//    element size is ignored for this instruction
	vs[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(CF_POS0),
			TYPE(SQ_EXPORT_POS),
			RW_GPR(1),
			RW_REL(ABSOLUTE),
			INDEX_GPR(0),
			ELEM_SIZE(0));
	vs[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
			SRC_SEL_Y(SQ_SEL_Y),
			SRC_SEL_Z(SQ_SEL_Z),
			SRC_SEL_W(SQ_SEL_W),
			R6xx_ELEM_LOOP(0),
			BURST_COUNT(0),
			END_OF_PROGRAM(0),
			VALID_PIXEL_MODE(0),
			CF_INST(SQ_CF_INST_EXPORT_DONE),
			WHOLE_QUAD_MODE(0),
			BARRIER(1));
	// 3  export parameter (color?) from GPR(2)
	vs[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(0),
			TYPE(SQ_EXPORT_PARAM),
			RW_GPR(2),
			RW_REL(ABSOLUTE),
			INDEX_GPR(0),
			ELEM_SIZE(0));
	//    and here we go: end of program
	vs[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
			SRC_SEL_Y(SQ_SEL_Y),
			SRC_SEL_Z(SQ_SEL_Z),
			SRC_SEL_W(SQ_SEL_W),
			R6xx_ELEM_LOOP(0),
			BURST_COUNT(0),
			END_OF_PROGRAM(1),
			VALID_PIXEL_MODE(0),
			CF_INST(SQ_CF_INST_EXPORT_DONE),
			WHOLE_QUAD_MODE(0),
			BARRIER(0));

	// 10 fetch vertex data, 12 bytes at once (x, y, color:ignored)
	//    address for fetch is in GPR 0 (X element)
	vs[i++] = VTX_DWORD0(VTX_INST(SQ_VTX_INST_FETCH),
			FETCH_TYPE(SQ_VTX_FETCH_VERTEX_DATA),
			FETCH_WHOLE_QUAD(0),
			BUFFER_ID(0),
			SRC_GPR(0),
			SRC_REL(ABSOLUTE),
			SRC_SEL_X(SQ_SEL_X),
			MEGA_FETCH_COUNT(12));
	//    write result to GPR 1 (X, Y = from memory, Z = 0, W = 1)
	//    signed integers 32 bit (pairs)
	vs[i++] = VTX_DWORD1_GPR(DST_GPR(1),
			DST_REL(0),
			DST_SEL_X(SQ_SEL_X),
			DST_SEL_Y(SQ_SEL_Y),
			DST_SEL_Z(SQ_SEL_0),
			DST_SEL_W(SQ_SEL_1),
			USE_CONST_FIELDS(0),
			DATA_FORMAT(FMT_32_32),
			NUM_FORMAT_ALL(SQ_NUM_FORMAT_INT),
			FORMAT_COMP_ALL(SQ_FORMAT_COMP_SIGNED),
			SRF_MODE_ALL(SRF_MODE_ZERO_CLAMP_MINUS_ONE));
	//    mega fetch
	vs[i++] = VTX_DWORD2(OFFSET(0),
			ENDIAN_SWAP(ENDIAN_NONE),
			CONST_BUF_NO_STRIDE(0),
			MEGA_FETCH(1));
	//    junk
	vs[i++] = VTX_DWORD_PAD;

	// 11 fetch color as 8:8:8:8 at offset 8 and place it to X, Y, Z, W of GPR 2
	//    as (khm) float [0,1] unsigned
	vs[i++] = VTX_DWORD0(VTX_INST(SQ_VTX_INST_FETCH),
			FETCH_TYPE(SQ_VTX_FETCH_VERTEX_DATA),
			FETCH_WHOLE_QUAD(0),
			BUFFER_ID(0),
			SRC_GPR(0),
			SRC_REL(ABSOLUTE),
			SRC_SEL_X(SQ_SEL_X),
			MEGA_FETCH_COUNT(4));
	vs[i++] = VTX_DWORD1_GPR(DST_GPR(2),
			DST_REL(0),
			DST_SEL_X(SQ_SEL_X),
			DST_SEL_Y(SQ_SEL_Y),
			DST_SEL_Z(SQ_SEL_Z),
			DST_SEL_W(SQ_SEL_W),
			USE_CONST_FIELDS(0),
			DATA_FORMAT(FMT_8_8_8_8),
			NUM_FORMAT_ALL(SQ_NUM_FORMAT_NORM),
			FORMAT_COMP_ALL(SQ_FORMAT_COMP_UNSIGNED),
			SRF_MODE_ALL(SRF_MODE_ZERO_CLAMP_MINUS_ONE));
	vs[i++] = VTX_DWORD2(OFFSET(8),
			ENDIAN_SWAP(ENDIAN_NONE),
			CONST_BUF_NO_STRIDE(0),
			MEGA_FETCH(0));
	vs[i++] = VTX_DWORD_PAD;

	radeon_cs_print(cs, fout);

	if (debug) {
		printf("vs:\n");
		for (j = 0; j < i; ++j)
			printf("0x%08X\n", vs[j]);
	}

	// Pixel shader program
	i = 0;
	// Send pixel to the pixel cache. 
	// element size is one dword, read data from GPR(0).
	ps[i++] = CF_ALLOC_IMP_EXP_DWORD0(ARRAY_BASE(CF_PIXEL_MRT0),
					TYPE(SQ_EXPORT_PIXEL),
					RW_GPR(0),
					RW_REL(ABSOLUTE),
					INDEX_GPR(0),
					ELEM_SIZE(1));
	// just export
	ps[i++] = CF_ALLOC_IMP_EXP_DWORD1_SWIZ(SRC_SEL_X(SQ_SEL_X),
					SRC_SEL_Y(SQ_SEL_Y),
					SRC_SEL_Z(SQ_SEL_Z),
					SRC_SEL_W(SQ_SEL_W),
					R6xx_ELEM_LOOP(0),
					BURST_COUNT(1),
					END_OF_PROGRAM(1),
					VALID_PIXEL_MODE(0),
					CF_INST(SQ_CF_INST_EXPORT_DONE),
					WHOLE_QUAD_MODE(0),
					BARRIER(1));
	if (debug) {
		printf("ps:\n");
		for (j = 0; j < i; ++j)
			printf("0x%08X\n", ps[j]);
	}

err:	
	;
}

