using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

// This class must not be in any namespace!
public class resxfinder
{
    //  This is a dirty workaround for a bug in 'GetImageFromResource'.
    //  Use the type of this this class in the 'ToolboxBitmap' Attribute and 
    //  NOT the Control class. The second parameter is the bmp name with 
    //  the DEFAULT(!) namespace of the assembly (see project settings).
    //  Class 'resfinder' must be within this assembly. I.e. it is not possible to put it
    //  in another assembly.
    //  See also: http://www.bobpowell.net/toolboxbitmap.htm
    //  Example:
    //  Default namespace: "Demo"
    //  Bitmap name: "Toolbox_Button.bmp"
    //  Attribute:
    //  [ToolboxBitmap(typeof(resfinder), "Demo.Toolbox_Button.bmp")]
}
