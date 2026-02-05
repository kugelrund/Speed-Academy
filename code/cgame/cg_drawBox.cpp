#pragma once
// cg_drawBox.cpp
#include "cg_drawBox.h"
#include "cg_headers.h"

/*
Tool used to be able to draw boxes for triggers, NPCs and items.
Application : routing all secrets in Academy and inspecting trigger range ingame instead of using an external mapping tool.

Still need improvement on colors / differentiation, white being the default as of now.
Shouldn't make the game heavily drop in fps if enabled, so even less when disabled (except some bool and int checks)

Entrypoint :
- CG_DrawActiveFrame : list every cgentity once per frame, and send their pointer here to prepare for rendering when conditions are met

Related variables :
- cg_drawBoxTriggers : 0 or 1 (any int not 0) : draw in different colors (ex : pink or orange for secrets) triggers around the map
- cg_drawBoxPlayer : 0 or 1 (any int not 0) : draw in RED, the box around the player
- cg_drawBoxPlayerFP : 0 or 1 (any int not 0) : allows rendering if the box even in first person if set to 1 or more
- cg_drawBoxNPC : 0 or 1 (any int not 0) : draw in GREEN, the boxes around NPCs (include spawned NPCs)
- cg_drawBoxItems : 0 or 1 (any int not 0) : draw in BLUE, the boxes around items (include dropped weapons)

Posto
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

void setColorForTrigger(gentity_t* self, refEntity_t& ent)
{
	gentity_t* subTrigger = NULL;

	//while ((subTrigger = G_Find(subTrigger, FOFS(targetname), self->target)) != NULL)
	if ((subTrigger = G_Find(subTrigger, FOFS(targetname), self->target)) != NULL)
	{
		switch (subTrigger->e_UseFunc)
		{
		// CASE : spawns, in orange. Can be seen everywhere.
		case(useF_NPC_Spawn):
		case(useF_item_spawn_use):
		case(useF_NPC_VehicleSpawnUse):
			ent.shaderRGBA[0] = 100;
			ent.shaderRGBA[1] = 50;
			ent.shaderRGBA[2] = 0;
			break;
		// CASE : world/save/map related, in pink.
		case(useF_target_autosave_use):
		case(useF_target_level_change_use):
		case(useF_target_secret_use):
			ent.shaderRGBA[0] = 100;
			ent.shaderRGBA[1] = 0;
			ent.shaderRGBA[2] = 50;
			break;
		// CASE : interactible elements, in green.
		case(useF_security_panel_use):
		case(useF_misc_model_use):
		case(useF_Use_Item):
		case(useF_Use_Shooter):
		case(useF_funcBBrushUse):
		case(useF_target_activate_use):
		case(useF_target_deactivate_use):
		case(useF_health_use):
		case(useF_ammo_use):
		case(useF_mega_ammo_use):
		case(useF_turret_base_use):
		case(useF_laser_arm_use):
		case(useF_emplaced_gun_use):
		case(useF_shield_power_converter_use):
		case(useF_ammo_power_converter_use):
		case(useF_camera_use):
		case(useF_sentry_use):
		case(useF_misc_atst_use):
		case(useF_panel_turret_use):
		case(useF_misc_dlight_use):
		case(useF_TieFighterUse):
		case(useF_TrainUse):
		case(useF_bomb_planted_use):
		case(useF_beacon_use):
		case(useF_ion_cannon_use):
		case(useF_spotlight_use):
		case(useF_welder_use):
		case(useF_jabba_cam_use):
		case(useF_misc_use):
		case(useF_pas_use):
		case(useF_misc_weapon_shooter_use):
		case(useF_eweb_use):
			ent.shaderRGBA[0] = 0;
			ent.shaderRGBA[1] = 100;
			ent.shaderRGBA[2] = 0;
			break;
		// CASE : 'scripts' / 'func', in greenish cyan.
		case(useF_Use_Multi):
		case(useF_func_usable_use):
		case(useF_trigger_entdist_use):
		case(useF_target_change_parm_use):
		case(useF_func_static_use):
		case(useF_func_bobbing_use):
		case(useF_func_rotating_use):
		case(useF_funcGlassUse):
		case(useF_func_timer_use):
			ent.shaderRGBA[0] = 0;
			ent.shaderRGBA[1] = 100;
			ent.shaderRGBA[2] = 50;
			break;
		// CASE : 'target' ??????, in purple. Note : not found anything is purple during tests
		case(useF_Use_Target_Give):
		case(useF_Use_Target_Delay):
		case(useF_Use_Target_Score):
		case(useF_Use_Target_Print):
		case(useF_Use_Target_Speaker):
		case(useF_target_laser_use):
		case(useF_target_relay_use):
		case(useF_target_kill_use):
		case(useF_target_counter_use):
		case(useF_target_random_use):
		case(useF_target_scriptrunner_use):
		case(useF_target_gravity_change_use):
		case(useF_target_friction_change_use):
		case(useF_target_teleporter_use):
		case(useF_Use_target_push):
			ent.shaderRGBA[0] = 100;
			ent.shaderRGBA[1] = 0;
			ent.shaderRGBA[2] = 100;
			break;
		// CASE : effects fx & sound, in yellow
		case(useF_fx_runner_use):
		case(useF_fx_explosion_trail_use):
		case(useF_fx_target_beam_use):
		case(useF_target_play_music_use):
			ent.shaderRGBA[0] = 100;
			ent.shaderRGBA[1] = 66;
			ent.shaderRGBA[2] = 0;
			break;
		// CASE : unknown cases, let it be white
		case(useF_GoExplodeDeath):
		case(useF_Use_BinaryMover):
		case(useF_use_wall):
		case(useF_hurt_use):
		case(useF_NPC_Use):
		case(useF_crew_beam_in_use):
		case(useF_misc_model_useup):
		case(useF_misc_portal_use):
		case(useF_switch_models):
		case(useF_misc_replicator_item_spawn):
		case(useF_misc_replicator_item_remove):
		case(useF_misc_trip_mine_activate):
		default:
			ent.shaderRGBA[0] = 20;
			ent.shaderRGBA[1] = 20;
			ent.shaderRGBA[2] = 20;
			break;
		}
		ent.shaderRGBA[3] = 128;
	}
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

void drawBoxWorldTriggers(gentity_t* self)
{
	refEntity_t	ent;
	memset(&ent, 0, sizeof(ent));
	ent = prepareRefEnt(self, ent);

	// Default color: blue
	ent.shaderRGBA[0] = 0;
	ent.shaderRGBA[1] = 0;
	ent.shaderRGBA[2] = 100;
	ent.shaderRGBA[3] = 25;

	// Override for some trigger in other color (ex : secrets)
	// Do not use blue nor red to know we correctly override the previous color.
	setColorForTrigger(self, ent);

	// At the end, if the trigger has been used, display it in red
	if (self->e_TouchFunc == touchF_NULL)
	{
		// Red for deactivated triggers
		ent.shaderRGBA[0] = 50;
		ent.shaderRGBA[1] = 0;
		ent.shaderRGBA[2] = 0;
		ent.shaderRGBA[3] = 25;
	}

	cgi_R_AddRefEntityToScene(&ent);
}

void drawBoxObjectTriggers(gentity_t* self)
{
	refEntity_t	ent;
	memset(&ent, 0, sizeof(ent));
	ent = prepareRefEnt(self, ent);

	// Default color: blue
	ent.shaderRGBA[0] = 0;
	ent.shaderRGBA[1] = 0;
	ent.shaderRGBA[2] = 100;
	ent.shaderRGBA[3] = 25;

	// Change in other color (ex : secrets). The first secret in t1_fatal is obtained by destroying a 3D object, it will be colored in pink.
	// Do not use blue nor red to know we correctly override the previous color.
	setColorForTrigger(self, ent);

	cgi_R_AddRefEntityToScene(&ent);
}
