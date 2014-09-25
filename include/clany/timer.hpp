#ifndef CLANY_TIMER_HPP
#define CLANY_TIMER_HPP

#if defined(WIN32) || defined(_WIN32) || defined(__MINGW32__)
#  include <ctime>
#  define CLOCK()  clock()
#  define TICK_FACTOR  CLOCKS_PER_SEC
#else
#  include <unistd.h>
#  include <sys/times.h>
#  define CLOCK()  ::times(NULL)
#  define TICK_FACTOR  TICK_PER_SECOND
   const int TICK_PER_SECOND = ::sysconf(_SC_CLK_TCK);
#endif

#include <iostream>
#include <iomanip>

#include "clany_macros.h"


_CLANY_BEGIN
class ScopeTimer
{
public:
    ScopeTimer(int precision = 3) :timer_ (CLOCK()), precision_ (precision) {}

    ~ScopeTimer()
    {
        timer_ = CLOCK() - timer_;
        cout << setprecision(precision_) << "Time elapsed: "
             << static_cast<double>(timer_) / TICK_FACTOR << "s"
             << endl;
    }

private:
    clock_t timer_;
    int precision_;
};


class CPUTimer
{
public:
    CPUTimer(bool is_started = true) :
        timer_(0), duration_berfore_(0), is_stopped_(false)
    {
        if (is_started) {
            start();
        }
    }

    double elapsed(int precision = 3, string process_name = "Since begin")
    {
        if (is_stopped_) {
            cout << "Paused" << endl;
            return -1;
        }

        double last_duration = static_cast<double>(CLOCK() - timer_) / TICK_FACTOR;
        double since_begin = last_duration + duration_berfore_;
        cout << setprecision(precision) << process_name.c_str() << ": "
             << since_begin << "s" << endl;
             
        timer_ = CLOCK();
        duration_berfore_ += last_duration;
        
        return since_begin;
    }
    
    double delta(int precision = 3, string process_name = "Since last check")
    {
        if (is_stopped_) {
            cout << "Paused" << endl;
            return -1;
        }

        double last_duration = static_cast<double>(CLOCK() - timer_) / TICK_FACTOR;
        cout << setprecision(precision) << process_name.c_str() << ": "
             << last_duration << "s" << endl;

        timer_ = CLOCK();
        duration_berfore_ += last_duration;
        
        return last_duration;
    }

    void start()
    {
        is_stopped_ = false;
        timer_ = CLOCK();
    }

    void pause()
    {
        if (!is_stopped_) {
            is_stopped_ = true;
            duration_berfore_ += static_cast<double>(CLOCK() - timer_) / TICK_FACTOR;
        }
    }

    void resume()
    {
        start();
    }

    void reset(bool is_started = true)
    {
        duration_berfore_ = 0;
        is_stopped_ = false;
        if (is_started) {
            start();
        }
    }

private:
    clock_t timer_;
    double duration_berfore_;
    bool is_stopped_;
};
_CLANY_END

#endif // CLANY_TIMER_HPP
