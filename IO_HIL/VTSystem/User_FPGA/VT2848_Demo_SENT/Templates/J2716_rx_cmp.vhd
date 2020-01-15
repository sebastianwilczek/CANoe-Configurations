--============================================================================
--  This confidential and proprietary software may be used
--  only as authorized by a licensing agreement from
--  Vector Informatik GmbH.
--  In an event of publication, the following notice is applicable:
--
--  (C) COPYRIGHT 2012 VECTOR INFORMATIK GMBH
--  ALL RIGHTS RESERVED
--
--  The entire notice above must be reproduced on all authorized copies.
--
--  $Workfile: J2716_rx_cmp.vhd $
--  $Author: visjt $
--  $Date: 2012-11-29 10:50:33 +0100 (Do, 29 Nov 2012) $
--  $Revision: 20601 $
--  Abstract      :
--============================================================================


------------------------------------------------------------------------
-- Signal Description

--  SIGNAL         ; DIRECTION ; WIDTH ; DESCRIPTION
-- ----------------+-----------+-------+-----------------------------------------------------------
--  i_reset        ; IN        ; 1     ; reset signal - active reset level is
--                 ;           ;       ;  defined via generic g_reset_level
-- ----------------+-----------+-------+-----------------------------------------------------------
--  i_clock        ; IN        ; 1     ; clock signal 
-- ----------------+-----------+-------+-----------------------------------------------------------
--  i_rx           ; IN        ; 1     ; data signal of J2716 bus 
-- ----------------+-----------+-------+-----------------------------------------------------------
--  i_activate     ; IN        ; 1     ; activation signal of module
--                 ;           ;       ;  0 - module is inactive
--                 ;           ;       ;  1 - module is active
-- ----------------+-----------+-------+-----------------------------------------------------------
--  i_tic_length   ; IN        ; 10    ; Config: length of a tic (length is defined in clock tics)
--                 ;           ;       ;  the receiver accepts tics with +/-25% of configured value
-- ----------------+-----------+-------+-----------------------------------------------------------
--  i_data_nibbles ; IN        ; 3     ; Config: number of data nibbles of a J2716 frame
--                 ;           ;       ;  accepted values are 1 to 6
-- ----------------+-----------+-------+-----------------------------------------------------------
--  i_pause        ; IN        ; 1     ; Config: a pause pulse is on bus after J2716 frame (pause
--                 ;           ;       ;  pulse is supported since version J2716-JAN2010)
--                 ;           ;       ;  0 - no pause on bus
--                 ;           ;       ;  1 - pause on bus
-- ----------------+-----------+-------+-----------------------------------------------------------
--  i_crc_type     ; IN        ; 1     ; Config: defines which CRC type is used in the J2716 frame
--                 ;           ;       ;  0 - CRC type J2716-FEB2008 (legacy mode)
--                 ;           ;       ;  1 - CRC type J2716-JAN2010 (recommended mode)
-- ----------------+-----------+-------+-----------------------------------------------------------
--  o_error        ; OUT       ; 1     ; error detected
--                 ;           ;       ;  0 - no error detected
--                 ;           ;       ;  1 - error detected --> see o_error_code for description
-- ----------------+-----------+-------+-----------------------------------------------------------
--  o_error_code   ; OUT       ; 4     ; Error code reports error type
--                 ;           ;       ;  0 - no error
--                 ;           ;       ;  1 - Received value is out of range 0..15
--                 ;           ;       ;  2 - Consecutive sync pulses differs more than +/-1,5625%
--                 ;           ;       ;  3 - No falling edge detected
--                 ;           ;       ;  4 - Received sync pulse is above max settings
--                 ;           ;       ;  5 - Received sync pulse is below min settings
--                 ;           ;       ;  6 - CRC error
-- ----------------+-----------+-------+-----------------------------------------------------------
--  o_tic_length   ; OUT       ; 10    ; measured tic length (length is reported in clock tics)
-- ----------------+-----------+-------+-----------------------------------------------------------
--  o_rx_status    ; OUT       ; 4     ; received value of status nibble
-- ----------------+-----------+-------+-----------------------------------------------------------
--  o_rx_data_n0   ; OUT       ; 4     ; received value of Data1 nibble
-- ----------------+-----------+-------+-----------------------------------------------------------
--  o_rx_data_n1   ; OUT       ; 4     ; received value of Data2 nibble
-- ----------------+-----------+-------+-----------------------------------------------------------
--  o_rx_data_n2   ; OUT       ; 4     ; received value of Data3 nibble
-- ----------------+-----------+-------+-----------------------------------------------------------
--  o_rx_data_n3   ; OUT       ; 4     ; received value of Data4 nibble
-- ----------------+-----------+-------+-----------------------------------------------------------
--  o_rx_data_n4   ; OUT       ; 4     ; received value of Data5 nibble
-- ----------------+-----------+-------+-----------------------------------------------------------
--  o_rx_data_n5   ; OUT       ; 4     ; received value of Data6 nibble
-- ----------------+-----------+-------+-----------------------------------------------------------
--  o_rx_crc       ; OUT       ; 4     ; received value of CRC nibble
-- ----------------+-----------+-------+-----------------------------------------------------------
--  o_pause_length ; OUT       ; 11    ; measured length of pause pulse (length is reported in tics)
-- ----------------+-----------+-------+-----------------------------------------------------------


