using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Vector.PanelControlPlugin;

namespace Demo
{
  public class ControlLibraryImpl : IPanelControlPluginLibrary
  {
    #region IPanelControlPluginLibrary Members

    /// <summary>
    /// Returns the name of the plugin control library as it shall be shown in the toolbox of the panel designer.
    /// </summary>
    public string LibraryName
    {
      get 
      { 
        // =====>
        return "Demo Library Name"; 
        // <=====
      }
    }

    /// <summary>
    /// Returns a bitmap (16x16), which shall be shown in the toolbox of the panel designer.
    /// Return null, if no image shall be shown. In this case, the name of the library is shown.
    /// </summary>
    public System.Drawing.Image LibraryImage
    {
      get 
      {
        // =====>
        return Properties.Resources.LibraryImage;
        // <=====
      }
    }

    #endregion
  }
}
