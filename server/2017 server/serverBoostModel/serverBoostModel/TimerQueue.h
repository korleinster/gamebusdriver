#pragma once

enum time_queue_event
{
	HP_ADD = 1,

};

using event_type = struct Event_type
{
	unsigned int obj_id;
	unsigned int wakeup_time;
	int event_id;
	bool is_ai{ false };
};

//auto wake_time_cmp = [](const event_type lhs, const event_type rhs) -> const bool { return (lhs.wakeup_time > rhs.wakeup_time); };
class wake_time_cmp { public: bool operator() (const event_type *lhs, const event_type *rhs) const { return (lhs->wakeup_time > rhs->wakeup_time); } };

class TimerQueue
{
public:
	TimerQueue() {};
	~TimerQueue() {};

	void lock() { time_lock.lock(); }
	void unlock() { time_lock.unlock(); }

	void TimerThread();
	void add_event(const unsigned int& id, const int& sec, time_queue_event type, bool is_ai);
private:
	void processPacket(event_type *p);

	// lock
	mutex time_lock;

	// timer thread queue
	priority_queue < event_type*, vector<event_type*>, wake_time_cmp/*decltype(wake_time_cmp)*/ > timer_queue;
};
