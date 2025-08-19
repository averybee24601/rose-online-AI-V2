@echo off
setlocal EnableDelayedExpansion
cd /d %~dp0

:: Watchdog.bat - restarts servers if they exit and logs restarts.
:: Non-invasive. Keep this window open to keep watchdog running.

set LOG=watchdog.log
echo [%date% %time%] Watchdog started.>> %LOG%

:loop
  :: LoginServer
  tasklist /fi "imagename eq LoginServer.exe" | find /i "LoginServer.exe" >nul
  if errorlevel 1 (
    if exist LoginServer.exe (
      echo [%date% %time%] Restarting LoginServer...>> %LOG%
      start "LoginServer" cmd /c "LoginServer.exe ^& pause"
    )
  )

  :: CharServer
  tasklist /fi "imagename eq CharServer.exe" | find /i "CharServer.exe" >nul
  if errorlevel 1 (
    if exist CharServer.exe (
      echo [%date% %time%] Restarting CharServer...>> %LOG%
      start "CharServer" cmd /c "CharServer.exe ^& pause"
    )
  )

  :: WorldServer
  tasklist /fi "imagename eq WorldServer.exe" | find /i "WorldServer.exe" >nul
  if errorlevel 1 (
    if exist WorldServer.exe (
      echo [%date% %time%] Restarting WorldServer...>> %LOG%
      start "WorldServer" cmd /c "WorldServer.exe ^& pause"
    )
  )

  :: HealthWatcher
  tasklist /fi "imagename eq powershell.exe" | find /i "HealthWatcher.ps1" >nul
  if errorlevel 1 (
    if exist HealthWatcher.ps1 (
      echo [%date% %time%] Restarting HealthWatcher...>> %LOG%
      start "HealthWatcher" powershell -ExecutionPolicy Bypass -File "%~dp0HealthWatcher.ps1"
    )
  )

  timeout /t 5 /nobreak >nul
  goto loop

endlocal

