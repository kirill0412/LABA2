#include "module.h"

static PSECURITY_DESCRIPTOR create_security_descriptor()
{
    const char* sddl = "D:(A;OICI;GRGW;;;AU)(A;OICI;GA;;;BA)";
    PSECURITY_DESCRIPTOR security_descriptor = NULL;
    ConvertStringSecurityDescriptorToSecurityDescriptor(sddl, SDDL_REVISION_1, &security_descriptor, NULL);
    return security_descriptor;
}
static SECURITY_ATTRIBUTES create_security_attributes()
{
    SECURITY_ATTRIBUTES attributes;
    attributes.nLength = sizeof(attributes);
    attributes.lpSecurityDescriptor = create_security_descriptor();
    attributes.bInheritHandle = FALSE;
    return attributes;
}

BOOL WINAPI MakeSlot(LPCTSTR lpszSlotName, HANDLE& hSlot, bool& isServer)
{
    bool exist = FALSE;
    string s(lpszSlotName);
    if (s.find("\\\\.\\") !=0)
    {
        isServer = FALSE;
        HANDLE hFile;

        hSlot = CreateFile(lpszSlotName,
                           GENERIC_WRITE,
                           FILE_SHARE_READ,
                           (LPSECURITY_ATTRIBUTES) NULL,
                           OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL,
                           (HANDLE) NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            auto err =  GetLastError();
            cout << "Error while trying to connect to remote Mailslot. Error code: " <<err;
            CloseHandle(hFile);
            return FALSE;
        }
        else
        {
            CloseHandle(hFile);
            return TRUE;
        }

    }

    auto attributes = create_security_attributes();
    hSlot = CreateMailslot(lpszSlotName,
                           0,
                           MAILSLOT_WAIT_FOREVER,
                           &attributes);
    if (hSlot == INVALID_HANDLE_VALUE)
    {
        auto err =  GetLastError();

        if (err == ERROR_ALREADY_EXISTS)
        if (err == ERROR_ALREADY_EXISTS || err ==ERROR_INVALID_NAME)
        {
            isServer = FALSE;
            hSlot = CreateFile(lpszSlotName,
                               GENERIC_WRITE,
                               FILE_SHARE_READ,
                               (LPSECURITY_ATTRIBUTES) NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               (HANDLE) NULL);


            if (hSlot == INVALID_HANDLE_VALUE)
            {
                auto err =  GetLastError();
                cout << "Error while trying to connect to Mailslot. Error code: " <<err;
                cout << "[ERR] Error while trying to connect to Mailslot. Error code: " <<err<<endl;
                return FALSE;
            }
            DWORD cbMessageL, cMessageL;
            BOOL fResult;

            fResult = GetMailslotInfo( hSlot, (LPDWORD) NULL,(LPDWORD) NULL,(LPDWORD) NULL,(LPDWORD) NULL);
            if (!fResult)
            {
                auto err =  GetLastError();
                cout << "[ERR] Something wrong with mailslot connection. May be you enter incorrect net path. GetMailslotInfo() error code: " <<err<<endl;
                return FALSE;
            }


        }
        else
        {
            cout << "Error while creating Mailslot. Error code: " <<err;
            cout << "[ERR] Error while creating Mailslot. Error code: " <<err;
            return FALSE;
        }
    }
    else
    {
        isServer = TRUE;
    }
    return TRUE;
}
BOOL CheckMsgs(HANDLE hSlot, DWORD* cbMessage, DWORD* cMessage )
{
    DWORD cbMessageL, cMessageL;
    BOOL fResult;
    fResult = GetMailslotInfo( hSlot,
                               (LPDWORD) NULL,
                               &cbMessageL,
                               &cMessageL,
                               (LPDWORD) NULL);

    if (fResult)
    {
        cout <<"Found "<< cMessageL<<" in active mailslot"<<endl ;
        cout <<"[OUT] Found "<< cMessageL<<" messages in active mailslot"<<endl ;
        if (cbMessage != NULL) *cbMessage = cbMessageL;
        if (cMessage != NULL) *cMessage = cMessageL;
        return TRUE;
    }
    else return FALSE;
    else
    {

        cout<<"[ERR] GetMailslotInfo failed with "<<GetLastError()<<endl;
        return FALSE;
    }

}


BOOL WriteSlot(HANDLE hSlot, LPCTSTR lpszMessage)
{
    cout <<"Start write file"<<endl ;

    BOOL fResult;
    DWORD cbWritten;

    fResult = WriteFile(hSlot,
                        lpszMessage,
                        (DWORD) (lstrlen(lpszMessage)+1)*sizeof(TCHAR),
                        &cbWritten,
                        (LPOVERLAPPED) NULL);

    if (!fResult)
    {
        printf("WriteFile failed with %d.\n", GetLastError());
        cout<<"[ERR] WriteFile failed with "<<GetLastError()<<endl;
        return FALSE;
    }

    printf("Slot written to successfully.\n");
    cout<<"[OUT] Your message has been successfully sent!"<<endl;

    return TRUE;
}
BOOL ReadSlot(HANDLE hSlot)
{
    DWORD cbMessage, cMessage, cbRead;
    BOOL fResult;
    cbMessage = cMessage = cbRead = 0;
    fResult = GetMailslotInfo( hSlot,
                               (LPDWORD) NULL,
                               &cbMessage,
                               &cMessage,
                               (LPDWORD) NULL);

    if (!fResult)
    {
        printf("GetMailslotInfo failed with %d.\n", GetLastError());
        cout<<"[ERR] GetMailslotInfo failed with "<<GetLastError()<<endl;
        return FALSE;
    }

    if (cbMessage == MAILSLOT_NO_MESSAGE)
    {
        printf("Waiting for a message...\n");
        cout<< "[OUT] No messages found in the active mailslot" << endl;
        return TRUE;
    }
    cout<< "Found "<<cMessage<<" messages" << endl;


    string Buffer(cbMessage,'\0');

    fResult = ReadFile(hSlot,
                       &Buffer[0],
                       Buffer.size(),
                       &cbRead,
                       NULL);

    if (!fResult)
        {
            printf("ReadFile failed with %d.\n", GetLastError());
            return FALSE;
        }
    {
        printf("[ERR] ReadFile failed with %d.\n", GetLastError());
        return FALSE;
    }

    cout<<Buffer<<endl;

    return TRUE;
}
