#include "LxCore/core/Timer.h"

Timer::Timer()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    m_SecondsPerCount = 1.0 / frequency.QuadPart;
    Reset();
}

void Timer::Resume()
{
    if (m_IsStopped)
    {
        LARGE_INTEGER count;
        QueryPerformanceCounter(&count);
        m_PrevTime = count.QuadPart;
        m_PausedTime += (count.QuadPart - m_StopTime);
        m_IsStopped = false;
    }
}

void Timer::Stop()
{
    if (!m_IsStopped)
    {
        LARGE_INTEGER count;
        QueryPerformanceCounter(&count);
        m_StopTime = count.QuadPart;
        m_IsStopped = true;
    }
}

void Timer::Reset()
{
    m_IsStopped = false;
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    m_DeltaTime = -1.0;
    m_CurrTime = count.QuadPart;
    m_PrevTime = count.QuadPart;
    m_PausedTime = 0;
    m_BaseTime = count.QuadPart;
}

void Timer::Tick()
{
    if (m_IsStopped)
    {
        m_DeltaTime = 0.0;
        return;
    }

    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    m_CurrTime = count.QuadPart;
    m_DeltaTime = (m_CurrTime - m_PrevTime) * m_SecondsPerCount;
    m_PrevTime = m_CurrTime;

    // Force nonnegative. (Under certains circumstances, delta time can be negative.)
    if (m_DeltaTime < 0.0) m_DeltaTime = 0.0;
}
