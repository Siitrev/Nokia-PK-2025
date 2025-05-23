#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Ports/BtsPort.hpp"
#include "Mocks/ILoggerMock.hpp"
#include "Mocks/IBtsPortMock.hpp"
#include "Messages/PhoneNumber.hpp"
#include "Mocks/ITransportMock.hpp"
#include "Messages/OutgoingMessage.hpp"
#include "Messages/IncomingMessage.hpp"

namespace ue
{
using namespace ::testing;

class BtsPortTestSuite : public Test
{
protected:
    const common::PhoneNumber PHONE_NUMBER{112};
    const common::BtsId BTS_ID{13121981ll};
    NiceMock<common::ILoggerMock> loggerMock;
    StrictMock<IBtsEventsHandlerMock> handlerMock;
    StrictMock<common::ITransportMock> transportMock;
    common::ITransport::MessageCallback messageCallback;
    common::ITransport::DisconnectedCallback disconnectedCallback;

    BtsPort objectUnderTest{loggerMock, transportMock, PHONE_NUMBER};

    BtsPortTestSuite()
    {
        EXPECT_CALL(transportMock, registerMessageCallback(_))
                .WillOnce(SaveArg<0>(&messageCallback));
        EXPECT_CALL(transportMock, registerDisconnectedCallback(_))
                .WillOnce(SaveArg<0>(&disconnectedCallback));
        objectUnderTest.start(handlerMock);
    }
    ~BtsPortTestSuite()
    {

        EXPECT_CALL(transportMock, registerMessageCallback(IsNull()));
        EXPECT_CALL(transportMock, registerDisconnectedCallback(IsNull()));
        objectUnderTest.stop();
    }
};

TEST_F(BtsPortTestSuite, shallRegisterHandlersBetweenStartStop)
{
}

TEST_F(BtsPortTestSuite, shallIgnoreWrongMessage)
{
    common::OutgoingMessage wrongMsg{};
    wrongMsg.writeBtsId(BTS_ID);
    messageCallback(wrongMsg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleSib)
{
    EXPECT_CALL(handlerMock, handleSib(BTS_ID));
    common::OutgoingMessage msg{common::MessageId::Sib,
                                common::PhoneNumber{},
                                PHONE_NUMBER};
    msg.writeBtsId(BTS_ID);
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleAttachAccept)
{
    EXPECT_CALL(handlerMock, handleAttachAccept());
    common::OutgoingMessage msg{common::MessageId::AttachResponse,
                                common::PhoneNumber{},
                                PHONE_NUMBER};
    msg.writeNumber(true);
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleAttachReject)
{
    EXPECT_CALL(handlerMock, handleAttachReject());
    common::OutgoingMessage msg{common::MessageId::AttachResponse,
                                common::PhoneNumber{},
                                PHONE_NUMBER};
    msg.writeNumber(false);
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallSendAttachRequest)
{
    common::BinaryMessage msg;
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce([&msg](auto param) { msg = std::move(param); return true; });
    objectUnderTest.sendAttachRequest(BTS_ID);
    common::IncomingMessage reader(msg);
    ASSERT_NO_THROW(EXPECT_EQ(common::MessageId::AttachRequest, reader.readMessageId()) );
    ASSERT_NO_THROW(EXPECT_EQ(PHONE_NUMBER, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(common::PhoneNumber{}, reader.readPhoneNumber()));
    ASSERT_NO_THROW(EXPECT_EQ(BTS_ID, reader.readBtsId()));
    ASSERT_NO_THROW(reader.checkEndOfMessage());
}

TEST_F(BtsPortTestSuite, shallHandleSmsMessage)
{
    const std::string TEXT = "Hello from BTS";
    const common::PhoneNumber FROM{123};

    EXPECT_CALL(handlerMock, handleSms(FROM, TEXT));

    common::OutgoingMessage msg{common::MessageId::Sms, FROM, PHONE_NUMBER};
    msg.writeText(TEXT);

    messageCallback(msg.getMessage());
}


TEST_F(BtsPortTestSuite, shallHandleDisConnect)
{
    EXPECT_CALL(handlerMock, handleDisconnect());
    disconnectedCallback();
}

TEST_F(BtsPortTestSuite, shallHandleCallAccepted)
{
    EXPECT_CALL(handlerMock, handleCallAccepted());

    common::OutgoingMessage msg{common::MessageId::CallAccepted,
                                common::PhoneNumber{123},
                                PHONE_NUMBER};
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleCallDropped)
{
    EXPECT_CALL(handlerMock, handleCallDropped());

    common::OutgoingMessage msg{common::MessageId::CallDropped,
                                common::PhoneNumber{123},
                                PHONE_NUMBER};
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleUnknownRecipientForCall)
{
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce(Return(true));

    objectUnderTest.sendCallRequest(PHONE_NUMBER, common::PhoneNumber{123});

    EXPECT_CALL(handlerMock, handleCallRecipientNotAvailable(common::PhoneNumber{123}));

    common::OutgoingMessage msg{common::MessageId::UnknownRecipient,
                                PHONE_NUMBER,
                                common::PhoneNumber{123}};
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallHandleUnknownRecipientForSms)
{
    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce(Return(true));

    objectUnderTest.sendSms(SmsEntity{PHONE_NUMBER.value, 123, "test message"});

    EXPECT_CALL(handlerMock, handleSmsDeliveryFailure(common::PhoneNumber{123}));

    common::OutgoingMessage msg{common::MessageId::UnknownRecipient,
                                PHONE_NUMBER,
                                common::PhoneNumber{123}};
    messageCallback(msg.getMessage());
}

TEST_F(BtsPortTestSuite, shallSendCallRequest)
{
    common::BinaryMessage msg;

    const common::PhoneNumber from = PHONE_NUMBER;
    const common::PhoneNumber to{123};

    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce([&msg](auto param) {
        msg = std::move(param);
        return true;
    });

    objectUnderTest.sendCallRequest(from, to);

    common::IncomingMessage reader(msg);
    ASSERT_EQ(reader.readMessageId(), common::MessageId::CallRequest);
    ASSERT_EQ(reader.readPhoneNumber(), from);
    ASSERT_EQ(reader.readPhoneNumber(), to);
    ASSERT_NO_THROW(reader.checkEndOfMessage());
}

TEST_F(BtsPortTestSuite, shallSendCallDropped)
{
    common::BinaryMessage msg;

    const common::PhoneNumber from = PHONE_NUMBER;
    const common::PhoneNumber to{123};

    EXPECT_CALL(transportMock, sendMessage(_)).WillOnce([&msg](auto param) {
        msg = std::move(param);
        return true;
    });

    objectUnderTest.sendCallDropped(from, to);

    common::IncomingMessage reader(msg);
    ASSERT_EQ(reader.readMessageId(), common::MessageId::CallDropped);
    ASSERT_EQ(reader.readPhoneNumber(), from);
    ASSERT_EQ(reader.readPhoneNumber(), to);
    ASSERT_NO_THROW(reader.checkEndOfMessage());
}

}
