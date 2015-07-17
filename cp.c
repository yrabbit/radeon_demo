/*
 * Test command processor
 */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <xf86drm.h>
#include <libdrm/radeon_cs_gem.h>

#include "r600.h"
#include "r600_reg_auto_r6xx.h"
#include "r600_reg_r6xx.h"

extern FILE* fout;


/*
 * Test for working CP
 */
#define NDW 20
void
testCP(int fd, struct radeon_cs_manager *csm)
{
	struct radeon_cs *cs;
	int ret;
	uint32_t val;

	cs = radeon_cs_create(csm, NDW);
	if (cs == NULL) {
		fprintf(stderr, "%s: can't create CS.", __FUNCTION__);
		goto err;
	}

	/* Initial state of scratch reg */
	ret = read_reg(fd, SCRATCH_REG7, &val);
	if (ret) {
		fprintf(stderr, "%s: get value returned %d.", __FUNCTION__, ret);
		goto err;
	}
	printf("Before pack0 SCRATCH_REG7:%08x\n", val);

	/* Make command stream for writing to scratch register some value */
	/* packet type 0  */
	radeon_cs_write_dword(cs, CP_PACKET0(SCRATCH_REG7, 1));
	radeon_cs_write_dword(cs, 0xcafebabe);
	ret = radeon_cs_emit(cs);
	if (ret) {
		fprintf(stderr, "%s: emit returned %d.", __FUNCTION__, ret);
		goto err;
	}

	radeon_cs_print(cs, fout);
	wait_reg(fd, SCRATCH_REG7, 0xcafebabe, "Pack0 test");

err:
	if (cs != NULL) {
		radeon_cs_destroy(cs);
	}
}

/*
 * Test for working CP+DMA
 */
void
testCPDMA(int fd, struct radeon_cs_manager *csm)
{
	struct radeon_cs *cs;
	int ret;
	uint32_t val;

	cs = radeon_cs_create(csm, NDW);
	if (cs == NULL) {
		fprintf(stderr, "%s: can't create CS.", __FUNCTION__);
		goto err;
	}

	/* Initial state of scratch reg */
	
	ret = read_reg(fd, SCRATCH_REG7, &val);
	if (ret) {
		fprintf(stderr, "%s: get value returned %d.", __FUNCTION__, ret);
		goto err;
	}
	printf("Before pack0 SCRATCH_REG7:%08x\n", val);

	/* Make command stream for writing to scratch register some value */
	/* 0: packet type 0  */
	radeon_cs_write_dword(cs, CP_PACKET0(SCRATCH_REG7, 1));
	radeon_cs_write_dword(cs, 0xdeadbeef);
	ret = radeon_cs_emit(cs);
	wait_reg(fd, SCRATCH_REG7, 0xdeadbeef, "DMA test");
	/* 1: packet type 0  */
	radeon_cs_write_dword(cs, CP_PACKET0(SCRATCH_REG7, 1));
	radeon_cs_write_dword(cs, 0xfeedface);
	ret = radeon_cs_emit(cs);
	wait_reg(fd, SCRATCH_REG7, 0xfeedface, "DMA test");

err:
	if (cs != NULL) {
		radeon_cs_destroy(cs);
	}
}
