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

static void drawLine(vec3_t p1, vec3_t p2, byte color[4], float width, bool onFirstHit)
{
	trace_t trace;

	// Copied from CG_ScanForCrosshairEntity
	memset(&trace, 0, sizeof(trace));
	if (onFirstHit)
	{
		gi.trace(&trace, p1, vec3_origin, vec3_origin, p2,
			1, MASK_OPAQUE | CONTENTS_SHOTCLIP | CONTENTS_BODY | CONTENTS_ITEM | CONTENTS_TERRAIN, G2_NOCOLLIDE, 10);
		VectorCopy(trace.endpos, p2);
	}

	refEntity_t re{};
	re.reType = RT_LINE;
	re.radius = width;
	re.customShader = cgi_R_RegisterShader("gfx/misc/whiteline2");
	for (int i = 0; i < 4; ++i) { re.shaderRGBA[i] = color[i]; }
	VectorCopy(p1, re.origin);
	VectorCopy(p2, re.oldorigin);
	cgi_R_AddRefEntityToScene(&re);
}

// Draws a cube given it's center. Mostly copied from drawBoundingBox
static void drawPositionMarker(vec3_t center, byte color[4], float width, float deltaZ)
{
	refEntity_t re{};
	re.reType = RT_SPRITE;
	re.radius = width;
	re.customShader = cgi_R_RegisterShader("gfx/misc/nav_node");
	for (int i = 0; i < 4; ++i) { re.shaderRGBA[i] = color[i]; }
	VectorCopy(center, re.origin);
	re.origin[2] += deltaZ;
	cgi_R_AddRefEntityToScene(&re);
}

static void drawLineOfSight(const gentity_t* ent, float width)
{
	byte color[4] = { 0, 0, 150, 255 };
	vec3_t p1, p2;
	vec3_t d_f, d_rt, d_up;

	// Set the first point to be where the entity is looking at (eye) and the end where it's looking at (direction_forward)
	VectorCopy(ent->client->renderInfo.eyePoint, p1);
	AngleVectors(ent->currentAngles, d_f, d_rt, d_up);
	// Copied from CG_ScanForCrosshairEntity
	VectorMA(p1, 4096, d_f, p2);

	drawLine(p1, p2, color, width, true);
}

static void drawVelocityVector(const gentity_t* ent, float width)
{
	byte color[4] = { 100, 100, 100, 255 };
	vec3_t p1, p2;

	VectorCopy(ent->client->ps.origin, p1);
	p1[2] += 10; // Add an offset so that we can see the start of line around the waist instead of the feets

	// Length in 3D is velovity divided by 10
	VectorMA(p1, 1.0 / 10.0, ent->client->ps.velocity, p2);
	p2[2] = p1[2]; // We don't care about the Z component (?)

	// TODO for next time : make it longer but curved, to also add a prediction instead of only the direction.

	drawLine(p1, p2, color, width, true);
}

static void drawNPCPathing(gentity_t* self)
{
	vec3_t centerNPC = { 0, 0, 0 };
	vec3_t centerGoal = { 0, 0, 0 };
	byte colorGoal[4] = { 150, 100, 0, 255 };

	// If the NPC has a goalEntity (points in the world called 'waypoints'), they will try to get to it.
	// For robots, the goalEntity is often Kyle, meaning that they will try to get straight to him.
	if (self->NPC && self->NPC->goalEntity)
	{
		VectorAdd(self->absmin, self->absmax, centerNPC);
		centerNPC[2] -= 20; // To start a little big lower
		VectorScale(centerNPC, 0.5, centerNPC);
		VectorAdd(self->NPC->goalEntity->absmin, self->NPC->goalEntity->absmax, centerGoal);
		VectorScale(centerGoal, 0.5, centerGoal);

		drawLine(centerNPC, centerGoal, colorGoal, 4, false);
		drawPositionMarker(centerGoal, colorGoal, 10, 0);
	}
}

