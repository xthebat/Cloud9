### Add Python Unreal Engine stub

See here [Generate Python Stub](https://blog.csdn.net/Jingsongmaru/article/details/89493620)

1. Enable the Python Editor Script Plugin
2. Go to `Project Settings -> Plugins -> Python` and turn-on developer mode (look for Drop-down marker)
3. Restart Unreal Engine and Python stub will be generated. 
   UE print the directory with [stub file](../../Intermediate/PythonStub/unreal.py) 
   into console e.g.: `...\Cloud9\Intermediate\PythonStub\unreal.py` 
4. Change Rider maximum intellisense limit:
   - In Rider go to `Help -> Edit Custom Properties`
   - Add the next line: `idea.max.intellisense.filesize=16777216` (for 16 MB stub file)
   - Restart Rider
5. Add this path to Rider class folder (index folder):
   - Change the view type to `Solution view` if you in `File System view` in the left top corner of Rider.
   - Near the switch button (Solution itself now) on mouse hover an eye icon with 'B' shown.
   - Click the eye-button from the previous step: full project view should be shown.
   - Lookup folder `Intermediate\PythonStub`, make right click on this folder and then `Tools -> Attach to project`

Note #1: Rider adds additional paths to index into `indexLayout.xml` file.
Note #2: Add folder with Python scripts like step 5 (Rider will make better indexing for it)
Note #3: Command `UE4Editor.exe -run=PythonOnlineDocs -IncludeEngine -IncludeProject` generate not complete stub 