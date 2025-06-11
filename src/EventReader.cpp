#include "EventReader.hpp"
#include <iostream>
#include <cassert>
#include <dv-processing/visualization/event_visualizer.hpp>
#include <opencv2/highgui.hpp>

EventReader::EventReader() {
    //Oeffne Kamera
    m_camera = std::make_unique<dv::io::CameraCapture>();
    assert(m_camera->isEventStreamAvailable());
    
    m_running = false;
    m_trigger = false;
}

EventReader::~EventReader() {
    stop();
}

void EventReader::start(bool triggered_start) {
    m_running = true;
    assert(m_camera->isRunning());
    m_readerThread = std::thread(&EventReader::readLoop, this, triggered_start);
}

void EventReader::stop() {
    m_running = false;
    if (m_readerThread.joinable()) { //if joinable == true, thread not detached and not empty
        m_readerThread.join();  // main thread waits that the thread finishes 
    }
}

std::vector<int64_t> EventReader::getSyncEventDifferences() {
    std::lock_guard<std::mutex> lock(m_dataMutex); 
    return m_syncEventDiffs;
}

void EventReader::readTrigger() {
    using namespace std::chrono_literals;
    while (!m_trigger.load(std::memory_order_acquire)) {
        auto trigger = m_camera->getNextTriggerBatch();
        if (trigger.has_value()) {
            m_trigger.store(true, std::memory_order_release);
            m_trigger_events = trigger;
            //debugging
            std::cout << "Received " << m_trigger_events->size() << " Triggers" << std::endl;
            assert(m_trigger_events->size()>1);
        }
        else
            std::this_thread::sleep_for(1ms);
    }
}

void EventReader::visualize_Events() {
    // Visualizer
    using namespace std::chrono_literals;
    dv::visualization::EventVisualizer visualizer(*(m_camera -> getEventResolution())); //Event resolution return std::optional value- * to dereference

    // Optional: Adjust background and polarity colors
    visualizer.setBackgroundColor(dv::visualization::colors::white);
    visualizer.setPositiveColor(dv::visualization::colors::iniBlue);
    visualizer.setNegativeColor(dv::visualization::colors::darkGrey);

    // OpenCV window
    cv::namedWindow("Events", cv::WINDOW_AUTOSIZE);
    

    dv::EventStreamSlicer slicer;

    // Register a callback every 33 milliseconds
    slicer.doEveryTimeInterval(33ms, [&visualizer](const dv::EventStore& events) {
        // Generate a preview frame
        cv::Mat image = visualizer.generateImage(events);

        // Show the accumulated image
        cv::imshow("Events", image);
        cv::waitKey(2);
        });

    // Run the event processing while the camera is connected
    while (m_camera -> isRunning() && !m_stop_visualizer.load(std::memory_order_acquire)) {
        // Receive events, check if anything was received
        if (const auto events = m_camera -> getNextEventBatch()) {
            std::cout << "Event rate: " << events->size() << " events received." << std::endl;
            // If so, pass the events into the slicer to handle them
            slicer.accept(*events);
        }
    }
}

void EventReader::readLoop(bool trigger) {
    //Show Events when visualize = true
    bool visualize{ false };
    dv::EventStreamSlicer slicer();
    int64_t timestamp_start = dv::now();

    if (trigger) {
        m_camera->deviceConfigSet(DAVIS_CONFIG_EXTINPUT, DAVIS_CONFIG_EXTINPUT_DETECT_RISING_EDGES, true); //Enable Edge detection
        m_camera -> deviceConfigSet(DAVIS_CONFIG_EXTINPUT, DAVIS_CONFIG_EXTINPUT_RUN_DETECTOR, true); //Hopefully needed but don´t know
        m_triggerThread = std::thread(&EventReader::readTrigger, this);
        if (m_triggerThread.joinable())
            m_triggerThread.join();

    }
    else
        m_trigger = true;

    
    std::thread visualizer;
    if (visualize) {
        m_stop_visualizer.store(false, std::memory_order_release);
        visualizer = std::thread(&EventReader::visualize_Events, this);
    }

    const auto startTime = std::chrono::steady_clock::now();
    dv::EventStore store;

    while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(2)) {  //Duration is 2 secounds 
        if (const auto events = m_camera->getNextEventBatch()) {
            store.add(*events);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    for(const auto trigger_event: m_trigger_events.value()) {
        m_lastTriggerTimestamp = trigger_event.timestamp; //just member not function of struct Trigger
    }

    dv::EventStore store_sliced = store.sliceTime(m_lastTriggerTimestamp.value());
    /*this makes no sense. The event Buffer must be sliced after the point where a trigger signal from the arduino occured. */

    m_timestamp_first = store.getLowestTime(); 
    m_timestamp_last = store.getHighestTime();
    
    m_stop_visualizer = true;
    visualizer.join();
}

void EventReader::getTimeDelta() const {
    std::cout << "Time between first and Last Event is " << m_timestamp_last - m_timestamp_first << '\n';
}