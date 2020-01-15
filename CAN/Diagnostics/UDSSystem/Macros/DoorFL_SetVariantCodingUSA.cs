#region Header
// This VDS Library script has been recorded on 2017/07/07, 11:56:07.
// it was manually modified to unlock the ECU before writing variant coding.
// You might manually add appropriate program logic; e.g. error handling and message parameter access.
#endregion

#region Imports
using System.Collections.Generic;
using System.Text;
using Diag = Vector.Diagnostics;
using Tool = Vector.Tools;
using Scripting = Vector.Scripting;
#endregion

#region Class
public class DoorFL_SetVariantCodingUSA
{
	#region Method
	public void Execute()
	{
		
		Scripting.Execution.Wait(500);
		
		#region Setup
		Diag.Ecu ecu = Diag.Application.GetEcu("DoorFL");
		if (ecu == null)
		{
			Tool.Output.WriteLine("The ECU 'DoorFL' does not exist. Please check the configuration.");
			return;
		}
		#endregion
		
		#region Recording
		// anonymous diagnostic objects
		Diag.Request    request = null;
		Diag.SendResult result  = null;
		Diag.Parameter  param   = null;
		
		Tool.Output.WriteLine("Request: Extended Diagnostic Session Start (STDS_RQ): 0x10 0x03");
		using (request = ecu.CreateRequest("ExtendedDiagnosticSession_Start"))
		{
			  if(null != request)
			  {
			    DumpPdu(request.Pdu);
			    string ecuQualifier = ecu.Qualifier;
			    DumpParameters(request.Parameters);
			    Tool.Output.WriteLine("Sending Request 'ExtendedDiagnosticSession_Start' to '"+ ecuQualifier +"' ...");
			    result = request.Send();
			    DumpResponses(result);
			  }
			  else
			  {
			    // Remove the exception below when the script shouldn't stop its execution
			    throw new Diag.DiagnosticException("Couldn't create request 'ExtendedDiagnosticSession_Start'!");
			  }
		}
		
		Tool.Output.WriteLine("Request: Programming Session Start (STDS_RQ): 0x10 0x02");
		using (request = ecu.CreateRequest("ProgrammingSession_Start"))
		{
			  if(null != request)
			  {
			    DumpPdu(request.Pdu);
			    string ecuQualifier = ecu.Qualifier;
			    DumpParameters(request.Parameters);
			    Tool.Output.WriteLine("Sending Request 'ProgrammingSession_Start' to '"+ ecuQualifier +"' ...");
			    result = request.Send();
			    DumpResponses(result);
			  }
			  else
			  {
			    // Remove the exception below when the script shouldn't stop its execution
			    throw new Diag.DiagnosticException("Couldn't create request 'ProgrammingSession_Start'!");
			  }
		}

        //***************** Start manual modification		
		Tool.Output.WriteLine("Trying to unlock ECU (Level 1)...");
		// Unlock ECU in Level 1:
		// - Request seed with $27 01
		// - Calculate key using seed key dll and response of $27 01
		// - Send key using $27 02 and calculated key
		// - if ECU responds with positive response 
        Diag.SecurityAccessResult securityAccessResult = ecu.Unlock(1);
		// Check result of unlocking the ECU
		if (securityAccessResult == Diag.SecurityAccessResult.Success)
		{
		   Tool.Output.WriteLine("Unlocking ECU was successful.");
		}
		else
		{
		   Tool.Output.WriteLine("Unlocking ECU failed.");
		}
		//***************** End manual modification
		
		Tool.Output.WriteLine("Request: Variant Coding Write (WDBI_RQ): 0x2E 0x00 0xA0");
		using (request = ecu.CreateRequest("Variant_Coding_Write"))
		{
			  if(null != request)
			  {
			    param = request.GetParameter("Codingstring.CountryType");
			    if(param == null)
			    {
			      throw new Diag.DiagnosticException("Couldn't get parameter 'Codingstring.CountryType'!");
			    }
			    if(param.Value == null)
			    {
			      throw new Diag.DiagnosticException("Couldn't get Value object of parameter 'Codingstring.CountryType'!");
			    }
			    param.Value.Set("USA");
			    param = request.GetParameter("Codingstring.VehicleType");
			    if(param == null)
			    {
			      throw new Diag.DiagnosticException("Couldn't get parameter 'Codingstring.VehicleType'!");
			    }
			    if(param.Value == null)
			    {
			      throw new Diag.DiagnosticException("Couldn't get Value object of parameter 'Codingstring.VehicleType'!");
			    }
			    param.Value.Set("Coupe");
			    param = request.GetParameter("Codingstring.WindowLift_Support");
			    if(param == null)
			    {
			      throw new Diag.DiagnosticException("Couldn't get parameter 'Codingstring.WindowLift_Support'!");
			    }
			    if(param.Value == null)
			    {
			      throw new Diag.DiagnosticException("Couldn't get Value object of parameter 'Codingstring.WindowLift_Support'!");
			    }
			    param.Value.Set("on");
			    param = request.GetParameter("Codingstring.VehicleSpeedToLockDoor");
			    if(param == null)
			    {
			      throw new Diag.DiagnosticException("Couldn't get parameter 'Codingstring.VehicleSpeedToLockDoor'!");
			    }
			    if(param.Value == null)
			    {
			      throw new Diag.DiagnosticException("Couldn't get Value object of parameter 'Codingstring.VehicleSpeedToLockDoor'!");
			    }
			    param.Value.Set(20);
			    DumpPdu(request.Pdu);
			    string ecuQualifier = ecu.Qualifier;
			    DumpParameters(request.Parameters);
			    Tool.Output.WriteLine("Sending Request 'Variant_Coding_Write' to '"+ ecuQualifier +"' ...");
			    result = request.Send();
			    DumpResponses(result);
			  }
			  else
			  {
			    // Remove the exception below when the script shouldn't stop its execution
			    throw new Diag.DiagnosticException("Couldn't create request 'Variant_Coding_Write'!");
			  }
		}
		#endregion
	}
	#endregion
	
