@echo off
rem move /Y *.emf canoe
rem del /F *.emf
del /F *.mexw32
rmdir /S /Q slprj
rmdir /S /Q vector_susp_ctrl_canoe_cn_rtw
rmdir /S /Q vector_vehicle_canoe_cn_rtw
rmdir /S /Q Chassis_cn_rtw
rmdir /S /Q ECUDamperFL_cn_rtw
rmdir /S /Q ECUDamperFR_cn_rtw
rmdir /S /Q ECUDamperRL_cn_rtw
rmdir /S /Q ECUDamperRR_cn_rtw
del /S /Q canoe\*.lib canoe\*.exp
del blender_ground.txt

echo.
echo All temporary files had been deleted!
echo.
pause
