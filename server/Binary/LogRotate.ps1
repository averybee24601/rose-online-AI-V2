param(
  [int]$MaxSizeMB = 20,
  [int]$MaxBackups = 5
)

# LogRotate.ps1 - rotates large log files in server/Binary/log and server/Binary/plog
# Non-invasive; safe to run while servers are running (rotation is rename-only + recreate).

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Rotate-OneFile([string]$filePath, [int]$maxSizeBytes, [int]$maxBackups) {
  if (-not (Test-Path $filePath)) { return }
  $fi = Get-Item $filePath -ErrorAction SilentlyContinue
  if (-not $fi) { return }
  if ($fi.Length -lt $maxSizeBytes) { return }

  $dir = Split-Path -Parent $filePath
  $base = Split-Path -Leaf $filePath

  # Shift backups: base.(n-1) -> base.n
  for ($i = $maxBackups; $i -ge 1; $i--) {
    $old = Join-Path $dir ("$base.$i")
    $older = Join-Path $dir ("$base." + ($i + 1))
    if (Test-Path $older) { Remove-Item -Force $older -ErrorAction SilentlyContinue }
    if (Test-Path $old) { Rename-Item -Force $old $older -ErrorAction SilentlyContinue }
  }
  # Current -> .1
  $one = Join-Path $dir ("$base.1")
  if (Test-Path $one) { Remove-Item -Force $one -ErrorAction SilentlyContinue }
  Rename-Item -Force $filePath $one
  # Recreate empty current file
  New-Item -ItemType File -Path $filePath -Force | Out-Null
}

$here = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $here

$logDir  = Join-Path $here 'log'
$plogDir = Join-Path $here 'plog'
$targets = @()
if (Test-Path $logDir)  { $targets += Get-ChildItem -Path $logDir  -File -ErrorAction SilentlyContinue }
if (Test-Path $plogDir) { $targets += Get-ChildItem -Path $plogDir -File -ErrorAction SilentlyContinue }

$maxBytes = $MaxSizeMB * 1MB
foreach ($t in $targets) {
  Rotate-OneFile -filePath $t.FullName -maxSizeBytes $maxBytes -maxBackups $MaxBackups
}

Write-Host "[LogRotate] Rotation complete. Checked $($targets.Count) files. MaxSizeMB=$MaxSizeMB MaxBackups=$MaxBackups" -ForegroundColor Green

