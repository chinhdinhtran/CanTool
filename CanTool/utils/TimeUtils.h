#ifndef TIMEUTILS_H
#define TIMEUTILS_H

#include <QElapsedTimer>

class TimeUtils
{
public:
    static double now()
    {
        static QElapsedTimer timer;
        static bool started = false;
        if (!started)
        {
            timer.start();
            started = true;
        }
        return timer.elapsed() / 1000.0;
    }
};


#endif // TIMEUTILS_H
