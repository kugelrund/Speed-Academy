# Movement Key Effect on Air Acceleration

In vanilla Quake 3 you can accelerate basically just as well with a single input key (i.e. only A or only D) than with the usual combination of two keys (W+A or W+D).
But in the Jedi Academy singleplayer, strafing with only a single key is significantly slower while in the air.
On the other hand, acceleration in Quake 3 is significantly slower when holding the jump key while in the air, whereas it has no influence whatsoever in Jedi Academy.

Jedi Academy inherits this behavior directly from its engine antecessor *Star Trek: Voyager - Elite Force*.
So the technical explanation in its Elite-Reinforce source port ([kugelrund/Elite-Reinforce/docs/movekey_air_accel_effect.md](https://github.com/kugelrund/Elite-Reinforce/blob/master/docs/movement_physics/movekey_air_accel_effect.md)) applies to Jedi Academy too, with the relevant code snippets looking very much the same.

Just like in Elite Force, the line `wishspeed *= scale` is missing in `PM_AirMove` ([jediacademy/code/game/bg_pmove.cpp#L2942](https://github.com/kugelrund/Speed-Academy/blob/4bebb8ec23200ee150a9aa566cea6122c19eba44/code/game/bg_pmove.cpp#L2942)), with the variable `scale` still being set ([jediacademy/code/game/bg_pmove.cpp#L2870](https://github.com/kugelrund/Speed-Academy/blob/4bebb8ec23200ee150a9aa566cea6122c19eba44/code/game/bg_pmove.cpp#L2870)) but remaining unused.
Other functions like `PM_WalkMove` do apply the scaling ([jediacademy/code/game/bg_pmove.cpp#L3202](https://github.com/kugelrund/Speed-Academy/blob/4bebb8ec23200ee150a9aa566cea6122c19eba44/code/game/bg_pmove.cpp#L3202)).
The added deceleration scaling is also identical ([jediacademy/code/game/bg_pmove.cpp#L2945](https://github.com/kugelrund/Speed-Academy/blob/4bebb8ec23200ee150a9aa566cea6122c19eba44/code/game/bg_pmove.cpp#L2945)).
