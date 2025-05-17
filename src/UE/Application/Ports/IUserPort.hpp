#pragma once
#include "Messages/PhoneNumber.hpp"
#include <vector>
#include "SmsRepository/SmsEntity.h"


namespace ue
{

class IUserEventsHandler
{
   public:
    virtual void viewSmsList() = 0;
    virtual void viewSms(unsigned) = 0;
    virtual void sendSms(const SmsEntity& sms) = 0;
    virtual void composeSms() = 0;
    virtual ~IUserEventsHandler() = default;
    virtual void startDial() = 0;
    virtual void sendCallRequest(common::PhoneNumber number) = 0;
    virtual void cancelCallRequest() = 0;
};

class IUserPort
{
   public:
    virtual ~IUserPort() = default;

    virtual void showNotConnected() = 0;
    virtual void showConnecting() = 0;
    virtual void showConnected() = 0;
    virtual void showSmsList(const std::vector<SmsEntity> &) = 0;
    virtual void showSms(const SmsEntity &) = 0;
    virtual void composeSms() = 0;
    virtual void showNewSms() = 0;
    virtual common::PhoneNumber getPhoneNumber() const = 0;
    virtual void startDial() = 0;
    virtual void showDialing() = 0;
    virtual void showTalking() = 0;
    virtual void showPartnerNotAvailable() = 0;
};

}
