#include <windows.h>
#include <cstdio>
int wmain(int argc, wchar_t** argv) {
    if (argc != 2) return 1;
    HMODULE module = LoadLibraryW(argv[1]);
    if (!module) { std::printf("LoadLibraryW failed: %lu\n", GetLastError()); return 1; }
    auto base = reinterpret_cast<const BYTE*>(module);
    auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    auto nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    auto dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    if (!dir.VirtualAddress) { FreeLibrary(module); return 1; }
    auto exports = reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(base + dir.VirtualAddress);
    auto functions = reinterpret_cast<const DWORD*>(base + exports->AddressOfFunctions);
    std::printf("Base=%lu NumberOfFunctions=%lu NumberOfNames=%lu\n", exports->Base, exports->NumberOfFunctions, exports->NumberOfNames);
    for (DWORD i = 0; i < exports->NumberOfFunctions; ++i) {
        DWORD ordinal = exports->Base + i;
        SetLastError(ERROR_SUCCESS);
        FARPROC address = GetProcAddress(module, MAKEINTRESOURCEA(ordinal));
        DWORD error = address ? ERROR_SUCCESS : GetLastError();
        std::printf("ordinal=%lu EAT_RVA=0x%08lX GetProcAddress=%p error=%lu%s\n", ordinal, functions[i], reinterpret_cast<void*>(address), error, functions[i] ? "" : " [HOLE: DO NOT CALL]");
    }
    FreeLibrary(module);
    return 0;
}