static void setColorForTrigger(gentity_t* self, byte color[4])
{
	gentity_t* subTrigger = NULL;
	byte red[4]{ 50, 0, 0, 25 };

	//while ((subTrigger = G_Find(subTrigger, FOFS(targetname), self->target)) != NULL)
	if ((subTrigger = G_Find(subTrigger, FOFS(targetname), self->target)) != NULL)
	// subtrigger has the u_useFunc but it's 'self' possess the e_TouchFunc defined to know if it's disabled
	{
		switch (subTrigger->e_UseFunc)
		{
		// CASE : spawns, in orange. Can be seen everywhere.
		case(useF_NPC_Spawn):
		case(useF_item_spawn_use):
		case(useF_NPC_VehicleSpawnUse):
			if (!cg_drawBoxTriggersFilter.integer || (cg_drawBoxTriggersFilter.integer & trig_filter_spawner))
			{
				if (self->e_TouchFunc == touchF_NULL && cg_drawBoxTriggersFilterDisabled.integer)
				{
					color[0] = red[0];
					color[1] = red[1];
					color[2] = red[2];
				}
				else if (self->e_TouchFunc != touchF_NULL)
				{
					color[0] = 200;
					color[1] = 100;
					color[2] = 0;
				}
			}
			break;
		// CASE : world/save/map related, in pink.
		case(useF_target_autosave_use):
		case(useF_target_level_change_use):
		case(useF_target_secret_use):
			if (!cg_drawBoxTriggersFilter.integer || (cg_drawBoxTriggersFilter.integer & trig_filter_world))
			{
				if (self->e_TouchFunc == touchF_NULL && cg_drawBoxTriggersFilterDisabled.integer)
				{
					color[0] = red[0];
					color[1] = red[1];
					color[2] = red[2];
				}
				else if (self->e_TouchFunc != touchF_NULL)
				{
					color[0] = 100;
					color[1] = 0;
					color[2] = 150;
				}
			}
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
			if (!cg_drawBoxTriggersFilter.integer || (cg_drawBoxTriggersFilter.integer & trig_filter_usable))
			{
				if (self->e_TouchFunc == touchF_NULL && cg_drawBoxTriggersFilterDisabled.integer)
				{
					color[0] = red[0];
					color[1] = red[1];
					color[2] = red[2];
				}
				else if (self->e_TouchFunc != touchF_NULL)
				{
					color[0] = 0;
					color[1] = 100;
					color[2] = 0;
				}
			}
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
			if (!cg_drawBoxTriggersFilter.integer || (cg_drawBoxTriggersFilter.integer & trig_filter_func))
			{
				if (self->e_TouchFunc == touchF_NULL && cg_drawBoxTriggersFilterDisabled.integer)
				{
					color[0] = red[0];
					color[1] = red[1];
					color[2] = red[2];
				}
				else if (self->e_TouchFunc != touchF_NULL)
				{
					color[0] = 0;
					color[1] = 100;
					color[2] = 50;
				}
			}
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
			if (!cg_drawBoxTriggersFilter.integer || (cg_drawBoxTriggersFilter.integer & trig_filter_target))
			{
				if (self->e_TouchFunc == touchF_NULL && cg_drawBoxTriggersFilterDisabled.integer)
				{
					color[0] = red[0];
					color[1] = red[1];
					color[2] = red[2];
				}
				else if (self->e_TouchFunc != touchF_NULL)
				{
					color[0] = 100;
					color[1] = 0;
					color[2] = 50;
				}
			}
			break;
		// CASE : effects fx & sound, in yellow
		case(useF_fx_runner_use):
		case(useF_fx_explosion_trail_use):
		case(useF_fx_target_beam_use):
		case(useF_target_play_music_use):
			if (!cg_drawBoxTriggersFilter.integer || (cg_drawBoxTriggersFilter.integer & trig_filter_soundsfx))
			{
				if (self->e_TouchFunc == touchF_NULL && cg_drawBoxTriggersFilterDisabled.integer)
				{
					color[0] = red[0];
					color[1] = red[1];
					color[2] = red[2];
				}
				else if (self->e_TouchFunc != touchF_NULL)
				{
					color[0] = 100;
					color[1] = 50;
					color[2] = 0;
				}
			}
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
			if (!cg_drawBoxTriggersFilter.integer || (cg_drawBoxTriggersFilter.integer & trig_filter_uncategorized))
			{
				if (self->e_TouchFunc != touchF_NULL)
				{
					color[0] = 20;
					color[1] = 20;
					color[2] = 20;
				}
				else if (self->e_TouchFunc == touchF_NULL && cg_drawBoxTriggersFilterDisabled.integer)
				{
					color[0] = red[0];
					color[1] = red[1];
					color[2] = red[2];
				}
			}
			break;
		}
		color[3] = 128;
	}
	else
	{
		// Example : some triggers aren't found by G_Find but are instead linked directly to the 'self' entity.
		switch (self->e_UseFunc)
		{
		case(useF_Use_Multi):
		case(useF_func_usable_use): // Useless but there are some entities hitting this check, leave it present ?
			if (!cg_drawBoxTriggersFilter.integer || (cg_drawBoxTriggersFilter.integer & trig_filter_func))
			{
				if (self->e_TouchFunc == touchF_NULL && cg_drawBoxTriggersFilterDisabled.integer)
				{
					color[0] = red[0];
					color[1] = red[1];
					color[2] = red[2];
				}
				else if (self->e_TouchFunc != touchF_NULL)
				{
					color[0] = 0;
					color[1] = 100;
					color[2] = 50;
				}
			}
			break;
		case(useF_funcBBrushUse): // Push to destroy wall brush (yavin2)
			if (!cg_drawBoxTriggersFilter.integer || (cg_drawBoxTriggersFilter.integer & trig_filter_usable))
			{
				if (self->e_TouchFunc == touchF_NULL && cg_drawBoxTriggersFilterDisabled.integer)
				{
					color[0] = red[0];
					color[1] = red[1];
					color[2] = red[2];
				}
				else if (self->e_TouchFunc != touchF_NULL)
				{
					color[0] = 0;
					color[1] = 100;
					color[2] = 0;
				}
			}
			break;
		case(useF_Use_BinaryMover): // Push/Pull walls (yavin2)
			if (self->spawnflags & 1 || self->spawnflags & 2) // Same check as in cg_draw CG_ScanForCrosshairEntity
			{
				if (!cg_drawBoxTriggersFilter.integer || (cg_drawBoxTriggersFilter.integer & trig_filter_uncategorized))
				{
					if (self->e_TouchFunc != touchF_NULL)
					{
						color[0] = 20;
						color[1] = 20;
						color[2] = 20;
					}
					else if (self->e_TouchFunc == touchF_NULL && cg_drawBoxTriggersFilterDisabled.integer)
					{
						color[0] = red[0];
						color[1] = red[1];
						color[2] = red[2];
					}
				}
			}
		default:
			if (self->e_TouchFunc == touchF_NULL && cg_drawBoxTriggersFilterDisabled.integer)
			{
				color[0] = red[0];
				color[1] = red[1];
				color[2] = red[2];
			}
			break;
		}
	}

	// Specific case for Academy "models/map_objects/imperial/switch.md3" on some maps. They don't have e_TouchFunc defined .
	//model "models/map_objects/imperial/switch.md3"
	//classname "misc_model_breakable"
	//useF_misc_model_use
	if (self->e_UseFunc == useF_misc_model_use && self->e_TouchFunc == touchF_NULL)
	{
		if (!cg_drawBoxTriggersFilter.integer || (cg_drawBoxTriggersFilter.integer & trig_filter_usable))
		{
			// Same as buttons
			color[0] = 0;
			color[1] = 100;
			color[2] = 0;
		}
	}

	// trigger_doors don't have a e_UseFunc defined, so we check them here with their e_TouchFunc
	// These are always valid and don't need the disabled filter
	if (self->e_TouchFunc == touchF_Touch_DoorTrigger)
	{
		if (!cg_drawBoxTriggersFilter.integer || (cg_drawBoxTriggersFilter.integer & trig_filter_doors))
		{
			// Yellow for doors
			color[0] = 100;
			color[1] = 100;
			color[2] = 0;
		}
	}

	// trigger_hurt don't have a e_UseFunc defined, so we check them here with their e_TouchFunc
	// These are always valid and don't need the disabled filter
	if (self->e_TouchFunc == touchF_hurt_touch)
	{
		if (!cg_drawBoxTriggersFilter.integer || (cg_drawBoxTriggersFilter.integer & trig_filter_hurt))
		{
			// Red for death triggers
			color[0] = 200;
			color[1] = 0;
			color[2] = 0;
			color[3] = 255;
		}
	}

}

