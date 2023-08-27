from dataclasses import dataclass
from sys import stdout
from typing import TextIO


def string_between(string: str, first: str, last: str):
    start = string.index(first) + len(first)
    end = string.index(last, start)
    return string[start:end]


def subtract_base_bone_from_frames_ex(frames: list[dict], bone_name: str, indexes: list = None):
    indexes = indexes or [1]

    base_frame = frames[0]
    base_bones = base_frame[bone_name]

    for frame in frames[1:]:
        bones = frame[bone_name]
        for it in indexes:
            bones[it] = base_bones[it]

        # base_bone_coordinates = frame[base_bone_name]
        # for it in indexes:
        #     base_bone_coordinates[it] = 0.0
        # for bone_name in frame:
        #     coordinates = frame[bone_name].copy()
        #     for i in range(len(base_bone_coordinates)):
        #         if i in indexes:
        #             coordinates[i] -= base_bone_coordinates[i]
        #     frame[bone_name] = coordinates


def subtract_base_bone_from_frames(frames: list[dict], base_bone_names: list[str], indexes: list = None):
    for it in base_bone_names:
        subtract_base_bone_from_frames_ex(frames, it, indexes)


@dataclass
class Node:
    index: int
    name: str
    parent: int

    @staticmethod
    def from_string(string: str):
        tokens = string.split()
        return Node(int(tokens[0]), string_between(tokens[1], "\"", "\""), int(tokens[2]))

    def to_string(self):
        return f"{self.index} \"{self.name}\" {self.parent}"


def import_frames_from_smd(file: TextIO, ignore: set[str] = None):
    ignore = ignore or set()
    text = file.read()
    nodes_text = string_between(text, "nodes", "end")
    nodes: dict[int, Node] = {}
    for line in nodes_text.splitlines():
        if not line.strip():
            continue
        node = Node.from_string(line)
        nodes[node.index] = node

    text = text.replace(nodes_text, "")
    frames_text = string_between(text, "skeleton", "end")
    frames_lines = frames_text.splitlines()
    frames = []

    frame = None

    for line in frames_lines:
        if not line.strip():
            continue
        tokens = line.split()
        if tokens[0] == "time":
            if frame is not None:
                frames.append(frame)
            frame = {}
        else:
            bone_index = int(tokens[0])
            bone_node = nodes[bone_index]
            if bone_node.name not in ignore:
                coordinates = [float(it) for it in tokens[1:]]
                frame[bone_node.name] = coordinates

    return frames, nodes


def export_frames_to_smd(file: TextIO, frames: list[dict], nodes: dict[int, Node]):
    print("version 1", file=file)
    print("nodes", file=file)
    for node in nodes.values():
        print(node.to_string(), file=file)
    print("end", file=file)

    bone_map = {node.name: node.index for node in nodes.values()}

    print("skeleton", file=file)
    for index, frame in enumerate(frames):
        print(f"time {index}", file=file)
        for bone in frame:
            index = bone_map[bone]
            coordinates = frame[bone]
            print(f"{index}", file=file, end=" ")
            for coordinate in coordinates:
                print(f"{coordinate:.6f}", file=file, end=" ")
            print(file=file)
    print("end", file=file)


def main():
    with open("a_move_knife_walkN.smd", "rt") as smd:
        frames, nodes = import_frames_from_smd(smd, {"ValveBiped.weapon_bone"})

    subtract_base_bone_from_frames(frames, ["pelvis", "lean_root", "cam_driver"])

    # for it in frames:
    #     print(it)

    with open("a_move_knife_walkN_static.smd", "wt") as smd:
        export_frames_to_smd(smd, frames, nodes)


if __name__ == '__main__':
    main()
