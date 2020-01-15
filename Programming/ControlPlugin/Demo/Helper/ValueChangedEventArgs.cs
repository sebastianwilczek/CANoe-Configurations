using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Demo
{
    public class ValueChangedEventArgs : EventArgs
    {
        public ValueChangedEventArgs(object value)
        {
            mValue = value;
        }

        public readonly object mValue = 0;
    }
}
