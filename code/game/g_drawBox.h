#pragma once
#include "g_headers.h"
#include "g_local.h"
#include "b_local.h"
#include "g_functions.h"
#include "../cgame/cg_local.h" // centity_t declaration

void drawBoxPlayer(gentity_t* self);
void drawBoxNPC(gentity_t* self);
void drawBoxItems(gentity_t* self);
void drawBoxTriggers(centity_t* self);
