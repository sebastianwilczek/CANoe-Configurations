Demonstrator for In-Cycle Repetition and In-Cycle Responses of frames.
======================================================================
Version 1.0

This demo runs in simulation and with real bus if VN3300 (PCI!) is used.
(Assure for real bus usage that Tx acknowledges are enabled.)

Description of node behavior:
------------------------------------------------------------------------
Node_A generates the 3 binary (switch) signals and 3 counter signals with a period of 5 ms, 2.5 ms, and 1.25 ms. Node_B repeats these signals in appropriate mirror signals.

Motivation for desktop "In-Cycle Repetition: Switch Signals":
------------------------------------------------------------------------
The Graphics window shows the generated 3 binary switch signals and one environment variable that is 1 ms "high" right after the start of a FlexRay cycle. Although the signals with used In-Cycle repetition have an higher period, their signal curve progression is not homogeneous, because of the asymmetric slot assignment within the FlexRay cycle.

Motivation for desktop "In-Cycle Repetition: Counter Signals":
------------------------------------------------------------------------
The Graphics window shows the generated 3 counter signals. Each counter makes 100 steps (values 0 - 99). The periods of the In-Cycle Multiplexed counter signals are 2 (4)-times higher than the FlexRay cycle period.

Motivation for desktop "In-Cycle Response: Switch Signals":
------------------------------------------------------------------------
The Graphics window shows the generated 3 binary switch signals from node A and the 3 repeated switch signals from node B. The signal's curve progression with using the simulated bus or the real bus will differ (especially for the 2.5 ms signal)! The ideal response from the signal in slot 3 to a signal in slot 4 and from the signal in slot 88 to a signal in slot 89 is only possible in simulation mode! In real mode the signal in slot 3 will be repeated in slot 89 and the signal in slot 88 will be repeated in slot 4. Therefore the signal in real mode looks like to be inverted, but in fact it is only delayed!

--------------------------------------------------------------------------------------------------------
Copyright (c) by Vector Informatik GmbH Dep. Tools Automotive  2007
--------------------------------------------------------------------------------------------------------