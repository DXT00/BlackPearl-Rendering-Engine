#pragma once
namespace BlackPearl {

	class TimeCounter
	{
	public:

		static void Start();
		
		static void End(std::string str);
		

	private:
		static double m_StartTimeMs;
	};



}
