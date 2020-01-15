using System;
using Vector.Tools;
using Vector.CANoe.Runtime;
using Vector.CANoe.Threading;
using Vector.Diagnostics;
using Vector.Scripting.UI;
using Vector.CANoe.TFS;
using Vector.CANoe.VTS;
using NetworkDB;
using SysPars.LockingTests;
using System.Collections.Generic;


[TestClass]
public class MyTestClass
{
  /// <summary>
	/// Checks the static locking behavior of the car
	/// </summary>
  [Export]
  [TestCase]
  public static void TC_LockStatically()
  {
    // Initialize the system
    Report.TestStep("Initialize the system...");
    LockingTestLibrary.ResetLockingTestSignals();
    
    // Check lock function when engine is off
    Report.TestStep("Check lock function when engine is off...");
    EngineRunning.Value = 0;
    LockRequest.Value = LockRequest.Request_lock;
    Execution.Wait(LockRequestWaitTime.GetValue());
    if (LockState.Value != LockState.Locked)
    {
      Report.TestStepFail("Lock state is 'unlocked'. Expected lock state is 'locked'.");
    }
    else
    {
      Report.TestStepPass("Lock state is 'locked' as expected.");
    }
    
    // Check lock function when engine is  running
    Report.TestStep("Check lock function when engine is running...");
    EngineRunning.Value = 1;
    LockRequest.Value = LockRequest.Request_lock;
    Execution.Wait(SysPars.LockingTests.LockRequestWaitTime.GetValue());
    if (LockState.Value != LockState.Locked)
    {
      Report.TestStepFail("Lock state is 'unlocked'. Expected lock state is 'locked'.");
    }
    else
    {
      Report.TestStepPass("Lock state is 'locked' as expected.");
    }
    
    // Reset the system 
    LockingTestLibrary.ResetLockingTestSignals();
  }
  
  /// <summary>
	/// Checks the static unlock behavior of the car
	/// </summary>
  [Export]
  [TestCase]
  public static void TC_UnlockStatically()
  {
    // Intialize the system
    Report.TestStep("Initialize the system...");
    LockingTestLibrary.ResetLockingTestSignals();
    
    // Check unlock function when engine is not running
    Report.TestStep("Check lock function when engine is off...");
    EngineRunning.Value = 0;
    LockRequest.Value = LockRequest.Request_unlock;
    Execution.Wait(SysPars.LockingTests.LockRequestWaitTime.GetValue());
    if (LockState.Value != LockState.Unlocked)
    {
     Report.TestStepFail("Lock state is 'locked'. Expected lock state is 'unlocked'.");
    }
    else
    {
      Report.TestStepPass("Lock state is 'unlocked' as expected.");
    }
    
    // Check unlock function when engine is  running
    Report.TestStep("Check lock function when engine is off...");
    EngineRunning.Value = 1;
    LockRequest.Value = LockRequest.Request_unlock;
    Execution.Wait(SysPars.LockingTests.LockRequestWaitTime.GetValue());
    if (LockState.Value != LockState.Unlocked)
    {
      Report.TestStepFail("Lock state is 'locked'. Expected lock state is 'unlocked'.");
    }
    else
    {
      Report.TestStepPass("Lock state is 'unlocked' as expected.");
    }
    
    // Reset the system 
    LockingTestLibrary.ResetLockingTestSignals();
  }
 
  /// <summary>
	/// Checks the crash detection of the car while the engine is off and not moving
	/// </summary>
  [Export]
  [TestCase]
  public static void TC_CrashDetectionWhileEngineOff()
  {
    // Initialize signals
    Report.TestStep("Initialize all signals...");
    LockingTestLibrary.ResetLockingTestSignals();
    
    // Set lock state to locked
    Report.TestStep("Set initial lock state to locked...");
    LockRequest.Value = LockRequest.Request_lock;
    EngineRunning.Value = 0;
    Velocity.Value = 0;
    Execution.Wait(LockRequestWaitTime.GetValue());
    
    // Simulate crash    
    Report.TestStep("Simulate crash...");
    CrashDetected.Value = 1;
    Execution.Wait(CrashDetectionWaitingTime.GetValue());
    
    // Check lock state is locked
    Report.TestStep("Check expected lock state to be locked.");
    if (LockState.Value != LockState.Locked)
    {
      Report.TestStepFail("Lock state is 'unlocked'. Expected lock state is 'locked'.");
    }
    else
    {
      Report.TestStepPass("Lock state is 'locked' as expected.");
    }
   
    // Reset signals
    Report.TestStep("Rest all signals...");
    LockingTestLibrary.ResetLockingTestSignals();
  }
  
