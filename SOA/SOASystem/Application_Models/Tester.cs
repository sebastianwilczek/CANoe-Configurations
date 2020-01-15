using System;
using Vector.CANoe.Runtime;
using Vector.CANoe.Threading;
using Vector.CANoe.TFS;

// Import communication model
using DataTypes;
using NetworkDB.Services;
using NetworkDB.Participants;

public class Tester : TestModule
{
    uint[] object_distance_test_values = {100U, 90U, 80U, 70U, 60U, 50U, 40U, 30U, 20U, 10U, 0U, 
                                            0U, 10U, 20U, 30U, 40U, 50U, 60U, 70U, 80U, 90U, 100U};
 
    public override void Main()
    {
        Passenger_Information.SelectSide(ServiceSide.Provider);
        Object_Recognition.SelectSide(ServiceSide.Provider);
        Navigation.SelectSide(ServiceSide.Provider);
        //Test cases to be performed
        Test_Collision_Info_Front();
        Test_Collision_Info_Rear();
        Test_Navigation_Data();   
    }

    // Check propagation of detected objects from LIDAR_Front endpoint to Dashboard endpoint 
    [TestCase("Test_Collision_Info_Front")]
    public void Test_Collision_Info_Front()
    {
      Report.TestCaseDescription("Check propagation of detected objects from LIDAR_Front endpoint to Dashboard endpoint");
      Object_Recognition.SelectProvider(LIDAR_Front.Instance);

        // Iterate classification enum values
        foreach (Object_Classification_Enum classification in Enum.GetValues(typeof(Object_Classification_Enum)))
        {
            foreach (uint distance in object_distance_test_values)
            {
                Execution.Wait(200);
                Report.TestStep("Propagate detected object", "Object: " + classification.ToString() + ", Distance: " + distance.ToString());
                Object_Recognition.Object.Classification.SymbValue = classification;
                Object_Recognition.Object.Distance = distance;
                // Wait for setter call from Drive_Master to Dashboard
                var wait_result = Passenger_Information.Collision_Info_Front.Set.WaitForNextCall(100);
                // Check for expected value (further checks omitted in this example)
                if (wait_result.Call.newValue.Distance.ImplValue != Object_Recognition.Object.Distance)
                {
                  Report.TestStepFail("Check distance", "Unexpected value received: " + wait_result.Call.newValue.Distance.ImplValue.ToString());
                }
                else
                {
                  Report.TestStepPass("Check distance", "Expected value received: " + wait_result.Call.newValue.Distance.ImplValue.ToString());
                }
            }
        }
        Object_Recognition.Object.Classification.SymbValue = Object_Classification_Enum.NO_OBJECT;
    }

    // Check propagation of detected objects from LIDAR_Rear endpoint to Dashboard endpoint 
    [TestCase("Test_Collision_Info_Rear")]
    public void Test_Collision_Info_Rear()
    {
        Report.TestCaseDescription("Check propagation of detected objects from LIDAR_Rear endpoint to Dashboard endpoint");
        Object_Recognition.SelectProvider(LIDAR_Rear.Instance);

        // Iterate classification enum values
        foreach (Object_Classification_Enum classification in Enum.GetValues(typeof(Object_Classification_Enum)))
        {
            foreach (uint distance in object_distance_test_values)
            {
                Execution.Wait(200);
                Report.TestStep("Propagate detected object", "Object: " + classification.ToString() + ", Distance: " + distance.ToString());
                Object_Recognition.Object.Classification.SymbValue = classification;
                Object_Recognition.Object.Distance = distance;
                // Wait for setter call from Drive_Master to Dashboard
                var wait_result = Passenger_Information.Collision_Info_Rear.Set.WaitForNextCall(100);
                // Check for expected value (further checks omitted in this example)
                if (wait_result.Call.newValue.Distance.ImplValue != Object_Recognition.Object.Distance)
                {
                    Report.TestStepFail("Check distance", "Unexpected value received: " + wait_result.Call.newValue.Distance.ImplValue.ToString());
                }
                else
                {
                  Report.TestStepPass("Check distance", "Expected value received: " + wait_result.Call.newValue.Distance.ImplValue.ToString());
                }
            }
        }
        Object_Recognition.Object.Classification.SymbValue = Object_Classification_Enum.NO_OBJECT;
    }

    // Check propagation of navigation data from Backend endpoint to Dashboard endpoint 
    [TestCase("Test_Navigation_Data")]
    public void Test_Navigation_Data()
    {
        Report.TestCaseDescription("Check propagation of navigation data from Backend endpoint to Dashboard endpoint");


        Tuple<string, uint>[] destinations = {   Tuple.Create("Weilimdorf", 10U), 
                                                 Tuple.Create("Muenchen", 200U), 
                                                 Tuple.Create("Dortmund", 400U), 
                                                 Tuple.Create("Hannover", 500U), 
                                                 Tuple.Create("Berlin", 700U)      };

        Tuple<string, uint>[] pois =  {  Tuple.Create("Roadworks", 1U),
                                         Tuple.Create("Restaurant", 2U),
                                         Tuple.Create("Traffic Jam", 1U),
                                         Tuple.Create("Gas Station", 2U),
                                         Tuple.Create("Detour", 1U)         };


        foreach (var destination in destinations)
        {
            Navigation_Info navigation_info = new Navigation_Info();  
            navigation_info.Destination = destination.Item1;
            navigation_info.Distance.ImplValue = destination.Item2;
            navigation_info.POIs.Length = 3;
            Random random = new Random();
            
            for (int j = 0; j < navigation_info.POIs.Length; j++)
            {
                uint poi_index = (uint)random.Next(pois.Length);
                navigation_info.POIs[j].Distance.ImplValue = ((uint)j*10U) + poi_index;
                navigation_info.POIs[j].Message = pois[poi_index].Item1;
                navigation_info.POIs[j].Category.ImplValue = pois[poi_index].Item2;
            }

            Report.TestStep("Propagate navigation info", "Destination: " + navigation_info.Destination.ToString() + ", Distance: " + navigation_info.Distance.ImplValue.ToString());
            
            // Wait for call from Drive_Master to Backend
            var wait_result_backend = Navigation.Get_Navigation_Info.WaitForNextCall(2000);
            // Set method call out parameter
            wait_result_backend.Call.navigation_info = navigation_info;
            // Wait for setter call from Drive_Master to Dashboard
            var wait_result_dashboard = Passenger_Information.Navigation_Info.Set.WaitForNextCall(2000);

            // Check for expected distance value (further checks omitted in this example)
            if (wait_result_dashboard.Call.newValue.Distance.ImplValue != navigation_info.Distance.ImplValue)
            {
                Report.TestStepFail("Check distance", "Unexpected value received: " + wait_result_dashboard.Call.newValue.Distance.ImplValue.ToString());
            }
            else 
            {
                Report.TestStepPass("Check distance", "Expected value received: " + wait_result_dashboard.Call.newValue.Distance.ImplValue.ToString());
            }
        }
    }


  
}