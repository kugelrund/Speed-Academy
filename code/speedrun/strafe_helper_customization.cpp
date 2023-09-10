#include "strafe_helper/strafe_helper_customization.h"

#include "../cgame/cg_local.h"
#include "../cgame/cg_media.h"

#include <cassert>


namespace {

struct StrafeHelperColor {
    vec4_t data;
};

StrafeHelperColor getColorForElement(const shc_ElementId element_id)
{
	switch (element_id) {
		case shc_ElementId_AcceleratingAngles:
			return {cg_strafeHelperColorAcceleratingR.value,
			        cg_strafeHelperColorAcceleratingG.value,
			        cg_strafeHelperColorAcceleratingB.value,
			        cg_strafeHelperColorAcceleratingA.value};
		case shc_ElementId_OptimalAngle:
			return {cg_strafeHelperColorOptimalR.value,
			        cg_strafeHelperColorOptimalG.value,
			        cg_strafeHelperColorOptimalB.value,
			        cg_strafeHelperColorOptimalA.value};
		case shc_ElementId_CenterMarker:
			return {cg_strafeHelperColorCenterMarkerR.value,
			        cg_strafeHelperColorCenterMarkerG.value,
			        cg_strafeHelperColorCenterMarkerB.value,
			        cg_strafeHelperColorCenterMarkerA.value};
		case shc_ElementId_SpeedText:
			return {cg_strafeHelperColorSpeedR.value,
			        cg_strafeHelperColorSpeedG.value,
			        cg_strafeHelperColorSpeedB.value,
			        cg_strafeHelperColorSpeedA.value};
	}
	assert(false);
	return {1.0, 1.0, 1.0, 1.0};
}

}  // anonymous namespace


extern "C" {

void shc_drawFilledRectangle(const float x, const float y,
                             const float w, const float h,
                             const shc_ElementId element_id)
{
	const auto color = getColorForElement(element_id);
	CG_FillRect(x, y, w, h, color.data);
}

void shc_drawString(const float x, const float y, const char* string,
                    const float scale, const shc_ElementId element_id)
{
	const auto color = getColorForElement(element_id);
	const float w = cgi_R_Font_StrLenPixels(string, cgs.media.qhFontMedium, scale);
	cgi_R_Font_DrawString(x - w/2, y, string, color.data, cgs.media.qhFontMedium, -1, scale);
}

}  // extern "C"