static void drawPlayerRelated(gentity_t* self)
{
	// Step 2 : Check what is enabled as to not show everything
	if (cg_drawBoxPlayer.integer && cg.renderingThirdPerson) // Don't want a red filter in first person
	{
		// Make it red and semi-transparent
		byte color[4];
		color[0] = 50;
		color[1] = 0;
		color[2] = 0;
		color[3] = 25;

		drawBoundingBox(self, color);
	}
	if (cg_drawVelocityVector.integer)
	{
		// Draw velocity vector, its 3d length will be it's vec3 values / 10 ; so a speed of 300 will display a line of 30 units in game
		drawVelocityVector(self, 4);
	}
}

static void drawNPCRelated(gentity_t* self)
{
	// Step 2 : Check what is enabled as to not show everything
	if (cg_drawBoxNPC.integer)
	{
		// Make it green and semi-transparent
		byte color[4];
		color[0] = 0;
		color[1] = 50;
		color[2] = 0;
		color[3] = 25;

		drawBoundingBox(self, color);
	}
	if (cg_drawLineOfSight.integer)
	{
		// Width of 4, can be adjusted later of needs be
		drawLineOfSight(self, 4);
	}
	if (cg_drawNPCPath.integer)
	{
		drawNPCPathing(self);
	}
}

