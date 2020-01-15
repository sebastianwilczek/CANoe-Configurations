// Created by Brock, Boris (visbbr), 2015-02-23
// Copyright (c) Vector Informatik GmbH. All rights reserved.

#region Usings

using ASAM.XIL.Interfaces.Testbench.MAPort;

#endregion

namespace XILAPIServer
{
  internal class MAPortFactory : IMAPortFactory
  {
    #region IMAPortFactory Implementation

    /// <summary>
    ///   Creates a new Model Access port.
    /// </summary>
    /// <param name="name">The name of the port instance.</param>
    /// <returns></returns>
    public IMAPort CreateMAPort(string name)
    {
      var port = new MAPort {Name = name};
      return port;
    }

    #endregion
  }
}