------------------------------------------------------------------------
-- component of J2716 Rx module

  COMPONENT J2716_rx
    GENERIC (
      g_reset_level  : std_logic);
    PORT (
      i_reset        : IN  std_logic;
      i_clock        : IN  std_logic;
      i_rx           : IN  std_logic;
      i_activate     : IN  std_logic;
      i_tic_length   : IN  std_logic_vector(9 DOWNTO 0);
      i_data_nibbles : IN  std_logic_vector(2 DOWNTO 0);
      i_pause        : IN  std_logic;
      i_crc_type     : IN  std_logic;
      o_error_code   : OUT std_logic_vector(3 DOWNTO 0);
      o_tic_length   : OUT std_logic_vector(9 DOWNTO 0);
      o_rx_status    : OUT std_logic_vector(3 DOWNTO 0);
      o_rx_data_n0   : OUT std_logic_vector(3 DOWNTO 0);
      o_rx_data_n1   : OUT std_logic_vector(3 DOWNTO 0);
      o_rx_data_n2   : OUT std_logic_vector(3 DOWNTO 0);
      o_rx_data_n3   : OUT std_logic_vector(3 DOWNTO 0);
      o_rx_data_n4   : OUT std_logic_vector(3 DOWNTO 0);
      o_rx_data_n5   : OUT std_logic_vector(3 DOWNTO 0);
      o_rx_crc       : OUT std_logic_vector(3 DOWNTO 0);
      o_error        : OUT std_logic;
      o_pause_length : OUT std_logic_vector(10 DOWNTO 0));
  END COMPONENT;

------------------------------------------------------------------------
-- instance of J2716 Rx module

  inst_J2716_rx_1: J2716_rx
    GENERIC MAP (
      g_reset_level  => )
    PORT MAP (
      i_reset        => i_reset,
      i_clock        => i_clock,
      i_rx           => ,
      i_activate     => ,
      i_tic_length   => ,
      i_data_nibbles => ,
      i_pause	     => ,
      i_crc_type     => ,
      o_error_code   => ,
      o_tic_length   => ,
      o_rx_status    => ,
      o_rx_data_n0   => ,
      o_rx_data_n1   => ,
      o_rx_data_n2   => ,
      o_rx_data_n3   => ,
      o_rx_data_n4   => ,
      o_rx_data_n5   => ,
      o_rx_crc	     => ,
      o_error        => ,
      o_pause_length => );
