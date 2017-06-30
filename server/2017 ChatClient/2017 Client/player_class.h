#pragma once
class player_class
{
	//POS m_pos;
	unsigned long long m_id = { 0 };
	//unordered_map<unsigned long long, POS> m_other_players;

public:
	player_class();
	~player_class();

	void process_packet(Packet*);
	int key_input(WPARAM);

	//inline POS* get_pos() { return &m_pos; };
	//inline unordered_map<unsigned long long, POS>* get_other_players() { return &m_other_players; }
};

