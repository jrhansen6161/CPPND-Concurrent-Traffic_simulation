#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include <random>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

static std::random_device rd;
static std::mt19937 eng(rd());
static std::uniform_int_distribution<> distr(4000, 6000);


enum TrafficLightPhase
{
    red,
    green
};

template <class T>
class MessageQueue
{
public:
    T receive();
    void send(T &&msg);

private:
    std::mutex _mutex;
    std::condition_variable _cond;
    std::deque<T> _messages;
    
};

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters

    // typical behaviour methods
    void waitForGreen();
    void simulate();
    TrafficLightPhase getCurrentPhase();

private:
    // typical behaviour methods
    void cycleThroughPhases();

    MessageQueue<TrafficLightPhase> _queue;    
    TrafficLightPhase _currentPhase;
};

#endif