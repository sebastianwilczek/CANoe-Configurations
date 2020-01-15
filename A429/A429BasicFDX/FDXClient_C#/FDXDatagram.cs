/*-----------------------------------------------------------------------------
Module: CANoeFDX
Interfaces: FDXDatagram
-------------------------------------------------------------------------------
Datagram builder class for CANoe FDX protocol
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/
using System;
using System.Collections.Generic;

namespace FDXClient
{
  public class FDXDatagram
  {
    public FDXDatagram()
    {
      this.mHeader = new DatagramHeader();
      this.mCommands = new List<CommandHeader>();
    }

    public void InitWithHeader()
    {
      this.mHeader.FDXSignature = FDXHelper.kFDXSignature;
      this.mHeader.FDXMajorVersion = FDXHelper.kFDXMajorVersion;
      this.mHeader.FDXMinorVersion = FDXHelper.kFDXMinorVersion;
      this.mHeader.NumberOfCommands = 0;
      this.mHeader.SequenceNumber = FDXHelperSequenceNumber.kSequenceNumberUnused;
      this.mHeader.Reserved = 0;

      // the command list gets cleared when a new datagram is initialized
      this.mCommands.Clear();
      this.Size = 16;
    }

    public void SetSequenceNumber(ushort sequenceNumber)
    {
      this.mHeader.SequenceNumber = sequenceNumber;
    }

    public void AddDataRequest(ushort groupId)
    {
      this.AddCommand(new DataRequestCommand{ CommandCode = FDXHelperCommandCode.kCommandCodeDataRequest, CommandSize = 6, GroupId = groupId});
      this.Size += 6;
    }

    public void AddDataExchange(ushort groupId, byte[] data)
    {
      this.AddCommand(new DataExchangeCommand { CommandCode = FDXHelperCommandCode.kCommandCodeDataExchange, CommandSize = (ushort)(8 + data.Length), GroupId = groupId, DataSize = (ushort)data.Length, DataBytes = data });
      this.Size += (uint)(8 + data.Length);
    }

    public void AddStartCommand()
    {
      this.AddCommand(new CommandHeader { CommandSize = 4, CommandCode = FDXHelperCommandCode.kCommandCodeStart });
      this.Size += 4;
    }

    public void AddStopCommand()
    {
      this.AddCommand(new CommandHeader { CommandSize = 4, CommandCode = FDXHelperCommandCode.kCommandCodeStop });
      this.Size += 4;
    }

    public void AddStatusRequest()
    {
      this.AddCommand(new CommandHeader { CommandSize = 4, CommandCode = FDXHelperCommandCode.kCommandCodeStatusRequest});
      this.Size += 4;
    }

    public void AddKeyCommand(UInt32 keyCode)
    {
      this.AddCommand(new KeyCommand{CommandSize = 8, CommandCode = FDXHelperCommandCode.kCommandCodeKey, CanoeKeyCode = keyCode});
      this.Size += 8;
    }
    
    public byte[] GenerateBuffer()
    {
      this.mBuffer = new byte[this.Size];
      var arrHeader = this.mHeader.Serialize();
      var arrPos = 0;
      Array.Copy(arrHeader, 0, this.mBuffer, arrPos, arrHeader.Length);
      arrPos = arrHeader.Length;
      foreach (var command in this.mCommands)
      {
        var tmpArr = command.Serialize();
        Array.Copy(tmpArr, 0, this.mBuffer, arrPos, tmpArr.Length);
        arrPos += tmpArr.Length;
      }
      return this.mBuffer;
    }

    public byte[] Buffer
    {
      get { return this.mBuffer; }
      set { this.mBuffer = value; }
    }

    public static UInt16 IncrementSequenceNumber(UInt16 seqNr)
    {
      if (seqNr == FDXHelperSequenceNumber.kSequenceNumberUnused)
      {
        return FDXHelperSequenceNumber.kSequenceNumberUnused;
      }
      else if ((seqNr & FDXHelperSequenceNumber.kSequenceNumberSessionEndFlag) > 0)
      {
        return FDXHelperSequenceNumber.kSequenceNumberUnused;
      }
      else if (seqNr == 0x7FFF)
      {
        return 1;
      }
      else
      {
        return (ushort)(seqNr + 1);
      }
    }

    private void AddCommand(CommandHeader newCommand)
    {
      this.mCommands.Add(newCommand);
      this.mHeader.NumberOfCommands++;
    }

    #region Variables
    public uint Size { get; set; }
    private readonly DatagramHeader mHeader;
    private readonly List<CommandHeader> mCommands;
    private byte[] mBuffer;
    #endregion
  }
}
