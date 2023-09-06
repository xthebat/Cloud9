import math

import unreal
from unreal import CollisionTraceFlag, ScriptingCollisionShapeType, \
    EditorStaticMeshLibrary, ConverterFunctions, EditorLevelLibrary, StaticMeshComponent

threshold = 10.0

static_mesh = None

actors = EditorLevelLibrary.get_all_level_actors()
for actor in actors:
    mesh_component = actor.get_component_by_class(StaticMeshComponent)
    if mesh_component is None:
        continue
    static_mesh = mesh_component.static_mesh
    # scale = ConverterFunctions.collision_volume_scale(static_mesh)
    #
    # if math.isnan(scale):
    #     EditorStaticMeshLibrary.remove_collisions(static_mesh)
    #     ConverterFunctions.set_collision_complexity(
    #         static_mesh, 3)
    #
    # elif scale > threshold:
    #     print(f"{static_mesh.get_name()} -> {scale}")
    #
    #     for variant in [
    #         ScriptingCollisionShapeType.NDOP10_X,
    #         ScriptingCollisionShapeType.NDOP10_Y,
    #         ScriptingCollisionShapeType.NDOP10_Z
    #     ]:
    #         EditorStaticMeshLibrary.remove_collisions(static_mesh)
    #         EditorStaticMeshLibrary.add_simple_collisions(static_mesh, variant)
    #         scale = ConverterFunctions.collision_volume_scale(static_mesh)
    #         if scale < threshold:
    #             break
    #
    #     if scale > threshold:
    #         print(f"Can't fix this shit: {static_mesh.get_name()}")
    #         EditorStaticMeshLibrary.remove_collisions(static_mesh)
    ConverterFunctions.set_collision_complexity(static_mesh, 3)
    EditorStaticMeshLibrary.remove_collisions(static_mesh)
