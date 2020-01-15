/*-----------------------------------------------------------------------------
Module: CANoeFDX
Interfaces: FDXDispatcher
-------------------------------------------------------------------------------
Datagram dispatcher class for CANoe FDX protocol
-------------------------------------------------------------------------------
Copyright (c) Vector Informatik GmbH. All rights reserved.
-----------------------------------------------------------------------------*/
namespace FDXClient
{
  using System;

  /// <summary>
  /// This class implements a Dispatcher for incoming datagrams and implements
  /// basic handlers for standard behavior.
  /// </summary>
  /// <remarks>
  /// To handle exchange data you need to inherit from FDXDispatcher and implement 
  /// your own data handler.
  /// </remarks>
  public class FDXDispatcher
  {
    public FDXDispatcher()
    {
      this.mNextExpectedSequenceNumber = FDXHelperSequenceNumber.kSequenceNumberStart;
    }

    public void DispatchDatagram(ref FDXDatagram datagram)
    {
      var lCopy = datagram.Buffer;

      //if the received bytes are too short, skip
      if (lCopy.Length < 16)
      {
        this.OnFormatError("Datagram too short");
        return;
      }

      // extract datagram header information
      var header = DatagramHeader.DeSerialize(lCopy);

      //if the signature doesn't match, skip
      if (header.FDXSignature != FDXHelper.kFDXSignature)
      {
        this.OnFormatError("Signature mismatch");
        return;
      }

      //if the major version doesn't match, skip
      if (header.FDXMajorVersion != FDXHelper.kFDXMajorVersion)
      {
        this.OnFormatError("Incorrect Major version");
        return;
      }

      //if the minor version doesn't match, skip
      if (header.FDXMinorVersion != FDXHelper.kFDXMinorVersion)
      {
        this.OnFormatError("Incorrect Minor Version");
        return;
      }

      //checking the sequence number
      if (header.SequenceNumber == FDXHelperSequenceNumber.kSequenceNumberUnused)
      {
        //we don't use sequence numbering
      }
      else
      {
        // check if the received sequence number is the expected
        if ( (header.SequenceNumber & 0x7FFF) != this.mNextExpectedSequenceNumber)
        {
          this.OnSequenceError(this.mNextExpectedSequenceNumber, header.SequenceNumber);
        }
        // if the session end sequence number arrives, reset the expected sequence number
        else if ((header.SequenceNumber & FDXHelperSequenceNumber.kSequenceNumberSessionEndFlag) != 0)
        {
          this.mNextExpectedSequenceNumber = FDXHelperSequenceNumber.kSequenceNumberStart;
        }
        else
        {
          // if expected sequence number matches the received sequence number
          this.mNextExpectedSequenceNumber = FDXDatagram.IncrementSequenceNumber(header.SequenceNumber);
        }
      }

      //extract the number of commands and set the position
      var numOfCommands = header.NumberOfCommands;
      var commandPosAndLength = DatagramHeader.Size();

      //create a buffer for the single commands
      var data = new byte[lCopy.Length - commandPosAndLength];

      for (var i = 0; i < numOfCommands; ++i)
      {
        //copy command number i into the buffer
        Array.Copy(lCopy, commandPosAndLength, data, 0, lCopy.Length - commandPosAndLength);

        //extract the header information and copy the data from the buffer into the concrete command
        // buffer
        var cmdHead = CommandHeader.DeSerialize(data);
        var concreteCommand = new byte[cmdHead.CommandSize];
        Array.Copy(data, 0, concreteCommand, 0, concreteCommand.Length);
        commandPosAndLength += concreteCommand.Length;

        // with the command code we can decide what to do with the received command
        switch (cmdHead.CommandCode)
        {
          case FDXHelperCommandCode.kCommandCodeDataExchange:
            {
              var cmd = DataExchangeCommand.DeSerialize(concreteCommand);
              this.OnDataExchange(cmd);
              break;
            }
          case FDXHelperCommandCode.kCommandCodeStatus:
            {
              var cmd = StatusCommand.DeSerialize(concreteCommand);
              this.OnStatus(cmd);
              break;
            }
          case FDXHelperCommandCode.kCommandCodeSequenceNumberError:
            {
              var com = SequenceNumberErrorCommand.DeSerialize(concreteCommand);
              this.OnSequenceError(com.ExpectedSeqNr, com.ReceivedSeqNr);
              break;
            }
        }

      }
    }

    // Data handler for format error, prints out the error message
    public virtual void OnFormatError(string errorString)
    {
      Console.Out.WriteLine("Format Error: {0}", errorString);
    }

    // Data handler for status, prints out the state of the measurement and the state
    public virtual void OnStatus(StatusCommand stData)
    {
      var elapsedTime = new TimeSpan(stData.Timestamp);
      Console.Out.WriteLine("MeasurementState: {0} TimeStamp: {1}", stData.MeasurementState, elapsedTime);
    }
    
    // Data handler for Sequence numbers, prints out the expected and the received sequence number
    public virtual void OnSequenceError(UInt16 expected, UInt16 received)
    {
      Console.Out.WriteLine("Sequence Number Error: Expected Seq Nr: {0}, Received Seq Nr: {1}", expected, received);
    }

    // Data handler for data errors, prints out the groupId and error code
    public virtual void OnDataError(DataErrorCommand error)
    {
      Console.Out.WriteLine("Data Error: GroupId: {0}, Errorcode: {1}", error.GroupId, error.DataErrorCode);
    }

    //Empty data handler for data exchange, can be overwritten by an own FDXDispatcher that only implement this function
    public virtual void OnDataExchange(DataExchangeCommand exData) { }

    #region Variables
    private UInt16 mNextExpectedSequenceNumber;
    #endregion
  }
}
