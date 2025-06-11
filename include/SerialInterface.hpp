#pragma once

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <array>
#include <string_view>

class SerialInterface {
public:
    SerialInterface(const std::string& port, unsigned int baud_rate);
    ~SerialInterface();

    void sendCommand(const std::string& command);
    std::int16_t sendtimedCommand_feedback(const std::string& command);
    std::string readLine();  // Optional: receive from Arduino
    std::vector<std::int32_t> m_timestamps{};

private:
    boost::asio::io_service m_io;
    boost::asio::serial_port m_serial;
    static constexpr std::array<std::string_view, 1> m_arduino_response{ "Time to send Signal " };
};
