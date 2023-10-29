"""
This script is used to fix properties of light actors in a game level.

Usage:
- Call the `fix_light_sources()` function to fix light actors at the current level.

Functions:
- `fix_light_actor()`: Adjusts the properties of a light actor.
- `fix_light_sources()`: Fixes light actors at the current level.

Note:
- The script assumes the presence of the `get_component_by_class()` function.
- Uncomment the lines in the `fix_light_sources()` function corresponding 
  to specific light types to fix their properties as well.
"""

from typing import Optional, Type

import unreal
from Common import get_component_by_class


def fix_light_actor(
        actor: unreal.Actor,
        cls: Type[unreal.LocalLightComponent],
        intensity_scale: Optional[float] = None,
        radius_scale: Optional[float] = None,
        cast_shadows: Optional[bool] = None,
        mobility: Optional[unreal.ComponentMobility] = None
):
    component = get_component_by_class(actor, cls)

    print(f"Setup lights for: {actor.get_name()}")

    prev_mobility = component.mobility
    # Only movable lights can change their radius at runtime
    component.set_mobility(unreal.ComponentMobility.MOVABLE)

    if intensity_scale is not None:
        value = component.intensity * intensity_scale
        print(f" -> Change intensity: {value}")
        component.set_intensity(value)

    if radius_scale is not None:
        value = component.attenuation_radius * radius_scale
        print(f" -> Change attenuation radius: {value}")
        component.set_attenuation_radius(value)

    if cast_shadows is not None:
        print(f" -> Change cast shadows: {cast_shadows}")
        component.set_cast_shadows(cast_shadows)

    if mobility is not None:
        print(f" -> Change mobility: {mobility}")
        component.set_mobility(mobility)
    else:
        component.set_mobility(prev_mobility)


def fix_light_sources():
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        name: str = actor.get_name()
        if name.startswith("light_"):
            fix_light_actor(actor, unreal.PointLightComponent, 0.5, 0.5)

        # if name.startswith("light_spot_"):
        #     fix_light_actor(actor, unreal.SpotLightComponent)


if __name__ == '__main__':
    fix_light_sources()
