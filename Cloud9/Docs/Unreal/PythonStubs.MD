### Add Python Unreal Engine stubs

See here [Generate Python Stubs](https://blog.csdn.net/Jingsongmaru/article/details/89493620)

1. Enable the Python Editor Script Plugin
2. Go to `Project Settings -> Plugins -> Python` and turn-on developer mode (look for Drop-down marker)
3. Restart Unreal Engine and Python stubs will be generated. UE print the directory with stub file into console
   (e.g., `c:\Users\bat\Documents\Work\unreal_tasks\Cloud9\Intermediate\PythonStub\`)
4. Change Rider maximum intellisense limit:
   - In Rider go to `Help -> Edit Custom Properties`
   - Add the next line: `idea.max.intellisense.filesize=16777216` (for 16 MB stub file)
   - Restart Rider
5. Add this path to Rider Class folder or just copy it to Python site-packages folder and change extension to `*.pyi`

Note: Command `UE4Editor.exe -run=PythonOnlineDocs -IncludeEngine -IncludeProject` generate not complete stub 