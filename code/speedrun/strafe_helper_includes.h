#ifndef STRAFE_HELPER_INCLUDES_H
#define STRAFE_HELPER_INCLUDES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdio.h>

struct shi_ColorRGBA {
    float r;
    float g;
    float b;
    float a;
};

struct shi_ColorRGBA shi_getColorAccelerating(void);
struct shi_ColorRGBA shi_getColorOptimal(void);
struct shi_ColorRGBA shi_getColorCenterMarker(void);
struct shi_ColorRGBA shi_getColorSpeed(void);

void shi_drawFilledRectangle(float x, float y, float w, float h, struct shi_ColorRGBA color);
void shi_drawString(float x, float y, const char* string, float scale, struct shi_ColorRGBA color);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // STRAFE_HELPER_INCLUDES_H
