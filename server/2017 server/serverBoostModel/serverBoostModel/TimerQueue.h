#pragma once

enum time_queue_event
{
	HP_ADD = 1,
	DEAD_TO_ALIVE,
	CHANGE_PLAYER_STATE,
	FEVER_REDUCE,
	POSTION,
	AI_STATE_ATT,
	AI_STATE_RAND_MOV,
	AI_STATE_RAND_MOV_STOP,
	AI_STATE_TRACKING_PLAYER,

	CHANGE_AI_STATE_ATT,
	CHANGE_AI_STATE_MOV,
};

using event_type = struct Event_type
{
	unsigned int id;
	unsigned int wakeup_time;
	int type;
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
	void add_event(const unsigned int& id, const float& sec, time_queue_event type, bool is_ai);
private:
	void processPacket(event_type *p);

	// lock
	mutex time_lock;

	// timer thread queue
	priority_queue < event_type*, vector<event_type*>, wake_time_cmp/*decltype(wake_time_cmp)*/ > timer_queue;
};