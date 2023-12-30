#pragma once

class Timer
{
public:
    /// <summary>
    /// Create and start a new timer.
    /// </summary>
    Timer();
    /// <summary>
    /// Unpauses the timer.
    /// </summary>
    void Resume();
    /// <summary>
    /// Pauses the timer.
    /// </summary>
    void Stop();
    /// <summary>
    /// Resets and restarts the timer.
    /// </summary>
    void Reset();
    /// <summary>
    /// Call this to update all the timer values.
    /// </summary>
    void Tick();

public: // Const methods, will be available in const objects.
    /// <summary>
    /// Gets the time (in seconds) elapsed since the last frame.
    /// </summary>
    float DeltaTime() const { return (float)m_DeltaTime; }
    /// <summary>
    /// Gets the time (in seconds) elapsed since the last frame.
    /// </summary>
    double PreciseDeltaTime() const { return m_DeltaTime; }
    /// <summary>
    /// Gets the total time (in seconds) elapsed since the timer started.
    /// </summary>
    float TotalTime() const { return ((m_IsStopped ? m_StopTime : m_CurrTime) - m_PausedTime - m_BaseTime) * (float)m_SecondsPerCount; }
    /// <summary>
    /// Gets the total time (in seconds) elapsed since the timer started.
    /// </summary>
    double PreciseTotalTime() const { return ((m_IsStopped ? m_StopTime : m_CurrTime) - m_PausedTime - m_BaseTime) * m_SecondsPerCount; }

private:
    bool m_IsStopped;           // True if the timer is stopped
    double m_SecondsPerCount;   // 1.0 / Frequency (how many counts per second)
    double m_DeltaTime;         // Time elapsed since the last tick

    __int64 m_CurrTime;         // Current time
    __int64 m_PrevTime;         // Previous time
    __int64 m_PausedTime;       // Number of counts when the timer was paused
    __int64 m_StopTime;         // Time at which the timer was stopped
    __int64 m_BaseTime;         // Time at which the timer was started (or reset)
};
