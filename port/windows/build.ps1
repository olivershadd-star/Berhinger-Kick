param(
    [string]$JuceSource = "",
    [string]$BuildDirectory = "build-original-windows"
)
$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path
$Arguments = @("-S", $Root, "-B", $BuildDirectory, "-A", "x64")
if ($JuceSource) { $Arguments += "-DKICK_JUCE_SOURCE=$JuceSource" }
& cmake @Arguments
if ($LASTEXITCODE -ne 0) { throw "Preparation de KICK impossible : voir le diagnostic CMake." }
& cmake --build $BuildDirectory --config Release
if ($LASTEXITCODE -ne 0) { throw "Compilation originale incomplete : voir les erreurs du compilateur. Aucun ZIP portable valide n'a ete produit." }
Write-Host "Compilation terminee. Verifier encore l'interface, le son et les projets avant publication portable."
