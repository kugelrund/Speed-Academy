#include "strafe_helper_includes.h"
#include "../cgame/cg_local.h"
#include "../cgame/cg_media.h"


extern "C" {

shi_ColorRGBA shi_getColorAccelerating() {
	return {cg_strafeHelperColorAcceleratingR.value,
	        cg_strafeHelperColorAcceleratingG.value,
	        cg_strafeHelperColorAcceleratingB.value,
	        cg_strafeHelperColorAcceleratingA.value};
}

shi_ColorRGBA shi_getColorOptimal() {
	return {cg_strafeHelperColorOptimalR.value,
	        cg_strafeHelperColorOptimalG.value,
	        cg_strafeHelperColorOptimalB.value,
	        cg_strafeHelperColorOptimalA.value};
}

shi_ColorRGBA shi_getColorCenterMarker() {
	return {cg_strafeHelperColorCenterMarkerR.value,
	        cg_strafeHelperColorCenterMarkerG.value,
	        cg_strafeHelperColorCenterMarkerB.value,
	        cg_strafeHelperColorCenterMarkerA.value};
}

shi_ColorRGBA shi_getColorSpeed() {
	return {cg_strafeHelperColorSpeedR.value,
	        cg_strafeHelperColorSpeedG.value,
	        cg_strafeHelperColorSpeedB.value,
	        cg_strafeHelperColorSpeedA.value};
}

void shi_drawFilledRectangle(const float x, const float y,
                             const float w, const float h,
                             const shi_ColorRGBA color)
{
	const vec4_t color_converted = {color.r, color.g, color.b, color.a};
	CG_FillRect(x, y, w, h, color_converted);
}

void shi_drawString(float x, float y, const char* string, float scale, const shi_ColorRGBA color)
{
	const vec4_t color_converted = {color.r, color.g, color.b, color.a};
	const float w = cgi_R_Font_StrLenPixels(string, cgs.media.qhFontMedium, scale);
	cgi_R_Font_DrawString(x - w/2, y, string, color_converted, cgs.media.qhFontMedium, -1, scale);
}

} // extern "C"
