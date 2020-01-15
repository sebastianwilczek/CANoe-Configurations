/*-----------------------------------------------------------------------------
Module: CANoeFDX
Interfaces: -
-------------------------------------------------------------------------------
Fast Data eXchange (FDX)
Protocol definition for connecting a HIL system over UPD to CANoe
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/
using System;
using System.IO;
using System.Linq;


namespace FDXClient
{
  // ==========================================================================
  // Basic Helper for Information of CANoe FDX Protocol
  // ==========================================================================
  public static class FDXHelper
  {
    public const byte kFDXMajorVersion = 1;

    public const byte kFDXMinorVersion = 2;

    public const ulong kFDXSignature = 0x584446656F4E4143;
  }

  // ==========================================================================
  // Helper for Sequence numbers
  // ==========================================================================
  public static class FDXHelperSequenceNumber
  {
    public const ushort kSequenceNumberUnused = 0x8000;

    public const ushort kSequenceNumberStart = 0x000;

    public const ushort kSequenceNumberSessionEndFlag = 0x8000;
  }

  // ==========================================================================
  // Helper for Command Codes in the command header
  // ==========================================================================
  public static class FDXHelperCommandCode
  {
    public const ushort kCommandCodeStart = 0x0001;

    public const ushort kCommandCodeStop = 0x0002;

    public const ushort kCommandCodeKey = 0x0003;

    public const ushort kCommandCodeStatus = 0x0004;

    public const ushort kCommandCodeDataExchange = 0x0005;

    public const ushort kCommandCodeDataRequest = 0x0006;

    public const ushort kCommandCodeDataError = 0x0007;

    public const ushort kCommandCodeFreeRunningReuqest = 0x0008;

    public const ushort kCommandCodeFreeRunningCancel = 0x0009;

    public const ushort kCommandCodeStatusRequest = 0x000A;

    public const ushort kCommandCodeSequenceNumberError = 0x000B;

    public const ushort kCommandCodeHardwareChanged = 0x0010;
  }

  // ==========================================================================
  // Helper for Measurement states and stores the actual state
  // ==========================================================================
  public static class FDXHelperMeasurementState
  {
    public const byte kMeasurementStateNotRunning = 1;

    public const byte kMeasurementStatePreStart = 2;

    public const byte kMeasurementStateRunning = 3;

    public const byte kMeasurementStateStop = 4;

    public static byte CurrentMeasurementState { set; get; }

    static FDXHelperMeasurementState()
    {
      CurrentMeasurementState = kMeasurementStateNotRunning;
    }
  }

  // ==========================================================================
  // Helper for FfreeRunningFlags in the FreeRunningRequestCommand
  // ==========================================================================
  public static class FDXHelperFreeRunningFlags
  {
    public const ushort kFreeRunningFlagTransmitAtPreStart = 1;

    public const ushort kFreeRunningFlagTransmitAtStop = 2;

    public const ushort kFreeRunningFlagTransmitCyclic = 4;

    public const ushort kFreeRunningFlagTransmitAtTrigger = 5;
  }

  // ==========================================================================
  // DatagramHeader
  // Each datagram transmitted by the UPD protocol start with a header. The
  // datagram header contains a signature (magic cookie) and the version of 
  // the CANoe FDX protocol.
  // The header is followed by one ore more command structures. The field 
  // numberOfCommands specifies, how many command structures are following.
  // The datagram header is equipped with a sequence number, that enable CANoe
  // and his counterpart to detect lost datagrams.
  // ==========================================================================
  [Serializable]
  public class DatagramHeader
  {
    public DatagramHeader()
    {
      this.Reserved = 0;
    }
    // FDX signature, that is used as a magic cookie.
    // This field must contain the value kFDXSignature 
    // otherwise the datagram is ignored.
    public ulong FDXSignature { get; set; }

    // major version of CANoe FDX protocol
    public byte FDXMajorVersion { get; set; }

    // minor version of CANoe FDX protocol
    public byte FDXMinorVersion { get; set; }

    // number of commands, that are contained in the datagram
    public ushort NumberOfCommands { get; set; }

    // datagram sequence number of FDX session
    public ushort SequenceNumber { get; set; }

    //returns the Size of the header
    public static int Size() { return 16; }

    // 2 unused bytes for a better alignment, may be used in
    // a future version of the protocol.
    // This field should be initialized to zero.
    public ushort Reserved { get; set; }

    /// <summary>
    /// This function serialize the class to a byte array
    /// </summary>
    /// <returns>byte array with the serialized data</returns>
    public byte[] Serialize()
    {
      using (var m = new MemoryStream())
      {
        using (var writer = new BinaryWriter(m))
        {
          writer.Write(this.FDXSignature);
          writer.Write(this.FDXMajorVersion);
          writer.Write(this.FDXMinorVersion);
          writer.Write(this.NumberOfCommands);
          writer.Write(this.SequenceNumber);
          writer.Write(this.Reserved);
        }
        return m.ToArray();
      }
    }

    /// <summary>
    /// This function deserialize from a byte array and return a new class filled
    /// with the data
    /// </summary>
    /// <param name="data">serialized data that contains the information</param>
    /// <returns>a new class filled with the deserialized data</returns>
    public static DatagramHeader DeSerialize(byte[] data)
    {
      var header = new DatagramHeader();
      using (var m = new MemoryStream(data))
      {
        using (var reader = new BinaryReader(m))
        {
          header.FDXSignature = reader.ReadUInt64();
          header.FDXMajorVersion = reader.ReadByte();
          header.FDXMinorVersion = reader.ReadByte();
          header.NumberOfCommands = reader.ReadUInt16();
          header.SequenceNumber = reader.ReadUInt16();
        }
      }
      return header;
    }
  }

  // ==========================================================================
  // CommandHeader
  // Each command structure starts with a command header. The header contains 
  // the size of the complete command structure and the command code. 
  // ==========================================================================
  [Serializable]
  public class CommandHeader
  {
    // size of this command in bytes
    public ushort CommandSize { get; set; }

    // the kind of command
    public ushort CommandCode { get; set; }

    /// <summary>
    /// This function serialize the class to a byte array
    /// </summary>
    /// <returns>byte array with the serialized data</returns>
    public virtual byte[] Serialize()
    {
      using (var m = new MemoryStream())
      {
        using (var writer = new BinaryWriter(m))
        {
          writer.Write(this.CommandSize);
          writer.Write(this.CommandCode);
        }
        return m.ToArray();
      }
    }

    /// <summary>
    /// This function deserialize from a byte array and return a new class filled
    /// with the data
    /// </summary>
    /// <param name="data">serialized data that contains the information</param>
    /// <returns>a new class filled with the deserialized data</returns>
    public static CommandHeader DeSerialize(byte[] data)
    {
      var header = new CommandHeader();
      using (var m = new MemoryStream(data))
      {
        using (var reader = new BinaryReader(m))
        {
          header.CommandSize = reader.ReadUInt16();
          header.CommandCode = reader.ReadUInt16();
        }
      }
      return header;
    }
  }

  // ==========================================================================
  // KeyCommand
  // command code: kCommandCode_Key
  // Transmit a key stroke to the runtime kernel. This can invoke a 'on key' 
  // handler in CAPL.
  // ==========================================================================
  [Serializable]
  public class KeyCommand : CommandHeader
  {
    // key code as used CAPL programs of CANoe
    // use code 0 for KEYUP events
    public uint CanoeKeyCode { get; set; }

    /// <summary>
    /// This function serialize the class to a byte array
    /// </summary>
    /// <returns>byte array with the serialized data</returns>
    public override byte[] Serialize()
    {
      using (var m = new MemoryStream())
      {
        using (var writer = new BinaryWriter(m))
        {
          writer.Write(this.CanoeKeyCode);
        }
        return base.Serialize().Concat(m.ToArray()).ToArray();
      }
    }
  }

  // ==========================================================================
  // StatusCommand
  // command code: kCommandCode_Status
  // This command stores status information from the CANoe-system
  // ==========================================================================
  [Serializable]
  public class StatusCommand : CommandHeader
  {
    // state of measurement (NotRunning, PreStart,
    // Running, Stop)
    public byte MeasurementState { get; private set; }

    public byte Reserved1 { get; private set; }

    public byte Reserved2 { get; private set; }

    public byte Reserved3 { get; private set; }

    // current time of measurement in nanoseconds
    public long Timestamp { get; private set; }

    /// <summary>
    /// This function deserialize from a byte array and return a new class filled
    /// with the data
    /// </summary>
    /// <param name="data">serialized data that contains the information</param>
    /// <returns>a new class filled with the deserialized data</returns>
    public new static StatusCommand DeSerialize(byte[] data)
    {
      var header = new StatusCommand();
      using (var m = new MemoryStream(data))
      {
        using (var reader = new BinaryReader(m))
        {
          header.CommandSize = reader.ReadUInt16();
          header.CommandCode = reader.ReadUInt16();
          header.MeasurementState = reader.ReadByte();
          header.Reserved1 = reader.ReadByte();
          header.Reserved2 = reader.ReadByte();
          header.Reserved3 = reader.ReadByte();
          header.Timestamp = reader.ReadInt64();
        }
      }
      return header;
    }
  }

  // ==========================================================================
  // DataExchangeCommand
  // command code: kCommandCode_DataExchange
  // This commands is used for exchanging data between CANoe and the FDX-Client
  // ==========================================================================
  [Serializable]
  public class DataExchangeCommand : CommandHeader
  {
    // ID of the data group
    public ushort GroupId { get; set; }

    // size of the following array
    public ushort DataSize { get; set; }

    // the data bytes
    public byte[] DataBytes { get; set; }

    /// <summary>
    /// This function serialize the class to a byte array
    /// </summary>
    /// <returns>byte array with the serialized data</returns>
    public override byte[] Serialize()
    {
      using (var m = new MemoryStream())
      {
        using (var writer = new BinaryWriter(m))
        {
          writer.Write(this.GroupId);
          writer.Write(this.DataSize);
          writer.Write(this.DataBytes);
        }
        return base.Serialize().Concat(m.ToArray()).ToArray();
      }
    }

    /// <summary>
    /// This function deserialize from a byte array and return a new class filled
    /// with the data
    /// </summary>
    /// <param name="data">serialized data that contains the information</param>
    /// <returns>a new class filled with the deserialized data</returns>
    public new static DataExchangeCommand DeSerialize(byte[] data)
    {
      var header = new DataExchangeCommand();
      using (var m = new MemoryStream(data))
      {
        using (var reader = new BinaryReader(m))
        {
          header.CommandSize = reader.ReadUInt16();
          header.CommandCode = reader.ReadUInt16();
          header.GroupId = reader.ReadUInt16();
          header.DataSize = reader.ReadUInt16();
          header.DataBytes = reader.ReadBytes(header.DataSize);
        }
      }
      return header;
    }
  }

  // ==========================================================================
  // DataRequestCommand
  // command code: kCommandCode_DataRequest
  // ==========================================================================
  [Serializable]
  public class DataRequestCommand : CommandHeader
  {
    // ID of the requested data group
    public ushort GroupId { get; set; }

    /// <summary>
    /// This function serialize the class to a byte array
    /// </summary>
    /// <returns>byte array with the serialized data</returns>
    public override byte[] Serialize()
    {      
      using (var m = new MemoryStream())
      {
        using (var writer = new BinaryWriter(m))
        {
          writer.Write(this.GroupId);
        }
        return base.Serialize().Concat(m.ToArray()).ToArray();
      }
    }
  }

  // ==========================================================================
  // DataErrorCommand
  // command code: kCommandCode_DataError
  // ==========================================================================
  [Serializable]
  public class DataErrorCommand : CommandHeader
  {
    // ID of the data group, that was requested
    public ushort GroupId { get; private set; }

    // The reason, why CANoe cannot process the 
    // data request command
    public ushort DataErrorCode { get; private set; }

    /// <summary>
    /// This function deserialize from a byte array and return a new class filled
    /// with the data
    /// </summary>
    /// <param name="data">serialized data that contains the information</param>
    /// <returns>a new class filled with the deserialized data</returns>
    public new static DataErrorCommand DeSerialize(byte[] data)
    {
      var com = new DataErrorCommand();
      using (var m = new MemoryStream(data))
      {
        using (var reader = new BinaryReader(m))
        {
          com.CommandSize = reader.ReadUInt16();
          com.CommandCode = reader.ReadUInt16();
          com.GroupId = reader.ReadUInt16();
          com.DataErrorCode = reader.ReadUInt16();
        }
      }
      return com;
    }
  }

  // ==========================================================================
  // FreeRunningRequestCommand
  // command code: kCommandCode_FreeRunningRequest
  // ==========================================================================
  [Serializable]
  public class FreeRunningRequestCommand : CommandHeader
  {
    //  ID of the data group, that is requested
    public ushort GroupId { get; set; }

    // Describe the transmit behavior of the data group
    public ushort Flags { get; set; }

    // Time period in nanoseconds
    public uint CycleTime { get; set; }

    // Time period in nanoseconds for the first transmit cycle
    public uint FirstDuration { get; set; }

    /// <summary>
    /// This function serialize the class to a byte array
    /// </summary>
    /// <returns>byte array with the serialized data</returns>
    public override byte[] Serialize()
    {
      using (var m = new MemoryStream())
      {
        using (var writer = new BinaryWriter(m))
        {
          writer.Write(this.GroupId);
          writer.Write(this.Flags);
          writer.Write(this.CycleTime);
          writer.Write(this.FirstDuration);
        }
        return base.Serialize().Concat(m.ToArray()).ToArray();
      }
    }
  }

  // ==========================================================================
  // FreeRunningCancelCommand
  // command code: kCommandCode_FreeRunningCancel
  // ==========================================================================
  [Serializable]
  public class FreeRunningCancelCommand : CommandHeader
  {
    // ID of the data group, for which the free 
    // running mode is canceled
    public ushort GroupId { get; set; }

    /// <summary>
    /// This function serialize the class to a byte array
    /// </summary>
    /// <returns>byte array with the serialized data</returns>
    public override byte[] Serialize()
    {
      using (var m = new MemoryStream())
      {
        using (var writer = new BinaryWriter(m))
        {
          writer.Write(this.GroupId);
        }
        return base.Serialize().Concat(m.ToArray()).ToArray();
      }
    }
  }

  // ==========================================================================
  // SequenceNumberErrorCommand
  // command code: kCommandCode_SequenceNumberError
  // ==========================================================================
  [Serializable]
  public class SequenceNumberErrorCommand : CommandHeader
  {
    public ushort ReceivedSeqNr { get; private set; }

    public ushort ExpectedSeqNr { get; private set; }

    /// <summary>
    /// This function deserialize from a byte array and return a new class filled
    /// with the data
    /// </summary>
    /// <param name="data">serialized data that contains the information</param>
    /// <returns>a new class filled with the deserialized data</returns>
    public new static SequenceNumberErrorCommand DeSerialize(byte[] data)
    {
      var com = new SequenceNumberErrorCommand();
      using (var m = new MemoryStream(data))
      {
        using (var reader = new BinaryReader(m))
        {
          com.CommandSize = reader.ReadUInt16();
          com.CommandCode = reader.ReadUInt16();
          com.ReceivedSeqNr = reader.ReadUInt16();
          com.ExpectedSeqNr = reader.ReadUInt16();
        }
      }
      return com;
    }
  }

  // ==========================================================================
  // HardwareChangedCommand
  // command code: kCommandCode_HardwareChanged
  // ==========================================================================
  [Serializable]
  public class HardwareChangedCommand : CommandHeader
  {
    public uint Reserved { get; private set; }

    /// <summary>
    /// This function deserialize from a byte array and return a new class filled
    /// with the data
    /// </summary>
    /// <param name="data">serialized data that contains the information</param>
    /// <returns>a new class filled with the deserialized data</returns>
    public new static HardwareChangedCommand DeSerialize(byte[] data)
    {
      var com = new HardwareChangedCommand();
      using (var m = new MemoryStream(data))
      {
        using (var reader = new BinaryReader(m))
        {
          com.CommandSize = reader.ReadUInt16();
          com.CommandCode = reader.ReadUInt16();
          com.Reserved = reader.ReadUInt32();
        }
      }
      return com;
    }
  }

}
