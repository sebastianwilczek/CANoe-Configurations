/*-----------------------------------------------------------------------------
Module: CANoeFDX
Interfaces: -
-------------------------------------------------------------------------------
Fast Data eXchange (FDX)
data definition for connecting a HIL system over UPD to CANoe
Implementation of dedicated dispatcher to match data definition
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/
using System;

namespace FDXClient
{
  /// <summary>
  /// This class implements an own A429DataDispatcher to overwrite the data handler for status and data exchange
  /// </summary>
  public class A429DataDispatcher : FDXDispatcher
  {
    private uint _lastGrossWeight;

    /// <summary>
    /// This function overrides the empty data handler in the base class
    /// </summary>
    /// <param name="exData">Received exchange data</param>
    public override void OnDataExchange(DataExchangeCommand exData)
    {
      switch (exData.GroupId)
      {
        case ExchangeData.A429InputData.sGroupId:
          {
            var dt = ExchangeData.A429InputData.DeSerialize(exData.DataBytes);
            Console.Out.WriteLine("SigFuel_Temp_And_Advisory_Warning: {0} Probe_Capacitance: {1}", dt.FuelTempAndAdvisoryWarning, dt.ProbeCapacitance);
            break;
          }

        case ExchangeData.A429OutputData.sGroupId:
          {
            var dt = ExchangeData.A429OutputData.DeSerialize(exData.DataBytes);
            if (this._lastGrossWeight != dt.GrossWeight)
            {
              Console.Out.WriteLine("Gross_Weight: {0}", dt.GrossWeight);
              this._lastGrossWeight = dt.GrossWeight;
            }

            break;
          }

        default:
          {
            Console.Out.WriteLine("Invalid GroupId received: {0}", exData.GroupId);
            break;
          }
      }
    }

    /// <summary>
    /// This function overrides the status handler of the base class to set the internal communication status.
    /// </summary>
    /// <param name="comStat"></param>
    public override void OnStatus(StatusCommand comStat)
    {
      if (FDXHelperMeasurementState.CurrentMeasurementState != comStat.MeasurementState)
      {
        base.OnStatus(comStat);
        FDXHelperMeasurementState.CurrentMeasurementState = comStat.MeasurementState;
      }
    }
  }

  namespace ExchangeData
  {
    [Serializable]
    public class A429InputData
    {
      public const ushort sGroupId = 1;

      public short FuelTempAndAdvisoryWarning { get; set; }

      public float ProbeCapacitance { get; set; }

      public byte Reserved1 { get; private set; }

      /// <summary>
      /// This function serializes the class to a byte array.
      /// </summary>
      /// <returns>byte array with the serialized data</returns>
      public byte[] Serialize()
      {
        using (var m = new System.IO.MemoryStream())
        {
          using (var writer = new System.IO.BinaryWriter(m))
          {
            writer.Write(this.FuelTempAndAdvisoryWarning);
            writer.Write(this.ProbeCapacitance);
            writer.Write(this.Reserved1);
          }

          return m.ToArray();
        }
      }

      /// <summary>
      /// This function deserializes from a byte array and returns a new class filled
      /// with the data.
      /// </summary>
      /// <param name="data">serialized data that contains the information</param>
      /// <returns>a new class filled with the deserialized data</returns>
      public static A429InputData DeSerialize(byte[] data)
      {
        var a429InputData = new A429InputData();
        using (var m = new System.IO.MemoryStream(data))
        {
          using (var reader = new System.IO.BinaryReader(m))
          {
            a429InputData.FuelTempAndAdvisoryWarning = reader.ReadInt16();
            a429InputData.ProbeCapacitance = reader.ReadByte();
            a429InputData.Reserved1 = reader.ReadByte();
          }

        }

        return a429InputData;
      }

    }

    [Serializable]
    public class A429OutputData
    {
      public const ushort sGroupId = 2;

      public uint GrossWeight { get; set; }

      public byte Reserved1 { get; private set; }

      /// <summary>
      /// This function serializes the class to a byte array.
      /// </summary>
      /// <returns>byte array with the serialized data</returns>
      public byte[] Serialize()
      {
        using (var m = new System.IO.MemoryStream())
        {
          using (var writer = new System.IO.BinaryWriter(m))
          {
            writer.Write(this.GrossWeight);
            writer.Write(this.Reserved1);
          }
          return m.ToArray();
        }
      }

      /// <summary>
      /// This function deserializes from a byte array and returns a new class filled
      /// with the data.
      /// </summary>
      /// <param name="data">serialized data that contains the information</param>
      /// <returns>a new class filled with the deserialized data</returns>
      public static A429OutputData DeSerialize(byte[] data)
      {
        var a429OutputData = new A429OutputData();
        using (var m = new System.IO.MemoryStream(data))
        {
          using (var reader = new System.IO.BinaryReader(m))
          {
            a429OutputData.GrossWeight = reader.ReadUInt32();
            a429OutputData.Reserved1 = reader.ReadByte();
          }
        }
        return a429OutputData;
      }
    }


    [Serializable]
    public class A429OvhdData
    {
      public const byte sGroupId = 3;

      // In case of SysVar-Arrays we need 4 Bytes padding at the beginning...
      public uint Reserved1 { get; private set; }
      // ...and and 2 Bytes at the end.
      public ushort Reserved2 { get; private set; }

      /// <summary>
      /// Getter and Setter for SysMainEng1 (first bit in System variable-array)
      /// </summary>
      public byte SysMainEng1
      {
        get
        {
          return ((this.mInternalData[0] & 0x01) > 0) ? (byte)1 : (byte)0;
        }
        set
        {
          if (value == 1)
            this.mInternalData[0] = (byte)(this.mInternalData[0] | 0x01);
          else
          {
            this.mInternalData[0] = (byte)(this.mInternalData[0] & 0xFE);
          }
        }
      }

      /// <summary>
      /// Getter and Setter for SysMainEng2 (second bit in System variable-array)
      /// </summary>
      public byte SysMainEng2
      {
        get
        {
          return ((this.mInternalData[0] & 0x02) > 0) ? (byte)1 : (byte)0;
        }
        set
        {
          if (value == 1)
            this.mInternalData[0] = (byte)(this.mInternalData[0] | 0x02);
          else
          {
            this.mInternalData[0] = (byte)(this.mInternalData[0] & 0xFD);
          }
        }
      }

      /// <summary>
      /// Getter and Setter for SysMainEng3 (third bit in System variable-array)
      /// </summary>
      public byte SysMainEng3
      {
        get
        {
          return ((this.mInternalData[0] & 0x04) > 0) ? (byte)1 : (byte)0;
        }
        set
        {
          if (value == 1)
            this.mInternalData[0] = (byte)(this.mInternalData[0] | 0x04);
          else
          {
            this.mInternalData[0] = (byte)(this.mInternalData[0] & 0xFB);
          }
        }
      }

      /// <summary>
      /// Getter and Setter for SysMainEng4 (fourth bit in System variable-array)
      /// </summary>
      public byte SysMainEng4
      {
        get
        {
          return ((this.mInternalData[0] & 0x08) > 0) ? (byte)1 : (byte)0;
        }
        set
        {
          if (value == 1)
            this.mInternalData[0] = (byte)(this.mInternalData[0] | 0x08);
          else
          {
            this.mInternalData[0] = (byte)(this.mInternalData[0] & 0xF7);
          }
        }
      }

      /// <summary>
      /// Getter and Setter for SysTTankL (seventh bit in System variable-array)
      /// </summary>
      public byte SysTTankL
      {
        get
        {
          return ((this.mInternalData[0] & 0x40) > 0) ? (byte)1 : (byte)0;
        }
        set
        {
          if (value == 1)
            this.mInternalData[0] = (byte)(this.mInternalData[0] | 0x40);
          else
          {
            this.mInternalData[0] = (byte)(this.mInternalData[0] & 0xBF);
          }
        }
      }

      /// <summary>
      /// Getter and Setter for SysTTankR (eighth bit in System variable-array)
      /// </summary>
      public byte SysTTankR
      {
        get
        {
          return ((this.mInternalData[0] & 0x80) > 0) ? (byte)1 : (byte)0;
        }
        set
        {
          if (value == 1)
          {
            this.mInternalData[0] = (byte)(this.mInternalData[0] | 0x80);
          }
          else
          {
            this.mInternalData[0] = (byte)(this.mInternalData[0] & 0x7F);
          }
        }
      }

      /// <summary>
      /// This function serializes the class to a byte array.
      /// </summary>
      /// <returns>byte array with the serialized data</returns>
      public byte[] Serialize()
      {
        using (var m = new System.IO.MemoryStream())
        {
          using (var writer = new System.IO.BinaryWriter(m))
          {
            writer.Write(this.Reserved1);
            writer.Write(this.mInternalData.Length);
            writer.Write(this.mInternalData);
            writer.Write(this.Reserved2);
          }
          return m.ToArray();
        }
      }

      /// <summary>
      /// This function deserializes from a byte array and returns a new class filled
      /// with the data.
      /// </summary>
      /// <param name="data">byte array to be deserialized</param>
      /// <returns>a A429OvhData instance containing the deserialized data</returns>
      public static A429OvhdData DeSerialize(byte[] data)
      {
        var ovhdData = new A429OvhdData();

        using (var m = new System.IO.MemoryStream(data))
        {
          using (var reader = new System.IO.BinaryReader(m))
          {
            ovhdData.Reserved1 = reader.ReadUInt32();
            ovhdData.mInternalData[0] = reader.ReadByte();
            ovhdData.mInternalData[1] = reader.ReadByte();
            ovhdData.Reserved2 = reader.ReadByte();
          }
        }
        return ovhdData;
      }

      #region Variables
      private readonly byte[] mInternalData = new byte[2];
      #endregion
    }
  }
}
