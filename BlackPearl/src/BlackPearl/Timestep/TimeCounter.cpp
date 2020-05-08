#include "pch.h"
#include "TimeCounter.h"
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

		GE_CORE_WARN("{0} total time:{1}", str, runtime / 1000.0f);
		
	}

}