import unreal


def fix_static_actors_collision():
    complex_as_simple = int(unreal.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE.value)
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        print(f"Setup collision for: {actor.get_name()}")
        mesh_component = actor.get_component_by_class(unreal.StaticMeshComponent)
        if mesh_component is None:
            continue
        static_mesh = mesh_component.static_mesh
        unreal.Cloud9ToolsLibrary.set_collision_complexity(static_mesh, complex_as_simple)
        unreal.EditorStaticMeshLibrary.remove_collisions(static_mesh)


def main():
    fix_static_actors_collision()


if __name__ == '__main__':
    main()
