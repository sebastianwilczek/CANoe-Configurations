// Created by Brock, Boris (visbbr), 2015-02-23
// Copyright (c) Vector Informatik GmbH. All rights reserved.

#region Usings

using System;
using System.Collections.Generic;
using ASAM.XIL.Implementation.Testbench.Common.ValueContainer;
using ASAM.XIL.Interfaces.Testbench.Common.Capturing;
using ASAM.XIL.Interfaces.Testbench.Common.MetaInfo;
using ASAM.XIL.Interfaces.Testbench.Common.SignalGenerator;
using ASAM.XIL.Interfaces.Testbench.Common.ValueContainer;
using ASAM.XIL.Interfaces.Testbench.Common.ValueContainer.Enum;
using ASAM.XIL.Interfaces.Testbench.MAPort;
using ASAM.XIL.Interfaces.Testbench.MAPort.Enum;

#endregion

namespace XILAPIServer
{
  internal class MAPort : IMAPort
  {
    #region Members

    private const string VarNameSine = "Sine";

    private const string VarNameAmplitude = "Amplitude";

    private const string VarNameFrequency = "Frequency";

    private const string VarNameTest = "TestVariable";

    private double mAmplitude = 100.0;

    private double mFrequency = 1.0;
    private DateTime mLastUpdateTime;

    private double mSineTime;

    private double mTestValue;

    #endregion

    #region Constructors

    public MAPort()
    {
      State = MAPortState.eDISCONNECTED;
    }

    #endregion

    #region IMAPort Implementation

    public IMAPortConfig Configuration { get; private set; }

    public void Configure(IMAPortConfig config, bool forceConfig)
    {
    }

    public DataType GetDataType(string variableName)
    {
      return DataType.eFLOAT;
    }

    public IMAPortVariableInfo GetVariableInfo(string variableName)
    {
      return null;
    }

    public bool IsReadable(string variableName)
    {
      return true;
    }

    public bool IsWritable(string variableName)
    {
      if (variableName == VarNameAmplitude || variableName == VarNameFrequency || variableName == VarNameTest) return true;
      return false;
    }

    public IMAPortConfig LoadConfiguration(string filepath)
    {
      return null;
    }

    public IBaseValue Read(string variableName)
    {
      if (variableName == VarNameSine)
      {
        var curTime = DateTime.Now;
        var delta = (curTime - mLastUpdateTime).TotalMilliseconds*0.001;
        mSineTime += delta;
        mLastUpdateTime = curTime;

        return new FloatValue(Math.Sin(mSineTime*2.0*Math.PI*mFrequency)*mAmplitude);
      }
      if (variableName == VarNameAmplitude)
      {
        return new FloatValue(mAmplitude);
      }
      if (variableName == VarNameFrequency)
      {
        return new FloatValue(mFrequency);
      }
      if (variableName == VarNameTest)
      {
        return new FloatValue(mTestValue);
      }

      return null;
    }

    public void StartSimulation()
    {
      State = MAPortState.eSIMULATION_RUNNING;
    }

    public MAPortState State { get; private set; }

    public void StopSimulation()
    {
      State = MAPortState.eSIMULATION_STOPPED;
    }

    public IList<ITaskInfo> TaskInfos
    {
      get { return null; }
    }

    public IList<string> VariableNames
    {
      get { return new[] {VarNameSine, VarNameAmplitude, VarNameFrequency}; }
    }

    public void Write(string variableName, IBaseValue value)
    {
      if (variableName == VarNameAmplitude)
      {
        mAmplitude = (value as IFloatValue).Value;
      }
      else if (variableName == VarNameFrequency)
      {
        mFrequency = (value as IFloatValue).Value;
      }
      else if (variableName == VarNameTest)
      {
        mTestValue = (value as IFloatValue).Value;
      }
    }

    public void Disconnect()
    {
      State = MAPortState.eDISCONNECTED;
    }

    public void Dispose()
    {
    }

    public string Name { get; set; }

    #endregion

    #region Not Implemented

    public IList<string> TaskNames
    {
      get { throw new NotImplementedException(); }
    }

    public double DAQClock
    {
      get { throw new NotImplementedException(); }
    }

    public ICapture CreateCapture(string taskName)
    {
      throw new NotImplementedException();
    }

    public ISignalGenerator CreateSignalGenerator()
    {
      throw new NotImplementedException();
    }

    #endregion
  }
}