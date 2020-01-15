using System;
using System.Collections.ObjectModel;
using Vector.Tools;
using Vector.CANoe.Runtime;
using Vector.CANoe.Threading;
using Vector.Diagnostics;
using Vector.Scripting.UI;
using Vector.CANoe.TFS;
using Vector.CANoe.VTS;
using NetworkDB;

public class BasicDiagnostics
{
    
  public static long ReadNumberOfDTCs(Ecu ecu)
  {
    using (Request reqReadFaultMemory = ecu.CreateRequest("FaultMemory_ReadAllIdentified"))
    {     
      Parameter statusOfDTC = reqReadFaultMemory.GetParameter("DtcStatusbyte");
      
      if (statusOfDTC != null) 
      {
        statusOfDTC.Value.Set(2);
      }
      else
      {
        return -1;
      }

      using (SendResult result = reqReadFaultMemory.Send())
      {
        if (SendStatus.Ok != result.Status)
        {
          return -3;
        } 
        else
        {
          Parameter listOfDTC = result.Response.GetParameter("ListOfDTC");
          return (listOfDTC != null) ? listOfDTC.Parameters.Count : -4;
        }
      }
    }
  }

  public static long ClearFaultMemory(Ecu ecu)
  {
    using (Request reqClearFaultMemory = ecu.CreateRequest("FaultMemory_Clear")) 
    {
      if (reqClearFaultMemory == null) 
      {
        return -1;
      }
      
      using (SendResult result = reqClearFaultMemory.Send()) 
      {
        if (result.Status != SendStatus.Ok) 
        {
          return -2;
        }
        
        if (result.Response == null || !result.Response.IsPositive)
        {
          return -3;
        }
      }
    }
    return 0;
  }
}