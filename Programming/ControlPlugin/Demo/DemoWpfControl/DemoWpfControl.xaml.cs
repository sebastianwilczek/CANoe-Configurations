using System.Windows.Interop;
using System.Windows.Controls;
using Vector.PanelControlPlugin;
using System.Windows;
using System.Collections.Generic;
using System;
using System.Linq;
using System.ComponentModel;
using System.Collections.ObjectModel;
using System.Windows.Media;

namespace Demo
{
    // =====>
    // exchange the name of the bitmap
    // the file must be found in the subdirectory "Resources" of the project
    // set Build Action of the image-file to "Embedded Resource"
    [System.Drawing.ToolboxBitmap(typeof(resxfinder), "Demo.Resources.UserControlWPF.png")]
    // <=====
    // =====>
    // rename the source file and the class DemoWpfControl in the whole file
    public partial class DemoWpfControl : UserControl, IPluginPanelControl, IProvidesSupportedDataTypes
    // <=====
    {
        #region Members

        // symbol value for exchanging the value between CANoe and plugin control and vice versa
        IExchangeSymbolValue mSymbolValue = null;

        HwndSource mHwndSource = null;
        System.Windows.Forms.Control mExternalControl = null;
        const int WM_GETDLGCODE = 0x0087;
        const int DLGC_WANTALLKEYS = 0x0004;

        private string mDescription = "Description";
        ObservableCollection<DataRow> mData = new ObservableCollection<DataRow>();

        #endregion

        #region construction, initalization
        public DemoWpfControl()
        {
            InitializeComponent();

            this.Loaded += DemoWpfControl_Loaded;
            this.Unloaded += DemoWpfControl_Unloaded;
            mItems.ItemsSource = mData;
            mData.Add(new DataRow("Description[0]", 10));
            mData.Add(new DataRow("Description[1]", 20));
            mData.Add(new DataRow("Description[2]", 30));
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
        }

        public ExchangeSymbolDataType SupportedDataTypes
        {
            get { return ExchangeSymbolDataType.LongArray; }
        }

        /// <summary>
        /// Returns the name of the plugin control, which is shown in the toolbox of the panel designer.
        /// </summary>
        public string ControlName
        {
            get
            {
                // =====>
                return "WPF Control";
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
                if (mExternalControl == null)
                    mExternalControl = CreateElementHost();  // create a WinForms control, which is host of the WPF control

                return mExternalControl;
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
        /// Returns the list of property names, which are supported by the plugin  control, i.e.
        /// the list of properties, which can be configured in the property grid of the 
        /// panel designer.
        /// </summary>
        public IList<string> SupportedProperties
        {
            get
            {
                List<string> supportedProperties = new List<string>();

                // =====>
                supportedProperties.Add("Description");
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
        public System.Drawing.Color ControlBackColor
        {
            // =====>
            // dummy implementation, if the plugin control does not support changing the background color:
            // get; set;
            // otherwise:
            get
            {
                var brush = this.Background as SolidColorBrush;
                return System.Drawing.Color.FromArgb(brush.Color.A, brush.Color.R, brush.Color.G, brush.Color.B);
            }
            set
            {
                this.Background = new SolidColorBrush(Color.FromArgb(value.A, value.R, value.G, value.B));
            }
            // <=====
        }

        /// <summary>
        /// Property to set or get the foreground color of the plugin control.
        /// This method can called via CAPL, to control the display of controls.
        /// Provide a dummy implementation (do nothing), if the plugin control cannot change its foreground color.
        /// </summary>
        public System.Drawing.Color ControlForeColor
        {
            // =====>
            // dummy implementation, if the plugin control does not support changing the foreground color:
            // get; set;
            // otherwise:
            get
            {
                var brush = this.Foreground as SolidColorBrush;
                return System.Drawing.Color.FromArgb(brush.Color.A, brush.Color.R, brush.Color.G, brush.Color.B);
            }
            set
            {
                this.Foreground = new SolidColorBrush(Color.FromArgb(value.A, value.R, value.G, value.B));
            }
            // <=====
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

            // property "Description"
            serializationString += Description.ToString();
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

            string[] propertyValues = serializationString.Split(new char[] { ';' });

            if (propertyValues.Count() > 0)
            {
                Description = propertyValues[0];
            }
            // <=====

            return retVal;
        }

        #endregion

        #region supported properties

        // =====>
        // implement all properties of the plugin control, which shall be configured in the 
        // property grid of the paneldesigner

        [CategoryAttribute("WPF Control Settings ")]
        [DisplayName("Description")]
        public string Description
        {
            get
            {
                return mDescription;
            }
            set
            {
                mDescription = value;
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
            try
            {
                if (SymbolValue.SymbolDataType != ExchangeSymbolDataType.LongArray)
                    return;

                mData.Clear();
                for (int i = 0; i < SymbolValue.LongArray.Count(); i++)
                {
                    mData.Add(new DataRow(Description + "[" + i.ToString() + "]", SymbolValue.LongArray[i]));
                }
            }
            catch (System.Exception)
            {
            }
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            SendValue(sender);
        }

        void SendValue(object sender)
        {
            if (mSymbolValue == null ||
                mSymbolValue.SymbolDataType != ExchangeSymbolDataType.LongArray)
                return;

            int[] newArrayValue = new int[mSymbolValue.LongArray.Count()];

            for (int i = 0; i < mSymbolValue.LongArray.Count(); i++)
            {
                // Value.SymbolDataType depends on the symbol data type of the assigned symbol
                // It is given from the CANoe database and must not be changed.
                // =====>
                // put the value from the plugin control to Value.LongValue
                newArrayValue[i] = mData[i].Value;
                // <=====
            }
            mSymbolValue.LongArray = newArrayValue;
        }

        #endregion

        #region WinForms/WPF

        private System.Windows.Forms.Control CreateElementHost()
        {
            var elementHost = new System.Windows.Forms.Integration.ElementHost()
            {
                Dock = System.Windows.Forms.DockStyle.Fill,
                Child = (UIElement)this,
            };

            return elementHost;
        }

        void DemoWpfControl_Loaded(object sender, RoutedEventArgs e)
        {
            if (mHwndSource != null)
                return;

            mHwndSource = PresentationSource.FromVisual(this) as HwndSource;
            if (mHwndSource != null)
                mHwndSource.AddHook(WndProc);
        }

        void DemoWpfControl_Unloaded(object sender, RoutedEventArgs e)
        {
            if (mHwndSource != null)
                mHwndSource.RemoveHook(WndProc);
            mHwndSource = null;
        }

        // the following method is necessary, to support key handling during the measurement
        private IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            if (msg == WM_GETDLGCODE)
            {
                handled = true;
                return new IntPtr(DLGC_WANTALLKEYS);
            }
            return IntPtr.Zero;
        }

        #endregion
    }
}
