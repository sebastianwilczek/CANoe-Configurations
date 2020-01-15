
==================================================================================

0. Table of Contents
----------------------------
1. Copyright and Version
2. Overview
3. Description


1. Copyright
--------------
Version: 2.0
(c) 2001-2002 Vector Informatik GmbH

2. Overview
--------------
The configuration 'TractorECU.cfg' can be used with CANoe 4.0 Option ISO11783 2.0 and higher.
It simulates a Tractor ECU (#240) (Classification 2F) and an Experimental ECU (#252).


2. Overview
--------------
The configuration contains two nodes:

  Node 'TractorECU' is as simple implementation of a tractor
  ECU (Classification 2F). The key switch, tractor speed, rear
  hitch and rear PTO output shaft can be set in a panel.

  Node 'ExperimentalImplement' displays the values, received from
  the tractor ECU. It can also send the power maintain message
  to keep the ECU_PWR active.

After measurement start, the bus communication must be enabled
with the key switch. The tractor ECU starts sending the wheel/ground
based speed and the rear hitch position.
The slider sets the tractor speed. The stop button reduces the
tractor speed and enables the stop lights.
With the 'Engagement' switch, the rear PTO output shaft can be
enabled. The speed of the output shaft depends on the tracor speed.
The '+' and '-' buttons controls the rear hitch position.

The panel for 'ExperimentalImplement' displays the values which are received
from the tractor ECU. The time value keeps the ECU_PWR active after
the key switch is disabled.
