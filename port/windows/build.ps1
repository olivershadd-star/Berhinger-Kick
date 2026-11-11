param(
    [string]$AudioSource = "",
    [string]$SerialPortSource = "",
    [string]$JuceSource = "",
    [string]$BuildDirectory = "build-original-windows"
)
$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path
$Arguments = @("-S", $Root, "-B", $BuildDirectory, "-A", "x64")
if ($AudioSource) { $Arguments += "-DKICK_AUDIO_SOURCE=$AudioSource" }
if ($SerialPortSource) { $Arguments += "-DKICK_SERIALPORT_SOURCE=$SerialPortSource" }
if ($JuceSource) { $Arguments += "-DKICK_JUCE_SOURCE=$JuceSource" }
& cmake @Arguments
if ($LASTEXITCODE -ne 0) { throw "KICK original bloque : voir les dependances signalees. Aucun executable de substitution n'a ete cree." }
& cmake --build $BuildDirectory --config Release
if ($LASTEXITCODE -ne 0) { throw "La compilation du code original a echoue. Ne pas presenter cette etape comme un portage fonctionnel." }
Write-Host "Compilation terminee. L'interface, le son et les projets KICK doivent encore etre verifies avant toute publication portable."
