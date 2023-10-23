import unreal


def fix_decals_overlays():
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        name: str = actor.get_name()
        if not name.startswith("info_overlay"):
            continue

        print(f"Setup decals for: {actor.get_name()}")

        mesh_component: unreal.StaticMeshComponent = actor.get_component_by_class(unreal.StaticMeshComponent)
        if mesh_component is None:
            continue

        for index in range(mesh_component.get_num_materials()):
            material = mesh_component.get_material(index)

            material.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)

            assert isinstance(material, unreal.Material), \
                f"Material required, not a material interface for {material.get_name()}"

            texture = unreal.MaterialEditingLibrary.get_material_property_input_node(
                material, unreal.MaterialProperty.MP_BASE_COLOR)

            status = unreal.MaterialEditingLibrary.connect_material_property(
                texture, "A", unreal.MaterialProperty.MP_OPACITY)

            unreal.EditorAssetLibrary.save_loaded_asset(material)

            print(f" -> Add alpha channel to material: '{material.get_name()}' status={status}")


if __name__ == '__main__':
    fix_decals_overlays()
