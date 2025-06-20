#pragma once

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <array>
#include <string_view>
#include <iostream>


struct serialoutput {
    int pulsePhase_timeDelta{};
    int diodeValBackround{};
    int diodeValPulse{};
};


class SerialInterface {
public:
    SerialInterface(const std::string& port, unsigned int baud_rate);
    ~SerialInterface();

    void sendCommand(const char& command);
    void readArduinoData();
    std::int16_t sendtimedCommand_feedback(const std::string& command);
    std::string readLine();  // Optional: receive from Arduino
    std::vector<std::int32_t> m_timestamps{};
    void printArudinoData() {
        std::cout << "timeDelta: " << m_arduino_output.pulsePhase_timeDelta << '\n'
            << "DiodenSpannungBackround: " << m_arduino_output.diodeValBackround << '\n' << "DiodenSpannungPulse: " <<
            m_arduino_output.diodeValPulse << '\n';
    };

private:
    boost::asio::io_service m_io;
    boost::asio::serial_port m_serial;
    static constexpr std::array<std::string_view, 1> m_arduino_response{ "Time to send Signal " };

    serialoutput m_arduino_output{};
};
