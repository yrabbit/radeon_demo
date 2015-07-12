/*
 * 2015 Yellow Rabbit
 * A play with radeon 6670 command processor.
 */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <xf86drm.h>

const char cardName[] = "/dev/dri/card0";

void printInfo(int);

int 
main (int argc, char *argv[])
{
	int fd;

	printf("%s, version %s\n", argv[0], VERSION);

	/* Our card is first */
	fd = open(cardName, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Cant open card: %s\n", cardName);
		goto err;
	}

	printInfo(fd);

	return(0);
err:
	if (fd >= 0) {
		close(fd);
	}
	return(1);
}

/*
 * Driver info
 */
void
printInfo(int fd)
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
