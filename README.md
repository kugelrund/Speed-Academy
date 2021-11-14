# Speed Academy

Source port of the singleplayer of Star Wars Jedi Knight: Jedi Academy.
Focuses on staying true to the original game to be used for speedrun purposes.
See the [speedrun.com leaderboard](https://www.speedrun.com/jka) for more information.

Based on the original source code released by Raven, see README.txt.

## New Console Variables

### Input

#### `in_mouse_raw` (0 or 1)

Use raw mouse input. Only applies after an `in_restart` (or after restarting the game).
Default: `1`.

### HUD

#### `cg_drawMovementRestriction` (0 or 1)

Draw the currently active movement restriction.
`--` represents no restrictions.
Default: `1`.

#### `cg_drawOverbounceInfo` (0 or 1)

Draw overbounce probabilities, G is for Go, J is for Jump.
Default: `0`.

#### `cg_drawSecrets` (0 or 1)

Show a tracker for found versus total number of secrets on the current level.
Default `0`.

#### `cg_drawSpeedrunTotalTimer` (0-3)

Draw a total run timer.
Higher values show more decimals.
Note that this is currently not the timing that is used on the leaderboards.
Default: `0`.

#### `cg_drawSpeedrunLevelTimer` (0-3)

Draw a current level timer.
Higher values show more decimals.
Note that this is currently not the timing that is used on the leaderboards.
Default: `0`.

#### `cg_drawStrafeHelper` (0 or 1)

Draw a strafe helper.
Default: `0`.

#### `cg_strafeHelperCenter` (0 or 1)

Put the strafe helper in the center of the screen.
Default: `1`.

#### `cg_strafeHelperCenterMarker` (0 or 1)

Draw a marker in the center of the strafe helper.
Default: `1`.

#### `cg_strafeHelperHeight` (#)

Height of strafe helper bar.
Default: `20.0`.

#### `cg_strafeHelperScale` (#)

Horizontal scale of the strafe helper.
Angles correspond directly to the field of view if this scale is set to `360/cg_fov`.
Default: `4.0`.

#### `cg_strafeHelperSpeedScale` (#)

Scale of the text for speed display.
Default: `0.9`.

#### `cg_strafeHelperSpeedY` (#)

Vertical offset of the text for speed display, relative to the strafe helper.
Default: `0.0` (directly under the strafe helper).

#### `cg_strafeHelperY` (#)

Vertical position of the strafe helper.
Default: `50.0`.

#### `r_showElevationBoosts` (0 or 1)

Enable or disable coloring heights that give an elevation boost if jumping from
the current height.

### Gameplay

#### `g_vrgi` (0 or 1)

Enable or disable the "velocity reduction on ground impact" (VRGI).
This is the usual slowdown when landing on ground that Jedi Academy introduced.
The "No VRGI" category runs with this being set to `0`.
Default: `1`.

#### `g_spinGlitch` (0 or 1)

Allows to disable "spin glitch".
When set to `0`, VRGI can not be bypassed by spinning anymore.
Default: `1`.

#### `g_crouchBoosts` (0 or 1)

Allows to disable "crouch boosting".
When set to `0`, VRGI can not be bypassed by crouching anymore.
Default: `1`.

#### `g_randomBoosts` (0 or 1)

Allows to disable "random" boosts.
**Cheat protected**.
Default: `1`.

#### `g_selfKnockback` (0, or 1)

Allow players to damage boost themselves with explosive weapons.
**Cheat protected**.
Default: `0`.

#### `sv_speedrunModeIL` (0 or 1)

Individual-level run mode.
When set to `1`, ingame timer will reset every time an autosave is loaded.
When set to `0`, ingame timer will reset only when yavin1b is loaded.
Note that this is currently not the timing that is used on the leaderboards.
Default: `0`.

#### `sv_speedrunModeCheckpoint` (0 or 1)

Mode for running from quicksave to a specified checkpoint on a map.
Coordinates can be set with the command `settimedcheckpoint x1 y1 z1 x2 y2 z2`.
When the player enters the box spanned by the two points `(x1 y1 z1)` and `(x2 y2 z2)` the run is finished.
If `x2`, `y2` and `z2` are omitted, a checkpoint of reasonable size will be created at `(x1 y1 z1)`.
Default: `0`.

#### `sv_speedrunModeCheckpointSave` (0 or 1)

Create an automatic save when a checkpoint is reached in checkpoint mode.
Default: `0`.
