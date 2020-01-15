using System;
using Vector.Tools;
using Vector.CANoe.Runtime;

// Import communication model
using DataTypes;
using NetworkDB.Services;
using NetworkDB.Participants;


public class Start_Values : MeasurementScript
{

    public override void Start()
    {
        // Set default values for backend navigation method
        Navigation.Backend.Get_Navigation_Info.ParamDefaults.navigation_info.Destination = "Weilimdorf";
        Navigation.Backend.Get_Navigation_Info.ParamDefaults.navigation_info.Distance.ImplValue = 10;
        Navigation.Backend.Get_Navigation_Info.ParamDefaults.navigation_info.POIs.Length = 3;

        Navigation.Backend.Get_Navigation_Info.ParamDefaults.navigation_info.POIs[0].Category.SymbValue = Category_Enum.WARN;
        Navigation.Backend.Get_Navigation_Info.ParamDefaults.navigation_info.POIs[0].Distance.ImplValue = 10;
        Navigation.Backend.Get_Navigation_Info.ParamDefaults.navigation_info.POIs[0].Message = "Roadworks";

        Navigation.Backend.Get_Navigation_Info.ParamDefaults.navigation_info.POIs[1].Category.SymbValue = Category_Enum.INFO;
        Navigation.Backend.Get_Navigation_Info.ParamDefaults.navigation_info.POIs[1].Distance.ImplValue = 20;
        Navigation.Backend.Get_Navigation_Info.ParamDefaults.navigation_info.POIs[1].Message = "Gas Station";

        Navigation.Backend.Get_Navigation_Info.ParamDefaults.navigation_info.POIs[2].Category.SymbValue = Category_Enum.INFO;
        Navigation.Backend.Get_Navigation_Info.ParamDefaults.navigation_info.POIs[2].Distance.ImplValue = 30;
        Navigation.Backend.Get_Navigation_Info.ParamDefaults.navigation_info.POIs[2].Message = "Restaurant";

        // Set defaults for LIDAR providers

        Object_Recognition.LIDAR_Front.Object.Value.Distance.ImplValue = 100;
        Object_Recognition.LIDAR_Front.Object.Value.Classification.SymbValue = Object_Classification_Enum.NO_OBJECT;
        Object_Recognition.LIDAR_Rear.Object.Value.Distance.ImplValue = 100;
        Object_Recognition.LIDAR_Rear.Object.Value.Classification.SymbValue = Object_Classification_Enum.NO_OBJECT;
    }
}