#include "Session.h"

int Session::lastSessionID = 0;

Session::Session(int _sessionID){
    sessionID = _sessionID;
}

int Session::getNextSessionID(){
    // TODO lock
    return lastSessionID++;
}

int Session::getSessionID(){
    return sessionID;
}

void Session::pushData(string new_data){
    data.push_back(new_data);
}

vector<string> Session::getData(){
    return data;
}

void Session::setData(vector<string> _data){
    data = _data;
}

Request Session::getNextRequest(){
    Request ret = requests.front();
    requests.pop();
    return ret;
}

void Session::pushNewRequest(Request req){
    requests.push(req);
}

bool Session::hasNextRequest(){
    return !requests.empty();
}