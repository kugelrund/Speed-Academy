// cg_drawBox.cpp
#include "cg_draw_speedrun_extras.h"
#include "cg_headers.h"
#include "cg_media.h"

// Filter consts
static const int trig_filter_spawner = 1 << 0;
static const int trig_filter_world = 1 << 1;
static const int trig_filter_usable = 1 << 2;
static const int trig_filter_func = 1 << 3;
static const int trig_filter_target = 1 << 4;
static const int trig_filter_soundsfx = 1 << 5;
static const int trig_filter_uncategorized = 1 << 6;
static const int trig_filter_doors = 1 << 7;
static const int trig_filter_hurt = 1 << 8;
static const int trig_filter_UNUSED = 1 << 9;
// 1<<10 to 1<<15 not used

static void drawBoundingBox(const gentity_t* ent, const byte color[4])
{
	polyVert_t vertices[4];
	for (int i = 0; i < 4; ++i)
	{
		vertices[i].modulate[0] = color[0];
		vertices[i].modulate[1] = color[1];
		vertices[i].modulate[2] = color[2];
		vertices[i].modulate[3] = color[3];
		vertices[i].st[0] = 0.0f;
		vertices[i].st[1] = 0.0f;
	}

	for ( int axis_fixed = 0; axis_fixed < 3; ++axis_fixed )
	{
		const int axis_prev = (axis_fixed == 0) ? 2 : (axis_fixed - 1);
		const int axis_next = (axis_fixed == 2) ? 0 : (axis_fixed + 1);

		vertices[0].xyz[axis_prev] = ent->absmin[axis_prev];
		vertices[0].xyz[axis_next] = ent->absmin[axis_next];
		
		vertices[1].xyz[axis_prev] = ent->absmax[axis_prev];
		vertices[1].xyz[axis_next] = ent->absmin[axis_next];
		
		vertices[2].xyz[axis_prev] = ent->absmax[axis_prev];
		vertices[2].xyz[axis_next] = ent->absmax[axis_next];

		vertices[3].xyz[axis_prev] = ent->absmin[axis_prev];
		vertices[3].xyz[axis_next] = ent->absmax[axis_next];

		for (int i = 0; i < 4; ++i) {
			vertices[i].xyz[axis_fixed] = ent->absmin[axis_fixed];
		}
		cgi_R_AddPolyToScene( cgs.media.solidWhiteShader, 4, vertices );
		for (int i = 0; i < 4; ++i) {
			vertices[i].xyz[axis_fixed] = ent->absmax[axis_fixed];
		}
		cgi_R_AddPolyToScene( cgs.media.solidWhiteShader, 4, vertices );
	}
}

static void setColorForTrigger(gentity_t* self, byte color[4])
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
			color[0] = 100;
			color[1] = 50;
			color[2] = 0;
			break;
		// CASE : world/save/map related, in pink.
		case(useF_target_autosave_use):
		case(useF_target_level_change_use):
		case(useF_target_secret_use):
			color[0] = 100;
			color[1] = 0;
			color[2] = 50;
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
			color[0] = 0;
			color[1] = 100;
			color[2] = 0;
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
			color[0] = 0;
			color[1] = 100;
			color[2] = 50;
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
			color[0] = 100;
			color[1] = 0;
			color[2] = 100;
			break;
		// CASE : effects fx & sound, in yellow
		case(useF_fx_runner_use):
		case(useF_fx_explosion_trail_use):
		case(useF_fx_target_beam_use):
		case(useF_target_play_music_use):
			color[0] = 100;
			color[1] = 66;
			color[2] = 0;
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
			color[0] = 20;
			color[1] = 20;
			color[2] = 20;
			break;
		}
		color[3] = 128;
	}
}

static void drawBoxPlayer(gentity_t* self)
{
	// Make it red and semi-transparent
	byte color[4];
	color[0] = 50;
	color[1] = 0;
	color[2] = 0;
	color[3] = 25;

	drawBoundingBox(self, color);
}

static void drawBoxNPC(gentity_t* self)
{
	// Make it green and semi-transparent
	byte color[4];
	color[0] = 0;
	color[1] = 50;
	color[2] = 0;
	color[3] = 25;

	drawBoundingBox(self, color);
}

static void drawBoxItems(gentity_t* self)
{
	// Make it blue and semi-transparent
	byte color[4];
	color[0] = 0;
	color[1] = 0;
	color[2] = 50;
	color[3] = 25;

	drawBoundingBox(self, color);
}

