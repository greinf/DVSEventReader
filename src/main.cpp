#include <dv-processing/core/multi_stream_slicer.hpp>
#include <dv-processing/io/camera_capture.hpp>
#include <dv-processing/visualization/event_visualizer.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "SerialInterface.hpp"
#include "EventReader.hpp"



int main() {
    
    
    //EventReader reader;
    SerialInterface arduino("COM3", 9600);
    //reader.start(true);  //bool value if if trigger is enabled
    std::this_thread::sleep_for(std::chrono::seconds(2));
    arduino.sendCommand("P");
    
    
    /*
    std::this_thread::sleep_for(std::chrono::seconds(2));  //When opening serial Port Arduino restarts!!!
    arduino.sendCommand("P");

    //reader.visualize_Events(); 

    std::cout << "Lese Ereignisse... bitte sende SYNC-Takt an Kamera." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    reader.stop();
    reader.getTimeDelta();
    */
    
    
    
    
}
