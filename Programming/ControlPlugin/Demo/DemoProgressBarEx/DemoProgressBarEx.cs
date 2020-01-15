using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Linq;
using System.Text;
using Vector.PanelControlPlugin;

namespace Demo
{
    // =====>
    // exchange the name of the bitmap
    // the file must be found in the subdirectory "Resources" of the project
    // set Build Action of the image-file to "Embedded Resource"
    [ToolboxBitmap(typeof(resxfinder), "Demo.Resources.UserControlProgressBar.png")]
    // <=====
    // =====>
    // rename the source file and the class DemoProgressBarEx in the whole file
    public class DemoProgressBarEx : IPluginPanelControl
    // <=====
    {
        #region Members

        // symbol value for exchanging the value between CANoe and plugin control and vice versa
        IExchangeSymbolValue mSymbolValue = null;

        // =====>
        // replace the class name "DemoProgressBarEx" by the class name of the control, which shall be used in the paneldesigner
        ProgressBarInternal mProgressBar = new ProgressBarInternal();  // control, which actually is shown in the panel
        // <=====

        #endregion

        #region construction

        /// <summary>
        /// Constructor.
        /// </summary>
        public DemoProgressBarEx()
        {
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

            // =====> ONLY FOR TX: necessary only, if plugin control shall send values
            // Assign an event handler for sending a value from the plugin control.
            // If the plugin control only receives values, this is not necessary.
            mProgressBar.ValueChanged += new ProgressBarInternal.ValueChangedEventHandler(OnTxValue);
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
                return "Extended ProgressBar";
                // <=====
            }
        }

        public ExchangeSymbolDataType SupportedDataTypes
        {
            get { return ExchangeSymbolDataType.Long | ExchangeSymbolDataType.Double; }
        }

        /// <summary>
        /// Returns the control, which actually shall be shown in the panel.
        /// </summary>
        public System.Windows.Forms.Control ExternalControl
        {
            get
            {
                return mProgressBar;
            }
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
        /// Returns the list of property names, which are supported by the uplugin  control, i.e.
        /// the list of properties, which can be configured in the property grid of the 
        /// panel designer.
        /// </summary>
        public IList<string> SupportedProperties
        {
            get
            {
                List<string> supportedProperties = new List<string>();

                // =====>
                supportedProperties.Add("Min");
                supportedProperties.Add("Max");
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
                return false;
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
                return false;
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
            // dummy implementation, if the plugin control does not support changing the background color:
            get; set;
        }

        /// <summary>
        /// Property to set or get the foreground color of the plugin control.
        /// This method can called via CAPL, to control the display of controls.
        /// Provide a dummy implementation (do nothing), if the plugin control cannot change its foreground color.
        /// </summary>
        public Color ControlForeColor
        {
            // dummy implementation, if the control does not support changing the foreground color:
            get; set;
        }

        /// <summary>
        /// Property to enable or disable the plugin control.
        /// This property is called, when the DisplayOnly-property is set in the property grid of the panel designer.
        /// This method can called via CAPL, to control the display/behavior of controls.
        /// </summary>
        public bool Enabled
        {
            get
            {
                return ExternalControl.Enabled;
            }
            set
            {
                ExternalControl.Enabled = value;
            }
        }

        /// <summary>
        /// Property to set the plugin control visible or invisible.
        /// This method can called via CAPL, to control the display of controls.
        /// </summary>
        public bool Visible
        {
            get
            {
                return ExternalControl.Visible;
            }
            set
            {
                ExternalControl.Visible = value;
            }
        }

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

            // property "Min"
            serializationString += Min.ToString();
            serializationString += ";";

            // property "Max"
            serializationString += Max.ToString();
            serializationString += ";";
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
            int val = 0;

            if (propertyValues.Count() > 0)
            {
                if (Int32.TryParse(propertyValues[0], out val))
                    Min = val;
            }

            if (propertyValues.Count() > 1)
            {
                if (Int32.TryParse(propertyValues[1], out val))
                    Max = val;
            }
            // <=====

            return retVal;
        }

        #endregion

        #region supported properties

        // =====>
        // implement all properties of the plugin control, which shall be configured in the 
        // property grid of the paneldesigner

        [CategoryAttribute("ProgressBarEx Settings ")]
        [DisplayName("Minimum")]
        public int Min
        {
            get
            {
                return mProgressBar.Min;
            }
            set
            {
                mProgressBar.Min = value;
            }
        }

        [CategoryAttribute("ProgressBarEx Settings ")]
        [DisplayName("Maximum")]
        public int Max
        {
            get
            {
                return mProgressBar.Max;
            }
            set
            {
                mProgressBar.Max = value;
            }
        }
        // <=====

        #endregion

        #region Rx/Tx values

        /// <summary>
        /// Actions which are necessary, when a new value is received from CANoe.
        /// The received value is in this-value and must be sent to the plugin control.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void OnRxValue(object sender, EventArgs e)
        {
            // =====>
            // this line may be obsolete, if the plugin control only displays values but does not change them
            mProgressBar.ValueChanged -= OnTxValue;
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
                        mProgressBar.OnRxValue(SymbolValue.LongValue);
                        // <=====
                        break;
                    case ExchangeSymbolDataType.Double:
                        // =====>
                        // display Value.DoubleValue in the plugin control
                        mProgressBar.OnRxValue(SymbolValue.DoubleValue);
                        // <=====
                        break;
                    default:
                        break;
                }
            }
            catch (System.Exception)
            {
            }
            // =====>
            // this line may be obsolete, if the plugin control only displays values but does not change them
            mProgressBar.ValueChanged += OnTxValue;
            // <=====
        }

        // =====> ONLY FOR TX: necessary only, if plugin control shall send values
        /// <summary>
        /// Actions which are necessary, when a new value shall be sent to CANoe.
        /// The sent value must be written to this.Value.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e">ValueChangedEventArgs containing the value to be sent</param>
        void OnTxValue(object sender, ValueChangedEventArgs e)
        {
            try
            {
                if (mSymbolValue == null ||
                    mSymbolValue.SymbolDataType == ExchangeSymbolDataType.Unknown)
                    return;

                // Value.SymbolDataType depends on the symbol data type of the assigned symbol
                // It is given from the CANoe database and must not be changed.
                switch (mSymbolValue.SymbolDataType)
                {
                    case ExchangeSymbolDataType.Long:
                        // =====>
                        // put the value from the plugin control to Value.LongValue
                        mSymbolValue.LongValue = Convert.ToInt32(e.mValue);
                        // <=====
                        break;
                    case ExchangeSymbolDataType.Double:
                        // =====>
                        // put the value from the plugin control to Value.DoubleValue
                        mSymbolValue.DoubleValue = Convert.ToDouble(e.mValue);
                        // <=====
                        break;
                    default:
                        break;
                }
            }
            catch (System.Exception)
            {
                // make sure, that no exceptions are passed
            }
        }
        // <=====

        #endregion
    }
}
