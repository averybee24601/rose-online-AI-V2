@echo off
cd /d %~dp0
if not exist .\log mkdir .\log
start notepad .\log\health.txt

