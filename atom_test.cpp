#include <windows.h>
extern "C" ATOM WINAPI TestAddAtomW(LPCWSTR name) { return ::AddAtomW(name); }
extern "C" DWORD WINAPI TestAnchor() { return 0x12345678; }