static void drawBoxWorldTriggers(gentity_t* self)
{
	// Default color: blue
	byte color[4];
	color[0] = 0;
	color[1] = 0;
	color[2] = 100;
	color[3] = 25;

	// Override for some trigger in other color (ex : secrets)
	// Do not use blue nor red to know we correctly override the previous color.
	setColorForTrigger(self, color);

	// At the end, if the trigger has been used, display it in red
	if (self->e_TouchFunc == touchF_NULL)
	{
		// Red for deactivated triggers
		color[0] = 50;
		color[1] = 0;
		color[2] = 0;
		color[3] = 25;
	}

	drawBoundingBox(self, color);
}

static void drawBoxObjectTriggers(gentity_t* self)
{
	// Default color: blue
	byte color[4];
	color[0] = 0;
	color[1] = 0;
	color[2] = 100;
	color[3] = 25;

	// Change in other color (ex : secrets). The first secret in t1_fatal is obtained by destroying a 3D object, it will be colored in pink.
	// Do not use blue nor red to know we correctly override the previous color.
	setColorForTrigger(self, color);

	drawBoundingBox(self, color);
}

void CG_DrawBoxes()
{
	///// Any new / other display than trigger related /////

	// Checkpoint visualizer
	if (gi.Cvar_VariableIntegerValue("sv_speedrunModeCheckpoint") && cg_drawSpeedrunCheckpoint.integer)
	{
		gentity_t fakeEntity{};
		byte color[4];
		if (!cgi_SpeedrunIsRunFinished())
		{
			color[0] = 0;
			color[1] = 50;
			color[2] = 0;
			color[3] = 25;
		}
		else
		{
			color[0] = 50;
			color[1] = 0;
			color[2] = 0;
			color[3] = 25;
		}
		fakeEntity.absmin[0] = gi.cvar("sv_timedCheckpointMinX", "", CVAR_ARCHIVE)->value;
		fakeEntity.absmin[1] = gi.cvar("sv_timedCheckpointMinY", "", CVAR_ARCHIVE)->value;
		fakeEntity.absmin[2] = gi.cvar("sv_timedCheckpointMinZ", "", CVAR_ARCHIVE)->value;
		fakeEntity.absmax[0] = gi.cvar("sv_timedCheckpointMaxX", "", CVAR_ARCHIVE)->value;
		fakeEntity.absmax[1] = gi.cvar("sv_timedCheckpointMaxY", "", CVAR_ARCHIVE)->value;
		fakeEntity.absmax[2] = gi.cvar("sv_timedCheckpointMaxZ", "", CVAR_ARCHIVE)->value;
		drawBoundingBox(&fakeEntity, color);
	}

	///// Trigger related boxes /////


	// Player
	if (cg_drawBoxPlayer.integer && (cg.renderingThirdPerson || (!cg.renderingThirdPerson && cg_drawBoxPlayerFP.integer)))
	{
		// Player is always the first on this array
		drawBoxPlayer(&g_entities[0]);
	}

	if (!cg_drawBoxNPC.integer && !cg_drawBoxItems.integer && !cg_drawBoxTriggers.integer) {
		// no need to do the loop if nonce of these is enabled
		return;
	}

	for (int i = 0; i < MAX_GENTITIES; ++i)
	{
		// NPCs
		if (cg_drawBoxNPC.integer && g_entities[i].e_ThinkFunc == thinkF_NPC_Think)
		{
			drawBoxNPC(&g_entities[i]);
		}

		// Items
		if (cg_drawBoxItems.integer && g_entities[i].e_TouchFunc == touchF_Touch_Item)
		{
			drawBoxItems(&g_entities[i]);
		}

		// Triggers, but related to the world (not associated with an ingame object like a button or a camera)
		if (cg_drawBoxTriggers.integer && g_entities[i].classname &&
		    ( strcmp( g_entities[i].classname, "trigger_multiple" ) == 0  ||
		      strcmp( g_entities[i].classname, "trigger_once" ) == 0 ))
		{
			drawBoxWorldTriggers(&g_entities[i]);
		}

		// Triggers, but related to	an object like a button or a camera
		if (cg_drawBoxTriggers.integer && g_entities[i].classname &&
		    ( strncmp(g_entities[i].classname, "func_", strlen("func_")) == 0 ||
		      strncmp(g_entities[i].classname, "misc_", strlen("misc_")) == 0)
			// bigbomb | t2_wedge
			)
		{
			drawBoxObjectTriggers(&g_entities[i]);
		}
	}
}
