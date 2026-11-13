#include <windows.h>
#include <cstdio>
#include <cwchar>

using AddAtomWFn = ATOM (WINAPI *)(LPCWSTR);

int wmain(int argc, wchar_t* argv[])
{
    if (argc < 2) {
        std::fwprintf(stderr, L"Usage: ordtest.exe <absolute-dll-path> [--call]\n");
        return 1;
    }

    HMODULE dll = LoadLibraryW(argv[1]);
    if (!dll) {
        const DWORD error = GetLastError();
        std::printf("LoadLibraryW failed: %lu\n", error);
        return 1;
    }

    wchar_t loadedPath[MAX_PATH]{};
    if (GetModuleFileNameW(dll, loadedPath, MAX_PATH)) {
        std::wprintf(L"Loaded: %ls\n", loadedPath);
    }

    FARPROC byName = GetProcAddress(dll, "AddAtomW");
    const DWORD nameError = byName ? ERROR_SUCCESS : GetLastError();

    FARPROC byOrdinal = GetProcAddress(dll, MAKEINTRESOURCEA(3));
    const DWORD ordinalError = byOrdinal ? ERROR_SUCCESS : GetLastError();

    std::wprintf(L"AddAtomW by name:  %ls (error %lu)\n",
                 byName ? L"found" : L"missing", nameError);
    std::wprintf(L"AddAtomW by #3:    %ls (error %lu)\n",
                 byOrdinal ? L"found" : L"missing", ordinalError);

    if (byOrdinal && argc >= 3 && std::wcscmp(argv[2], L"--call") == 0) {
        auto addAtom = reinterpret_cast<AddAtomWFn>(byOrdinal);
        const ATOM atom = addAtom(L"OrdinalTest");
        std::wprintf(L"AddAtomW returned: 0x%04X\n",
                     static_cast<unsigned>(atom));
        if (atom != 0) {
            DeleteAtom(atom);
        }
    }

    FreeLibrary(dll);
    return byOrdinal ? 0 : 1;
}