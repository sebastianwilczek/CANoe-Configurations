/* 
   Consumer side C# application model for service 'Calculator'
   This model simply calls a method and prints the result
   Additionally, the value of the event 'State' is printed
*/

using System;
using Vector.CANoe.Runtime;
using Vector.Tools;
using NetworkDB.CommunicationObjects;

namespace Application_Models
{
    public class Controller : MeasurementScript
    {
        // Call method 'Add' of service 'Calculator' when key 'c' is pressed 
        [OnKey('c')]
        public void CallAdd(char c)
        {

            // Note: The third argument is a lambda expression
            //       This expression is executed asynchronously, when the method result is received
            Calculator.Add.CallAsync(3, 6, result => Output.WriteLine("Result of Add method: " + result.ToString()) );
        }

        // Toggle subscription for Event 'State' upon panel switch button is pressed
        [OnChange(typeof(ConsumerPanel.SubscribeStateEvent))]
        public void ToggleSubsribtion()
        {
            if (ConsumerPanel.SubscribeStateEvent.Value == 2)
            {
                Calculator.Abstract.State.RequestEvent();
            }
            else
            {
                Calculator.Abstract.State.ReleaseEvent();
            }
        }

        // This handler is called when the value of the event 'State' changed
        [OnChange(typeof(Calculator.consumerSide.State))]
        public void StateEventReceived()
        {
            Output.WriteLine(String.Format("State event received. AddCount: {0}, SubstractCount: {1}, MultiplyCount: {2}, DivideCount: {3}",
                               Calculator.State.AddCount,
                               Calculator.State.SubstractCount,
                               Calculator.State.MultiplyCount,
                               Calculator.State.DivideCount));
        }
    }
}
