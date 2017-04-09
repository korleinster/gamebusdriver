#include "stdafx.h"

void TimerQueue::TimerThread() {
	while (true) {
		Sleep(1);
		time_lock.lock();
		while (false == timer_queue.empty()) {
			if (timer_queue.top()->wakeup_time > GetTickCount()) { break; }

			event_type *event_ptr = timer_queue.top();

			timer_queue.pop();
			time_lock.unlock();

			processPacket(event_ptr);
			if (event_ptr != nullptr) { delete event_ptr; }

			time_lock.lock();
		}
		time_lock.unlock();
	}
}

void TimerQueue::add_event(const unsigned int& id, const int& sec, time_queue_event type, bool is_ai) {

	event_type *ptr = new event_type;

	ptr->obj_id = id;
	ptr->wakeup_time = GetTickCount() + (sec * 1000);
	ptr->event_id = type;
	ptr->is_ai = is_ai;

	time_lock.lock();
	timer_queue.push(ptr);
	time_lock.unlock();
}

void TimerQueue::processPacket(event_type *p) {

	switch (p->event_id)
	{
	case HP_ADD: {	// 1초마다 hp 5씩 채우기

		// 이미 통신이 끊기거나 ( ai 가 죽은 녀석이면 pass )
		if (DISCONNECTED == g_clients[p->obj_id]->get_current_connect_state()) { break; }

		int adding_hp_size = 5;

		// hp가 maxhp 이상이 아니면, 아래 실행
		if (false == (g_clients[p->obj_id]->get_player_data()->state.hp > (g_clients[p->obj_id]->get_player_data()->state.maxhp - 1))) {
			g_clients[p->obj_id]->get_player_data()->state.hp += adding_hp_size;

			// 만피가 되었다면, 계속 hp 더해주는 모드 끄기
			if (g_clients[p->obj_id]->get_player_data()->state.maxhp == g_clients[p->obj_id]->get_player_data()->state.hp) { *g_clients[p->obj_id]->get_hp_adding() = false; }
			add_event(p->obj_id, 1, HP_ADD, false);

			//Packet buf[MAX_BUF_SIZE]{ 0 };
			//buf[0] = (sizeof(int) * 2) + 2;	// 패킷 size
			//buf[1] = SERVER_MESSAGE_HP_CHANGED;
			//*reinterpret_cast<int *>(&buf[2]) = g_clients[p->obj_id]->get_player_data()->state.hp;	// hp 입력
			//*reinterpret_cast<int *>(&buf[6]) = p->obj_id;	// id 입력

			sc_hp packet;
			packet.hp == g_clients[p->obj_id]->get_player_data()->state.hp;
			packet.id = p->obj_id;

			for (auto players : g_clients) {
				if (DISCONNECTED == players->get_current_connect_state()) { continue; }
				//if (players->m_id == other_players->m_id) { continue; }	// 자기 hp 가 변해도 해당 패킷을 받아야 한다.
				if (true == players->get_player_data()->is_ai) { continue; }

				players->send_packet(reinterpret_cast<Packet*>(&packet));
			}
		}
	}
		break;
	default:
		break;
	}
}