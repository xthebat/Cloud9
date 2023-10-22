import os

import unreal


def get_level_actors():
    actors = unreal.EditorLevelLibrary.get_all_level_actors()
    return {actor.get_name(): actor for actor in actors}


def get_actor_name_from_path(path: str):
    return os.path.basename(path).split(".")[0]
