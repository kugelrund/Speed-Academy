# Velocity Reduction on Ground Impact

The Jedi Academy singleplayer has a curious change to the movement code that Jedi Outcast did not have;
Raven added some pieces of code that causes a player to lose half their speed whenever they touch the ground.
This effect is called *velocity reduction on ground impact (VRGI)* in the speedrun community.

## Code Change

The change is located in the file `bg_pmove.cpp` in [function `PM_GetLandingAnim`](https://github.com/kugelrund/Speed-Academy/blob/4bebb8ec23200ee150a9aa566cea6122c19eba44/code/game/bg_pmove.cpp#L3569-L3733) which determines which animation the player model should do now upon landing, depending on what the current animation is.
The relevant change that causes VRGI is the addition of the lines

```cpp
//stick landings some
pm->ps->velocity[0] *= 0.5f;
pm->ps->velocity[1] *= 0.5f;
```

in most code paths of this function, meaning for most current animations.
However, there are a few types of animations that did not get this halving of speed.
Among these are spinning animations that are triggered by the player holding the attack button with the lightsaber equipped while moving in the air.
By performing these spins with the correct timing, VRGI can be avoided due to the spinning animation being active while landing.
This is known as **Spin Glitch** in the community.

Other animations that don't suffer from VRGI are special moves like cartwheels that are not really relevant for use in speedruns, as they force a fixed slow speed of the player in the first place.
Comparing [the function in Jedi Outcast](https://github.com/jedis/jedioutcast/blob/1963e9b17e0b3b897bb44a0bc4901a8cad007ff4/code/game/bg_pmove.cpp#L2272-L2347) to [the function in Jedi Academy](https://github.com/kugelrund/Speed-Academy/blob/4bebb8ec23200ee150a9aa566cea6122c19eba44/code/game/bg_pmove.cpp#L3569-L3733) gives the full picture of where the speed halving was applied:

<details>
<summary>
Diff of <code>PM_GetLandingAnim</code> between Jedi Outcast and Jedi Academy
</summary>

```diff
int PM_GetLandingAnim( void )
{
    int anim = pm->ps->legsAnim;
+
+   //special cases:
+   if ( anim == BOTH_FLIP_ATTACK7
+       || anim == BOTH_FLIP_HOLD7 )
+   {
+       return BOTH_FLIP_LAND;
+   }
+   else if ( anim == BOTH_FLIP_LAND )
+   {
+       //stick landings some
+       pm->ps->velocity[0] *= 0.5f;
+       pm->ps->velocity[1] *= 0.5f;
+       return BOTH_LAND1;
+   }
+   else if ( PM_InAirKickingAnim( anim ) )
+   {
+       switch ( anim )
+       {
+       case BOTH_A7_KICK_F_AIR:
+           return BOTH_FORCELAND1;
+           break;
+       case BOTH_A7_KICK_B_AIR:
+           return BOTH_FORCELANDBACK1;
+           break;
+       case BOTH_A7_KICK_R_AIR:
+           return BOTH_FORCELANDRIGHT1;
+           break;
+       case BOTH_A7_KICK_L_AIR:
+           return BOTH_FORCELANDLEFT1;
+           break;
+       }
+   }
+
    if ( PM_SpinningAnim( anim ) || PM_SaberInSpecialAttack( anim ) )
    {
        return -1;
    }
    switch ( anim )
    {
    case BOTH_FORCEJUMPLEFT1:
    case BOTH_FORCEINAIRLEFT1:
        anim = BOTH_FORCELANDLEFT1;
+       //stick landings some
+       pm->ps->velocity[0] *= 0.5f;
+       pm->ps->velocity[1] *= 0.5f;
        break;
    case BOTH_FORCEJUMPRIGHT1:
    case BOTH_FORCEINAIRRIGHT1:
        anim = BOTH_FORCELANDRIGHT1;
+       //stick landings some
+       pm->ps->velocity[0] *= 0.5f;
+       pm->ps->velocity[1] *= 0.5f;
        break;
    case BOTH_FORCEJUMP1:
    case BOTH_FORCEINAIR1:
+       //stick landings some
+       pm->ps->velocity[0] *= 0.5f;
+       pm->ps->velocity[1] *= 0.5f;
        anim = BOTH_FORCELAND1;
        break;
    case BOTH_FORCEJUMPBACK1:
    case BOTH_FORCEINAIRBACK1:
+       //stick landings some
+       pm->ps->velocity[0] *= 0.5f;
+       pm->ps->velocity[1] *= 0.5f;
        anim = BOTH_FORCELANDBACK1;
        break;
    case BOTH_JUMPLEFT1:
    case BOTH_INAIRLEFT1:
        anim = BOTH_LANDLEFT1;
+       //stick landings some
+       pm->ps->velocity[0] *= 0.5f;
+       pm->ps->velocity[1] *= 0.5f;
        break;
    case BOTH_JUMPRIGHT1:
    case BOTH_INAIRRIGHT1:
        anim = BOTH_LANDRIGHT1;
+       //stick landings some
+       pm->ps->velocity[0] *= 0.5f;
+       pm->ps->velocity[1] *= 0.5f;
        break;
    case BOTH_JUMP1:
    case BOTH_INAIR1:
        anim = BOTH_LAND1;
+       //stick landings some
+       pm->ps->velocity[0] *= 0.5f;
+       pm->ps->velocity[1] *= 0.5f;
        break;
    case BOTH_JUMPBACK1:
    case BOTH_INAIRBACK1:
        anim = BOTH_LANDBACK1;
+       //stick landings some
+       pm->ps->velocity[0] *= 0.5f;
+       pm->ps->velocity[1] *= 0.5f;
        break;
    case BOTH_BUTTERFLY_LEFT:
    case BOTH_BUTTERFLY_RIGHT:
+   case BOTH_BUTTERFLY_FL1:
+   case BOTH_BUTTERFLY_FR1:
    case BOTH_FJSS_TR_BL:
    case BOTH_FJSS_TL_BR:
    case BOTH_LUNGE2_B__T_:
    case BOTH_FORCELEAP2_T__B_:
    case BOTH_ARIAL_LEFT:
    case BOTH_ARIAL_RIGHT:
    case BOTH_ARIAL_F1:
    case BOTH_CARTWHEEL_LEFT:
    case BOTH_CARTWHEEL_RIGHT:
    case BOTH_JUMPFLIPSLASHDOWN1://#
    case BOTH_JUMPFLIPSTABDOWN://#
+   case BOTH_JUMPATTACK6:
+   case BOTH_JUMPATTACK7:
+   case BOTH_A7_KICK_F:
+   case BOTH_A7_KICK_B:
+   case BOTH_A7_KICK_R:
+   case BOTH_A7_KICK_L:
+   case BOTH_A7_KICK_S:
+   case BOTH_A7_KICK_BF:
+   case BOTH_A7_KICK_RL:
+   case BOTH_A7_KICK_F_AIR:
+   case BOTH_A7_KICK_B_AIR:
+   case BOTH_A7_KICK_R_AIR:
+   case BOTH_A7_KICK_L_AIR:
+   case BOTH_STABDOWN:
+   case BOTH_STABDOWN_STAFF:
+   case BOTH_STABDOWN_DUAL:
+   case BOTH_A6_SABERPROTECT:
+   case BOTH_A7_SOULCAL:
+   case BOTH_A1_SPECIAL:
+   case BOTH_A2_SPECIAL:
+   case BOTH_A3_SPECIAL:
+   case BOTH_PULL_IMPALE_STAB:
+   case BOTH_PULL_IMPALE_SWING:
        anim = -1;
        break;
+   case BOTH_FORCELONGLEAP_START:
+   case BOTH_FORCELONGLEAP_ATTACK:
+       return BOTH_FORCELONGLEAP_LAND;
+       break;
    case BOTH_WALL_RUN_LEFT://#
    case BOTH_WALL_RUN_RIGHT://#
        if ( pm->ps->legsAnimTimer > 500 )
        {//only land at end of anim
            return -1;
        }
        //NOTE: falls through on purpose!
    default:
        if ( pm->ps->pm_flags & PMF_BACKWARDS_JUMP )
        {
            anim = BOTH_LANDBACK1;
        }
        else
        {
            anim = BOTH_LAND1;
        }
+       //stick landings some
+       pm->ps->velocity[0] *= 0.5f;
+       pm->ps->velocity[1] *= 0.5f;
        break;
    }
    return anim;
}
```

</details>

## Influence of the Change

The [calls to `PM_GetLandingAnim`](https://github.com/kugelrund/Speed-Academy/blob/4bebb8ec23200ee150a9aa566cea6122c19eba44/code/game/bg_pmove.cpp#L4054-L4141) are all located in `PM_CrashLand` which itself is called everytime the player hits the ground.
The [relevant logic](https://github.com/kugelrund/Speed-Academy/blob/4bebb8ec23200ee150a9aa566cea6122c19eba44/code/game/bg_pmove.cpp#L4086-L4100) for when `PM_GetLandingAnim` is called from `PM_CrashLand` boils down to something like this:

```cpp
static void PM_CrashLand( void )
{
    float delta = /* ... */;        // these are set according to the height difference
    bool forceLanding = /* ... */;  // between jump-off and landing position

    // ...

    if ( pm->cmd.upmove >= 0 )
    {    
        if ( delta > 10
            || pm->ps->pm_flags & PMF_BACKWARDS_JUMP
            || forceLanding )
        {
            int anim = PM_GetLandingAnim();
            // ...
        }
    }

    // ...
}
```

So with these conditions, there are a few ways to avoid `PM_GetLandingAnim` being called and thereby avoiding VRGI:

* By crouching while landing, the player can make `pm->cmd.upmove < 0`. This is known as **Crouch Boost**.
* When landing with the right height difference between jump-off and landing position, the player can make `delta <= 10` and `forceLanding == false`. This is known as **Elevation Boost**. Note that for this case the backwards key is not allowed to have been pressed when the jump was initiated either.
* TODO: **Velocity Boost**

Otherwise, `PM_GetLandingAnim` is called and the only way to still avoid VRGI is to land while being in one of the special non-VRGI animations like with the **Spin Glitch**, as described before.

## Intention of the Change

Of course we can't really know for sure what the intention behind VRGI was, unless someone from the Raven developer team remembers (please hit us up James Monroe!).
But we can speculate.

An immediate theory that could come to mind is that Raven actually wanted to make strafe jumping less effective by slowing down the player on every landing.
But looking at the implementation, this seems quite unlikely.
If that was the goal, the halving of the speed could've simply been added a single time somewhere closer to the top, separated from all the extra conditions, like in `PM_CrashLand` directly.

Instead, as described, the halving was added in `PM_GetLandingAnim`, right next to decisions about player model animations.
So it seems much more likely that the intention was to make the animations look better/smoother/more realistic.
Perhaps someone felt that the speed that the player retained when landing on the ground did not match well with their landing animation, so they decided to "stick landings some" as the comment mentions, to make landings look less floaty and more sticky.
Jedi Academy removed the first person lightsaber mode that Jedi Outcast had, so there was a lot more emphasis on the thirdperson perspective for which the player model animations become relevant, which also speaks for this theory.
TODO: Make comparison videos of landing animations with/without VRGI.

Whatever the intention was, the outcome is a very unique and rich set of movement techniques that is much appreciated by the speedrun community.
So thank you very much Raven!
