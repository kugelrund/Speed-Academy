#pragma once
// g_drawBox.cpp
#include "g_drawBox.h"
#include "g_headers.h" // Why do I need to include it in the cpp and not the header ?!

/*
Tool used to be able to draw boxes for triggers, NPCs and items.
Application : routing all secrets in Academy and inspecting trigger range ingame instead of using an external mapping tool.

Still need improvement on colors / differentiation, white being the default as of now.
Shouldn't make the game heavily drop in fps if enabled, so even less when disabled (except some bool and int checks)

Entrypoint :
- CG_DrawActiveFrame : list every cgentity once per frame, and send their pointer here to prepare for rendering when conditions are met

Related variables :
- g_drawBoxTriggers : 0 or 1 (any int not 0) : draw in different colors (ex : pink or orange for secrets) triggers around the map
- g_drawBoxPlayer : 0 or 1 (any int not 0) : draw in RED, the box around the player
- g_drawBoxPlayerFP : 0 or 1 (any int not 0) : allows rendering if the box even in first person if set to 1 or more
- g_drawBoxNPC : 0 or 1 (any int not 0) : draw in GREEN, the boxes around NPCs (include spawned NPCs)
- g_drawBoxItems : 0 or 1 (any int not 0) : draw in BLUE, the boxes around items (include dropped weapons)

Posto
*/

// Could be more beautifully implemented instead of using the 'extern' keyword
// But since they are functions, and if I remember correctly, it should only be different during linking and shouldn't impact performances.
extern qhandle_t cgi_R_RegisterModel(const char* name);
extern void ScaleModelAxis(refEntity_t* ent);
extern qhandle_t cgi_R_RegisterShader(const char* name);
extern void cgi_R_AddRefEntityToScene(const refEntity_t* re);

/*
	Memo
	thinkF_NPC_Think = 28
	touchF_Touch_Item = 1
*/

refEntity_t prepareRefEnt(gentity_t* self, refEntity_t ent)
{
	vec3_t mins, maxs, center, size;

	VectorCopy(self->absmin, mins);
	VectorCopy(self->absmax, maxs);

	VectorAdd(mins, maxs, center);
	VectorScale(center, 0.5f, center);
	VectorSubtract(maxs, mins, size);

	ent.hModel = cgi_R_RegisterModel("models/lecube.md3");
	ent.reType = RT_MODEL;

	// Position at center, the box is around the center of the player
	VectorCopy(center, ent.origin);
	VectorCopy(center, ent.oldorigin);
	VectorCopy(center, ent.lightingOrigin);

	float divisor = 1.0f;
	VectorSet(ent.modelScale, size[0] / divisor, size[1] / divisor, size[2] / divisor);
	AxisClear(ent.axis);
	ScaleModelAxis(&ent);

	ent.customShader = cgi_R_RegisterShader("gfx/effects/solidWhite");
	ent.renderfx = RF_RGB_TINT;// | RF_NOSHADOW;

	return ent;
}

refEntity_t prepareRefEnt(centity_t* self, refEntity_t ent)
{
	vec3_t mins, maxs, center, size;
	// Get the bounds from the entity state, set in MakeTriggerVisible

	VectorCopy(self->currentState.origin, mins);
	VectorCopy(self->currentState.origin2, maxs);

	VectorAdd(mins, maxs, center);
	VectorScale(center, 0.5f, center);
	VectorSubtract(maxs, mins, size);

	ent.hModel = cgi_R_RegisterModel("models/lecube.md3");
	ent.reType = RT_MODEL;

	// Position at center
	VectorCopy(center, ent.origin);
	VectorCopy(center, ent.oldorigin);
	VectorCopy(center, ent.lightingOrigin);

	float divisor = 1.0f;
	VectorSet(ent.modelScale, size[0] / divisor, size[1] / divisor, size[2] / divisor);
	AxisClear(ent.axis);
	ScaleModelAxis(&ent);

	ent.customShader = cgi_R_RegisterShader("gfx/effects/solidWhite");
	ent.renderfx = RF_RGB_TINT;// | RF_NOSHADOW;

	return ent;
}

