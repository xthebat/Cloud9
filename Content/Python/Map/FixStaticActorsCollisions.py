import unreal
from Common import get_component_by_class


def fix_static_actors_collision():
    complex_as_simple = int(unreal.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE.value)
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        print(f"Setup collision for: {actor.get_name()}")
        component = get_component_by_class(actor, unreal.StaticMeshComponent)
        static_mesh = component.static_mesh
        unreal.Cloud9ToolsLibrary.set_collision_complexity(static_mesh, complex_as_simple)
        unreal.EditorStaticMeshLibrary.remove_collisions(static_mesh)


if __name__ == '__main__':
    fix_static_actors_collision()
