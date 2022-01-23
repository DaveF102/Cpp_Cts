#include <iostream>
#include <random>
#include <chrono>
#include "TrafficLight.h"

using namespace std::chrono_literals;

// Implementation of class "MessageQueue"

template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a
    //Lock prior to accessing
    std::unique_lock<std::mutex> uLock(_mutex);
    //Wait for new message
    _cond.wait(uLock, [this] { return !_queue.empty(); });
    //Use move semantics to remove message from queue
    T msg = std::move(_queue.back());
    _queue.pop_back();

    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a
    //Lock prior to accessing
    std::lock_guard<std::mutex> uLock(_mutex);
    //Remove old messages
    _queue.clear();
    //Add new message to queue
    _queue.emplace_back(std::move(msg));
    _cond.notify_one();
    std::cout << " Message " << msg << " has been added to the queue" << std::endl;
}

// Implementation of class "TrafficLight"

TrafficLight::TrafficLight()
{
    //Initialize phase of traffic light
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b
    while (true)
    {  
        //Test phase of traffic light message
        if (_message.receive() == TrafficLightPhase::green)
        {
            //If green light received, notify console and exit loop
            std::cout << " Green Light!" << std::endl;
            return;
        }
    }
}

TrafficLight::TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}


void TrafficLight::simulate()
{
    // FP.2b
    //Start looping through traffic light phases
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    //Added this temporary variable for the if condition below
    TrafficLightPhase tempPhase;
    //Range of cycle time in Milliseconds
    long lowCycle = 4000;
    long highCycle = 6000;
    //Select random cycle time within range
    long randomCycle = lowCycle + rand() % (highCycle - lowCycle);
    std::cout << " Light cycle time = " << randomCycle << std::endl;
    //Initialize time of "This" loop cycle
    auto timeThis = std::chrono::high_resolution_clock::now();
    long cycleDuration = 0;
    while (true)
    {
        //Time of "Previous" loop cycle
        auto timeLast = timeThis;
        timeThis = std::chrono::high_resolution_clock::now();
        //Time between loop cycles
        auto loopDuration = std::chrono::duration_cast<std::chrono::milliseconds>(timeThis - timeLast).count();
        //Increment cycle time by loop time
        cycleDuration += loopDuration;
        if (cycleDuration > randomCycle)
        {
            //If duration is complete, cycle traffic light phase
            tempPhase = _currentPhase;
            if (tempPhase == red)
            {
                _currentPhase = green;
            }
            else
            {
                _currentPhase = red;
            }
            //Use move semantics to send update method to message queue
            _message.send(std::move(_currentPhase));
            //Reset cycle duration for next phase change
            cycleDuration = 0;
        }
        //Delay
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}