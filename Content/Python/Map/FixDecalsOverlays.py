﻿"""
This script fixes decals and overlays in the level by updating their materials.

This script searches for all actors in the level with names starting with "info_overlay_".
For each matching actor, it retrieves the Static Mesh Component and updates the materials to add an alpha channel.
The materials are set to use the BLEND_TRANSLUCENT blend mode, and an alpha channel is added to the base color texture.

Note:
- This script assumes that the Common module is available and provides the get_component_by_class function.
- The materials that are modified are saved and recompiled.

Example:
    To fix decals and overlays in the level, call the `fix_decals_overlays` function without any arguments.

        fix_decals_overlays()

"""

import unreal
from Common import get_component_by_class


def fix_decals_overlays():
    materials = list()

    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        name: str = actor.get_name()
        if not name.startswith("info_overlay_"):
            continue

        print(f"Setup decals for: {actor.get_name()}")

        try:
            component = get_component_by_class(actor, unreal.StaticMeshComponent)
        except KeyError:
            continue

        for index in range(component.get_num_materials()):
            material = component.get_material(index)

            assert isinstance(material, unreal.Material), \
                f"Material required, not a material interface for {material.get_name()}"

            material.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)

            texture = unreal.MaterialEditingLibrary.get_material_property_input_node(
                material, unreal.MaterialProperty.MP_BASE_COLOR)

            status = unreal.MaterialEditingLibrary.connect_material_property(
                texture, "A", unreal.MaterialProperty.MP_OPACITY)

            unreal.MaterialEditingLibrary.recompile_material(material)

            print(f" -> Add alpha channel to material: '{material.get_name()}' status={status}")
            materials.append(material)

    unreal.EditorAssetLibrary.save_loaded_assets(materials, only_if_is_dirty=False)


if __name__ == '__main__':
    fix_decals_overlays()
