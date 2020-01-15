using System;
using Vector.Tools;
using Vector.CANoe.Runtime;

// Import communication model
using DataTypes;
using NetworkDB.Services;
using NetworkDB.Participants;


// Implementation of the service interface Mirror
public class Mirror_Provider_Impl : Mirror_Interface_ProviderInterface
{
	public override string AdjustMirror(MirrorParametersStruct parameterStruct)
    {
		//0:unfolded 
		//1:folded
		if(0 == parameterStruct.Mirror_Folded.ImplValue)
			SysVars.MirrorPanel.AdjustAndFold.Fold.Value = 0;
		else
			SysVars.MirrorPanel.AdjustAndFold.Fold.Value = 1;
		
		//0:heat is off
		//1:heat is on
		if(0 == parameterStruct.Heat_OnOff.ImplValue)
			SysVars.MirrorPanel.Heat.Value = 0;
		else
			SysVars.MirrorPanel.Heat.Value = 1;
		
		//save the valid direction in a system variable
		uint newDirectionValue = parameterStruct.Direction.ImplValue;
        if(newDirectionValue < 0 || newDirectionValue >= 9)
		{
			//don't change the SysVars.MirrorPanel.AdjustAndFold.AdjustPosition.Value
			if(1 == SysVars.MirrorPanel.AdjustAndFold.Fold.Value) //mirror is fold
			{
				SysVars.MirrorPanel.MirrorSwitch.Value = 0;
				return "Mirror is folded in";
			}
			else //mirror is unfold - show the error message
				return "Enter a valid value (0-8) for direction";
		}
		else
		{
			SysVars.MirrorPanel.AdjustAndFold.AdjustPosition.Value = newDirectionValue + 1;
			//1: Middle 
			//2: Left 
			//3: BottomLeft 
			//4: TopLeft 
		    //5: Right 
			//6: BottomRight 
			//7: TopRight 
			//8: Top 
			//9: Bottom
			if(1 == SysVars.MirrorPanel.AdjustAndFold.Fold.Value) //mirror is fold
			{
				SysVars.MirrorPanel.MirrorSwitch.Value = 0;
				return "Mirror is folded in";
			}
			else //mirror is unfold - show the correct mirror position
			{
				SysVars.MirrorPanel.MirrorSwitch.Value = SysVars.MirrorPanel.AdjustAndFold.AdjustPosition.Value;
				return "ok";
			}
		}
    }
}

public class Mirror_Model : MeasurementScript
{
    public override void Start()
    {	
		  // Set service implementation for participant Mirror_Driver
       	  Mirror.SelectProvider(Mirror_Driver.Instance);
          Mirror.SetProviderImplementation(new Mirror_Provider_Impl());
		  // Set service implementation for participant Mirror_Passenger
          Mirror.SelectProvider(Mirror_Passenger.Instance);
          Mirror.SetProviderImplementation(new Mirror_Provider_Impl());

    }
}

