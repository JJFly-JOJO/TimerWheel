#ifndef TIMERWHEEL_TIMERWHEEL_H
#define TIMERWHEEL_TIMERWHEEL_H

#include <functional>
#include <thread>
#include <vector>

class Timer {

   public:
    Timer(int rotations, int slot, std::function<void(void)> f, void* args)
        : rotations_(rotations), slot_(slot), f_callback(f), args(args)
    {}

    inline void action() { f_callback(); }

    inline int getRotations() { return rotations_; }

    inline int getSlot() { return slot_; }

    inline void decreaseRotations() { rotations_--; }

   private:
    int rotations_;
    int slot_;
    std::function<void(void)> f_callback;
    void* args;
};

class TimerWheel {
   public:
    TimerWheel(int nSlots)
        : nSlots_(nSlots),
          curSlot_(0),
          startTime_(getCurrentMillisecs()),
          slots_(nSlots, std::vector<Timer*>())
    {}

    ~TimerWheel()
    {
        for (auto& slot : slots_)
        {
            for (auto& timer : slot)
            {
                delete timer;
            }
        }
    }

    static void handler(TimerWheel* tw);

    Timer* addTimer(unsigned long long timeout, std::function<void(void)> f,
                    void* args = nullptr);

    void delTimer(Timer* timer);

    void tick();

    void takeAllTimeout();

    unsigned long long getCurrentMillisecs();

    std::thread* run();

   private:
    int nSlots_;
    int curSlot_;
    unsigned long long startTime_;
    std::vector<std::vector<Timer*>> slots_;
};

#endif  //TIMERWHEEL_TIMERWHEEL_H
