// SystemVariables.c : Example of a CANalyzer/CANoe C Library
//
// This sample file demonstrates the usage of system variables in a 
// CANalyzer/CANoe C Library.
//
// The CANalyzer/CANoe configuration contains 6 system variables:
//   X::a of type integer
//   X::b of type float
//   X::c of type string
//   X::d of type integer array with 5 elements
//   X::e of type float array with 5 elements
//   X::f of type byte array with 5 elements
//

#include "CCL/CCL.h"


extern void OnMeasurementPreStart();
extern void OnMeasurementStart();
extern void OnMeasurementStop();
extern void OnSysVar_a(long long time, int sysVarID);
extern void ReadSystemVariables();
extern void WriteSystemVariables();


int gSysVarID_a;
int gSysVarID_b;
int gSysVarID_c;
int gSysVarID_d;
int gSysVarID_e;
int gSysVarID_f;
int gSysVarID_g;


void cclOnDllLoad()
{
  cclSetMeasurementPreStartHandler(&OnMeasurementPreStart);
  cclSetMeasurementStartHandler(&OnMeasurementStart);
}


void OnMeasurementPreStart()
{
  gSysVarID_a = cclSysVarGetID("X::a");
  gSysVarID_b = cclSysVarGetID("X::b");
  gSysVarID_c = cclSysVarGetID("X::c");
  gSysVarID_d = cclSysVarGetID("X::d");
  gSysVarID_e = cclSysVarGetID("X::e");
  gSysVarID_f = cclSysVarGetID("X::f");
  gSysVarID_g = cclSysVarGetID("X::g");
  cclSysVarSetHandler(gSysVarID_a, &OnSysVar_a);

  // Type check
  if (cclSysVarGetType(gSysVarID_a) != CCL_SYSVAR_INTEGER)
  {
    cclWrite("C Library Example: System variable X::a is not of type Integer");
  }
  if (cclSysVarGetType(gSysVarID_b) != CCL_SYSVAR_FLOAT)
  {
    cclWrite("C Library Example: System variable X::b is not of type Float");
  }
}


void OnMeasurementStart()
{
  ReadSystemVariables();
  WriteSystemVariables();
}


void ReadSystemVariables()
{
  int rc;
  int a;
  double b;
  char c[100];
  long d[10];
  double e[10];
  unsigned char f[10];
  long long g;
  int size_d, size_e, size_f;

  rc = cclSysVarGetInteger(gSysVarID_a, &a);
  rc = cclSysVarGetFloat(gSysVarID_b, &b);
  rc = cclSysVarGetString(gSysVarID_c, c, 100);
  size_d = cclSysVarGetArraySize(gSysVarID_d);
  rc = cclSysVarGetIntegerArray(gSysVarID_d, d, 10);
  size_e = cclSysVarGetArraySize(gSysVarID_e);
  rc = cclSysVarGetFloatArray(gSysVarID_e, e, 10);
  size_f = cclSysVarGetData(gSysVarID_f, f, 10);
  rc = cclSysVarGetInteger64(gSysVarID_g, &g);
};


void WriteSystemVariables()
{
  int rc;
  long d[5] = { 5, 4, 3, 2, 1 };
  double e[5] = { 5.5, 4.4 ,3.3, 2.2, 1.1 };
  unsigned char f[6] = { 6, 5, 4, 3, 2, 1 };

  rc = cclSysVarSetInteger(gSysVarID_a, 11);
  rc = cclSysVarSetFloat(gSysVarID_b, 12.0);
  rc = cclSysVarSetString(gSysVarID_c, "CANoe / CANalyzer C Library");
  rc = cclSysVarSetIntegerArray(gSysVarID_d, d, 5);
  rc = cclSysVarSetFloatArray(gSysVarID_e, e, 5);
  rc = cclSysVarSetData(gSysVarID_f, f, 6);
  rc = cclSysVarSetInteger64(gSysVarID_g, 13);
}


void OnSysVar_a(long long time, int sysVarID)
{
  cclWrite("C Library Example: OnSysVar_a");
}