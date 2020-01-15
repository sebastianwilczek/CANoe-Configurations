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
--  $Workfile: J2716_tx_cmp.vhd $
--  $Author: visjt $
--  $Date: 2014-08-20 08:13:05 +0200 (Mi, 20 Aug 2014) $
--  $Revision: 34694 $
--  Abstract      :
--============================================================================


------------------------------------------------------------------------
-- Signal Description

--  SIGNAL          ; DIRECTION ; WIDTH ; DESCRIPTION
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_reset         ; IN        ; 1     ; Control: reset signal - active reset level is
--                  ;           ;       ;  defined via generic g_reset_level
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_clock         ; IN        ; 1     ; Control: clock signal
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_activate      ; IN        ; 1     ; Control: activation signal of module
--                  ;           ;       ;  0 - module is inactive
--                  ;           ;       ;  1 - module is active
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_cyclic_tx     ; IN        ; 1     ; Config: Transmission Mode
--                  ;           ;       ;  0 - single transmission of config data
--                  ;           ;       ;  1 - cyclic transmission
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_tx_start      ; IN        ; 1     ; Control: starts transmission if transmission mode is
--                  ;           ;       ;  configured to single transmission (i_cyclic_tx = '0') and
--                  ;           ;       ;  o_tx_active = '0'
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_tic_length    ; IN        ; 10    ; Config: length of a tic (length is defined in clock tics)
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_low_ticks     ; IN        ; 3     ; Config: number of clock tics driven low time
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_status_nibble ; IN        ; 4     ; Config: transmit value of Status nibble
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_data_nibbles  ; IN        ; 3     ; Config: number of data nibbles of a J2716 frame
--                  ;           ;       ;  accepted values are 1 to 6
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_tx_data_n0    ; IN        ; 4     ; Config: transmit value of Data1 nibble
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_tx_data_n1    ; IN        ; 4     ; Config: transmit value of Data2 nibble
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_tx_data_n2    ; IN        ; 4     ; Config: transmit value of Data3 nibble
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_tx_data_n3    ; IN        ; 4     ; Config: transmit value of Data4 nibble
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_tx_data_n4    ; IN        ; 4     ; Config: transmit value of Data5 nibble
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_tx_data_n5    ; IN        ; 4     ; Config: transmit value of Data6 nibble
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_crc_type      ; IN        ; 1     ; Config: defines which CRC type is used in the J2716 frame
--                  ;           ;       ;  0 - CRC type J2716-FEB2008 (legacy mode)
--                  ;           ;       ;  1 - CRC type J2716-JAN2010 (recommended mode)
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_pause         ; IN        ; 1     ; Config: a pause pulse is on bus after J2716 frame (pause
--                  ;           ;       ;  pulse is supported since version J2716-JAN2010)
--                  ;           ;       ;  0 - no pause on bus
--                  ;           ;       ;  1 - pause on bus
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_pause_type    ; IN        ; 1     ; Config: two types of pause pulses are supported
--                  ;           ;       ;  0 - sum of length of J2716 frame + pause pulse is fixed,
--                  ;           ;       ;      the length is configured in i_pause_length
--                  ;           ;       ;  1 - each pause pulse has a constant length (regardless of
--                  ;           ;       ;      the frame length) as defined in i_pause_length
-- -----------------+-----------+-------+-----------------------------------------------------------
--  i_pause_length  ; IN        ; 11    ; Config: length of pause pulse/complete J2716 frame (depends
--                  ;           ;       ;  on configured  pause type (length is defined in tics)
-- -----------------+-----------+-------+-----------------------------------------------------------
--  o_tx_active     ; OUT       ; 1     ; status signal: transmission status
--                  ;           ;       ;  0 - no transmission is running
--                  ;           ;       ;  1 - transmission is running
-- -----------------+-----------+-------+-----------------------------------------------------------
--  o_tx            ; OUT       ; 1     ; transmit signal of J2716 bus
-- -----------------+-----------+-------+-----------------------------------------------------------



------------------------------------------------------------------------
-- component of J2716 Tx module

COMPONENT J2716_tx
  GENERIC (
    g_reset_level : std_logic);
  PORT (
    i_reset         : IN  std_logic;
    i_clock         : IN  std_logic;
    i_activate      : IN  std_logic;
    i_cyclic_tx     : IN  std_logic;
    i_tx_start      : IN  std_logic;
    i_tic_length    : IN  std_logic_vector(9 DOWNTO 0);
    i_low_ticks     : IN  std_logic_vector(2 DOWNTO 0);
    i_status_nibble : IN  std_logic_vector(3 DOWNTO 0);
    i_data_nibbles  : IN  std_logic_vector(2 DOWNTO 0);
    i_tx_data_n0    : IN  std_logic_vector(3 DOWNTO 0);
    i_tx_data_n1    : IN  std_logic_vector(3 DOWNTO 0);
    i_tx_data_n2    : IN  std_logic_vector(3 DOWNTO 0);
    i_tx_data_n3    : IN  std_logic_vector(3 DOWNTO 0);
    i_tx_data_n4    : IN  std_logic_vector(3 DOWNTO 0);
    i_tx_data_n5    : IN  std_logic_vector(3 DOWNTO 0);
    i_crc_type      : IN  std_logic;
    i_pause         : IN  std_logic;
    i_pause_type    : IN  std_logic;
    i_pause_length  : IN  std_logic_vector(10 DOWNTO 0);
    o_tx_active     : OUT std_logic;
    o_tx            : OUT std_logic);
END COMPONENT;

------------------------------------------------------------------------
-- instance of J2716 Tx module

inst_J2716_tx : J2716_tx
  GENERIC MAP (
    g_reset_level => )
  PORT MAP (
    i_reset         => i_reset,
    i_clock         => i_clock,
    i_activate      => ,
    i_cyclic_tx     => ,
    i_tx_start      => ,
    i_tic_length    => ,
    i_low_ticks     => ,
    i_status_nibble => ,
    i_data_nibbles  => ,
    i_tx_data_n0    => ,
    i_tx_data_n1    => ,
    i_tx_data_n2    => ,
    i_tx_data_n3    => ,
    i_tx_data_n4    => ,
    i_tx_data_n5    => ,
    i_crc_type      => ,
    i_pause         => ,
    i_pause_type    => ,
    i_pause_length  => ,
    o_tx_active     => ,
    o_tx            => );