static void drawItemRelated(gentity_t* self)
{
	if (cg_drawBoxItems.integer)
	{
		// Make it blue and semi-transparent
		byte color[4];
		color[0] = 0;
		color[1] = 0;
		color[2] = 50;
		color[3] = 25;

		drawBoundingBox(self, color);
	}
}

static void drawBoxWorldTriggers(gentity_t* self)
{
	// Step 2 : Check what is enabled as to not show everything
	if (cg_drawBoxTriggers.integer)
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

		// Default color = no overload by setColorForTrigger because of filtering : return early
		if (color[0] == 0 && color[1] == 0 && color[2] == 100)
		{
			return;
		}

		drawBoundingBox(self, color);
	}
}

static void drawBoxObjectTriggers(gentity_t* self)
{
	// Step 2 : Check what is enabled as to not show everything
	if (cg_drawBoxTriggers.integer)
	{
		// Default color: red / same as disabled
		byte color[4];
		color[0] = 50;
		color[1] = 0;
		color[2] = 0;
		color[3] = 25;

		// Change in other color (ex : secrets). The first secret in t1_fatal is obtained by destroying a 3D object, it will be colored in pink.
		// Do not use blue nor red to know we correctly override the previous color.
		setColorForTrigger(self, color);

		// Default color = no trigger found for this object : don't render the box
		if (color[0] == 50 && color[1] == 0 && color[2] == 0)
		{
			return;
		}

		drawBoundingBox(self, color);
	}
}

void CG_DrawSpeedrunExtras()
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


	// Player related stuff can be done indepedent from the loop so no needs for an early return
	drawPlayerRelated(&g_entities[0]);

	// Step 0 : big check of every variable, don't if none of them are enabled
	if (!cg_drawBoxNPC.integer && !cg_drawBoxItems.integer && !cg_drawBoxTriggers.integer
		&& !cg_drawLineOfSight.integer && !cg_drawNPCPath.integer)
	{
		return;
	}

	// Step 1 : loop all g_entities and give the pointer to the corresponding function.
	for (int i = 0; i < MAX_GENTITIES; ++i)
	{
		if (g_entities[i].e_ThinkFunc == thinkF_NPC_Think)
		{
			drawNPCRelated(&g_entities[i]);
		}
		if (g_entities[i].e_TouchFunc == touchF_Touch_Item)
		{
			drawItemRelated(&g_entities[i]);
		}
		// Different logic for objects and world trigger, so keep both functions separated.
		if (g_entities[i].classname &&
			(strncmp(g_entities[i].classname, "trigger_", strlen("trigger_")) == 0))
		{
			drawBoxWorldTriggers(&g_entities[i]);
		}
		if (g_entities[i].classname &&
			(strncmp(g_entities[i].classname, "func_", strlen("func_")) == 0 ||
				(strncmp(g_entities[i].classname, "misc_", strlen("misc_")) == 0)))
		{
			drawBoxObjectTriggers(&g_entities[i]);
		}
	}
}
