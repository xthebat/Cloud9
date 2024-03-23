import posixpath

import unreal
from Common import get_component_by_class_or_none

base_phys_material_dir = posixpath.join("Game", "Physicals")


def load_phys_material(name: str):
    path = posixpath.join(base_phys_material_dir, name)
    return unreal.EditorAssetLibrary.load_asset(path)


prefixes = {"worldspawn_", "func_brush_", "func_detail_"}

material_mapping = {
    ("concreate",): load_phys_material("PM_Concreate"),
    ("glass",): load_phys_material("PM_Glass"),
    ("metal",): load_phys_material("PM_Metal"),
    ("dirt",): load_phys_material("PM_Mud"),
    # ("???",): load_phys_material("PM_Paper"),
    # ("???",): load_phys_material("PM_Rock"),
    ("wood",): load_phys_material("PM_Wood")
}


def setup_phys_material_ex(actor: unreal.Actor, material: unreal.Material):
    if material.phys_material is not None:
        return True

    material_name = material.get_name()
    for keywords, phys_material in material_mapping.items():
        if any(keyword for keyword in keywords if keyword in material_name):
            print(f"Setup phys material '{phys_material.get_name()}' "
                  f"for actor='{actor.get_name()}' "
                  f"material='{material.get_name()}'")
            material.phys_material = phys_material
            return True

    return False


def setup_phys_material_actor(
        actor: unreal.Actor,
        changed_materials: list[unreal.MaterialInterface],
        actors_without_material: list[unreal.Actor]
):
    if component := get_component_by_class_or_none(actor, unreal.StaticMeshComponent):
        for index in range(component.get_num_materials()):
            material = component.get_material(index)

            assert isinstance(material, unreal.Material), \
                f"Material required, not a material interface for {material.get_name()}"

            if not setup_phys_material_ex(actor, material):
                actors_without_material.append(actor)

            unreal.MaterialEditingLibrary.recompile_material(material)
            changed_materials.append(material)


def fix_physical_materials():
    changed_materials = []
    actors_without_material = []

    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        actor_name: str = actor.get_name()
        if any(prefix for prefix in prefixes if actor_name.startswith(prefix)):
            setup_phys_material_actor(actor, changed_materials, actors_without_material)

    unreal.EditorAssetLibrary.save_loaded_assets(changed_materials, only_if_is_dirty=False)


if __name__ == '__main__':
    fix_physical_materials()
