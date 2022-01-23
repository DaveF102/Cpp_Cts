#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;


// FP.3
template <class T>
class MessageQueue
{
public:
    // constructor / desctructor
    MessageQueue() {}

    // getters / setters

    // typical behaviour methods
    void send(T &&msg);
    T receive();

private:
    std::deque<T> _queue;
    std::condition_variable _cond;
    std::mutex _mutex;
};

// FP.1
class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();

    enum TrafficLightPhase
    {
        red,
        green
    };

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods
    void cycleThroughPhases();

    // FP.4b
    MessageQueue<TrafficLightPhase> _message;

    TrafficLightPhase _currentPhase;
};

#endif