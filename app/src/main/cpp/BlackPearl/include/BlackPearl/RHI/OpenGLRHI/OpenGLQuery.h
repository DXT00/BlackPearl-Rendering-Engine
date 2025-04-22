#pragma once
#include "BlackPearl/RHI/RHIQuery.h"
#include "BlackPearl\RHI\OpenGLRHI\OpenGLDriver\OpenGLFunctions.h"

namespace BlackPearl {
	class OpenGLQuery
	{
	};


	 class EventQuery : public RefCounter<IEventQuery> {

	 public:

		 EventQuery();
		 ~EventQuery();

		 /** Issues an event for the query to poll. */
		 void IssueEvent();

		 /** Waits for the event query to finish. */
		 void WaitForCompletion();

	 private:
		 GLsync Sync = {};
	};

}

