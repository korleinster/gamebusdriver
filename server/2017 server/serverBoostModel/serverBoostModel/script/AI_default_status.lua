
ai_differs = 3	-- 몬스터 전체 종류

ai_status_slime = {
	pos = { x = 134.0, y = 403.0 }, -- 몬스터가 생성될 기본 중점 X Y
	radius = 10,		-- 중점으로 부터 생성될 최대 거리 제한 반지름
	
	howMany	= 10,		-- 맵 상의 슬라임 갯수
	aiMovSpeed = 0.3,	-- AI 초당 움직일 수 있는 최대 거리

	status = {
		maxHp	= 100,	-- 몬스터 최대 hp
	},

	subStatus = {
		crit	= 0,	-- 크리티컬 스텟
		def		= 1,	-- 방어 스텟
		str		= 10,	-- 힘 스텟
		agi		= 2,	-- 민첩 스텟
		intel	= 1,	-- 지능 스텟
		health	= 3,	-- 체력 스텟
	}
}

ai_status_goblin = {
	pos = { x = 134.0, y = 403.0 },
	radius = 10,
	
	howMany	= 10 + ai_status_slime.howMany,
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

ai_status_boss = {
	pos = { x = 150.0, y = 398.0 },
	radius = 1,
	
	howMany	= 1 + ai_status_goblin.howMany,
	aiMovSpeed = 0.7,
	
	status = {
		maxHp	= 1000,
	},

	subStatus = {
		crit	= 5,
		def		= 15,
		str		= 20,
		agi		= 10,
		intel	= 10,
		health	= 10,
	}
}