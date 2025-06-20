#include <dv-processing/core/multi_stream_slicer.hpp>
#include <dv-processing/io/camera_capture.hpp>
#include <dv-processing/visualization/event_visualizer.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "SerialInterface.hpp"
#include "EventReader.hpp"



int main() {
    
    SerialInterface arduino("COM3", 9600);
    EventReader reader;
    
    //necessary to not block let the viusliazation and event accumulation run in parallel to the light pulse.
    /*
    std::thread visualizer_thread([&]() {
        reader.visualize();
        });
    */
    std::this_thread::sleep_for(std::chrono::seconds(2));
    reader.start(true, &arduino);
    //bool value if if visualizer is enabled
    
    reader.getTimeDelta(EventReader::TimeDeltas::deltaFirst);
    reader.getTimeDelta(EventReader::TimeDeltas::deltaLast);
    arduino.printArudinoData();
    std::cout << "First Succesfull run. ";   
}