  /// <summary>
	/// Checks the automatic locking behavior of the car by increasing velocity
	/// </summary>
  [Export]
  [TestCase]
  public static void TC_LockByIncreasingVelocity()
  {
    // Initialize the system
    Report.TestStep("Initialize the system...");
    LockingTestLibrary.ResetLockingTestSignals();
    
    // Ensure initial state unlocked
    Report.TestStep("Ensure initial state 'unlocked'...");
    LockRequest.Value = LockRequest.Request_unlock;
    Execution.Wait(500);
    
    // Start engine
    Report.TestStep("Start engine...");
    EngineRunning.Value = 1;
    
    // Do tests for locking by velocity
    
    Report.TestStep("Accelerate to velocity lower than the lock-velocity");
    CaplTestFunctions.Utilities.AccelerateDecelerateToTargetSpeed(20);
    Execution.Wait(500);
    
    Report.TestStep("Check no automatic lock with velocity lower than the lock-velocity");
    if (LockState.Value != LockState.Unlocked)
    {
      Report.TestStepFail("Lock state is 'locked'. Expected lock state is 'unlocked'.");
    }
    else
    {
       Report.TestStepPass("Lock state is 'unlocked' as expected.");
    }
    
    Report.TestStep("Accelerate to velocity higher than the lock-velocity");
    CaplTestFunctions.Utilities.AccelerateDecelerateToTargetSpeed(60);
    Execution.Wait(500);
    
    Report.TestStep("Check automatic lock with velocity higher than the lock-velocity");
    if (LockState.Value != LockState.Locked)
    {
      Report.TestStepFail("Lock state is 'unlocked'. Expected lock state is 'locked'.");
    }
    else
    {
       Report.TestStepPass("Lock state is 'locked' as expected.");
    }
    
    Report.TestStep("Decelerate until stopped");
    CaplTestFunctions.Utilities.AccelerateDecelerateToTargetSpeed(0);
    
    // Reset all signals
    Report.TestStep("Reset all signals...");
    LockingTestLibrary.ResetLockingTestSignals();
  }
  
  [Export]
  [TestCase]
  public static void TC_ApplyCrashWithDifferentVelocities(int engineRunning, int velocity)
  {
    // Initialize the system
    Report.TestStep("Initialize the system...");
    LockingTestLibrary.ResetLockingTestSignals();
    
    EngineRunning.Value = engineRunning;

    Report.TestStep("Set initial state 'locked'.");   
    LockRequest.Value = LockRequest.Request_lock;
    Execution.Wait(500);
    
    Report.TestStep("Set velocity to execute test with...");
    Velocity.Value = velocity;
    Execution.Wait(500);
    
    // Simulate crash    
    Report.TestStep("Simulate crash...");
    CrashDetected.Value = 1;
    Execution.Wait(CrashDetectionWaitingTime.GetValue());
    
    // Check lock state is unlocked
    Report.TestStep("Check expected lock state to be unlocked.");
    if (LockState.Value != LockState.Unlocked)
    {
      Report.TestStepFail("Lock state is 'locked'. Expected lock state is 'unlocked'.");
    }
    else
    {
      Report.TestStepPass("Lock state is 'unlocked' as expected.");
    }
   
    // Reset signals
    Report.TestStep("Rest all signals...");
    LockingTestLibrary.ResetLockingTestSignals();
  }

