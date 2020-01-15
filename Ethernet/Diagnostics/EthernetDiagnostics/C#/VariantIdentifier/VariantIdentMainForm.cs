using System;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace VariantIdentifier
{
  public partial class VariantIdentMainForm : Form
  {
    CANoe.Variable mSysVarResult;
    CANoe.Variable mSysVarTarget;
    private bool mbDone;

    public VariantIdentMainForm()
    {
      InitializeComponent();
    }

    public void AddStatusText( string text)
    {
      mTB_Status.Text += Environment.NewLine + text;
      mTB_Status.Select(mTB_Status.Text.Length, 0);
      mTB_Status.ScrollToCaret(); // Make sure the last line is displayed
    }

    private void mBt_Connect_Click(object sender, EventArgs evargs)
    {
      mLV_Targets.Items.Clear();
      mLV_Targets.Enabled = false;
      mBt_StartIdent.Enabled = false;
      mBt_ConfigureVariants.Enabled = false;
      mTB_Status.Text = "Connecting to CANoe and looking for configured diagnostics targets ...";
      mTB_Status.BackColor = Color.LightGray;

      mSysVarResult = null;
      mSysVarTarget = null;

      try
      {
        CANoe.Application app = new CANoe.Application();
        CANoe.Configuration config = (CANoe.Configuration)app.Configuration;
        CANoe.GeneralSetup generalSetup = (CANoe.GeneralSetup)config.GeneralSetup;
        CANoe.DiagnosticsSetup diagSetup = (CANoe.DiagnosticsSetup)generalSetup.DiagnosticsSetup;
        CANoe.DiagDescriptions descriptions = (CANoe.DiagDescriptions)diagSetup.DiagDescriptions;
        foreach (CANoe.Network netw in (CANoe.Networks)app.get_Networks(null))
        {
          foreach (CANoe.Device dev in (CANoe.Devices)netw.Devices)
          {
            try
            {
              Diagnostic.Diagnostic diag = (Diagnostic.Diagnostic)dev.Diagnostic;
              if (null != diag)
              {
                ListViewItem item = new ListViewItem();
                item.Checked = false;
                item.Text = dev.Name;
                item.SubItems.Add(netw.Name);
                item.SubItems.Add("(unknown)");  // default for configured variant, should be overwritten
                item.SubItems.Add("(not identified yet)");  // default for configured variant, should be overwritten
                foreach (CANoe.DiagDescription descr in descriptions)
                {
                  if (descr.Qualifier == dev.Name)
                  {
                    item.SubItems[chConfiguredVariant.Index].Text = descr.Variant;
                    break;
                  }
                }
                mLV_Targets.Items.Add(item);
              }
            }
            catch
            {
              // device does not have a diagnostics interface, so ignore it
            }
          }
        }
      }
      catch (System.Exception ex)
      {
        mTB_Status.Text = "Error: " + ex.ToString();
        mTB_Status.BackColor = Color.Red;
        return;
      }

      mTB_Status.Text = "" + mLV_Targets.Items.Count + " targets found.";

      if (mLV_Targets.Items.Count > 0)
      {
        mLV_Targets.Enabled = true;
        mTB_Status.BackColor = Color.LightGreen;
      }
      {
        CANoe.Application app = new CANoe.Application();
        mBt_StartIdent.Enabled = !((CANoe.Measurement)app.Measurement).Running;
      }
    }

    static void DoSleep(int milliseconds)
    {
      // Looks like it uses milliseconds ...
      System.Threading.Thread.Sleep(milliseconds);
    }

    void StartIdentificationRun()
    {
      if( GetTargetString() == "")
      {
        mTB_Status.Text = "Select Targets first!";
        mTB_Status.BackColor = Color.LightSalmon;
        return;
      }

      if( PathToSourceNode() == "")
      {
        mTB_Status.Text = "Cannot find CAPL program that performs identification!";
        mTB_Status.BackColor = Color.Red;
        return;
      }

      AddStatusText("Configuring CANoe and trying to start the variant identification ...");
      mTB_Status.BackColor = Color.LightGray;

      const string csNSDiagIdentifier = "DiagIdentifier";
      const string csVarResult = "ResultOfIdentification";
      const string csVarTarget = "Target";
      const string csNodeName = "_TEMP_VarIdent_V1.0";

      // Prepare connection to application
      CANoe.Application app = new CANoe.Application();
      CANoe.Namespaces spaces = (CANoe.Namespaces)((CANoe.System)app.System).Namespaces;
      CANoe.Variables varsDiagIdentifier = null;

      // Check if namespace and/or variables have been defined already
      bool bCreateNamespace = true;
      bool bCreateSysVarResult = true;
      bool bCreateSysVarTarget = true;
      foreach (CANoe.Namespace ns in spaces)
      {
        if (ns.Name == csNSDiagIdentifier)
        {
          varsDiagIdentifier = (CANoe.Variables) ns.Variables;
          bCreateNamespace = false;
          foreach (CANoe.Variable var in varsDiagIdentifier)
          {
            if (var.Name == csVarResult)
            {
              mSysVarResult = var;
              bCreateSysVarResult = false;
            }
            else if (var.Name == csVarTarget)
            {
              mSysVarTarget = var;
              bCreateSysVarTarget = false;
            }
          }
          break;
        }
      }

      // Create namespace and variables, if necessary
      AddStatusText( "Creating namespace and system variables");
      if (bCreateNamespace)
      {
        varsDiagIdentifier = (CANoe.Variables) spaces.Add(csNSDiagIdentifier).Variables;
      }

      if( bCreateSysVarResult)
      {
        mSysVarResult = (CANoe.Variable) varsDiagIdentifier.AddWriteable(csVarResult, "");
      }
      if (bCreateSysVarTarget)
      {
        mSysVarTarget = (CANoe.Variable) varsDiagIdentifier.AddWriteable(csVarTarget, "");
      }

      if (mSysVarResult == null || mSysVarTarget == null)
      {
        AddStatusText( "Cannot attach to system variables needed for control!");
        mTB_Status.BackColor = Color.Red;
        return;
      }

      // Add the simulation node that communicates with this program an initiates the variant identification to the setup
      AddStatusText( "Adding and configuring simulation node");
      CANoe.Configuration config = (CANoe.Configuration)app.Configuration;
      CANoe.SimulationSetup simSetup = (CANoe.SimulationSetup)config.SimulationSetup;
      CANoe.Buses buses = (CANoe.Buses)simSetup.Buses;
      
      if (buses.Count < 1)
      {
        AddStatusText( "There is no bus configure to attach the simulation node to!");
        mTB_Status.BackColor = Color.Red;
        return;
      }

      CANoe.Bus bus = (CANoe.Bus)buses[1];
      mTB_Status.Text += " to bus " + bus.Name;

      CANoe.Nodes nodes = (CANoe.Nodes)bus.Nodes;
      CANoe.Node node = (CANoe.Node)nodes.Add(csNodeName, null, null);

      node.FullName = PathToSourceNode();

      // Start the measurement
      if( DialogResult.OK == MessageBox.Show("If you press OK, the CANoe measurement will be started and the variant identification is run!", "Attention: Measurement Start"
        , MessageBoxButtons.OKCancel, MessageBoxIcon.Warning))
      {
        CANoe.Measurement meas = (CANoe.Measurement)app.Measurement;
        AddStatusText( "Starting CANoe measurement ");
        meas.Start();
        while (!meas.Running)
        {
          mTB_Status.Text += ".";
          DoSleep(20);
        }

        DoVariantIdentification();

        AddStatusText( "Stopping CANoe measurement ");
        meas.Stop();
        while (meas.Running)
        {
          mTB_Status.Text += ".";
          DoSleep(20);
        }
      }

      // Cleanup
      AddStatusText( "Cleaning up ...");

      // Remove the simulation node
      int i;
      AddStatusText( "Removing simulation node");
      nodes.Remove(node);

      // Remove the namespace
      if (bCreateNamespace)
      {
        for (i = 1; i <= spaces.Count; ++i)
        {
          if (spaces[i].Name == csNSDiagIdentifier)
          {
            AddStatusText( "Removing namespace");
            spaces.Remove(i);
            break;
          }
        }
      }
    }

    // Compute the absolute file path of the CAPL program in the identifier node
    static string PathToSourceNode()
    {
      const string csCAPLFileName = "\\Identifier.can";
      string caplFile = Application.StartupPath + csCAPLFileName;
      if (System.IO.File.Exists(caplFile))
        return caplFile;
      caplFile = Application.StartupPath + "\\.." + csCAPLFileName;
      if (System.IO.File.Exists(caplFile))
        return caplFile;
      caplFile = Application.StartupPath + "\\..\\.." + csCAPLFileName;
      if (System.IO.File.Exists(caplFile))
        return caplFile;
      caplFile = Application.StartupPath + "\\..\\..\\.." + csCAPLFileName;
      if (System.IO.File.Exists(caplFile))
        return caplFile;
      return "";  // Error!
    }

    private void mBt_StartIdent_Click(object sender, EventArgs e)
    {
      mBt_StartIdent.Enabled = false;
      StartIdentificationRun();
      mBt_StartIdent.Enabled = true;
    }

    private void button1_Click(object sender, EventArgs e)
    {
      Application.Exit();
    }

    private void mBt_ConfigureVariants_Click(object sender, EventArgs e)
    {
      mTB_Status.Text = "Configuring variants in the configuration";

      try
      {
        CANoe.Application app = new CANoe.Application();
        CANoe.Configuration config = (CANoe.Configuration)app.Configuration;
        CANoe.GeneralSetup generalSetup = (CANoe.GeneralSetup)config.GeneralSetup;
        CANoe.DiagnosticsSetup diagSetup = (CANoe.DiagnosticsSetup)generalSetup.DiagnosticsSetup;
        CANoe.DiagDescriptions descriptions = (CANoe.DiagDescriptions)diagSetup.DiagDescriptions;

        foreach (CANoe.DiagDescription descr in descriptions)
        {
          foreach (ListViewItem item in mLV_Targets.Items)
          {
            if (item.Text != descr.Name)
              continue;

            string varQual = item.SubItems[chIdentifiedVariant.Index].Text;
            if (descr.Variant == varQual)
            {
              AddStatusText(descr.Name + "already has correct variant " + varQual);
              mLV_Targets.Items.Remove(item);
            } 
            else if (varQual[0] == '(') // Is this the initial dummy text?
              AddStatusText("No variant was identified for " + descr.Name + " yet!");
            else
            {
              try
              {
                descr.Variant = varQual;
                AddStatusText("Selecting variant " + varQual + " for description " + descr.Name);
                mLV_Targets.Items.Remove(item);
              }
              catch
              {
                AddStatusText("Error Selecting variant " + varQual + " for description " + descr.Name + "!");
                mTB_Status.BackColor = Color.Red;
                item.BackColor = Color.Red;
              }
            }
            break;
          }
        }
      } catch( Exception localEx)
      {
        AddStatusText("Error: " + localEx.ToString());
        mTB_Status.BackColor = Color.Red;
      }

      mBt_ConfigureVariants.Enabled = mLV_Targets.Items.Count > 0;
    }
    private void DoVariantIdentification()
    {
      mBt_Connect.Enabled = false;
      mLV_Targets.Enabled = false;

      mbDone = false;

      CANoe._IVariableEvents_OnChangeEventHandler handler = new CANoe._IVariableEvents_OnChangeEventHandler(SysVarResult_OnChange);
      mSysVarResult.OnChange += handler;

      string targets = GetTargetString();
      AddStatusText("Starting identification for: " + targets);
      mSysVarTarget.Value = targets;

      while (!mbDone)
      {
        Application.DoEvents();
        DoSleep(10);
      }

      mSysVarResult.OnChange -= handler;
      mBt_Connect.Enabled = true;
      mLV_Targets.Enabled = true;
    }

    private void SysVarResult_OnChange(object valueIn)
    {
      string value = (string)valueIn;
      AddStatusText("Result: " + value);
      if (value == "Done.")
      {
        mbDone = true;
      }
      else if (value.StartsWith("/Error:"))
      {
        // Handle error here, ignored at the moment
      }
      else // format: <target> ": " <variant>
      {
        int i = value.IndexOf(": ");
        if (i < 1)
        {
          AddStatusText("Error in Result format: " + value);
          return;
        }
        string target = value.Substring(0, i);
        string varQual = value.Substring(i + 2);
        foreach (ListViewItem item in mLV_Targets.Items)
        {
          if (item.Text == target)
          {
            item.BackColor = Color.LightGreen;
            item.SubItems[chIdentifiedVariant.Index].Text = varQual;
            item.Checked = false;
            mBt_ConfigureVariants.Enabled = true;
            return;
          }
        }
        AddStatusText("Cannot find target " + target + " in list!");
      }
    }

    private string GetTargetString()
    {
      string targets = "";
      foreach (ListViewItem item in mLV_Targets.Items)
      {
        if (!item.Checked)
          continue;

        targets += item.Text + ";";
      }
      return targets;
    }
  }
}