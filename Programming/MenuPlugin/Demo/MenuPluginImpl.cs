using System.Collections.Generic;
using System.Windows.Forms;

using Vector.MenuPlugin;


namespace Vector.CANalyzer.MenuPlugin
{
    public class MenuPlugin : IMenuPlugin
    {
        #region Fields
        List<IMenuItem> mMenuItems = new List<IMenuItem>();
        string mText = "Plugin";
        #endregion

        public MenuPlugin()
        {
            //  Add several MenuItem
            mMenuItems.Add(new MenuItem("Item1"));
            mMenuItems.Add(new MenuItem("Item2"));
            MenuItem menuItem = new MenuItem("Item3");
            menuItem.AddMenuItem(new MenuItem("SubItem1"));
            menuItem.AddMenuItem(new MenuItem("SubItem2"));
            mMenuItems.Add(menuItem);
            mMenuItems.Add(new MenuItem("Item4"));
        }

        private void UpdateItemStates()
        {
            //  Very simple example how you can enable or disable items
            //  or how you can toggle the checked state of an item.
            foreach (IMenuItem menuItem in mMenuItems)
            {
                MenuItem itemImpl = menuItem as MenuItem;
                if (itemImpl != null)
                {
                    if (itemImpl.Text == "Item2")
                    {
                        //  Toggle enabled state
                        itemImpl.Enabled = !itemImpl.Enabled;
                    }
                    else if (itemImpl.Text == "Item4")
                    {
                        //  Toggle checked state
                        itemImpl.Checked = !itemImpl.Checked;
                    }
                }
            }
        }

        #region IMenuPlugin Members

        string IMenuPlugin.Text
        {
            get
            {
                return mText;
            }
        }

        IList<IMenuItem> IMenuPlugin.MenuItems
        {
            get
            {
                UpdateItemStates();
                return mMenuItems;
            }
        }

        void IMenuPlugin.ItemClicked(IMenuItem clickedItem)
        {
            MenuItem itemImpl = clickedItem as MenuItem;
            if (itemImpl != null)
            {
                MessageBox.Show("Item clicked: " + itemImpl.Text);
            }
        }

        #endregion
    }

    public class MenuItem : IMenuItem
    {
        #region Fields
        List<IMenuItem> mSubMenuItems = new List<IMenuItem>();
        string mText;
        bool mEnabled = true;
        bool mChecked = false;
        #endregion

        internal MenuItem(string text)
        {
            mText = text;
        }

        internal void AddMenuItem(IMenuItem menuItem)
        {
            mSubMenuItems.Add(menuItem);
        }

        internal bool Checked
        {
            get { return mChecked; }
            set { mChecked = value; }
        }

        internal bool Enabled
        {
            get { return mEnabled; }
            set { mEnabled = value; }
        }

        internal string Text
        {
            get { return mText; }
            set { mText = value; }
        }


        #region IMenuItem Members
        string IMenuItem.Text
        {
            get
            {
                return mText;
            }
        }

        bool IMenuItem.Checked
        {
            get
            {
                return mChecked;
            }
        }

        bool IMenuItem.Enabled
        {
            get
            {
                return mEnabled;
            }
        }

        IList<IMenuItem> IMenuItem.SubMenuItems
        {
            get
            {
                return mSubMenuItems;
            }
        }
        #endregion
    }
}
