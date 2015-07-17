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
FILE *fout;

void printDriverInfo(int);

int 
main (int argc, char *argv[])
{
	int fd;
	struct radeon_cs_manager *csm;

	printf("%s, version %s\n", argv[0], VERSION);
	fout = fopen("cs.log", "w");

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

	/* test working CP + DMA */
	testCPDMA(fd, csm);

	/* cleanup */
	radeon_cs_manager_gem_dtor(csm);
	close(fd);
	fclose(fout);
	return(0);
err:
	if (csm != NULL) {
		radeon_cs_manager_gem_dtor(csm);
	}
	if (fd >= 0) {
		close(fd);
	}
	fclose(fout);
	return(1);
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
