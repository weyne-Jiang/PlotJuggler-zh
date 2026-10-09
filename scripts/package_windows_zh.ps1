# SPDX-License-Identifier: MPL-2.0
param(
    [Parameter(Mandatory = $true)][string]$BinaryDirectory,
    [Parameter(Mandatory = $true)][string]$BinaryCreator,
    [string]$OutputDirectory,
    [string]$Version = '3.17.2-zh.1'
)
$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path $PSScriptRoot -Parent
if (-not $OutputDirectory) { $OutputDirectory = Join-Path $repoRoot 'build-release' }
$binaryRoot = (Resolve-Path -LiteralPath $BinaryDirectory).Path
$creator = (Resolve-Path -LiteralPath $BinaryCreator).Path
if (-not (Test-Path (Join-Path $binaryRoot 'plotjuggler.exe'))) { throw 'Missing application' }
# A fresh staging directory prevents old package files from entering a new installer.
New-Item -ItemType Directory -Force $OutputDirectory | Out-Null
$stage = Join-Path $OutputDirectory ('stage-' + [Guid]::NewGuid().ToString('N'))
$packages = Join-Path $stage 'packages'
$component = Join-Path $packages 'io.plotjuggler.zh'
$data = Join-Path $component 'data'
New-Item -ItemType Directory -Force $data | Out-Null
Copy-Item (Join-Path $repoRoot 'installer/chinese/io.plotjuggler.zh/meta') $component -Recurse
Copy-Item (Join-Path $repoRoot 'installer/io.plotjuggler.application/meta/license_mpl.txt') (Join-Path $component 'meta')
Copy-Item (Join-Path $repoRoot 'installer/io.plotjuggler.application/meta/license_lgpl.txt') (Join-Path $component 'meta')
Get-ChildItem -LiteralPath $binaryRoot | Where-Object {
    $_.Name -notmatch '^(native_language_qa|language_manager_test|preferences_language_test|test_|runtime-smoke|vc_redist)' -and
    $_.Extension -notin @('.lib', '.exp', '.pdb') -and $_.Name -ne 'Qt5Test.dll'
} | Copy-Item -Destination $data -Recurse
Copy-Item (Join-Path $repoRoot 'README.zh-CN.md'), (Join-Path $repoRoot 'VALIDATION.zh-CN.md') $data -Force
$installer = Join-Path (Resolve-Path -LiteralPath $OutputDirectory) "PlotJuggler-$Version-Windows-x64.exe"
& $creator --offline-only -c (Join-Path $repoRoot 'installer/chinese/config.xml') -p $packages $installer
if ($LASTEXITCODE -ne 0) { throw 'Qt installer creation failed' }
$hash = (Get-FileHash -LiteralPath $installer -Algorithm SHA256).Hash.ToLowerInvariant()
"$hash  $([IO.Path]::GetFileName($installer))" | Set-Content -Encoding ascii ($installer + '.sha256')
Write-Output $installer
