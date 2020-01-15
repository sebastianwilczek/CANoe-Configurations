@echo off

REM Check if perl is installed:

perl -v

if errorlevel 1 (
  echo.
  echo "Perl is NOT installed! Renaming is not possible."
  exit /b %errorlevel%
)

del rename_emf.log
rename_emf.pl suspension_control_system 1
rename_emf.pl vector_susp_ctrl_canoe 1
rename_emf.pl vector_vehicle_canoe 1
rename_emf.pl vehicle_dynamics_system 1
rename_emf.pl ECUDamperFL 1
rename_emf.pl ECUDamperFR 1
rename_emf.pl ECUDamperRL 1
rename_emf.pl ECUDamperRR 1

rename_emf.pl suspension_control_system 2
rename_emf.pl vector_susp_ctrl_canoe 2
rename_emf.pl vector_vehicle_canoe 2
rename_emf.pl vehicle_dynamics_system 2
rename_emf.pl ECUDamperFL 2
rename_emf.pl ECUDamperFR 2
rename_emf.pl ECUDamperRL 2
rename_emf.pl ECUDamperRR 2

pause
