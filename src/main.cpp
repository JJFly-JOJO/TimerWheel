#include <unistd.h>
#include <iostream>
#include "TimerWheel.h"

int main()
{
    TimerWheel tw(10);
    tw.addTimer(1000, []() { std::cout << "1000ms trigger" << std::endl; });
    tw.addTimer(2000, []() { std::cout << "2000ms trigger" << std::endl; });
    tw.addTimer(3000, []() { std::cout << "3000ms trigger" << std::endl; });
    tw.addTimer(4000, []() { std::cout << "4000ms trigger" << std::endl; });

    tw.run();

    while (1)
    {
        sleep(1);
    }
    return 0;
}