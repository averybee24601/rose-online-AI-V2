@echo off
setlocal EnableDelayedExpansion
cd /d %~dp0

rem Start LoginServer
if exist LoginServer.exe (
  start "LoginServer" cmd /c "LoginServer.exe & pause"
) else (
  echo [WARN] LoginServer.exe not found
)

rem Start CharServer
if exist CharServer.exe (
  start "CharServer" cmd /c "CharServer.exe & pause"
) else (
  echo [WARN] CharServer.exe not found
)

rem Start WorldServer
if exist WorldServer.exe (
  start "WorldServer" cmd /c "WorldServer.exe & pause"
) else (
  echo [WARN] WorldServer.exe not found
)

rem Start HealthWatcher
if exist HealthWatcher.ps1 (
  start "HealthWatcher" powershell -ExecutionPolicy Bypass -File "%~dp0HealthWatcher.ps1"
) else (
  echo [WARN] HealthWatcher.ps1 not found
)

rem Open Health Viewer
if exist .\log\health.txt (
  start notepad .\log\health.txt
) else (
  echo time=%date% %time%> .\log\health.txt
  echo connected_clients=0>> .\log\health.txt
  echo maxfd=0>> .\log\health.txt
  start notepad .\log\health.txt
)

endlocal

