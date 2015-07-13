/*
 * 2015 Yellow Rabbit
 * A play with radeon 6670 command processor.
 */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <xf86drm.h>
#include <libdrm/radeon_cs_gem.h>

#include "r600.h"
#include "r600_reg_auto_r6xx.h"
#include "r600_reg_r6xx.h"

const char cardName[] = "/dev/dri/card0";

void printDriverInfo(int);
void testCP(int, struct radeon_cs_manager*);

int 
main (int argc, char *argv[])
{
	int fd;
	struct radeon_cs_manager *csm;

	printf("%s, version %s\n", argv[0], VERSION);

	/* our card is first */
	csm = NULL;
	fd = open(cardName, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Cant open card: %s\n", cardName);
		goto err;
	}

	printDriverInfo(fd);

	/* command stream manager */
	csm = radeon_cs_manager_gem_ctor(fd);
	if (csm == NULL) {
		fprintf(stderr, "Can't create CS manager.\n");
		goto err;
	}

	/* test working CP */
	testCP(fd, csm);

	/* cleanup */
	radeon_cs_manager_gem_dtor(csm);
	close(fd);
	return(0);
err:
	if (csm != NULL) {
		radeon_cs_manager_gem_dtor(csm);
	}
	if (fd >= 0) {
		close(fd);
	}
	return(1);
}

/*
 * Test for working CP
 */
#define NDW 10
void
testCP(int fd, struct radeon_cs_manager *csm)
{
	struct radeon_cs *cs;
	int ret, i;
	uint32_t val;

	cs = radeon_cs_create(csm, NDW);
	if (cs == NULL) {
		fprintf(stderr, "%s: can't create CS.", __FUNCTION__);
		goto err;
	}

	/* Initial state of scratch reg */
	ret = radeon_get_value(fd, SCRATCH_REG7, &val);
	if (ret) {
		fprintf(stderr, "%s: get value returned %d.", __FUNCTION__, ret);
		goto err;
	}
	printf("SCRATCH_REG7:%08x\n", val);

	/*  */
	radeon_cs_write_dword(cs, CP_PACKET0(SCRATCH_REG7, 0xdeadbeef));
	ret = radeon_cs_emit(cs);
	if (ret) {
		fprintf(stderr, "%s: emit returned %d.", __FUNCTION__, ret);
		goto err;
	}

	for(i = 0; ret < 1000L; ++ret) {
		radeon_get_value(fd, SCRATCH_REG7, &val);
		printf("SCRATCH_REG7:%08x\n", val);
	}
err:
	if (cs != NULL) {
		radeon_cs_destroy(cs);
	}
}

/*
 * Driver info.
 */
void
printDriverInfo(int fd)
{
	drmVersionPtr info;

	info = drmGetVersion(fd);
	if (info == NULL) {
		return;
	}
	printf("Driver info: %s %s v%d.%d\n", info->name, info->date,
				info->version_major, info->version_minor);

	drmFreeVersion(info);
}
