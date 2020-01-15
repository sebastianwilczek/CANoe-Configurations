using System;
using System.Collections.ObjectModel;
using Vector.Tools;
using Vector.CANoe.Runtime;
using Vector.CANoe.Threading;
using Vector.Diagnostics;
using Vector.Scripting.UI;
using Vector.CANoe.TFS;
using Vector.CANoe.VTS;
using ASAM.XIL.Interfaces.Testbench.MAPort;
using ASAM.XIL.Interfaces.Testbench.Common.ValueContainer;
using ASAM.XIL.Implementation.TestbenchFactory.Testbench;
using ASAM.XIL.Implementation.Testbench.Common.ValueContainer;

[TestClass]
public class XILWrapper
{
  private static IMAPort sMAPort;
  private static string sVariableName;
  private static string sManifestFolder, sVendorName, sProductName, sProductVersion;
  private static IBaseValue sVariableValue;
  
  [Export]
  public static void Init(string manifestFolder, string vendorName, string productName, string productVersion)
  {
    // Store the paramters to be used in the asynchronous task
    sManifestFolder = manifestFolder;
    sVendorName = vendorName;
    sProductName = productName;
    sProductVersion = productVersion;
    
    // Execute the task asynchronously so that CANoe is not blocked
    Execution.WaitForTask(InitTask);
  }
  
  public static int InitTask(TaskCancelToken tct)
  {
    // Initialize a test bench and a MA port
    try
    {
      var factory = new TestbenchFactory(sManifestFolder);
      var testbench = factory.CreateVendorSpecificTestbench(sVendorName, sProductName, sProductVersion);
      sMAPort = testbench.MAPortFactory.CreateMAPort("Example MA Port");
      sMAPort.StartSimulation();
    }
    catch(Exception ex)
    {
        Output.WriteLine("XIL API initialization failed: " + ex.Message);
    }
      
    return 1;
  }
  
  [Export]
  public static double ReadVariable(string variableName)
  {
    // Store the paramters to be used in the asynchronous task
    sVariableName = variableName;
    
    // Execute the task asynchronously so that CANoe is not blocked
    Execution.WaitForTask(ReadVariableTask);
    return (sVariableValue as IFloatValue).Value;
  }
  
  public static int ReadVariableTask(TaskCancelToken tct)
  {
    // Read the variable's value
    try
    {
      sVariableValue = sMAPort.Read(sVariableName);
    }
    catch(Exception ex)
    {
        Output.WriteLine("XIL API read failed: " + ex.Message);
    }
    
    return 1;
  }
  
   [Export]
  public static void WriteVariable(string variableName, double value)
  {
    // Store the paramters to be used in the asynchronous task
    sVariableName = variableName;
    sVariableValue = new FloatValue(value);
    
    // Execute the task asynchronously so that CANoe is not blocked
    Execution.WaitForTask(WriteVariableTask);
  }
  
  public static int WriteVariableTask(TaskCancelToken tct)
  {
    // Write to the variable
    try
    {
      sMAPort.Write(sVariableName, sVariableValue);
    }
    catch(Exception ex)
    {
        Output.WriteLine("XIL API write failed: " + ex.Message);
    }
    
    return 1;
  }
  
  [Export]
  public static void Shutdown()
  {
    // Execute the task asynchronously so that CANoe is not blocked
    Execution.WaitForTask(ShutdownTask);
  }
  
  public static int ShutdownTask(TaskCancelToken tct)
  {
    // Shut down the MA port
    try
    {
      sMAPort.StopSimulation();
      sMAPort.Disconnect();
    }
    catch(Exception ex)
    {
        Output.WriteLine("XIL API shutdown failed: " + ex.Message);
    }
    finally
    {
      sMAPort.Dispose();
    }
    
    return 1;
  }
}