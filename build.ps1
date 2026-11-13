$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    cl /nologo /W4 /LD atom_test.cpp kernel32.lib /link /DEF:atom_named.def /OUT:atom_named.dll
    if ($LASTEXITCODE -ne 0) { throw 'atom_named build failed' }
    cl /nologo /W4 /LD atom_test.cpp kernel32.lib /link /DEF:atom_ordinal.def /OUT:atom_ordinal.dll
    if ($LASTEXITCODE -ne 0) { throw 'atom_ordinal build failed' }
    cl /nologo /W4 ordtest.cpp /link /OUT:ordtest.exe
    if ($LASTEXITCODE -ne 0) { throw 'ordtest build failed' }
    cl /nologo /W4 eat_probe.cpp /link /OUT:eat_probe.exe
    if ($LASTEXITCODE -ne 0) { throw 'eat_probe build failed' }
} finally {
    Pop-Location
}