	#region Helper
	private void DumpResponses(Diag.SendResult result)
	{
		  if (result != null)
		  {
		    Tool.Output.WriteLine("Processing Response ...");
		    if(result.Response != null)
		    {
		      DumpResponse(result.Response);
		    }
		    else
		    {
		      List<Diag.Response> responses = result.Responses;
		      if (responses != null)
		      {
		        foreach (Diag.Response response in responses)
		        {
		          DumpResponse(response);
		        }
		      }
		      else
		      {
		        Tool.Output.WriteLine("INFO: SendResult contains no response.");
		      }
		    }
		    
		    if (result.Status != Diag.SendStatus.Ok)
		    {
		      Tool.Output.WriteLine("ERROR: The send result is not 'OK'!");
		    }
		  }
		  else
		  {
		    Tool.Output.WriteLine("ERROR: The send result is not valid!");
		  }
	}
	
	private void DumpResponse(Diag.Response response)
	{
		  if (response != null)
		  {
		    // The Ecu property requires at least CANoe 9.0 SP3 / CANape 15
		    Diag.Ecu sourceEcu = response.Ecu;
		    if (sourceEcu != null)
		    {
		      Tool.Output.WriteLine("Response from '"+ sourceEcu.Qualifier + "'...");
		    }
		    DumpPdu(response.Pdu);
		    Diag.ParameterCollection parameters = response.Parameters;
		    if (parameters != null && parameters.Count > 0)
		    {
		      DumpParameters(parameters);
		    }
		  }
		  else
		  {
		    Tool.Output.WriteLine("INFO: The Response is null!");
		  }
	}
	
	private void DumpParameters(Diag.ParameterCollection parameters)
	{
		  if(null != parameters)
		  {
		    foreach (Diag.Parameter parameter in parameters)
		    {
		      DumpParameter(parameter);
		    }
		  }
		  else
		  {
		    Tool.Output.WriteLine("ERROR: The ParameterCollection is null!");
		  }
	}
	
	private void DumpParameter(Diag.Parameter parameter)
	{
		  if(null != parameter)
		  {
		    StringBuilder traceMsg = new StringBuilder();
		    string paramName = (parameter.Name != null) ? parameter.Name : "<null>";
		    Diag.Value tempValue = parameter.Value;
		    
		    if(null != tempValue)
		    {
		      try
		      {
		        string valueAsString = tempValue.ToString();
		        traceMsg.AppendFormat("{0}: {1}", paramName, valueAsString);
		      }
		      catch (Diag.DiagnosticException ex)
		      {
		        traceMsg.AppendFormat("ERROR: Couldn't get string representation of {0}: {1}", paramName, ex.Message);
		      }
		    }
		    else
		    {
		      traceMsg.AppendFormat("{0}: <null>", paramName);
		    }
		    
		    Tool.Output.WriteLine(traceMsg.ToString());
		    
		    if (parameter.Parameters.Count > 0)
		    {
		      DumpParameters(parameter.Parameters); // recursion
		    }
		  }
		  else
		  {
		    Tool.Output.WriteLine("ERROR: The current parameter is null!");
		  }
	}
	
	private void DumpPdu(byte[] pdu)
	{
		  StringBuilder pduStr = new StringBuilder();
		  pduStr.Append("Pdu:");
		  if(null != pdu)
		  {
		    for (int i = 0; i < pdu.Length; ++i)
		    {
		      pduStr.Append(" ");
		      pduStr.Append(pdu[i].ToString("x2"));
		    }
		  }
		  else
		  {
		    pduStr.Append(" <null>");
		  }
		  Tool.Output.WriteLine(pduStr.ToString());
	}
	#endregion
}
#endregion
