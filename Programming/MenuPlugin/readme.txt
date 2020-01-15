Changelog
=========
1.1.0.0:
* The Vector.MenuPlugin assembly now targets the framework 4.7. Please update your existing plugins to also target this framework version.



MenuPlugin API Description
==========================

You can use the MenuPlugin API to display your own main menu entry with several subentries in CANoe. 
You can use these menu commands to run any applications you have created yourself.
To do so, you must create a .NET assembly:
* The Vector.MenuPlugin.dll component must be referenced in the assembly. 
  Note: Reference the newest version, which is located in the \Exec32\Components\Vector.MenuPlugin directory. 
        Do not use the DLL that is found in the Programming folder of the Sample Configurations. 
        Ensure that Copy Local is set to False in the project settings.
        Your project must target framework 4.7 or newer.
* The IMenuPlugin and IMenuItem interfaces are defined in the Vector.MenuPlugin namespace. Create two classes that implement these interfaces. 
  Note: For details on the interfaces, refer to the Vector.MenuPlugin help file:
        <Path to your CANoe Sample Configurations>\Programming\MenuPlugin\Vector.MenuPlugin.chm
* Build the assembly. 
* In order for to find the file, the path to your DLL must be entered under the MenuPlugin key in the [GUI] group in the CAN.ini.
  For example: [GUI] MenuPlugin = <path to Programming>\MenuPlugin\bin\debug\MenuPluginDemo.dll 

After restart of CANoe the main menu entry will be displayed.


