
ai_differs = 2	-- ���� ��ü ����

ai_status_slime = {
	pos = { x = 150.0, y = 400.0 }, -- ���Ͱ� ������ �⺻ ���� X Y
	radius = 10,		-- �������� ���� ������ �ִ� �Ÿ� ���� ������
	
	howMany	= 10,		-- �� ���� ������ ���� ******** ���� ������ �����ϸ� protocol.h �� �ִ� �ִ� ���� ������ �����ؾ� ��!!
	aiMovSpeed = 0.5,	-- AI �ʴ� ������ �� �ִ� �ִ� �Ÿ�

	status = {
		maxHp	= 100,	-- ���� �ִ� hp
	},

	subStatus = {
		crit	= 0,	-- ũ��Ƽ�� ����
		def		= 1,	-- ��� ����
		str		= 5,	-- �� ����
		agi		= 2,	-- ��ø ����
		intel	= 1,	-- ���� ����
		health	= 3,	-- ü�� ����
	}
}

ai_status_goblin = {
	pos = { x = 430.0, y = 300.0 },
	radius = 10,
	
	howMany	= 10,
	aiMovSpeed = 0.5,
	
	status = {
		maxHp	= 100,
	},

	subStatus = {
		crit	= 5,
		def		= 10,
		str		= 10,
		agi		= 10,
		intel	= 10,
		health	= 10,
	}
}