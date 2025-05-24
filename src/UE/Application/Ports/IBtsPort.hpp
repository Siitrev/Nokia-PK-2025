#pragma once

#include "Messages/BtsId.hpp"
#include "Messages/PhoneNumber.hpp"
#include "SmsRepository/SmsEntity.h"

namespace ue
{

class IBtsEventsHandler
{
public:
    virtual ~IBtsEventsHandler() = default;

    virtual void handleSib(common::BtsId) = 0;
    virtual void handleAttachAccept() = 0;
    virtual void handleAttachReject() = 0;
    virtual void handleDisconnect() = 0;
    virtual void handleSms(common::PhoneNumber from, std::string text) = 0;
    virtual void handleSmsDeliveryFailure(common::PhoneNumber from) = 0;
    virtual void handleCallAccepted(common::PhoneNumber from) = 0;
    virtual void handleCallDropped() = 0;
    virtual void handleCallRecipientNotAvailable(common::PhoneNumber from) = 0;
    virtual void handleTalkMessage(common::PhoneNumber from, const std::string& text) = 0;
    virtual void handleCallRequest(common::PhoneNumber from) = 0;

};

class IBtsPort
{
public:
    virtual ~IBtsPort() = default;

    virtual void sendAttachRequest(common::BtsId) = 0;
    virtual void sendSms(const SmsEntity& sms) = 0;
    virtual void sendCallRequest(common::PhoneNumber from, common::PhoneNumber to) = 0;
    virtual void sendCallDropped(common::PhoneNumber from, common::PhoneNumber to) = 0;
    virtual void sendTalkMessage(common::PhoneNumber to, const std::string& text) = 0;
    virtual void sendCallAccepted(common::PhoneNumber from) = 0;

};

}
