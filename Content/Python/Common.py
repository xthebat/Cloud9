import os
from typing import TypeVar, Type

import unreal

T = TypeVar("T")

ZeroVector = unreal.Vector(0.0, 0.0, 0.0)
ZeroRotator = unreal.Rotator(0.0, 0.0, 0.0)


def spawn_actor_from_class(
        cls: Type[T],
        location: unreal.Vector = ZeroVector,
        rotation: unreal.Rotator = ZeroRotator,
        transient: bool = False
) -> T:
    return unreal.EditorLevelLibrary.spawn_actor_from_class(cls, location, rotation, transient)


def get_component_by_class(actor: unreal.Actor, cls: Type[T]) -> T:
    component = actor.get_component_by_class(cls)
    if component is None:
        raise KeyError(f"Component class='{cls.__name__}' not found in actor='{actor}'")
    return component


def get_level_actors():
    actors = unreal.EditorLevelLibrary.get_all_level_actors()
    return {actor.get_name(): actor for actor in actors}


def get_actor_name_from_path(path: str):
    return os.path.basename(path).split(".")[0]
