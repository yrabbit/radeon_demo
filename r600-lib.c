#include <xf86drm.h>
#include <libdrm/radeon_cs_gem.h>

#include "r600.h"
#include "r600_reg_auto_r6xx.h"
#include "r600_reg_r6xx.h"

void
mk_packet3(struct radeon_cs *cs, uint32_t cmd, uint32_t val)
{
	radeon_cs_write_dword(cs, CP_PACKET3(cmd, val));
}

void
mk_packet0(struct radeon_cs *cs, uint32_t reg, uint32_t val)
{
	radeon_cs_write_dword(cs, CP_PACKET0(reg, 1));
	radeon_cs_write_dword(cs, val);
}

int
read_reg(int fd, uint32_t reg, uint32_t *value)
{
	struct drm_radeon_info info = {};
	int r;

	*value = reg;
	info.request = RADEON_INFO_READ_REGISTER;
	info.value = (uintptr_t)value;
	r = drmCommandWriteRead(fd, DRM_RADEON_INFO, &info,
                            sizeof(struct drm_radeon_info));
	return r;
}

void
wait_reg(int fd, uint32_t reg, uint32_t v, const char *when)
{
	int i;
	uint32_t val;

	for (i = 0; i < 1e6; ++i) {
		read_reg(fd, reg, &val);
		if (val == v)
			break;
	}
	if (i == 1e6) {
		fprintf(stderr, "%s: still no set after %d loops: 0x%x, should be 0x%x\n",
				when, i, val, v);
	} else {
		printf("%s: set correctly after %d loops: 0x%x\n", when, i, v);
	}
}

