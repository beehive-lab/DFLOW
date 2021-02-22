#include "WifiComms.h"
#include <string>
#include <BluetoothComms.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace {

    // Wifi tests

    class MockWifiComms : public WifiComms {
        public:
            MOCK_METHOD(int, create_socket, (int), (override));
            MOCK_METHOD(int, bind_socket, (), (override));
            MOCK_METHOD(int, listen_socket, (), (override));
            MOCK_METHOD(int, accept_connection, (), (override));
            MOCK_METHOD(int, send, (char*), (override));
            MOCK_METHOD(int, receive, (char[]), (override));
    };

    class WifiTest : public ::testing::Test {
    };


    TEST_F(WifiTest, Connection) {
        MockWifiComms mock_wifi_comms;
        EXPECT_CALL(mock_wifi_comms, create_socket(8080)).Times(1);
        EXPECT_CALL(mock_wifi_comms, bind_socket()).Times(1);
        EXPECT_CALL(mock_wifi_comms, listen_socket()).Times(1);
        EXPECT_CALL(mock_wifi_comms, accept_connection()).Times(1);

        mock_wifi_comms.establish_connection(8080);
    }

    using ::testing::Return;

    TEST_F(WifiTest, Send) {
        MockWifiComms mock_wifi_comms;
        std::string to_send = "hello/0";
        char to_send_array[to_send.size() + 1];
        strcpy(to_send_array, to_send.c_str());

        EXPECT_CALL(mock_wifi_comms, send(to_send_array)).WillOnce(Return(to_send.size()));

        int bytes = mock_wifi_comms.send(to_send_array);

        EXPECT_EQ(bytes, to_send.size());
    }

    TEST_F(WifiTest, Receive) {
        MockWifiComms mock_wifi_comms;
        std::string to_receive = "hello/0";
        char to_receive_array[to_receive.size() + 1];

        EXPECT_CALL(mock_wifi_comms, receive(to_receive_array)).WillOnce(Return(to_receive.size()));

        int bytes = mock_wifi_comms.receive(to_receive_array);

        EXPECT_EQ(bytes, to_receive.size());
    }

    // Bluetooth Tests

    class MockBluetoothComms : public BluetoothComms {
    public:
        MOCK_METHOD(int, create_socket, (int), (override));
        MOCK_METHOD(int, bind_socket, (), (override));
        MOCK_METHOD(int, listen_socket, (), (override));
        MOCK_METHOD(int, accept_connection, (), (override));
        MOCK_METHOD(int, send, (char*), (override));
        MOCK_METHOD(int, receive, (char[]), (override));
    };

    class BluetoothTest : public ::testing::Test {
    };

    TEST_F(BluetoothTest, BTConnection) {
        MockBluetoothComms mock_bluetooth_comms;
        EXPECT_CALL(mock_bluetooth_comms, create_socket(0)).Times(1);
        EXPECT_CALL(mock_bluetooth_comms, bind_socket()).Times(1);
        EXPECT_CALL(mock_bluetooth_comms, listen_socket()).Times(1);
        EXPECT_CALL(mock_bluetooth_comms, accept_connection()).Times(1);

        mock_bluetooth_comms.establish_connection(0);
    }

    TEST_F(WifiTest, BTSend) {
        MockBluetoothComms mock_bluetooth_comms;
        std::string to_send = "hello/0";
        char to_send_array[to_send.size() + 1];
        strcpy(to_send_array, to_send.c_str());

        EXPECT_CALL(mock_bluetooth_comms, send(to_send_array)).WillOnce(Return(to_send.size()));

        int bytes = mock_bluetooth_comms.send(to_send_array);

        EXPECT_EQ(bytes, to_send.size());
    }

    TEST_F(WifiTest, BTReceive) {
        MockBluetoothComms mock_bluetooth_comms;
        std::string to_receive = "hello/0";
        char to_receive_array[to_receive.size() + 1];

        EXPECT_CALL(mock_bluetooth_comms, receive(to_receive_array)).WillOnce(Return(to_receive.size()));

        int bytes = mock_bluetooth_comms.receive(to_receive_array);

        EXPECT_EQ(bytes, to_receive.size());
    }
}
