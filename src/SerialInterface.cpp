#include "SerialInterface.hpp"
#include <iostream>
#include <chrono>

SerialInterface::SerialInterface(const std::string& port, unsigned int baud_rate)
    : m_serial(m_io, port) {
    m_serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    m_serial.set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));
    m_serial.set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
    m_serial.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
    m_serial.set_option(boost::asio::serial_port::character_size(8));
}

SerialInterface::~SerialInterface() {
    if (m_serial.is_open()) {
        m_serial.close();
    }
}

void SerialInterface::readArduinoData() {
    std::string line = readLine();
    std::istringstream ss(line);
    char sep1, sep2;

    if (ss >> m_arduino_output.pulsePhase_timeDelta >> sep1 >> m_arduino_output.diodeValBackround >>
        sep2 >> m_arduino_output.diodeValPulse) {
        if (sep1 != ',' || sep2 != ',') {
            std::runtime_error("Data Transfer from Arudino to machine failed. \n");
        }
    }

    throw std::runtime_error("Invalid data format: " + line);
}

void SerialInterface::sendCommand(const char& command) {
    if (!m_serial.is_open()) {
        std::cerr << "Serial port not open!" << std::endl;
        return;
    }

    boost::asio::write(m_serial, boost::asio::buffer(&command, 1));
}

std::int16_t SerialInterface::sendtimedCommand_feedback(const std::string& command) {
    using namespace std::chrono;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    boost::asio::write(m_serial, boost::asio::buffer("P"));
    // Optional: timestamp from PC for measuring command-send delay
    const auto start_pc = steady_clock::now();

    // Send the command to Arduino
    boost::asio::write(m_serial, boost::asio::buffer(command));

    // Prepare local vars
    std::array<std::int32_t, 2> parsedTimestamps{};
    
    for (std::size_t i = 0; i < m_arduino_response.size(); ++i) {
        std::string line = readLine();  // Reads from serial
        if (line.rfind(m_arduino_response[i], 0) == 0) {  // starts with- rfind return the first position of last match
            // Extract integer part
            std::string numberPart = line.substr(m_arduino_response[i].size()); //subst(arg) arg specifies start charakter for substr, second == size_t len std::npos
            try {
                //std::string.erase() takes iterator on which charakters to erase. end is marked by numberpart.end(). std::remove_if gives position of 
                //start itartor by returning end iterator object for range of valid charakters
                numberPart.erase(std::remove_if(numberPart.begin(), numberPart.end(), [](char c) { 
                    return !std::isdigit(c);  // Entfernt alles außer Ziffern
                    }), numberPart.end());
                try {
                    parsedTimestamps[i] = std::stoi(numberPart);
                    std::cout << "Parsed: " << parsedTimestamps[i] << "\n";
                }
                catch (...) {
                    std::cerr << "Failed to convert cleaned string: '" << numberPart << "'\n";
                }
                //parsedTimestamps[i] = std::stoi(numberPart);  //should be substring containig the micorseconds. 
            }
            catch (const std::exception& e) {
                std::cerr << "Failed to parse timestamp from: " << line << "\n";
                return -1;
            }
        }
        else {
            std::cerr << "Unexpected line: " << line << "\n";
            return -1;
        }
    }

    const auto end_pc = steady_clock::now();

    // Store the timestamps
    m_timestamps.push_back(parsedTimestamps[0]);
    m_timestamps.push_back(parsedTimestamps[1]);
    auto start_pc_us = duration_cast<microseconds>(start_pc.time_since_epoch()).count();
    m_timestamps.push_back(start_pc_us);
    
    return 0;
    
}

std::string SerialInterface::readLine() {
    boost::asio::streambuf buffer;
    boost::asio::read_until(m_serial, buffer, "\n");
    std::istream is(&buffer);
    std::string line;
    std::getline(is, line);
    return line;
}
