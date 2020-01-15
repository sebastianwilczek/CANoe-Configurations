// Created by Brock, Boris (visbbr), 2015-02-23
// Copyright (c) Vector Informatik GmbH. All rights reserved.

#region Usings

using System;
using System.Windows.Forms;

#endregion

namespace XILAPIClient
{
  internal static class Program
  {
    /// <summary>
    ///   The main entry point for the application.
    /// </summary>
    [STAThread]
    private static void Main()
    {
      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);
      Application.Run(new MainForm());
    }
  }
}