from typing import TypeVar, Type, Dict, Iterable

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


def make_level_actors_dict() -> Dict[str, unreal.Actor]:
    actors = unreal.EditorLevelLibrary.get_all_level_actors()
    return {actor.get_name(): actor for actor in actors}


def get_actor_by_name(name: str):
    return next(it for it in unreal.EditorLevelLibrary.get_all_level_actors() if it.get_name() == name)


def delete_all_actors(actors: Iterable[unreal.Actor]):
    for it in actors:
        print(f"Destroy actor: {it.get_name()}")
        it.destroy_actor()


def delete_all_assets(assets_paths: Iterable[str]):
    for it in assets_paths:
        print(f"Delete asset: {it}")
        unreal.EditorAssetLibrary.delete_asset(it)
