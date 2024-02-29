#include "TimerWheel.h"
#include <sys/epoll.h>
#include <sys/time.h>
#include <algorithm>
#include <ctime>
#include <iostream>

unsigned long long TimerWheel::getCurrentMillisecs()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_COARSE, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / (1000 * 1000);
}

Timer* TimerWheel::addTimer(unsigned long long int timeout,
                            std::function<void(void)> f, void* args)
{
    if (timeout == 0)
    {
        return nullptr;
    }
    int slot = (curSlot_ + timeout % nSlots_) % nSlots_;
    Timer* timer = new Timer(timeout / nSlots_, slot, f, args);
    slots_[slot].push_back(timer);

    return timer;
}

void TimerWheel::delTimer(Timer* timer)
{
    if (timer == NULL)
    {
        return;
    }
    auto tmIter = std::find(slots_[timer->getSlot()].begin(),
                            slots_[timer->getSlot()].end(), timer);
    if (tmIter != slots_[timer->getSlot()].end())
    {
        slots_[timer->getSlot()].erase(tmIter);
        delete timer;
    }
}

void TimerWheel::tick()
{
    for (auto it = slots_[curSlot_].begin(); it != slots_[curSlot_].end();)
    {
        if ((*it)->getRotations() > 0)
        {
            (*it)->decreaseRotations();
            it++;
            continue;
        }
        Timer* timer = *it;
        timer->action();
        it = slots_[curSlot_].erase(it);
        delete timer;
    }
    curSlot_ = (curSlot_ + 1) % nSlots_;
}

void TimerWheel::takeAllTimeout()
{
    unsigned long long now = getCurrentMillisecs();
    int cnt = now - startTime_;
    for (int i = 0; i < cnt; i++)
    {
        tick();
    }
    startTime_ = now;
}

void TimerWheel::handler(TimerWheel* tw)
{
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        std::cerr << "epoll_create error" << std::endl;
        return;
    }
    struct epoll_event ev;
    for (;;)
    {
        //base frequency is 1ms
        epoll_wait(epoll_fd, &ev, 1, 1);
        tw->takeAllTimeout();
    }
}
std::thread* TimerWheel::run()
{
    return new std::thread(TimerWheel::handler, this);
}
