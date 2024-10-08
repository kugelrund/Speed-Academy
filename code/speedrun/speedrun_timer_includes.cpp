#include "speedrun_timer_includes.h"
#include "../../server/exe_headers.h"
#include "../qcommon/stv_version.h"


namespace {

void SpeedrunTimerPrint(const char* levelTimeString,
                        const char* totalTimeString) {
	Com_Printf(S_COLOR_RED "====== (v" VERSION_STRING_DOTTED ") ======\n"
	           S_COLOR_GREEN "Level time: " S_COLOR_WHITE "%s\n",
	           levelTimeString);
	if (totalTimeString != nullptr)
	{
		Com_Printf(S_COLOR_GREEN "Total time: " S_COLOR_WHITE "%s\n",
		           totalTimeString);
	}
	Com_Printf(S_COLOR_RED "====== (v" VERSION_STRING_DOTTED ") ======\n");
}

void SpeedrunTimerPrintNoNotify(const char* levelTimeString,
                                const char* totalTimeString) {
	Com_PrintfNoNotify(S_COLOR_RED "====== (v" VERSION_STRING_DOTTED ") ======\n"
	                   S_COLOR_GREEN "Level time: " S_COLOR_WHITE "%s\n",
	                   levelTimeString);
	if (totalTimeString != nullptr)
	{
		Com_PrintfNoNotify(S_COLOR_GREEN "Total time: " S_COLOR_WHITE "%s\n",
		                   totalTimeString);
	}
	Com_PrintfNoNotify(S_COLOR_RED "====== (v" VERSION_STRING_DOTTED ") ======\n");
}

}  // anonymous namespace


void SpeedrunTimerPrint(bool printNotify,
                        const char *levelTimeString,
                        const char *totalTimeString) {
    if (printNotify) {
        SpeedrunTimerPrint(levelTimeString, totalTimeString);
    } else {
        SpeedrunTimerPrintNoNotify(levelTimeString, totalTimeString);
    }
}

int SpeedrunTimerGetSystemMilliseconds() {
    return Sys_Milliseconds();
}
