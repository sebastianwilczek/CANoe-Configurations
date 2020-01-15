# Add assembly refernces
import clr
clr.AddReference("ASAM.XIL.Implementation.Testbench")
clr.AddReference("ASAM.XIL.Implementation.TestbenchFactory")
clr.AddReference("ASAM.XIL.Interfaces")

# Import required types
from ASAM.XIL.Implementation.TestbenchFactory.Testbench import TestbenchFactory
from ASAM.XIL.Implementation.Testbench.Common.ValueContainer import FloatValue

# Additional Python includes
import time
import System


# Instantiate Vector CANoe test bench for XIL API version 2.1.0
# Note: if using IronPython 32 bit change "CANoe64" to "CANoe32" (this has nothing to do with the CANoe version but the python interpreter version you use!)
factory = TestbenchFactory()
testBench = factory.CreateVendorSpecificTestbench("Vector", "CANoe64", "2.1.0")

# Instantiate a model access port and configure it
print "Creating the MA port"
maPort = testBench.MAPortFactory.CreateMAPort("Python MA Port")
maPort.Configure(maPort.LoadConfiguration("VectorMAPortConfig.xml"), False)
maPort.StartSimulation()

# Read a variable
print "Reading variable 'Test::Variable1'"
for i in range(0, 3):
  floatValue = maPort.Read("Test::Variable1")
  print "Value of the variable is %d" % (floatValue.Value)
  time.sleep(.25)
  
# Write a variable
print "Writing variable 'Test::Variable2'"
for i in range(0, 100):
  floatvalue = FloatValue(i*10)
  maPort.Write("Test::Variable2", floatvalue)
  time.sleep(.1)

# Shut the port down
print "Shutting down the MA port"
maPort.StopSimulation()
maPort.Disconnect()
maPort.Dispose()