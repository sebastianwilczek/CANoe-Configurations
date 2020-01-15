// Created by Brock, Boris (visbbr), 2015-02-23
// Copyright (c) Vector Informatik GmbH. All rights reserved.

#region Usings

using System;
using System.Collections.Generic;
using ASAM.XIL.Interfaces.Testbench;
using ASAM.XIL.Interfaces.Testbench.Common.Capturing;
using ASAM.XIL.Interfaces.Testbench.Common.Signal;
using ASAM.XIL.Interfaces.Testbench.Common.SignalGenerator;
using ASAM.XIL.Interfaces.Testbench.Common.Symbol;
using ASAM.XIL.Interfaces.Testbench.Common.ValueContainer;
using ASAM.XIL.Interfaces.Testbench.Common.WatcherHandling;
using ASAM.XIL.Interfaces.Testbench.DiagPort;
using ASAM.XIL.Interfaces.Testbench.ECUCPort;
using ASAM.XIL.Interfaces.Testbench.ECUMPort;
using ASAM.XIL.Interfaces.Testbench.EESPort;
using ASAM.XIL.Interfaces.Testbench.MAPort;
using ASAM.XIL.Interfaces.Testbench.NetworkPort;

#endregion

namespace XILAPIServer
{
  public class Testbench : ITestbench
  {
    #region Members

    private readonly Lazy<MAPortFactory> mMaPortFactory = new Lazy<MAPortFactory>();

    #endregion

    #region ITestbench Implementation

    public IList<Type> AvailablePortTypes
    {
      get { return new[] {typeof (IMAPort)}; }
    }

    public string BuildNumber
    {
      get { return "1.0.0.0"; }
    }

    public IMAPortFactory MAPortFactory
    {
      get { return mMaPortFactory.Value; }
    }

    public ulong MajorNumber
    {
      get { return 1; }
    }

    public ulong MinorNumber
    {
      get { return 0; }
    }

    public string ProductName
    {
      get { return "Example Product"; }
    }

    public string ProductVersion
    {
      get { return "1.0.0.0"; }
    }

    public ulong RevisionNumber
    {
      get { return 1; }
    }

    public string VendorName
    {
      get { return "Example Vendor"; }
    }

    #endregion

    #region Not Implemented

    public INetworkPortFactory NetworkPortFactory
    {
      get { throw new NotImplementedException(); }
    }

    public ICapturingFactory CapturingFactory
    {
      get { throw new NotImplementedException(); }
    }

    public IDiagPortFactory DiagPortFactory
    {
      get { throw new NotImplementedException(); }
    }

    public IECUCPortFactory ECUCPortFactory
    {
      get { throw new NotImplementedException(); }
    }

    public IECUMPortFactory ECUMPortFactory
    {
      get { throw new NotImplementedException(); }
    }

    public IEESPortFactory EESPortFactory
    {
      get { throw new NotImplementedException(); }
    }

    public ISignalFactory SignalFactory
    {
      get { throw new NotImplementedException(); }
    }

    public ISignalGeneratorFactory SignalGeneratorFactory
    {
      get { throw new NotImplementedException(); }
    }

    public ISymbolFactory SymbolFactory
    {
      get { throw new NotImplementedException(); }
    }

    public IValueFactory ValueFactory
    {
      get { throw new NotImplementedException(); }
    }

    public IWatcherFactory WatcherFactory
    {
      get { throw new NotImplementedException(); }
    }

    #endregion
  }
}