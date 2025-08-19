param(
  [int]$IntervalSeconds = 5
)

# HealthWatcher.ps1
# Non-invasive health snapshot generator for AI-Rose-Evolved servers
# Writes:
#   .\log\health.txt (snapshot)
#   .\log\health_history.log (append)
# Does not require rebuilding the servers.

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Get-PortFromConf($confFile, $keys) {
  $confPath = Join-Path -Path (Get-Location) -ChildPath $confFile
  if (-not (Test-Path $confPath)) { return $null }
  $text = Get-Content -Raw $confPath -ErrorAction SilentlyContinue
  if (-not $text) { return $null }
  foreach ($k in $keys) {
    $m = [regex]::Match($text, [regex]::Escape($k) + '\s*=\s*(\d+)', 'IgnoreCase')
    if ($m.Success) { return [int]$m.Groups[1].Value }
  }
  return $null
}

function Get-ConnectedCount([int]$port) {
  try {
    $count = (Get-NetTCPConnection -LocalPort $port -State Established -ErrorAction SilentlyContinue | Measure-Object).Count
    if (-not $count) { return 0 }
    return [int]$count
  } catch {
    return 0
  }
}

function Read-ServerStats($name, $exe, $port) {
  $p = Get-Process -Name $exe -ErrorAction SilentlyContinue | Select-Object -First 1
  $cpu = if ($p) { '{0:N2}' -f ($p.CPU) } else { '0.00' }
  $memMB = if ($p) { [math]::Round($p.WorkingSet64 / 1MB, 2) } else { 0 }
  $conns = if ($port) { Get-ConnectedCount -port $port } else { 0 }
  [pscustomobject]@{
    Name=$name; CPU=$cpu; MemMB=$memMB; Port=$port; Connections=$conns
  }
}

$logDir = Join-Path -Path (Get-Location) -ChildPath 'log'
if (-not (Test-Path $logDir)) { New-Item -ItemType Directory -Path $logDir | Out-Null }

$worldPort = Get-PortFromConf 'worldserver.conf' @('WorldPort','Port'); if (-not $worldPort) { $worldPort = 29200 }
$charPort  = Get-PortFromConf 'charserver.conf'  @('CharPort','Port');  if (-not $charPort)  { $charPort  = 29100 }
$loginPort = Get-PortFromConf 'loginserver.conf' @('LoginPort','Port'); if (-not $loginPort) { $loginPort = 29000 }

Write-Host "[HealthWatcher] Monitoring ports - Login:$loginPort Char:$charPort World:$worldPort" -ForegroundColor Green

while ($true) {
  $ts = Get-Date -Format 'yyyy-MM-dd HH:mm:ss'
  $login = Read-ServerStats -name 'LoginServer' -exe 'LoginServer' -port $loginPort
  $char  = Read-ServerStats -name 'CharServer'  -exe 'CharServer'  -port $charPort
  $world = Read-ServerStats -name 'WorldServer' -exe 'WorldServer' -port $worldPort

  $snapshot = @(
    "time=$ts",
    "Login:  cpu=$($login.CPU)s mem=${($login.MemMB)}MB port=$($login.Port) connections=$($login.Connections)",
    "Char:   cpu=$($char.CPU)s mem=${($char.MemMB)}MB port=$($char.Port) connections=$($char.Connections)",
    "World:  cpu=$($world.CPU)s mem=${($world.MemMB)}MB port=$($world.Port) connections=$($world.Connections)"
  ) -join "`r`n"

  $healthTxt = Join-Path $logDir 'health.txt'
  $historyLog = Join-Path $logDir 'health_history.log'

  Set-Content -Path $healthTxt -Value $snapshot -Encoding UTF8
  Add-Content -Path $historyLog -Value "$ts L:$($login.Connections) C:$($char.Connections) W:$($world.Connections)" -Encoding UTF8

  Start-Sleep -Seconds $IntervalSeconds
}

