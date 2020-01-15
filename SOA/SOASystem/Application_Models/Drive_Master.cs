using System;
using Vector.Tools;
using Vector.CANoe.Runtime;


// Import communication model
using DataTypes;
using NetworkDB.Services;
using NetworkDB.Participants;





public class Drive_Master_Model : MeasurementScript
{
    // Update Dashboard with velocity
    [OnChange(typeof(SysVars.Vehicle.Velocity))]
    public void OnVelocity()
    {
        if (Passenger_Information.ConnectionStateConsumer.Value == ConnectionStateConsumer.Available)
        {
            Passenger_Information.Velocity_Info.Set.CallAsync(SysVars.Vehicle.Velocity.Value);
        }
    }

    // Update Dashboard with event data received from LIDAR_Front
    [OnUpdate(typeof(Object_Recognition.consumerSide.LIDAR_Front.Object))]
    public void OnObjectFront()
    {
        Object_Recognition.SelectProvider(LIDAR_Front.Instance);
        Collision_Info collision_info = new Collision_Info();
        if (Object_Recognition.Object.Classification.SymbValue == Object_Classification_Enum.VEHICLE)
        {
            collision_info.Message = "Vehicle: " + Object_Recognition.Object.Distance.ToString() + "m";
            collision_info.Lamp.ImplValue = 1;
        }
        else if (Object_Recognition.Object.Classification.SymbValue == Object_Classification_Enum.OBSTACLE)
        {
            collision_info.Message = "Obstacle: " + Object_Recognition.Object.Distance.ToString() + "m";
            collision_info.Lamp.ImplValue = 1;
        }
        else if (Object_Recognition.Object.Classification.SymbValue == Object_Classification_Enum.PEDESTRIAN)
        {
            collision_info.Message = "Pedestrian: " + Object_Recognition.Object.Distance.ToString() + "m";
            collision_info.Lamp.ImplValue = 1;
        }
        collision_info.Warn_Level.ImplValue = 64 - (uint)(Object_Recognition.Object.Distance * 64.0 / 100);
        collision_info.Distance.ImplValue = Object_Recognition.Object.Distance;
        Passenger_Information.Collision_Info_Front.Set.CallAsync(collision_info);
    }

    // Update Dashboard with event data received from LIDAR_Rear
    [OnUpdate(typeof(Object_Recognition.consumerSide.LIDAR_Rear.Object))]
    public void OnObjectRear()
    {
        Object_Recognition.SelectProvider(LIDAR_Rear.Instance);
        Collision_Info collision_info = new Collision_Info();
        if (Object_Recognition.Object.Classification.SymbValue == Object_Classification_Enum.VEHICLE)
        {
            collision_info.Message = "Vehicle: " + Object_Recognition.Object.Distance.ToString() + "m";
            collision_info.Lamp.ImplValue = 1;
        }
        else if (Object_Recognition.Object.Classification.SymbValue == Object_Classification_Enum.OBSTACLE)
        {
            collision_info.Message = "Obstacle: " + Object_Recognition.Object.Distance.ToString() + "m";
            collision_info.Lamp.ImplValue = 1;
        }
        else if (Object_Recognition.Object.Classification.SymbValue == Object_Classification_Enum.PEDESTRIAN)
        {
            collision_info.Message = "Pedestrian: " + Object_Recognition.Object.Distance.ToString() + "m";
            collision_info.Lamp.ImplValue = 1;
        }
        collision_info.Warn_Level.ImplValue = 64 - (uint)(Object_Recognition.Object.Distance * 64.0 / 100);
        collision_info.Distance.ImplValue = Object_Recognition.Object.Distance;
        Passenger_Information.Collision_Info_Rear.Set.CallAsync(collision_info);
    }

    // Update navigation information initially, when dashboard provider is available
    [OnProviderDiscovered(typeof(Passenger_Information))]
    public void OnDashboardAvailable(IAddressHandle address)
    {
        var t = new Timer(TimeSpan.FromMilliseconds(1), (object obj, ElapsedEventArgs e) =>
                            {
                                Passenger_Information.Collision_Info_Front.Set.CallAsync(new Collision_Info());
                                Passenger_Information.Collision_Info_Rear.Set.CallAsync(new Collision_Info());
                            });
        t.AutoReset = false;
        t.Start();
    }

    // Update navigation information initially, when backend provider is available
    [OnProviderDiscovered(typeof(Navigation))]
    public void OnBackendAvailable(IAddressHandle address)
    {
        var t = new Timer(TimeSpan.FromMilliseconds(1), (object obj, ElapsedEventArgs e) => OnNavigationTimer());
        t.AutoReset = false;
        t.Start();
    }

    // Query navigation data from backend every 1000ms
    [OnTimer(1000)]
    public void OnNavigationTimer()
    {
        Navigation.Get_Navigation_Info.CallAsync("", 0.0, 0.0, result =>
        {

            Navigation_Info navigation_info = new Navigation_Info();
            navigation_info.Destination = result.Destination + ": " + result.Distance.ImplValue.ToString() + "km";
            navigation_info.Distance.ImplValue = result.Distance.ImplValue;
            navigation_info.POIs.Length = result.POIs.Length;

            for (int i = 0; i < navigation_info.POIs.Length; i++)
            {
                navigation_info.POIs[i].Category.ImplValue = result.POIs[i].Category.ImplValue;
                if (result.POIs[i].Category.ImplValue > 0)
                {
                    navigation_info.POIs[i].Message = result.POIs[i].Message + ": " + result.POIs[i].Distance.ImplValue.ToString() + "km";
                }
                else
                {
                    navigation_info.POIs[i].Message = "";
                }
            }
            Passenger_Information.Navigation_Info.Set.CallAsync(navigation_info);
        });
    }
}