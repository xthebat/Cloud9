from typing import Optional

import unreal


def fix_light_sources(
        intensity_scale: float = 0.5,
        radius_scale: float = 0.5,
):
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        name: str = actor.get_name()
        if not name.startswith("light_spot_") or not name.startswith("light_"):
            continue

        print(f"Setup lights for: {actor.get_name()}")

        light_component: Optional[unreal.PointLightComponent] = actor.get_component_by_class(unreal.PointLightComponent)
        if light_component is None:
            continue

        # Only movable lights can change their radius at runtime
        light_component.set_mobility(unreal.ComponentMobility.MOVABLE)
        light_component.set_intensity(light_component.intensity * intensity_scale)
        light_component.set_attenuation_radius(light_component.attenuation_radius * radius_scale)
        # STATIONARY not working for many actors :(
        # light_component.set_mobility(unreal.ComponentMobility.STATIONARY)
        light_component.set_mobility(unreal.ComponentMobility.STATIC)
        light_component.set_cast_shadows(True)


if __name__ == '__main__':
    fix_light_sources()
