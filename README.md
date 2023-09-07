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

#### `cg_drawJumpHelper` (0 or 1)

Draw a helper for correctly timing jump release and crouch jumping.

#### `cg_jumpHelperX` (#)

Horizontal position of the jump helper.
Default: `0.0` (center of the screen).

#### `cg_jumpHelperY` (#)

Vertical position of the jump helper.
Default: `0.0` (center of the screen).

#### `cg_jumpHelperWidth` (#)

Width of the jump helper.
Note that this becomes height when using the vertical jump helper (`cg_jumpHelperHorizontal 0`).
Default: `300`.

#### `cg_jumpHelperHeight` (#)

Height of the jump helper.
Note that this becomes width when using the vertical jump helper (`cg_jumpHelperHorizontal 0`).
Default: `20`.

#### `cg_jumpHelperHorizontal` (-1, 0 or 1)

Whether to draw the jump helper horizontally left-to-right (`1`), horizontally right-to-left (`-1`) or vertically bottom-to-top (`0`).
Default: `1`.

#### `cg_jumpHelperMirror` (0 or 1)

Whether to mirror the jump helper.
Mirrors horizontally when drawing horizontally, vertically otherwise.
Default: `1`.

#### `cg_jumpHelperAutoScale` (0 or 1)

Whether to rescale the jump helper to the same size for each force level.
Default: `0`.

#### `cg_jumpHelperCrouch` (0 or 1)

Show an additional height section for crouch jumping, appended to normal jump height.
Default: `1`.

#### `cg_jumpHelperCurrentScale` (#)

Scale of the indicator for the current height within the jump helper.
Negative values draw from top to bottom, positive values from bottom to top.
Default: `0.5`.

#### `cg_drawLandingInfo` (0 or 1)

Draw info about what kind of boost happened on landing.

#### `cg_landingInfoDuration` (#)

Duration for showing landing info in milliseconds.
Default: `500` (half a second).

#### `cg_landingInfoScale` (#)

Scale of the text for landing info display.
Default: `0.9`.

#### `cg_landingInfoX` (#)

Horizontal offset for drawing landing info text, relative to the center of the screen.
Default: `0.0` (center of the screen).

#### `cg_landingInfoY` (#)

Vertical offset for drawing landing info text, relative to the center of the screen.
Default: `90.0` (Slightly below center of the screen).

#### `cg_landingInfoText[...]` (text)

Set the text to display for each type of landing.
Types of landings are `CB`, `EB`, `RB`, `SG`, `VB`, `VRGI`.
Defaults are those abbreviations.

#### `cg_drawSpeed` (0 or 1)

Draw the current player speed.
Default: `0`.

#### `cg_speedScale` (#)

Scale of the text for speed display.
Default: `0.9`.

#### `cg_speedX` (#)

Horizontal offset for drawing speed text, relative to the center of the screen.
Default: `0.0` (center of the screen).

#### `cg_speedY` (#)

Vertical offset for drawing speed text, relative to the center of the screen.
Default: `75.0` (Slightly below center of the screen).

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

Scale of the text for speed display tied to strafe helper.
Note that this shows the speed after friction but prior to acceleration, as this is what is relevant for optimal strafing.
It is not the intuitive current player speed.
For that, see `cg_drawSpeed`.
Default: `0.9`.

#### `cg_strafeHelperSpeedX` (#)

Horizontal offset of the text for speed display, relative to the center of the screen.
Default: `0.0` (center of the screen).

#### `cg_strafeHelperSpeedY` (#)

Vertical offset of the text for speed display, relative to the strafe helper.
Default: `0.0` (directly under the strafe helper).

#### `cg_strafeHelperY` (#)

Vertical position of the strafe helper.
Default: `50.0`.

#### `cg_strafeHelperColor[...]` (0.0 - 1.0)

Color components (red, green, blue, alpha) for different strafe helper elements.
These are `Accelerating`, `Optimal`, `CenterMarker` and `Speed`.
Colors can be set more conveniently with the corresponding commands.

#### `r_showElevationBoosts` (0 or 1)

Enable or disable coloring heights that give an elevation boost if jumping from
the current height.

#### `r_showElevationBoostsColor[R,G or B]` (0 - 255)

Color components (red, green, blue) used to mark heights that will give an elevation boost.
Color can be set more conveniently with the command `showElevationBoostsColor`.
Applies after reloading the map.

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

#### `g_reverseBoosts` (0 or 1)

Flip elevation logic for VRGI.

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

## New Commands

### Menus

#### `forceselect [non-core level] [core level]`

Opens a menu to choose force power levels for any power without limitations.
The optional first argument allows setting all non-core force powers to a common level before showing the menu.
The optional second argument allows the same for core force powers.
Levels must be between `0` and `3`.

#### `missionselect [t1,t2,t3,hoth2,hoth3,vjun1,vjun2,vjun3,taspir1,taspir2,kor1,kor2]`

Opens mission selection menu to start a mission.
If no argument is given, shows an initial menu that allows choosing a mission category.
If argument is a tier, shows mission selection for that tier.
If argument is a story mission, shows the weapon selection for that story mission.

#### `saberselect`

Opens saber creation menu to choose any saber, including styles.

### HUD

#### `jumpHelperColor[...] <r> <g> <b> <a>` (components in range 0.0 to 1.0)

Sets the color for one of the jump helper elements `Background`, `Crouch`, `CrouchExtend`, `Extend`, `Optimal`, `Current` or `Cancel` to the given red, green, blue and alpha values.

#### `landingInfoColor[...] <r> <g> <b>` (components in range 0.0 to 1.0)

Sets the color for the landing info text of either landing type to the given red, green, blue and alpha values.
Types of landings are `CB`, `EB`, `RB`, `SG`, `VB`, `VRGI`.

#### `speedColor <r> <g> <b> <a>` (components in range 0.0 to 1.0)

Sets the color for the speed display to the given red, green, blue and alpha values.

#### `strafeHelperColor[...] <r> <g> <b> <a>` (components in range 0.0 to 1.0)

Sets the color for one of the strafe helper elements `Accelerating`, `Optimal`, `CenterMarker` or `Speed` to the given red, green, blue and alpha values.

#### `showElevationBoostsColor <r> <g> <b>` (components in range 0 to 255)

Sets the color for showing elevation boosts.
Applies after reloading the map.
