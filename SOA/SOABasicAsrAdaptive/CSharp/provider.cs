/* 
   Provider side C# application model for service 'Calculator'
   This model implements all methods and events of the service
*/

using System;
using Vector.CANoe.Runtime;
using NetworkDB;

namespace ApplicationModels
{
    // Class implementing the 'Calculator' service interface
    public class CalculatorProvider : DataTypes.Calculator_ProviderInterface
    {
        // Members to track of number calls
        int addCount, substractCount, multiplyCount, divideCount;

        // Implementation for method 'Add'
        public override void Add(int operand1, int operand2, out float result)
        {
            result = operand1 + operand2;
            addCount++;
        }

        // Implementation for method 'Substract'
		public override void Substract(int operand1, int operand2, out float result)
        {
            result = operand1 - operand2;
            substractCount++;
        }

        // Implementation for method 'Multiply'
		public override void Multiply(int operand1, int operand2, out float result)
        {
            result = operand1 * operand2;
            multiplyCount++;
        }

        // Implementation for method 'Divide'
		public override void Divide(int operand1, int operand2, out float result)
        {
            result = (float)operand1 / operand2;
            divideCount++;
        }

        // Update state event. This will also trigger the transmission of the event
        public void TransmitStateEvent()
        {
            if (Calculator.State.SubscriptionState.Value == SubscriptionState.Subscribed)
            {
                Calculator.State.AddCount = addCount;
                Calculator.State.SubstractCount = substractCount;
                Calculator.State.MultiplyCount = multiplyCount;
                Calculator.State.DivideCount = divideCount;
            }
        }
    }

    public class Controller : MeasurementScript
    {
        CalculatorProvider calculatorProvider = new CalculatorProvider();

        // Handler is called upon measurement start of CANoe
        public override void Start()
        {
            // Register provider implementation to handle calls
            Calculator.SetProviderImplementation(calculatorProvider);
            calculatorProvider.TransmitStateEvent();
        }

        // Trigger transmission of event 'State' every 1000 milliseconds
        [OnTimer(1000)]
        public void OnTimer()
        {
            calculatorProvider.TransmitStateEvent();
        }
    }
}
