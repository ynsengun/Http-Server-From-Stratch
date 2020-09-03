#include "Session.h"

Session::Session(int _sessionID){
    sessionID = _sessionID;
}

void Session::printSession(){
    cout<<"Session id: "<<sessionID<<endl;
}