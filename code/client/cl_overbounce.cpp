#include "client.h"
#include "..\speedrun\overbounce_prediction\overbounce_prediction.hpp"

static overbounce_prediction::CumulativeFrametimeStats cumulative_frametime;

void CL_ReportLastPmoveFrametime(int msec) {
	if ( Cvar_VariableIntegerValue("cg_drawOverbounceInfo") || Cvar_VariableIntegerValue("r_overbouncePrediction") ) {
		cumulative_frametime.reportLastFrametime(msec);
	}
}

float CL_OverbounceProbability(const float height_difference, const float vertical_speed, const float gravity) {
	return overbounce_prediction::computeProbability(
		cumulative_frametime.relativeFrequencies(),
		height_difference, vertical_speed, gravity);
}

OverbounceLevel CL_ClosestOverbounceLevel(const float height_difference) {
	const auto level = overbounce_prediction::closestOverbounceLevel(
		cumulative_frametime.relativeFrequencies(),
		height_difference, cl.frame.ps.velocity[2], cl.frame.ps.gravity);
	return {
		static_cast<float>(level.max_height_difference),
		static_cast<float>(level.min_height_difference),
		static_cast<float>(level.probability)
	};
}
