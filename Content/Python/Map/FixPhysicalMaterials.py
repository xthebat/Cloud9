import re
from dataclasses import dataclass
from typing import Set

import unreal
from Common import get_component_by_class


@dataclass
class MaterialDesc:
    pattern: re.Pattern
    material: unreal.PhysicalMaterial

    @staticmethod
    def load_phys_material(name: str) -> unreal.PhysicalMaterial:
        base_phys_material_dir = "/Game/Physicals"
        path = f"{base_phys_material_dir}/{name}"
        phys_material = unreal.EditorAssetLibrary.load_asset(path)
        if phys_material is None:
            raise ValueError(f"Can't load asset: {path}")
        # noinspection PyTypeChecker
        return phys_material

    @classmethod
    def load(cls, pattern: str, name: str):
        return cls(re.compile(pattern), MaterialDesc.load_phys_material(name))


materials = [
    MaterialDesc.load(r"\w*concrete\w*", "PM_Concrete"),
    MaterialDesc.load(r"\w*plaster\w*", "PM_Concrete"),
    MaterialDesc.load(r"\w*glass\w*", "PM_Glass"),
    MaterialDesc.load(r"\w*metal\w*", "PM_Metal"),
    MaterialDesc.load(r"\w*_tin_\w*", "PM_Metal"),
    MaterialDesc.load(r"\w*dirt\w*", "PM_Mud"),
    MaterialDesc.load(r"\w*wood\w*", "PM_Wood"),
    MaterialDesc.load(r"\w*tree\w*", "PM_Wood"),
    MaterialDesc.load(r"\w*crate\w*", "PM_Wood"),
    MaterialDesc.load(r"\w*brick\w*", "PM_Rock"),
    MaterialDesc.load(r"\w*stone\w*", "PM_Rock"),
    MaterialDesc.load(r"\w*de_dust\w*wall\w*", "PM_Rock"),
    MaterialDesc.load(r"\w*de_dust_sign\w*", "PM_Metal"),
]

prefixes = {"worldspawn_", "func_brush_", "func_detail_", "prop_static_"}


def setup_phys_material_ex(actor: unreal.Actor, material: unreal.Material):
    if material.get_editor_property("phys_material") is not None:
        return False

    material_name = material.get_name()
    for desc in materials:
        if desc.pattern.match(material_name):
            print(f"Setup phys material '{desc.material.get_name()}' "
                  f"for actor='{actor.get_name()}' "
                  f"material='{material.get_name()}'")
            material.set_editor_property("phys_material", desc.material)
            return True

    raise ValueError("Can't setup material")


def setup_phys_material_actor(
        actor: unreal.Actor,
        changed_materials: Set[unreal.MaterialInterface],
        without_material: Set[unreal.MaterialInterface]
):
    try:
        component = get_component_by_class(actor, unreal.StaticMeshComponent)
    except KeyError:
        return

    # print(f"Analyze {component.static_mesh.get_name()}")

    for index in range(component.get_num_materials()):
        material = component.get_material(index)

        assert isinstance(material, unreal.Material), \
            f"Material required, not a material interface for {material.get_name()}"

        try:
            if setup_phys_material_ex(actor, material):
                # unreal.MaterialEditingLibrary.recompile_material(material)
                changed_materials.add(material)
        except ValueError:
            without_material.add(material)


def fix_physical_materials():
    changed_materials = set()
    without_material = set()

    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        actor_name: str = actor.get_name()
        if any(prefix for prefix in prefixes if actor_name.startswith(prefix)):
            setup_phys_material_actor(actor, changed_materials, without_material)

    if changed_materials:
        print("<============================================================>")
        print("Changed materials:")

        for material in changed_materials:
            print(material.get_name())

    if without_material:
        print("<============================================================>")
        print("Material without physical materials:")

        for material in without_material:
            print(material.get_name())

    unreal.EditorAssetLibrary.save_loaded_assets(changed_materials, only_if_is_dirty=False)


if __name__ == '__main__':
    fix_physical_materials()
