### Fix Python plugin in JetBrains Rider

JetBrains Rider may incorrectly determine the Python interpreter or even ignore the configured Python interpreter. 
This is because JetBrains Rider does not have a default module system like other JetBrains IDEs. 
For example, in IntelliJ IDEA, the problem can be fixed by configuring Python in `File -> Project Structure`, 
but this is not the case in Rider. To fix this, you need to manually add it in the `.idea` folder.

1. Configure Python in `File -> Settings -> Build, Execution, Deployment -> Python Interpreter` 
2. Lookup `misc.xml` file in `.idea` folder
3. Add at least the following lines in it:
   ```xml
   <component name="ProjectRootManager" version="2" languageLevel="JDK_19" project-jdk-name="Python 3.11" project-jdk-type="Python SDK">
   </component>
   ```
   P.S.: Looks like string in `project-jdk-name` should be equal to your 
   configured Python interpreter name from the first step.
4. See also step 5 from [PythonStub](../Unreal/PythonStub.MD)