void drawBoxPlayer(gentity_t* self)
{
	refEntity_t	ent;
	memset(&ent, 0, sizeof(ent));
	ent = prepareRefEnt(self, ent);

	// Make it red and semi-transparent
	ent.shaderRGBA[0] = 50;
	ent.shaderRGBA[1] = 0;
	ent.shaderRGBA[2] = 0;
	ent.shaderRGBA[3] = 25;

	cgi_R_AddRefEntityToScene(&ent);
}

void drawBoxNPC(gentity_t* self)
{
	refEntity_t	ent;
	memset(&ent, 0, sizeof(ent));
	ent = prepareRefEnt(self, ent);

	// Make it green and semi-transparent
	ent.shaderRGBA[0] = 0;
	ent.shaderRGBA[1] = 50;
	ent.shaderRGBA[2] = 0;
	ent.shaderRGBA[3] = 25;

	cgi_R_AddRefEntityToScene(&ent);
}

void drawBoxItems(gentity_t* self)
{
	refEntity_t	ent;
	memset(&ent, 0, sizeof(ent));
	ent = prepareRefEnt(self, ent);

	// Make it blue and semi-transparent
	ent.shaderRGBA[0] = 0;
	ent.shaderRGBA[1] = 0;
	ent.shaderRGBA[2] = 50;
	ent.shaderRGBA[3] = 25;

	cgi_R_AddRefEntityToScene(&ent);
}

