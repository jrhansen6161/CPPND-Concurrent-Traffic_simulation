#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

 
template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> mLock(_mutex);
    _cond.wait(mLock, [this] { return !_messages.empty(); });
    T msg = std::move(_messages.back());
    _messages.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> mLock(_mutex);
    _messages.clear();
    _messages.emplace_back(std::move(msg));
    _cond.notify_one();
}

/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while(true)
    {
        if(_queue.receive() == TrafficLightPhase::green)
        {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    std::chrono::time_point<std::chrono::system_clock> lastUpdate;
    int cycleDuration = distr(eng); // duration of a single simulation cycle in S (4-6)
    lastUpdate = std::chrono::system_clock::now();
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        if (timeSinceLastUpdate >= cycleDuration)
        {
            if(_currentPhase == TrafficLightPhase::green)
            {
                _queue.send(std::move(TrafficLightPhase::red));
                _currentPhase = TrafficLightPhase::red;
            }
            else
            {
                _queue.send(std::move(TrafficLightPhase::green));
                _currentPhase = TrafficLightPhase::green;
            }
            lastUpdate = std::chrono::system_clock::now();
        }
    }
}

