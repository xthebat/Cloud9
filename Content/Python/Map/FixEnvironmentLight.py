﻿"""
Fixes the environment lighting in the scene by spawning and setting up the required light actors.

This script searches for any existing environment light actors in the level and removes them.
It then spawns and configures new light actors such as SkySphere, Directional Light, Post Process Volume,
Skylight, Atmospheric Fog, and Sphere Reflection Capture to set up the environment lighting.

Parameters:
- location: A tuple of three floats representing the initial location for spawning the light actors.
- shift: A tuple of three floats representing the shift to apply to the location for each spawned light actor.
- sun_brightness: The brightness of the sun in the SkySphere.
- sun_height: The height of the sun in the SkySphere.
- cloud_speed: The speed of the clouds in the SkySphere.
- cloud_opacity: The opacity of the clouds in the SkySphere.
- color_saturation: The saturation for PostProcessVolume
- direction_light_intensity: The intensity of the Directional Light.
- direction_light_rotation: A tuple of three floats representing the rotation of the Directional Light.
- direction_light_color: A tuple of four floats representing the color of the Directional Light in RGBA format.
- sky_light_scale: The intensity scale of the Skylight.
- folder: The folder path where the spawned actors will be placed in the editor.

Example:
    To fix the environment lighting, call the `fix_environment_light` function without any arguments.

        fix_environment_light()

"""

from typing import Tuple

import unreal
from Common import spawn_actor_from_class


def fix_environment_light(
        location: Tuple[float, float, float] = (0.0, 0.0, 0.0),
        shift: Tuple[float, float, float] = (-200.0, 0.0, 0.0),

        sun_brightness: float = 75.0,
        sun_height: float = 0.736771,
        cloud_speed: float = 2.0,
        cloud_opacity: float = 1.0,

        color_saturation: float = 1.5,

        direction_light_intensity: float = 2.0,
        direction_light_rotation: Tuple[float, float, float] = (0, -55.0, -45.0),
        direction_light_color: Tuple[float, float, float, float] = (1.0, 1.0, 0.760525, 1.0),

        sky_light_scale: float = 3.5,

        folder: str = "Lighting"
):
    location = unreal.Vector(*location)
    shift = unreal.Vector(*shift)
    folder = unreal.Name(folder)
    direction_light_rotation = unreal.Rotator(*direction_light_rotation)
    color_saturation = unreal.Vector4(1.0, 1.0, 1.0, color_saturation)

    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        name: str = actor.get_name()
        if not name.startswith("light_environment"):
            continue

        print(f"Setup destroy bad environment light: {actor}")
        path = actor.get_path_name()
        actor.destroy_actor()
        unreal.EditorAssetLibrary.delete_asset(path)

    bp_sky_sphere = unreal.EditorAssetLibrary.load_blueprint_class("/Engine/EngineSky/BP_Sky_Sphere")
    sky_sphere = spawn_actor_from_class(bp_sky_sphere, location)
    sky_sphere.set_folder_path(folder)
    sky_sphere.set_actor_label("SkySphere")
    sky_sphere.set_editor_property("Sun Brightness", sun_brightness)
    sky_sphere.set_editor_property("Cloud Speed", cloud_speed)
    sky_sphere.set_editor_property("Cloud Opacity", cloud_opacity)
    sky_sphere.set_editor_property("Cloud Opacity", cloud_opacity)
    sky_sphere.set_editor_property("Sun Height", sun_height)
    location += shift

    directional_light = spawn_actor_from_class(unreal.DirectionalLight, location)
    directional_light.set_folder_path(folder)
    directional_light.set_actor_rotation(direction_light_rotation, True)
    directional_light.light_component.set_intensity(direction_light_intensity)
    directional_light.light_component.set_atmosphere_sun_light(True)
    directional_light.light_component.set_light_color(unreal.LinearColor(*direction_light_color))
    location += shift

    post_process_volume = spawn_actor_from_class(unreal.PostProcessVolume, location)
    post_process_volume.set_folder_path(folder)
    post_process_volume.unbound = True
    settings = post_process_volume.settings

    settings.override_auto_exposure_bias = True
    settings.override_auto_exposure_min_brightness = True
    settings.override_auto_exposure_max_brightness = True
    settings.override_color_saturation = True

    settings.auto_exposure_bias = 0.0
    settings.auto_exposure_min_brightness = 1.0
    settings.auto_exposure_max_brightness = 1.0
    settings.color_saturation = color_saturation
    location += shift

    sky_light = spawn_actor_from_class(unreal.SkyLight, location)
    sky_light.set_folder_path(folder)
    sky_light.light_component.set_intensity(sky_light_scale)
    location += shift

    atmospheric_fog = spawn_actor_from_class(unreal.AtmosphericFog, location)
    atmospheric_fog.set_folder_path(folder)
    atmospheric_fog.atmospheric_fog_component.set_precompute_params(
        density_height=0.5,
        max_scattering_order=4,
        inscatter_altitude_sample_num=32)
    location += shift

    sphere_reflection_capture = spawn_actor_from_class(unreal.SphereReflectionCapture, location)
    sphere_reflection_capture.set_folder_path(folder)
    location += shift


if __name__ == '__main__':
    fix_environment_light()
