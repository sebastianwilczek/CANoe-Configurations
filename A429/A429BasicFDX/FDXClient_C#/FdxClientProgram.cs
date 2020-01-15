/*-----------------------------------------------------------------------------
Module: FdxClientProgram
Interfaces: 
-------------------------------------------------------------------------------
Example of the CANoeFDX interface.
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/
using System;

namespace FDXClient
{
  using System.Threading;

  using ExchangeData;

  class FdxClientProgram
  {
    private static bool sKeepRunning = true;

    static void Main(string[] args)
    {
      //Register delegate to catch CTRL-C on the console
      Console.CancelKeyPress += delegate(object sender, ConsoleCancelEventArgs e)
        {
          e.Cancel = true;
          sKeepRunning = false;
        };
      Console.Out.WriteLine("The FDX-Client can be terminated with CTRL-C");

      var addr = String.Empty;
      ushort port = 0;
      sbyte cProbCapModifier = 1;
      var isFuelConsumingActive = false;

      //setup connection parameters when specified
      if (args.Length == 2)
      {
        addr = args[0];
        port = ushort.Parse(args[1]);
      }

      //Create socket, diagram and Dispatcher
      var fdxSocket = new FDXSocket();
      var fdxDatagram = new FDXDatagram();
      var disp = new A429DataDispatcher();

      //Create data packet and initialize with data
      var data = new A429InputData
      {
        FuelTempAndAdvisoryWarning = -2048,
        ProbeCapacitance = 0
      };

      var ovhdData = new A429OvhdData();

      //Set the new connection settings only when specified
      if (addr != string.Empty)
      {
        fdxSocket.SetCANoeAddr(addr, port);
      }
      fdxSocket.Open();

      //Initialize datagram and set start command
      fdxDatagram.InitWithHeader();
      fdxDatagram.AddStartCommand();

      //Send start command to CANoe
      fdxSocket.Send(ref fdxDatagram);

      // wait one second for initializing CANoe
      Thread.Sleep(1000);

      while (sKeepRunning)
      {
        fdxDatagram.InitWithHeader();

        //Update values only when measurement is running
        if (FDXHelperMeasurementState.CurrentMeasurementState == FDXHelperMeasurementState.kMeasurementStateRunning)
        {
          if (data.FuelTempAndAdvisoryWarning < 2048)
          {
            data.FuelTempAndAdvisoryWarning += 4;
          }
          else
          {
            data.FuelTempAndAdvisoryWarning = -2048;
          }
          if (cProbCapModifier == 1 && data.ProbeCapacitance > 399)
          {
            cProbCapModifier = -1;
          }
          else if (cProbCapModifier == -1 && data.ProbeCapacitance < 1)
          {
            cProbCapModifier = 1;
          }
          else
          {
            // activate fuel consumption when probe capacitance is over 100
            if (isFuelConsumingActive == false && data.ProbeCapacitance > 10)
            {
              ovhdData.SysMainEng1 = 1;
              ovhdData.SysMainEng2 = 1;
              ovhdData.SysMainEng3 = 1;
              ovhdData.SysMainEng4 = 1;
              ovhdData.SysTTankL = 1;
              ovhdData.SysTTankR = 1;
              fdxDatagram.AddDataExchange(A429OvhdData.sGroupId, ovhdData.Serialize());
              isFuelConsumingActive = true;
            }
            // deactivate fuel consumption when probe capacitance is under 100
            else if (isFuelConsumingActive && data.ProbeCapacitance <= 10)
            {
              ovhdData.SysMainEng1 = 0;
              ovhdData.SysMainEng2 = 0;
              ovhdData.SysMainEng3 = 0;
              ovhdData.SysMainEng4 = 0;
              ovhdData.SysTTankL = 0;
              ovhdData.SysTTankR = 0;
              fdxDatagram.AddDataExchange(A429OvhdData.sGroupId, ovhdData.Serialize());
              isFuelConsumingActive = false;
            }

            data.ProbeCapacitance += (0.4f * cProbCapModifier);
          }

          fdxDatagram.AddDataExchange(A429InputData.sGroupId, data.Serialize());
          fdxDatagram.AddDataRequest(A429OutputData.sGroupId);
        }

        //poll status whether measurement is not running
        fdxDatagram.AddStatusRequest();
        fdxSocket.Send(ref fdxDatagram);

        //receive and dispatch datagram
        if (fdxSocket.Receive(ref fdxDatagram) > -1)
        {
          disp.DispatchDatagram(ref fdxDatagram);
        }

        // sleep 250ms for next interval
        Thread.Sleep(250);
      }

      //Wait one second before shutting down CANoe
      Thread.Sleep(1000);

      //Init and send stop command
      fdxDatagram.InitWithHeader();
      fdxDatagram.AddStopCommand();
      fdxSocket.Send(ref fdxDatagram);
    }
  }
}
