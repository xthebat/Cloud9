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

    if intensity_scale is not None:
        value = component.intensity * intensity_scale
        print(f" -> Change intensity: {value}")
        component.set_intensity(value)

    if radius_scale is not None:
        value = component.attenuation_radius * radius_scale
        print(f" -> Change attenuation radius: {value}")
        prev_mobility = component.mobility
        # Only movable lights can change their radius at runtime
        component.set_mobility(unreal.ComponentMobility.MOVABLE)
        component.set_attenuation_radius(value)
        component.set_mobility(prev_mobility)

    if mobility is not None:
        print(f" -> Change mobility: {mobility}")
        component.set_mobility(mobility)

    if cast_shadows is not None:
        print(f" -> Change cast shadows: {cast_shadows}")
        component.set_cast_shadows(cast_shadows)


def fix_light_sources():
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        name: str = actor.get_name()
        if name.startswith("light_"):
            fix_light_actor(actor, unreal.PointLightComponent, 2)

        # if name.startswith("light_spot_"):
        #     fix_light_actor(actor, unreal.SpotLightComponent)


if __name__ == '__main__':
    fix_light_sources()
