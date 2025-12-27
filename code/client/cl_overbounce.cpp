#include "client.h"
#include "..\speedrun\overbounce_prediction\overbounce_prediction.hpp"

static overbounce_prediction::CumulativeFrametimeStats cumulative_frametime;

void CL_ReportLastPmoveFrametime(int msec) {
	if ( Cvar_VariableIntegerValue("cg_drawOverbounceInfo") ) {
		cumulative_frametime.reportLastFrametime(msec);
	}
}

float CL_OverbounceProbability(const float height_difference, const float vertical_speed, const float gravity) {
	return overbounce_prediction::computeProbability(
		cumulative_frametime.relativeFrequencies(),
		height_difference, vertical_speed, gravity);
}
