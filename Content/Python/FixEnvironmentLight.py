from typing import cast

import unreal


def fix_environment_light():
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        name: str = actor.get_name()
        if not name.startswith("light_environment"):
            continue

        print(f"Setup destroy bad environment light: {actor}")
        path = actor.get_path_name()
        actor.destroy_actor()
        unreal.EditorAssetLibrary.delete_asset(path)

    directional_light = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.DirectionalLight, unreal.Vector(0, 0, 0))

    directional_light = cast(unreal.DirectionalLight, directional_light)

    directional_light.set_actor_label("Light Source")
    directional_light.set_actor_rotation(unreal.Rotator(0, -55.0, -45.0), True)
    directional_light.light_component.set_intensity(2.75)
    directional_light.light_component.set_atmosphere_sun_light(True)

    unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.PostProcessVolume, unreal.Vector(0, 100, 0))

    unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.SkyLight, unreal.Vector(0, 200, 0))

    unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.AtmosphericFog, unreal.Vector(0, 300, 0))

    unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.SphereReflectionCapture, unreal.Vector(0, 400, 0))

    # TODO: Add skysphere blueprint


if __name__ == '__main__':
    fix_environment_light()
