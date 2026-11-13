# PE export experiments

Windows x86 DLL experiments comparing named and ordinal-only exports, with a deliberately empty EAT slot. DLLs are built this way by the linker; no binary fields are patched afterward.

## Requirements and build

Use Visual Studio C++ tools and the Windows SDK in an x86 developer PowerShell environment (cl must target x86).

```powershell
.\build.ps1
.\ordtest.exe (Resolve-Path .\atom_named.dll).Path --call
.\ordtest.exe (Resolve-Path .\atom_ordinal.dll).Path --call
.\eat_probe.exe (Resolve-Path .\atom_named.dll).Path
.\eat_probe.exe (Resolve-Path .\atom_ordinal.dll).Path
```

## Export layout

| Public ordinal | EAT content |
| --- | --- |
| 1 | TestAnchor function RVA |
| 2 | Zero RVA: unassigned slot |
| 3 | TestAddAtomW function RVA |

atom_named.def exposes Anchor and AddAtomW by name and ordinal. atom_ordinal.def adds NONAME for both functions, producing NumberOfNames=0. Both DLLs have Base=1 and NumberOfFunctions=3, including the hole.

The wrapper calls the real Windows AddAtomW. Its name therefore still appears in the DLL imports even when no export names are present.

## Programs

- atom_test.cpp: Anchor and AddAtomW wrapper implementations.
- ordtest.cpp: loads the selected DLL, resolves AddAtomW by name and ordinal 3, and optionally calls ordinal 3. Releases the returned atom with DeleteAtom.
- eat_probe.cpp: prints the export fields, each EAT RVA, and the GetProcAddress result. It never calls the inspected entries.

## Observed results

Verified on the development Windows machine with MSVC x86:

| DLL | AddAtomW by name | Ordinal 3 lookup and call |
| --- | --- | --- |
| atom_named.dll | Success | Success |
| atom_ordinal.dll | NULL, error 127 | Success |

Ordinal 2 had EAT RVA zero and returned NULL with error 182 on this machine. Do not assume identical hole lookup behavior on every Windows version; never call an empty EAT slot.

Inspect the export directory to distinguish an absent export name from an import string. Export names pair with AddressOfNameOrdinals entries, whose values index the EAT. Ordinal lookup instead indexes the EAT using public ordinal minus Base.

File offsets and RVAs depend on the build. This repository does not yet test missing OriginalFirstThunk or malformed exports with names present but no name-ordinal mapping.

Generated binaries and original book example files are excluded from version control.
