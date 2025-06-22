#pragma once
namespace BlackPearl {

#define SCOPE_TIME_COUNTER(event) \
	TimeLocalCounter Count_##event(#event);


    //global counter
	class TimeCounter
	{
	public:

		static void Start();
		
		static void End(std::string str);
		

	private:
		static double m_StartTimeMs;
	};


    class TimeLocalCounter
    {
    public:
        TimeLocalCounter(const std::string eventStr);
        ~TimeLocalCounter();

        double GetRuntime() const { return m_RuntimeMs; }

    private:
        void Start();
        void End();
        double m_LocalStartTimeMs;
        std::string m_EventStr;
        double m_RuntimeMs = 0.0;

    };


}
