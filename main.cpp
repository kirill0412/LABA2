#include <iostream>
#define WINVER 0x0502
#include <windows.h>
#include <sddl.h>
#include "module.cpp"

using namespace std;

int  main()
{
    string InputName="\\\\.\\mailslot\\test";
    cout << "[SYS] Enter mailslot fullpath: ";
    cin>> InputName;
    LPCTSTR SlotName= InputName.c_str();
    HANDLE hSlot;
    bool isServer;
    bool cf = MakeSlot(SlotName, hSlot, isServer);
    if (!cf) return -1;


    cout << "Start session in "<< ((isServer) ? "Server" : "Client") << " mode"<< endl;
    cout << "[SYS] Start session in "<< ((isServer) ? "Server" : "Client") << " mode"<< endl;
    string command;
    cout << "[SYS] Commands: "<<endl<<"\t\t[check] messages"<<endl<<((isServer) ? "\t\t[read] messages" : "\t\t[write] new message") <<endl<<"\t\t[quit] prorgam"<<endl;

    while(TRUE)
    {

        cout << "You can: "<<endl<<"\t[check] messeges"<<endl<<((isServer) ? "\t[read] messeges" : "\t[write] new messege") <<endl<<"\t[quit] prorgam"<<endl;
        cout <<"[IN] "<< InputName <<" > ";
        cin>> command;

        if(command ==  "check")
        {
            CheckMsgs(hSlot);
        }
        else if(command == "quit")
        {
            return 0;
        }
         else if((command == "read") && (isServer))
        {
            ReadSlot(hSlot);
        }
        else if ((command == "write") && (!isServer))
        {
            cout<<"[OUT] Type your message here. Type '.' in new line to terminate."<<endl;
            string msg,line, tmp;

            do
            {
                cin>> tmp;
                getline(cin, line);
                line = tmp + line;
                msg += line +'\n';
            }
            while(line!=".");

            WriteSlot(hSlot,(LPCTSTR)msg.c_str());
        }
        else
        {
            ;
        }
    }

    return 0;
}
