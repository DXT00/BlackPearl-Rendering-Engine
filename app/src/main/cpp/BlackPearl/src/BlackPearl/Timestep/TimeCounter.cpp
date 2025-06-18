#include "pch.h"
#include "Timestep/TimeCounter.h"
#include <chrono>
#include "BlackPearl/Core.h"
namespace BlackPearl {
	using namespace std::chrono;

	double TimeCounter::m_StartTimeMs = 0;


	void TimeCounter::Start()
	{
		
			m_StartTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

		
	}

	void TimeCounter::End(std::string str)
	{
		
		milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		double runtime = currentTimeMs.count() - m_StartTimeMs;

		GE_CORE_WARN("%s total time:%f ms", str.c_str(), runtime );/// 1000.0f
		
	}

    TimeLocalCounter::TimeLocalCounter(const std::string eventStr){
        m_EventStr = eventStr;
        Start();
    }
    TimeLocalCounter::~TimeLocalCounter(){
        End();
    }
    void TimeLocalCounter::Start()
    {

        m_LocalStartTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();


    }

    void TimeLocalCounter::End()
    {

        milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        double runtime = currentTimeMs.count() - m_LocalStartTimeMs;

#ifdef GE_PLATFORM_WINDOWS
        GE_CORE_WARN("[time] {} total time:{:.2f} ms", m_EventStr.c_str(), runtime);
        if (m_EventStr == "FPS") {
            GE_CORE_WARN("[time] FPS {:.2f} ", 1000.0f/runtime);

        }
#elif defined GE_PLATFORM_ANDROID
        GE_CORE_WARN("[time] %s total time:%.2lf ms", m_EventStr.c_str(), runtime);
        if (m_EventStr == "FPS") {
            GE_CORE_WARN("[time] FPS %.2lf ", 1000.0 / runtime);

        }
#endif

    }

}
