#pragma once

#include <dv-processing/io/camera_capture.hpp>
#include <dv-processing/core/event.hpp>
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include <optional>
#include <SerialInterface.hpp>


class EventReader {
public:
    EventReader();
    ~EventReader();
    enum class TimeDeltas {
        deltaFirst,
        deltaLast,
        max_value,
    };
    void start(bool visualize, SerialInterface*);
    void stop();
    void visualize();

    // Zugriff auf die letzten gespeicherten Zeitdifferenzen
    std::vector<int64_t> getSyncEventDifferences();
    void getTimeDelta(TimeDeltas);

    

    std::array<std::string, static_cast<size_t>(TimeDeltas::max_value)> m_time_delta_strings 
        {"Delta Trigger-timestamp to fist Event-timestamp ",
        "Delta Trigger-timestamp to last Event-timestamp "};

private:
    void visualize_Events();
    void readLoop();
    void readTrigger();
    void stop_visualizer_input();
    std::unique_ptr<dv::io::CameraCapture> m_camera;
    std::thread m_readerThread;
    std::thread m_triggerThread;
    std::atomic<bool> m_running{};
    std::atomic<bool> m_trigger{};
    std::atomic<bool> m_stop_visualizer{ false };

    std::optional <dv::cvector<dv::Trigger>> m_trigger_events;
    std::vector<int64_t> m_TriggerTimestamp;
    std::mutex m_dataMutex;
    std::vector<int64_t> m_syncEventDiffs;
    std::int64_t m_timestamp_first{};
    std::int64_t m_timestamp_last{};
};
