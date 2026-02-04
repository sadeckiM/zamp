#ifndef __COMCHANNEL_HH__
#define __COMCHANNEL_HH__

#include "AbstractComChannel.hh"
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Port.hh"
#include <iostream>
#include <cstring>
#include <unistd.h>

class ComChannel : public AbstractComChannel {
    int _Socket;
    std::mutex _Mutex;
public:
    ComChannel() {}
    virtual void Init(int Socket) override {_Socket = Socket;}
    virtual int GetSocket() const override {return _Socket;}
    virtual void LockAccess() override {_Mutex.lock();}
    virtual void UnlockAccess() override {_Mutex.unlock();}
    virtual std::mutex &UseGuard() override {return _Mutex;}
    virtual bool OpenConnection() override;
    virtual int Send(const char *sMesg) override;
};

#endif