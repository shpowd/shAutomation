
@echo off
rem This script was created by Nuitka to execute 'pex.exe' with Python DLL being found.
set PATH=c:\users\admin\anaconda3\envs\plc_modbus;%PATH%
set PYTHONHOME=C:\Users\admin\anaconda3\envs\plc_modbus
set NUITKA_PYTHONPATH=C:\Users\admin\Desktop\code\git_code\shAutomation\exetest;C:\Users\admin\anaconda3\envs\plc_modbus\DLLs;C:\Users\admin\anaconda3\envs\plc_modbus\Lib;C:\Users\admin\anaconda3\envs\plc_modbus;C:\Users\admin\anaconda3\envs\plc_modbus\Lib\site-packages;C:\Users\admin\anaconda3\envs\plc_modbus\Lib\site-packages\setuptools\_vendor
"%~dp0pex.exe" %*
