#include "stdafx.h"

player_class::player_class()
{
}

player_class::~player_class()
{
}

void player_class::process_packet(Packet *buf)
{
	char type = buf[1];

	switch (type)
	{
	case CHAT: {
		
		break;
	}
	default:
		break;
	}
}

int player_class::key_input(WPARAM keyboard)
{
	switch (keyboard)
	{
	case VK_RETURN: {
		

		break;
	}
	case VK_ESCAPE: {
		PostQuitMessage(0);
		return 0;
	}
	default:
		return 0;
		break;
	}
}
