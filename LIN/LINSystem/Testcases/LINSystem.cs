using System;
using Vector.Tools;
using Vector.CANoe.Runtime;
using Vector.CANoe.Threading;
using Vector.CANoe.TFS;
using NetworkDB;
using System.Text;

public class LINSystem : TestModule
{
  #region Test Control
  /************************************************************************/
  /* The main function of the test module controls the sequence of the test
   * cases. It is required to override this function.
  /************************************************************************/
  public override void Main()
  {
    // The title is written to the report
    Title = "LIN System";

    TestGroupBegin("Test the windows", "Checks the control of the windows");

    SimpleWindowTest(0); // test case: open and close front window (left)
    SimpleWindowTest(1); // test case: open and close front window (right)
    SimpleWindowTest(2); // test case: open and close rear window (left)
    SimpleWindowTest(3); // test case: open and close rear window (right)

    TestGroupEnd();
  }

  #endregion

  #region Test Cases by simple functions
  /************************************************************************/
  /* The test case checks the open and close of the windows.
  /************************************************************************/
  [TestCase]
  public void SimpleWindowTest(UInt16 pos)
  {
    switch(pos)
    {
      case 0:
        DoSimpleWindowTestFL();
        break;
      case 1:
        DoSimpleWindowTestFR();
        break;
      case 2:
        DoSimpleWindowTestRL();
        break;
      case 3:
        DoSimpleWindowTestRR();
        break;
      default:
        break;
    }
  }

  private void DoSimpleWindowTestFL()
  {
    Report.TestCaseTitle("Front window on the left side");
    Report.TestStep("Open front window on the left side");

    GWI_FrontWindowsLeft.Value = 2;
    Execution.Wait(2000);
    GWI_FrontWindowsLeft.Value = 1;

    ICheck observeOpen = new ValueCheck<DWFL_WinPos>(16);
    observeOpen.Activate();
    Execution.Wait(1000);
    observeOpen.Deactivate();

    Report.TestStep("Close front window on the left side");

    GWI_FrontWindowsLeft.Value = 0;
    Execution.Wait(2000);
    GWI_FrontWindowsLeft.Value = 1;

    ICheck observeClose = new ValueCheck<DWFL_WinPos>(0);
    observeClose.Activate();
    Execution.Wait(1000);
    observeClose.Deactivate();
  }

  private void DoSimpleWindowTestFR()
  {
    Report.TestCaseTitle("Front window on the right side");
    Report.TestStep("Open front window on the right side");

    GWI_FrontWindowsRight.Value = 2;
    Execution.Wait(2000);
    GWI_FrontWindowsRight.Value = 1;

    ICheck observeOpen = new ValueCheck<DWFR_WinPos>(16);
    observeOpen.Activate();
    Execution.Wait(1000);
    observeOpen.Deactivate();

    Report.TestStep("Close front window on the right side");

    GWI_FrontWindowsRight.Value = 0;
    Execution.Wait(2000);
    GWI_FrontWindowsRight.Value = 1;

    ICheck observeClose = new ValueCheck<DWFR_WinPos>(0);
    observeClose.Activate();
    Execution.Wait(1000);
    observeClose.Deactivate();
  }

  private void DoSimpleWindowTestRL()
  {
    Report.TestCaseTitle("Rear window on the left side");
    Report.TestStep("Open rear window on the left side");

    GWI_RearWindowsLeft.Value = 2;
    Execution.Wait(2000);
    GWI_RearWindowsLeft.Value = 1;

    ICheck observeOpen = new ValueCheck<DWRL_WinPos>(16);
    observeOpen.Activate();
    Execution.Wait(1000);
    observeOpen.Deactivate();

    Report.TestStep("Close rear window on the left side");

    GWI_RearWindowsLeft.Value = 0;
    Execution.Wait(2000);
    GWI_RearWindowsLeft.Value = 1;

    ICheck observeClose = new ValueCheck<DWFL_WinPos>(0);
    observeClose.Activate();
    Execution.Wait(1000);
    observeClose.Deactivate();
  }

  private void DoSimpleWindowTestRR()
  {
    Report.TestCaseTitle("Rear window on the right side");
    Report.TestStep("Open rear window on the right side");

    GWI_RearWindowsRight.Value = 2;
    Execution.Wait(2000);
    GWI_RearWindowsRight.Value = 1;

    ICheck observeOpen = new ValueCheck<DWRR_WinPos>(16);
    observeOpen.Activate();
    Execution.Wait(1000);
    observeOpen.Deactivate();

    Report.TestStep("Close rear window on the right side");

    GWI_RearWindowsRight.Value = 0;
    Execution.Wait(2000);
    GWI_RearWindowsRight.Value = 1;

    ICheck observeClose = new ValueCheck<DWRR_WinPos>(0);
    observeClose.Activate();
    Execution.Wait(1000);
    observeClose.Deactivate();
  }

  #endregion
}
