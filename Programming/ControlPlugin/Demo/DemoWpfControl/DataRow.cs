using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace Demo
{
    public class DataRow : INotifyPropertyChanged
    {
        public DataRow(string description, int value)
        {
            Description = description;
            Value = value;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        #region Description Property (INotifyPropertyChanged)

        private String mDescription;
        public String Description
        {
            get
            {
                return mDescription;
            }
            set
            {
                mDescription = value;
                NotifyPropertyChanged("Description");
            }
        }

        #endregion

        #region Value Property (INotifyPropertyChanged)

        private Int32 mValue;
        public Int32 Value
        {
            get
            {
                return mValue;
            }
            set
            {
                mValue = value;
                NotifyPropertyChanged("Value");
            }
        }

        #endregion

        protected void NotifyPropertyChanged(string propertyName)
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

    }
}
