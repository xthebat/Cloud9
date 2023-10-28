import unreal
from Common import get_component_by_class


def fix_physics_props_collision():
    ctf_use_default = int(unreal.CollisionTraceFlag.CTF_USE_DEFAULT.value)
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        name: str = actor.get_name()
        if not name.startswith("prop_physics"):
            continue

        print(f"Setup physics collision for: {actor.get_name()}")

        component = get_component_by_class(actor, unreal.StaticMeshComponent)

        actor.set_mobility(unreal.ComponentMobility.MOVABLE)

        static_mesh = component.static_mesh
        unreal.Cloud9ToolsLibrary.set_collision_complexity(static_mesh, ctf_use_default)
        unreal.EditorStaticMeshLibrary.remove_collisions(static_mesh)
        unreal.EditorStaticMeshLibrary.add_simple_collisions(static_mesh, unreal.ScriptingCollisionShapeType.NDOP26)
        component.set_simulate_physics(True)

        # TODO: Remove collision with character and bots


if __name__ == '__main__':
    fix_physics_props_collision()
