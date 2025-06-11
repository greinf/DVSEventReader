#pragma once

#include <dv-processing/io/camera_capture.hpp>
#include <dv-processing/core/event.hpp>
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include <optional>

class EventReader {
public:
    EventReader();
    ~EventReader();

    void start(bool triggered_start);
    void stop();
    void visualize_Events();

    // Zugriff auf die letzten gespeicherten Zeitdifferenzen
    std::vector<int64_t> getSyncEventDifferences();
    void getTimeDelta() const;

private:
    void readLoop(bool trigger);
    void readTrigger();
    std::unique_ptr<dv::io::CameraCapture> m_camera;
    std::thread m_readerThread;
    std::thread m_triggerThread;
    std::atomic<bool> m_running;
    std::atomic<bool> m_trigger;
    std::atomic<bool> m_stop_visualizer;

    std::optional <dv::cvector<dv::Trigger>> m_trigger_events;
    std::optional<int64_t> m_lastTriggerTimestamp;
    std::mutex m_dataMutex;
    std::vector<int64_t> m_syncEventDiffs;
    std::atomic<std::int64_t> m_timestamp_first;
    std::atomic<std::int64_t> m_timestamp_last;

};
