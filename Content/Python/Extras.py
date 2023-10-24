import os.path

import unreal

importing_directory = "Importing"
fixups_directory = "Fixups"

project_path = unreal.SystemLibrary.get_project_directory()
fixups_path = os.path.join(project_path, importing_directory, fixups_directory)
