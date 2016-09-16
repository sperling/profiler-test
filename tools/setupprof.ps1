#run with . ./setupprof.ps1

function Get-ScriptDirectory
{
    $Invocation = (Get-Variable MyInvocation -Scope 1).Value
    Split-Path $Invocation.MyCommand.Path
}

$path = [System.IO.Path]::GetFullPath((Join-Path $(Get-ScriptDirectory) "..\bin\Product\Windows_NT.x64.Debug\profiler-test"));

[Environment]::SetEnvironmentVariable("COMPlus_LogEnable", 1)
[Environment]::SetEnvironmentVariable("COMPlus_LogToFile", 1)

[Environment]::SetEnvironmentVariable("COMPlus_JitDisasm", "*")
[Environment]::SetEnvironmentVariable("COMPlus_JitDiffableDasm", 1)

[Environment]::SetEnvironmentVariable("CORECLR_PROFILER_PATH", $path)
[Environment]::SetEnvironmentVariable("CORECLR_ENABLE_PROFILING", 1)
[Environment]::SetEnvironmentVariable("CORECLR_PROFILER", "{C4D6E538-1AF1-44D0-92C0-5525DE10B726}")