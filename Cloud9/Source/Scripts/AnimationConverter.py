from typing import TextIO


def string_between(string: str, first: str, last: str):
    start = string.index(first) + len(first)
    end = string.index(last, start)
    return string[start:end]


def import_frames_from_smd(file: TextIO, ignore: set[str] = None):
    ignore = ignore or set()
    text = file.read()
    nodes_text = string_between(text, "nodes", "end")
    nodes = {}
    for line in nodes_text.splitlines():
        if not line.strip():
            continue
        tokens = line.split()
        nodes[tokens[0]] = string_between(tokens[1], "\"", "\"")

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
            bone_name = nodes[tokens[0]]
            if bone_name not in ignore:
                coordinates = [float(it) for it in tokens[1:]]
                frame[bone_name] = coordinates

    return frames


def main():
    with open("a_move_knife_runN.smd", "rt") as smd:
        frames = import_frames_from_smd(smd, {"ValveBiped.weapon_bone"})
    for it in frames:
        print(it)


if __name__ == '__main__':
    main()
