using System;
using Vector.Tools;
using Vector.CANoe.Runtime;
using Vector.CANoe.Threading;
using Vector.Diagnostics;
using Vector.Scripting.UI;
using Vector.CANoe.TFS;
using Vector.CANoe.VTS;
using NetworkDB;

[TestClass]
public class LockingTests
{
  public static ITestTree CreateExecutionTree()
  {
    ITestTreeElement tgLockingTests = TestFactory.CreateGroup("Lock by velocity", 
      TestFactory.CreateElement(LockByVelocity));
    
    ITestTreeElement tgCrashDetection = TestFactory.CreateGroup("Crash detection", 
      TestFactory.CreateElementList<FctDel_int>(CrashDetection));
    
    ITestTreeElement tgWindowTests = TestFactory.CreateGroup("Window tests", 
      TestFactory.CreateElement("Open the window", SimpleWindowTest, 0),
      TestFactory.CreateElement("Close the window", SimpleWindowTest, 1));
    
    ITestTree testTree = TestFactory.CreateTestTree();
    testTree.Add(tgLockingTests);
    testTree.Add(tgCrashDetection);
    
    if (CtrlVars.Coverage.GetValue() == CtrlVars.Coverage.Full)
    {
      testTree.Add(tgWindowTests);
    }
    
    return testTree;
  }
  
  [TestCase]
  public static void LockByVelocity()
  {
    EngineRunning.Value = 1;
    
    foreach (SysPars.LockingTests.LockingTestVectorList lockTestVector in SysPars.LockingTests.LockingTestVectorList.GetStructList())
    {
      Velocity.Value = lockTestVector.Velocity.GetValue();
      CrashDetected.Value = lockTestVector.CrashDetected.GetValue();
      
      Execution.Wait(lockTestVector.Wait.GetValue());
      
      if (LockState.Value != lockTestVector.ExpectedLockState.GetValue())
      {
         Report.TestStepFail("Unexpected lock state.");
      }
    }
  }
  
  [TestCase("Crash detection")]
  public static void CrashDetection([Values(0,1)] int engineOn)
  { 
    // Engine is off and car is locked
    EngineRunning.Value = engineOn;
    LockState.Value = 1;
    
    int waitTime = SysPars.LockingTests.CrashDetectionWaitingTime.GetValue();
    Execution.Wait(waitTime);

    CrashDetectionTest crashTest = new CrashDetectionTest();
    crashTest.lockState = 1; // car should stay locked
    crashTest.Execute();

    LockState.Value = 0;
  }
  
  /************************************************************************/
  /* Test pattern to check the crash detection.
  /************************************************************************/
  public class CrashDetectionTest : StateChange
  {
    public CrashDetectionTest()
    {
      Description = "Checks the unlock of the car, if a crash will be detected";
      
      int waitTime = SysPars.LockingTests.CrashDetectionWaitingTime.GetValue();
      Wait = waitTime;
    }

    [Input(typeof(NetworkDB.CrashDetected))]
    public double crashDetected = 1;

    [Expected(typeof(NetworkDB.LockState), Relation.Equal)]
    public double lockState = 0;
  }
  
  /************************************************************************/
  /* The test case checks the open and close of the windows.
  /************************************************************************/
  [TestCase]
  public static void SimpleWindowTest(int open)
  {
    WindowTest checkWindows = new WindowTest();
    // Start the window lifts
    checkWindows.Description = open == 1 ? "Start opening the window. Check if the window is really opened." : "Start closing the window. Check if the window is really closed.";
    checkWindows.Wait = 500;
    checkWindows.KeyDown = open == 1 ? 1 : 0;
    checkWindows.KeyUp = open == 1 ? 0 : 1;
    checkWindows.WindowMotion = open == 1 ? 2 : 1;
    checkWindows.Execute();
    
    // Stop the window lifts
    checkWindows.Description = open == 1 ? "Stop opening the window. Check if the window is halted very soon." : "Stop closing the window. Check if the window is halted very soon.";
    checkWindows.Wait = 100;
    checkWindows.KeyDown = 0;
    checkWindows.KeyUp = 0;
    checkWindows.WindowMotion = 0;
    checkWindows.Execute();
  }
  
  /************************************************************************/
  /* Test pattern to check the window lifts.
  /************************************************************************/
  public class WindowTest : StateChange
  {
    [Input(typeof(NetworkDB.KeyUp))]
    public double KeyUp = 0;
    [Input(typeof(NetworkDB.KeyDown))]
    public double KeyDown = 0;
    [Expected(typeof(NetworkDB.WindowMotion))]
    public double WindowMotion = 0;
  }
}