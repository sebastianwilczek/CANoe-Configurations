--------------------------------------------------------------------------------
MostComClientCS v 1.0.0.0
--------------------------------------------------------------------------------

----------------------------------------
1) How to use the executable
----------------------------------------
- Start CANoe
- Load: MOST System Demo
- Start MostComClientCS.exe

In COM Client:
- Click 'Connect to CANoe'
- Click 'Start Measurement'
- Wait a few seconds
- Click 'Get Property Value'
-> Property 'DeckStatus' of the function 'AudioDiskPlayer.01.DeckStatus'
   is requested and symbolic as well as numeric value of the property are
   displayed in the COM client's GUI.

----------------------------------------
2) The source code of the COM client
----------------------------------------

2.1) General Information

The source code
- is provided as VisualStudio 2005 solution
- is implemented using C# and .NET 2.0
- references CANoe COM TypeLib v1.31 (as provided by CANoe 8.0 MainRelease)
  This is the minimal required TypeLib version for this client.

To use this COM Client together with a different version of the CANoe COM TypeLib:
- Replace the reference to the CANoe COM TypeLib in the solution by the one of
  the target CANoe version (e.g TypeLib v1.29 as provided by CANoe 7.6 MainRelease)
  The minimal required TypeLib version for this client is v1.27.
- Rebuild the solution


2.2) The classes of the demo client

There are comments provided in the source code. Read them carefully when implementing
your own COM client.

class CANoeAccess
This class demonstrates how to use the interfaces
- IMostNetworkInterface
- IMostApplicationFilter
- IMostDisassembler

class MainForm
This class demonstrates how to connect a client GUI to the CANoe COM server
