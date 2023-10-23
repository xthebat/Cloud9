import os
from typing import Iterable, Dict

import unreal
from UnrealCommon import get_level_actors, get_actor_name_from_path


def parse_failed_trimesh_logfile(filepath: str) -> Iterable[str]:
    signature = "Failed to cook TriMesh: "

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


def delete_all_actors(actors: Iterable):
    for it in actors:
        print(f"Destroy actor: {it.get_name()}")
        it.destroy_actor()


def delete_all_assets(assets_paths: Iterable[str]):
    for it in assets_paths:
        print(f"Delete asset: {it}")
        unreal.EditorAssetLibrary.delete_asset(it)


def cleanup_assets(
        level_actors: Dict,
        filename: str,
        destroy_actors: bool = False,
        delete_assets: bool = False
):
    script_directory = os.path.dirname(os.path.realpath(__file__))
    filepath = os.path.join(script_directory, filename)
    failed_actor_paths = parse_failed_trimesh_logfile(filepath)

    if destroy_actors:
        failed_actors = [
            level_actors[get_actor_name_from_path(it)] for it in failed_actor_paths
            if get_actor_name_from_path(it) in level_actors
        ]

        delete_all_actors(failed_actors)

    if delete_assets:
        delete_all_assets(failed_actor_paths)


if __name__ == '__main__':
    cleanup_assets(get_level_actors(), "de_shortdust_cleanup.txt", destroy_actors=True, delete_assets=True)
