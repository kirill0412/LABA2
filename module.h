#include <iostream>
#define WINVER 0x0502
#include <windows.h>
#include <sddl.h>

using namespace std;

static PSECURITY_DESCRIPTOR create_security_descriptor();

static SECURITY_ATTRIBUTES create_security_attributes();

BOOL WINAPI MakeSlot(LPCTSTR lpszSlotNameHANDLE, HANDLE& hSlot, bool& isServer);

BOOL WriteSlot(HANDLE hSlot, LPCTSTR lpszMessage);

BOOL ReadSlot(HANDLE hSlot);

BOOL CheckMsgs(HANDLE hSlot, DWORD* cbMessage = NULL, DWORD* cMessage = NULL);
