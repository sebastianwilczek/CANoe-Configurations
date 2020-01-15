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
    public partial class ProgressBarInternal : UserControl
    {
        public ProgressBarInternal()
        {
            InitializeComponent();
        }
        public delegate void ValueChangedEventHandler(object sender, ValueChangedEventArgs e);
        public event ValueChangedEventHandler ValueChanged;

        public int Min
        {
            get
            {
                return progressBar1.Minimum;
            }
            set
            {
                progressBar1.Minimum = value;
            }
        }

        public int Max
        {
            get
            {
                return progressBar1.Maximum;
            }
            set
            {
                progressBar1.Maximum = value;
            }
        }

        internal void OnRxValue(double value)
        {
            if (value >= Min)
            {
                if (value <= Max)
                    progressBar1.Value = (int)value;
                else
                    progressBar1.Value = Max;
            }
            else
            {
                progressBar1.Value = Min;
            }

            textBox1.Text = value.ToString(String.Format("F2"));
        }

        private void textBox1_Leave(object sender, EventArgs e)
        {
            SendValue(sender);
        }

        private void textBox1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
                SendValue(sender);
        }

        void SendValue(object sender)
        {
            if (ValueChanged != null)
            {
                double value = 0;
                if (Double.TryParse(textBox1.Text, out value))
                    ValueChanged(sender, new ValueChangedEventArgs(value));
            }
        }
    }
}