  /// <summary>
	/// Checks the open and comfort close of the windows
	/// </summary>
  [Export]
  [TestCase]
  public static void TC_OpenAndComfortClose()
  {
    // Initialize the system
    Report.TestStep("Initialize the system...");
    LockingTestLibrary.ResetLockingTestSignals();
    
    // Open the window
    Report.TestStep("Open the window...");
    KeyDown.Value = 1;
    Execution.Wait(5000);
    
    // Release key
    Report.TestStep("Release key...");
    KeyDown.Value = 0;
    Execution.Wait(100);
    
    // Try the comfort close and check if the window is really closing
    Report.TestStep("Try the comfort close and check if the window is really closing...");
    LockRequest.Value = LockRequest.Comfort_lock;
    Execution.Wait(4000);
    if (WindowMotion.Value != WindowMotion.currently_closing)
    {
      string txt;
      if (WindowMotion.Value == WindowMotion.currently_opening){
        txt = "Window motion is 'currently_opening'. Expected window motion is 'currently_closing'.";
      }
      else
      {
        txt = "Window motion is 'not_moving'. Expected window motion is 'currently_closing'.";
      }
      Report.TestStepFail(txt);
    }
    else
    {
       Report.TestStepPass("Window motion is 'currently_closing' as expected.");
    }
    
    // Check if the window stops moving when the car is opened again
    Report.TestStep("Check if the window stops moving when the car is opened again...");
    LockRequest.Value = LockRequest.Request_unlock;
    Execution.Wait(100);
    if (WindowMotion.Value != WindowMotion.not_moving)
    {
      string txt;
      if (WindowMotion.Value == WindowMotion.currently_opening){
        txt = "Window motion is 'currently_opening'. Expected window motion is 'not_moving'.";
      }
      else
      {
        txt = "Window motion is 'currently_closing'. Expected window motion is 'not_moving'.";
      }
      Report.TestStepFail(txt);
    }
    else
    {
       Report.TestStepPass("Window motion is 'not_moving' as expected.");
    }
    
    // Reset all signals
    Report.TestStep("Reset all signals...");
    WindowMotion.Value = WindowMotion.not_moving;
    WindowPosition.Value = 0;
    LockingTestLibrary.ResetLockingTestSignals();
  }
  
  /// <summary>
	/// Checks that a corresponding fault memory entry is created when ambiguous values of KeyUp and KeyDown occur
	/// </summary>
	/// <param name="engineRunning">Indicates whether engine is running or off</param>
  [Export]
  [TestCase]
  public static void TC_AmbiguousKeyUpDown(int engineRunning)
  {
     // Initialize the system
    Report.TestStep("Initialize the system...");
    KeyUp.Value = 0;
    KeyDown.Value = 0;
    
    EngineRunning.Value = engineRunning;
    
    // Retrieve the ECU by asking the application object
    Report.TestStep("Query ECU...");

    Ecu ecu = Application.GetEcu("SUT");
    if( null == ecu)
    {
      Report.TestStepFail("Cannot find ECU 'SUT'. Aborting.");
      return;
    }
    Report.TestStepPass("ECU 'SUT' found.");

    // Make sure the fault memory is clear, clear it when necessary
    Report.TestStep("Check fault memory status and clear it if not empty yet...");

    long numberOfDTCs = BasicDiagnostics.ReadNumberOfDTCs(ecu);
        
    switch (numberOfDTCs)
    {
      case 0: // Fault memory is already empty
        Report.TestStepPass("Empty fault memory.");
        break;
      case 1: // There is a DTC in the fault memory
        Report.TestStep("Entry found in fault memory, therefore clearing it.");
        if (0 > BasicDiagnostics.ClearFaultMemory(ecu) || 0 != BasicDiagnostics.ReadNumberOfDTCs(ecu))
        {
          Report.TestStepFail("Clearing fault memory failed.");
          return;
        }
        break;
      default: // Error reading fault memory, aborting
        Report.TestStepFail("Cannot confirm that fault memory is empty.");
        return;
    }

    // Provoke an error by concurrently setting lock request to 'lock' and 'unlock'.
    Report.TestStep("Provoke entry in fault memory");
    KeyUp.Value = 1;
    KeyDown.Value = 1;
    Execution.Wait(100);
    
    // There should be a DTC in the fault memory now.
    Report.TestStep("Check presence of DTC in fault memory...");

    if (BasicDiagnostics.ReadNumberOfDTCs(ecu) == 1)
    {
      Report.TestStepPass("DTC stored as expected.");
    }
    else
    {
      Report.TestStepFail("DTC not reported.");
    }

    // Reset fault memory
    Report.TestStep("Reset fault memory...");
    BasicDiagnostics.ClearFaultMemory(ecu);

     // Reset signals
    Report.TestStep("Reset all signals...");
    KeyUp.Value = 0;
    KeyDown.Value = 0;
  }
}

