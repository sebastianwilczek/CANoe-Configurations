using System;
using Vector.Tools;
using Vector.CANoe.Runtime;

// Import communication model
using DataTypes;
using NetworkDB.Services;
using NetworkDB.Participants;



public class Dashboard_Model : MeasurementScript
{
    Timer mIndexTimer = new Timer(OnIndexTimer);

    // Animation of roadway
    [OnChange(typeof(SysVars.Vehicle.Velocity))]
    public void OnVelocity()
    {
        mIndexTimer.Stop();
        int interval;
        if (SysVars.Vehicle.Velocity.Value < 20)
        {
            interval = 100;
        }
        else if (SysVars.Vehicle.Velocity.Value < 20)
        {
            interval = 50;
        }
        else
        {
            interval = 25;
        }
        mIndexTimer.Interval = TimeSpan.FromMilliseconds(interval);
        mIndexTimer.Start();
    }

    static public void OnIndexTimer(object sender, EventArgs e)
    {
        SysVars.Dashboard.LineIndex.Value = (SysVars.Dashboard.LineIndex.Value + 1) % 10;
        ((Timer)sender).Start();
    }
}
