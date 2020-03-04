#pragma once


typedef enum {
		STOP = 0,
		GENERIC,
		NUM_EVENTS
} Event_t;

void *eventHandler(void *args);

class Eventor {
		private:
		public:
				Eventor();
				~Eventor();
};
