using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Demo
{
    public partial class NavigationInfoInternal : UserControl
    {
        public delegate void ValueChangedEventHandler(object sender, ValueChangedEventArgs e);
        public event ValueChangedEventHandler ValueChanged;

        const int cDestinationLength = 99;  // corresponds to sys var struct definition

        public NavigationInfoInternal()
        {
            InitializeComponent();
            button1.ForeColor = Color.Empty;
            button1.ForeColor = Color.Black;
        }

        public override Color ForeColor
        {
            get
            {
                return label1.ForeColor;
            }
            set
            {
                label1.ForeColor = value;
                label2.ForeColor = value;
            }
        }

        /// <summary>
        /// Underlying data structure:
        /// Destination: char[100]
        /// Distance: Int32
        /// Time: Int32 (not used in the example)
        /// </summary>
        /// <param name="value"></param>
        internal void OnRxValue(byte[] value)
        {
            StringBuilder str = new StringBuilder(cDestinationLength);
            for (int i = 0; i <= cDestinationLength; i++)
                str.Append(Char.ConvertFromUtf32(value[i]));
            mTxtDestination.Text = str.ToString();

            UInt32 distance = (UInt32)value[cDestinationLength + 1];
            distance += (UInt32)(value[cDestinationLength + 2] * 0x100);
            distance += (UInt32)(value[cDestinationLength + 3] * 0x10000);
            distance += (UInt32)(value[cDestinationLength + 4] * 0x1000000);
            mTxtDistance.Text = distance.ToString();
        }

        /// <summary>
        /// Underlying data structure:
        /// Destination: char[100]
        /// Distance: Int32
        /// Time: Int32 (not used in the example, is set to 0)
        /// </summary>
        /// <param name="value"></param>
        void Send(object sender)
        {
            if (ValueChanged != null)
            {
                List<byte> byteList = new List<byte>();

                foreach (char item in mTxtDestination.Text.ToCharArray())
                {
                    byteList.Add((byte)item);
                }
                // fill with 0 up to cDestinationLength
                for (int i = byteList.Count; i <= cDestinationLength; i++)
                    byteList.Add((byte)0);

                UInt32 distance = 0;
                if (UInt32.TryParse(mTxtDistance.Text, out distance))
                {
                    byteList.Add((byte)(distance % 0x100));
                    distance /= 0x100;
                    byteList.Add((byte)(distance % 0x100));
                    distance /= 0x100;
                    byteList.Add((byte)(distance % 0x100));
                    byteList.Add((byte)(distance / 0x100));
                }
                // add member: time 
                for (int i = 0; i < 4; i++)
                    byteList.Add((byte)0);

                ValueChanged(sender, new ValueChangedEventArgs(byteList.ToArray()));
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Send(sender);
        }
    }
}