void drawBoxTriggers(centity_t* self)
{
	refEntity_t	ent;
	memset(&ent, 0, sizeof(ent));
	ent = prepareRefEnt(self, ent);

	switch (self->gent->e_TouchFunc)
	{
	case(touchF_NULL): // Small Red
		ent.shaderRGBA[0] = 50;
		ent.shaderRGBA[1] = 0;
		ent.shaderRGBA[2] = 0;
		ent.shaderRGBA[3] = 25;
		break;
	case(touchF_Touch_Item): // Small Green -- Reverse
		ent.shaderRGBA[0] = 0;
		ent.shaderRGBA[1] = 0;
		ent.shaderRGBA[2] = 50;
		ent.shaderRGBA[3] = 25;
		break;
	case(touchF_teleporter_touch): // Small Blue -- Reverse
		ent.shaderRGBA[0] = 0;
		ent.shaderRGBA[1] = 50;
		ent.shaderRGBA[2] = 0;
		ent.shaderRGBA[3] = 25;
		break;
	case(touchF_charge_stick): // Small Yellow
		ent.shaderRGBA[0] = 50;
		ent.shaderRGBA[1] = 50;
		ent.shaderRGBA[2] = 0;
		ent.shaderRGBA[3] = 25;
		break;
	case(touchF_Touch_DoorTrigger): // Small Purple
		ent.shaderRGBA[0] = 50;
		ent.shaderRGBA[1] = 0;
		ent.shaderRGBA[2] = 50;
		ent.shaderRGBA[3] = 25;
		break;
	case(touchF_Touch_PlatCenterTrigger): // Small Cyan
		ent.shaderRGBA[0] = 0;
		ent.shaderRGBA[1] = 50;
		ent.shaderRGBA[2] = 50;
		ent.shaderRGBA[3] = 25;
		break;
	case(touchF_Touch_Plat): // Small White
		ent.shaderRGBA[0] = 50;
		ent.shaderRGBA[1] = 50;
		ent.shaderRGBA[2] = 50;
		ent.shaderRGBA[3] = 25;
		break;
	case(touchF_Touch_Button): // Red
		ent.shaderRGBA[0] = 100;
		ent.shaderRGBA[1] = 0;
		ent.shaderRGBA[2] = 0;
		ent.shaderRGBA[3] = 25;
		break;
	case(touchF_Touch_Multi): // Blue
		ent.shaderRGBA[0] = 0;
		ent.shaderRGBA[1] = 100;
		ent.shaderRGBA[2] = 0;
		ent.shaderRGBA[3] = 25;
		break;
	case(touchF_trigger_push_touch): // Green
		ent.shaderRGBA[0] = 0;
		ent.shaderRGBA[1] = 0;
		ent.shaderRGBA[2] = 100;
		ent.shaderRGBA[3] = 25;
		break;
	case(touchF_trigger_teleporter_touch): // Yellow
		ent.shaderRGBA[0] = 100;
		ent.shaderRGBA[1] = 100;
		ent.shaderRGBA[2] = 0;
		ent.shaderRGBA[3] = 25;
		break;
	case(touchF_hurt_touch): // Purple
		ent.shaderRGBA[0] = 100;
		ent.shaderRGBA[1] = 0;
		ent.shaderRGBA[2] = 100;
		ent.shaderRGBA[3] = 25;
		break;
	case(touchF_NPC_Touch): // Cyan
		ent.shaderRGBA[0] = 0;
		ent.shaderRGBA[1] = 100;
		ent.shaderRGBA[2] = 100;
		ent.shaderRGBA[3] = 25;
		break;
	case(touchF_touch_ammo_crystal_tigger): // White
		ent.shaderRGBA[0] = 100;
		ent.shaderRGBA[1] = 100;
		ent.shaderRGBA[2] = 0;
		ent.shaderRGBA[3] = 25;
		break;
	case(touchF_funcBBrushTouch): // Big Red
		ent.shaderRGBA[0] = 150;
		ent.shaderRGBA[1] = 0;
		ent.shaderRGBA[2] = 0;
		ent.shaderRGBA[3] = 25;
		break;
	case(touchF_touchLaserTrap): // Big Blue
		ent.shaderRGBA[0] = 0;
		ent.shaderRGBA[1] = 150;
		ent.shaderRGBA[2] = 0;
		ent.shaderRGBA[3] = 25;
		break;
	case(touchF_prox_mine_stick): // Big Green
		ent.shaderRGBA[0] = 0;
		ent.shaderRGBA[1] = 0;
		ent.shaderRGBA[2] = 150;
		ent.shaderRGBA[3] = 25;
		break;
	case(touchF_func_rotating_touch): // Big Yellow
		ent.shaderRGBA[0] = 150;
		ent.shaderRGBA[1] = 150;
		ent.shaderRGBA[2] = 0;
		ent.shaderRGBA[3] = 25;
		break;
	case(touchF_TouchTieBomb): // Big Purple
		ent.shaderRGBA[0] = 150;
		ent.shaderRGBA[1] = 0;
		ent.shaderRGBA[2] = 150;
		ent.shaderRGBA[3] = 25;
		break;
	default:
		ent.shaderRGBA[0] = 50;
		ent.shaderRGBA[1] = 50;
		ent.shaderRGBA[2] = 0;
		ent.shaderRGBA[3] = 128;
		break;
	}
	// Override for some trigger in other color (ex : secrets)
	gentity_t* subTrigger = NULL;

	while ((subTrigger = G_Find(subTrigger, FOFS(targetname), self->gent->target)) != NULL)
	{
		switch (subTrigger->e_UseFunc)
		{
		case(useF_NPC_Spawn):
			ent.shaderRGBA[0] = 100;
			ent.shaderRGBA[1] = 50;
			ent.shaderRGBA[2] = 0;
			ent.shaderRGBA[3] = 128;
		case(useF_target_autosave_use):
			ent.shaderRGBA[0] = 0;
			ent.shaderRGBA[1] = 100;
			ent.shaderRGBA[2] = 50;
			ent.shaderRGBA[3] = 128;
		case(useF_target_secret_use):
			ent.shaderRGBA[0] = 100;
			ent.shaderRGBA[1] = 0;
			ent.shaderRGBA[2] = 50;
			ent.shaderRGBA[3] = 128;
		case(useF_security_panel_use):
			ent.shaderRGBA[0] = 25;
			ent.shaderRGBA[1] = 25;
			ent.shaderRGBA[2] = 25;
			ent.shaderRGBA[3] = 128;
		}
	}

	cgi_R_AddRefEntityToScene(&ent);
}