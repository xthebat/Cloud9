import unreal
from Common import get_component_by_class


def fix_info_overlay_collision():
    preset_name = "InvisibleWall"
    collision_profile = unreal.CollisionProfileName(preset_name)

    meshes = []
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        name: str = actor.get_name()
        if not name.startswith("info_overlay_"):
            continue

        try:
            component = get_component_by_class(actor, unreal.StaticMeshComponent)
        except KeyError:
            continue

        mesh: unreal.StaticMesh = component.static_mesh

        if unreal.Cloud9ToolsLibrary.set_collision_profile(mesh, collision_profile.name):
            print(f"Setup collision preset as {preset_name} for: {name}")
            meshes.append(mesh)

    unreal.EditorAssetLibrary.save_loaded_assets(meshes, only_if_is_dirty=False)


if __name__ == '__main__':
    fix_info_overlay_collision()
