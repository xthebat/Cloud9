"""
Contains functions to fix failed TriMesh objects in the game engine.

This module provides functions to parse a log file containing information about failed TriMesh objects and fix their build settings.
The TriMesh objects are identified by their actor paths, and their scale is adjusted to a specified upscale factor.
The fixed objects are then saved, and their scale is restored to the original value.

Example:
    To fix failed TriMesh objects, call the `fix_level_failed_trimesh` function.

        fix_level_failed_trimesh()

"""

import os
from typing import Iterable

import unreal
from Common import make_level_actors_dict, get_component_by_class
from Extras import fixups_path, fixups_ext


def parse_failed_trimesh_logfile(filepath: str) -> Iterable[str]:
    signature = "Failed to cook TriMesh: "

    assert os.path.isfile(filepath), f"Filepath with incorrect TriMesh isn't exist {filepath}"

    with open(filepath, "rt") as log_file:
        data = log_file.read()

    actor_paths = []

    for line in data.splitlines():
        line = line.strip()
        if not line:
            continue
        columns = line.split(maxsplit=2)
        message = columns[2]
        if not message.startswith(signature):
            continue
        path = message.lstrip(signature).rstrip(".")
        actor_paths.append(path)

    return actor_paths


def get_actor_name_from_path(path: str):
    basename = os.path.basename(path)
    name, ext = os.path.splitext(basename)
    return name


def fix_level_failed_trimesh(upscale: float = 10.0):
    world = unreal.EditorLevelLibrary.get_editor_world()
    level_name = world.get_fname()

    filepath = os.path.join(fixups_path, f"{level_name}.{fixups_ext}")
    failed_actor_paths = parse_failed_trimesh_logfile(filepath)

    level_actors = make_level_actors_dict()

    upscale3d = unreal.Vector(upscale, upscale, upscale)

    print(f"Upscale vector = {upscale3d}")

    actors = []
    meshes = []

    for actor_path in failed_actor_paths:
        name = get_actor_name_from_path(actor_path)
        actor = level_actors[name]

        print(f"Fix build settings of {actor}")

        # unreal.EditorLevelLibrary.set_actor_selection_state(actor, True)

        component = get_component_by_class(actor, unreal.StaticMeshComponent)

        static_mesh: unreal.StaticMesh = component.static_mesh

        build_settings = unreal.EditorStaticMeshLibrary.get_lod_build_settings(static_mesh, 0)
        build_settings.build_scale3d = upscale3d
        build_settings.remove_degenerates = False
        # According to documentation of EPIC this will make the same action as 'Apply Changes'
        unreal.EditorStaticMeshLibrary.set_lod_build_settings(static_mesh, 0, build_settings)

        actors.append(actor)
        meshes.append(static_mesh)

    unreal.EditorAssetLibrary.save_loaded_assets(meshes, only_if_is_dirty=False)

    for actor in actors:
        old_scale = actor.get_actor_scale3d()
        new_scale = old_scale / upscale
        actor.set_actor_scale3d(new_scale)
        print(f"Set actor scale name={actor.get_name()} -> old_scale={old_scale} new_scale={new_scale}")


if __name__ == '__main__':
    fix_level_failed_trimesh()
