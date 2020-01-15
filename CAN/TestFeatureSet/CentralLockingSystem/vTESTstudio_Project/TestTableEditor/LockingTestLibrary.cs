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

[TestClass]
public class LockingTestLibrary
{  
  /// <summary>
	/// Tests the locking behavior on crash detection.
	/// </summary>
	/// <param name="engineOn">Indicates whether the engine is running or not</param>
	/// <param name="velocity">The velocity of the engine</param>
  [Export]
  [TestCase("Crash detection")]
  public static void CrashDetection(int engineOn, double velocity)
  { 
    // Initialize signals
    Report.TestStep("Initialize all signals...");
    ResetLockingTestSignals();
    
    // Set lock state to locked
    Report.TestStep("Set initial lock state to locked...");
    EngineRunning.Value = engineOn;
    Velocity.Value = velocity;
    LockRequest.Value = LockRequest.Request_lock;
    Execution.Wait(LockRequestWaitTime.GetValue());
    
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
    
    // Check if the car remains opened, even if the velocity increases to higher than the lock-velocity
    if (engineOn == 1)
    {
      Report.TestStep("Check if the car remains opened, even if the velocity increases to higher than the lock-velocity");
      
      // Increase velocity to a higher value than lock-velocity
      
      double increasedVelocity = velocity + 10;
      if (increasedVelocity < LockVelocity.GetValue()) 
      {
        increasedVelocity = LockVelocity.GetValue() + 10;
      }
      
      Report.TestStep("Increase velocity: Set velocity to " + increasedVelocity);
      Velocity.Value = increasedVelocity;
      Execution.Wait(CrashDetectionWaitingTime.GetValue());
    
      // Check lock state is still unlocked
      Report.TestStep("Check expected lock state to be unlocked.");
      if (LockState.Value != LockState.Unlocked)
      {
        Report.TestStepFail("Lock state is 'locked'. Expected lock state is 'unlocked'.");
      }
      else
      {
        Report.TestStepPass("Lock state is 'unlocked' as expected.");
      }
    }
   
    // Reset signals
    Report.TestStep("Rest all signals...");
    ResetLockingTestSignals();
  }
  
  /// <summary>
	/// Helper function for resetting signals of the door control unit.
	/// </summary>
  public static void ResetLockingTestSignals()
  {
    EngineRunning.Value = 0;
    Velocity.Value = 0;
    CrashDetected.Value = 0;
    LockRequest.Value = LockRequest.idle;
    
    Execution.Wait(500);
  }
}