using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Vector.PanelControlPlugin;
using System.Drawing;
using System.Windows.Forms;
using System.Diagnostics;
using System.ComponentModel;

namespace Demo
{
    // =====>
    // Exchange the name of the bitmap.
    // The file must be found in the subdirectory "Resources" of the project.
    // Set Build Action of the image-file to "Embedded Resource".
    [ToolboxBitmap(typeof(resxfinder), "Demo.Resources.UserControlStaticText.png")]
    // <=====
    // =====>
    // Rename the source file and the class DemoTextBoxEx in the whole file.
    // Change the base class TextBox to the class name of the control, which shall be 
    // used in the paneldesigner.
    public class DemoTextBoxEx : TextBox, IPluginPanelControl, IProvidesSupportedDataTypes
    // <=====
    {
        #region Members

        // symbol value for exchanging the value between CANoe and plugin control and vice versa
        IExchangeSymbolValue mSymbolValue = null;

        // =====>
        // Remove the following member, add own members, if necessary.
        // special members of this plugin control
        string mUnit = "";
        // <=====

        #endregion

        #region construction

        /// <summary>
        /// Constructor.
        /// </summary>
        public DemoTextBoxEx()
        {
            this.ReadOnly = true;
        }

        #endregion

        #region IPluginPanelControl Members

        /// <summary>
        /// This method is called, when the panel is loaded in CANoe/CANalyzer.
        /// </summary>
        /// <param name="value">Object, which is used to transmit a symbol value during the measurement. </param>
        public void Initialize(IExchangeSymbolValue value)
        {
            // remember the symbol value object to be able to receive a value in the plugin control 
            // and send a value from the plugin control
            SymbolValue = value;

            // assign an event handler for receiving values from CANoe
            mSymbolValue.ValueChanged += OnRxValue;

            // =====> ONLY FOR TX: necessary only, if the plugin control shall send values
            // Assign an event handler for sending a value from the plugin control.
            // If the user control only receives values, this is not necessary.
            // this.Leave += new EventHandler(OnTxValue);
            // <=====
        }

        /// <summary>
        /// Returns the name of the plugin control, which is shown in the toolbox of the panel designer.
        /// </summary>
        public string ControlName
        {
            get
            {
                // =====>
                return "Extended TextBox";
                // <=====
            }
        }

        /// <summary>
        /// Returns the control, which actually shall be shown in the panel.
        /// </summary>
        public System.Windows.Forms.Control ExternalControl
        {
            get
            {
                return this;
            }
        }

        public ExchangeSymbolDataType SupportedDataTypes
        {
            get { return ExchangeSymbolDataType.Long | ExchangeSymbolDataType.Double | ExchangeSymbolDataType.String; }
        }

        /// <summary>
        /// This property is for exchanging the symbol value.
        /// </summary>
        public IExchangeSymbolValue SymbolValue
        {
            get
            {
                return mSymbolValue;
            }
            set
            {
                mSymbolValue = value;
            }
        }

        /// <summary>
        /// Returns the list of property names, which are supported by the plugin control, i.e.
        /// the list of properties, which can be configured in the property grid of the 
        /// panel designer.
        /// </summary>
        public IList<string> SupportedProperties
        {
            get
            {
                List<string> supportedProperties = new List<string>();

                // =====>
                supportedProperties.Add("Unit");
                supportedProperties.Add("MyBorderStyle");
                // <=====

                return supportedProperties;
            }
        }

        /// <summary>
        /// Returns true, if the plugin control can change the background color, otherwise false.
        /// </summary>
        public bool SupportsPropertyBackColor
        {
            get
            {
                // =====>
                // return false, if the plugin control does not support changing the background color
                return true;
                // <=====
            }
        }

        /// <summary>
        /// Returns true, if the plugin control can change the foreground color, otherwise false.
        /// </summary>
        public bool SupportsPropertyForeColor
        {
            get
            {
                // =====>
                // return false, if the plugin control does not support changing the foreground color
                return true;
                // <=====
            }
        }

        /// <summary>
        /// Property to set or get the background color of the plugin control.
        /// This method can called via CAPL, to control the display of controls.
        /// Provide a dummy implementation (do nothing), if the plugin control cannot change its background color.
        /// </summary>
        public Color ControlBackColor
        {
            // =====>
            // dummy implementation, if the plugin control does not support changing the background color:
            // get; set;
            // otherwise:
            get
            {
                return this.BackColor;
            }
            set
            {
                this.BackColor = value;
            }
            // <=====
        }

        /// <summary>
        /// Property to set or get the foreground color of the plugin control.
        /// This method can called via CAPL, to control the display of controls.
        /// Provide a dummy implementation (do nothing), if the plugin control cannot change its foreground color.
        /// </summary>
        public Color ControlForeColor
        {
            // =====>
            // dummy implementation, if the control does not support changing the foreground color:
            // get; set;
            // otherwise:
            get
            {
                return this.ForeColor;
            }
            set
            {
                this.ForeColor = value;
            }
            // <=====
        }

        // =====>
        // You may change the implementation of Enabled and Visible. Override the properties in this case.
        // Otherwise these properties need not be implemented here, cecause they are provided by the base class.

        ///// <summary>
        ///// Property to enable or disable the plugin control.
        ///// This property is called, when the DisplayOnly-property is set in the property grid of the panel designer.
        ///// This method can called via CAPL, to control the display/behavior of controls.
        ///// </summary>
        //public override bool Enabled
        //{
        //  get
        //  {
        //    return this.Enabled;
        //  }
        //  set
        //  {
        //    this.Enabled = value;
        //  }
        //}

        ///// <summary>
        ///// Property to set the plugin control visible or invisible.
        ///// This method can called via CAPL, to control the display of controls.
        ///// </summary>
        //public override bool Visible
        //{
        //  get
        //  {
        //    return this.Visible;
        //  }
        //  set
        //  {
        //    this.Visible = value;
        //  }
        //}
        // <=====

        /// <summary>
        /// Serializes all supported properties and returns the serialization string in the out parameter.
        /// </summary>
        /// <param name="serializationString">serialized properties</param>
        /// <returns>true, if serialization was successful, otherwise false</returns>
        public bool SerializeSupportedProperties(out string serializationString)
        {
            bool ok = true;
            serializationString = "";

            // =====>
            // provide a serialization for all properties supported by the plugin control
            // (without the properties ControlBackColor, ControlForeColor, Enabled and Visible)
            // the following examples show two different possibilities:
            // 1st: for a simple string property
            // 2nd: for a property, which has a TypeConverter
            // Take care, that the deserialization is implemented accordingly.

            // property "Unit"
            serializationString += Unit.ToString();
            serializationString += ";";

            // property "MyBorderStyle"
            TypeConverter tc = TypeDescriptor.GetConverter(MyBorderStyle);
            if (tc.CanConvertTo(typeof(string)))
            {
                object stringVal = tc.ConvertTo(MyBorderStyle, typeof(string));
                serializationString += stringVal.ToString();
                serializationString += ";";
            }
            else
            {
                ok = false;
            }
            // <=====

            return ok;
        }

        /// <summary>
        /// Deserializes the supported properties from the given string.
        /// </summary>
        /// <param name="serializationString">serialized properties</param>
        /// <returns>true, if deserialization was successful, otherwise false</returns>
        public bool DeserializeSupportedProperties(string serializationString)
        {
            bool retVal = true;

            // =====>
            // provide a serialization for all properties supported by the plugin control
            // (without the properties ControlBackColor, ControlForeColor, Enabled and Visible)
            // the following examples show two different possibilities 
            // 1st: for a simple string property
            // 2nd: for a property, which has a TypeConverter
            // Take care, that the serialization is implemented accordingly.
            string[] propertyValues = serializationString.Split(new char[] { ';' });

            if (propertyValues.Count() > 0)
            {
                Unit = propertyValues[0];
            }

            if (propertyValues.Count() > 1)
            {
                object valueObject = null;
                TypeConverter tc = TypeDescriptor.GetConverter(MyBorderStyle.GetType());
                if (tc.CanConvertFrom(typeof(string)))
                {
                    try
                    {
                        valueObject = tc.ConvertFrom(propertyValues[1]);
                        MyBorderStyle = (BorderStyle)valueObject;
                    }
                    catch (Exception)
                    {
                        MyBorderStyle = BorderStyle.None;
                    }
                }
            }
            // <=====

            return retVal;
        }

        #endregion

        #region supported properties

        // =====>
        // implement all properties of the plugin control, which shall be configured in the 
        // property grid of the paneldesigner

        [CategoryAttribute("DemoTextBoxEx Settings ")]
        [DisplayName("Unit")]
        public string Unit
        {
            get
            {
                return mUnit;
            }
            set
            {
                mUnit = value;
            }
        }

        [CategoryAttribute("DemoTextBoxEx Settings ")]
        [DisplayName("Border Style")]
        public BorderStyle MyBorderStyle
        {
            get
            {
                return this.BorderStyle;
            }
            set
            {
                this.BorderStyle = value;
            }
        }

        // <=====

        #endregion

        #region Rx/Tx values

        /// <summary>
        /// Actions which are necessary, when a new value is sent from CANoe.
        /// The received value is in this-value and must be sent to the plugin control.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void OnRxValue(object sender, EventArgs e)
        {
            // =====>
            // this line may be obsolete, if the plugin control only displays values but does not change them
            // this.Leave -= OnTxValue;
            // <=====
            try
            {
                // Value.SymbolDataType depends on the symbol data type of the assigned symbol
                // It is given from the CANoe database and must not be changed.
                switch (SymbolValue.SymbolDataType)
                {
                    case ExchangeSymbolDataType.Long:
                        // =====>
                        // display Value.LongValue in the plugin control
                        this.Text = (SymbolValue.LongValue.ToString() + " " + Unit);
                        // <=====
                        break;
                    case ExchangeSymbolDataType.Double:
                        // =====>
                        // display Value.DoubleValue in the plugin control
                        this.Text = (SymbolValue.DoubleValue.ToString() + " " + Unit);
                        // <=====
                        break;
                    case ExchangeSymbolDataType.String:
                        // =====>
                        // display Value.StringValue in the plugin control
                        string value = SymbolValue.StringValue;
                        if (value.EndsWith("\0"))
                            value = value.Substring(0, value.Length - 1);
                        this.Text = (value + " " + Unit);
                        // <=====
                        break;
                    default:
                        // =====>
                        this.Text = ("not implemented data type" + " " + Unit);
                        // <=====
                        break;
                }
            }
            catch (System.Exception)
            {
            }
            // =====>
            // this line may be obsolete, if the plugin control only displays values but does not change them
            // this.Leave += OnTxValue;
            // <=====
        }

        //// =====> ONLY FOR TX: necessary only, if the plugin control shall send values
        //void OnTxValue(object sender, EventArgs e)
        //{
        //  try
        //  {
        //    if (mSymbolValue == null || mSymbolValue.SymbolDataType == ExchangeSymbolDataType.Unknown)
        //      return;

        //    // =====>
        //    // remove these lines as they are special for the example
        //    int startPos = mUnit.Length + 2;
        //    string value = this.Text.Substring(startPos);
        //    // <=====

        //    // Value.SymbolDataType depends on the symbol data type of the assigned symbol
        //    // It is given from the CANoe database and must not be changed.
        //    switch (mSymbolValue.SymbolDataType)
        //    {
        //      case ExchangeSymbolDataType.Long:
        //        // =====>
        //        // put the value from the plugin control to Value.LongValue
        //        long longValue = 0;
        //        if (Int64.TryParse(value, out longValue))
        //          mSymbolValue.LongValue = longValue;
        //        // <=====
        //        break;
        //      case ExchangeSymbolDataType.Double:
        //        // =====>
        //        // put the value from the plugin control to Value.DoubleValue
        //        double doubleValue = 0;
        //        if (Double.TryParse(value, out doubleValue))
        //          mSymbolValue.DoubleValue = doubleValue;
        //        // <=====
        //        break;
        //      case ExchangeSymbolDataType.String:
        //        // =====>
        //        // put the value from the plugin control to Value.DoubleValue
        //        mSymbolValue.StringValue = value;
        //        // <=====
        //        break;
        //      default:
        //        break;
        //    }
        //  }
        //  catch (System.Exception)
        //  {
        //    // make sure, that no exceptions are passed
        //  }
        //}
        //// <=====

        #endregion
    }
}
