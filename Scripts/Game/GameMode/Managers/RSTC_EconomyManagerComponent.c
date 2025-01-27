class RSTC_EconomyManagerComponentClass: RSTC_ComponentClass
{
};


class RSTC_EconomyManagerComponent: RSTC_Component
{
	protected RSTC_CitiesManagerComponent m_Cities;
	protected RSTC_PlayerManagerComponent m_Players;
		
	const int ECONOMY_UPDATE_FREQUENCY = 60000;
	
	

  	protected int m_iHourPaidIncome = -1;
	protected int m_iHourPaidStock = -1;
	protected int m_iHourPaidRent = -1;
	
	//Streamed to clients..			
	int m_iResistanceMoney = 0;
	float m_fResistanceTax = 0;
	
	//Events
	ref ScriptInvoker m_OnPlayerMoneyChanged = new ScriptInvoker();
	ref ScriptInvoker m_OnResistanceMoneyChanged = new ScriptInvoker();
	ref ScriptInvoker m_OnPlayerBuy = new ScriptInvoker();
	ref ScriptInvoker m_OnPlayerSell = new ScriptInvoker();
		
	static RSTC_EconomyManagerComponent s_Instance;	
	static RSTC_EconomyManagerComponent GetInstance()
	{
		if (!s_Instance)
		{
			BaseGameMode pGameMode = GetGame().GetGameMode();
			if (pGameMode)
				s_Instance = RSTC_EconomyManagerComponent.Cast(pGameMode.FindComponent(RSTC_EconomyManagerComponent));
		}

		return s_Instance;
	}
	
	void RSTC_EconomyManagerComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{

	}
	
	void CheckUpdate()
	{
		if(!m_Time) 
		{
			ChimeraWorld world = GetOwner().GetWorld();
			m_Time = world.GetTimeAndWeatherManager();
		}
		
		PlayerManager mgr = GetGame().GetPlayerManager();		
		if(mgr.GetPlayerCount() == 0)
		{
			return;
		}
		
		TimeContainer time = m_Time.GetTime();		
		
		//Every 6 hrs get paid
		if((time.m_iHours == 0 
			|| time.m_iHours == 6 
			|| time.m_iHours == 12 
			|| time.m_iHours == 18)
			 && 
			m_iHourPaidIncome != time.m_iHours)
		{
			m_iHourPaidIncome = time.m_iHours;
			CalculateIncome();
		}
	}
	
	
	
	protected void CalculateIncome()
	{
		//Support donations
		int income = 500;
		AddResistanceMoney(income);
		
		PlayerManager mgr = GetGame().GetPlayerManager();
		int count = mgr.GetPlayerCount();
		if(count == 0)
		{
			AddResistanceMoney(income);
			return;
		}
		//Distribute remaining to all players online
		int incomePerPlayer = 500;
		
		autoptr array<int> players = new array<int>;
		mgr.GetPlayers(players);
		foreach(int playerId : players)
		{			
			AddPlayerMoney(playerId, incomePerPlayer);			
		}
	}


	int GetPlayerMoney(string playerId)
	{
		RSTC_PlayerData player = m_Players.GetPlayer(playerId);
		if(!player) return 0;
		return player.money;
	}
	
	int GetLocalPlayerMoney()
	{
		string playerId = RSTC_Global.GetPlayersManager().GetPersistentIDFromPlayerID(SCR_PlayerController.GetLocalPlayerId());
		
		return GetPlayerMoney(playerId);
	}
	
	int LocalPlayerHasMoney(int amount)
	{
		string playerId = RSTC_Global.GetPlayersManager().GetPersistentIDFromPlayerID(SCR_PlayerController.GetLocalPlayerId());		
		return PlayerHasMoney(playerId, amount);
	}
	
	bool PlayerHasMoney(string playerId, int amount)
	{
		RSTC_PlayerData player = m_Players.GetPlayer(playerId);
		if(!player) return false;
		return player.money >= amount;
	}
	
	void AddPlayerMoney(int playerId, int amount, bool doEvent=false)
	{
		if(Replication.IsServer())
		{
			DoAddPlayerMoney(playerId, amount);
			return;
		}
		RSTC_Global.GetServer().AddPlayerMoney(playerId, amount, doEvent);		
	}
	
	void DoAddPlayerMoney(int playerId, int amount)
	{
		string persId = RSTC_Global.GetPlayersManager().GetPersistentIDFromPlayerID(playerId);
		
		RSTC_PlayerData player = m_Players.GetPlayer(persId);
		if(!player) return;
		
		player.money = player.money + amount;
		StreamPlayerMoney(playerId);
		m_OnPlayerMoneyChanged.Invoke(persId, player.money);
	}
	
	void AddResistanceMoney(int amount)
	{
		if(Replication.IsServer())
		{
			DoAddResistanceMoney(amount);
			return;
		}
		RSTC_Global.GetServer().AddResistanceMoney(amount);		
	}
	
	void DoAddResistanceMoney(int amount)
	{		
		RpcDo_SetResistanceMoney(m_iResistanceMoney + amount);
		StreamResistanceMoney();
	}
	
	void TakeResistanceMoney(int amount)
	{
		if(Replication.IsServer())
		{
			DoTakeResistanceMoney(amount);
			return;
		}
		RSTC_Global.GetServer().TakeResistanceMoney(amount);		
	}
	
	void DoTakeResistanceMoney(int amount)
	{		
		RpcDo_SetResistanceMoney(m_iResistanceMoney - amount);
		StreamResistanceMoney();
	}
	

	void TakeLocalPlayerMoney(int amount)
	{
		int id = SCR_PlayerController.GetLocalPlayerId();
		TakePlayerMoney(id, amount);
	}
	
	void TakePlayerMoney(int playerId, int amount)
	{
		if(Replication.IsServer())
		{
			DoTakePlayerMoney(playerId, amount);
			return;
		}
		RSTC_Global.GetServer().TakePlayerMoney(playerId, amount);	
	}
	
	void TakePlayerMoneyPersistentId(string persId, int amount)
	{
		RSTC_PlayerData player = m_Players.GetPlayer(persId);
		if(!player) return;		
		
		player.money = player.money - amount;
		if(player.money < 0) player.money = 0;
		
		int playerId = RSTC_Global.GetPlayersManager().GetPlayerIDFromPersistentID(persId);
		if(playerId > -1){
			StreamPlayerMoney(playerId);	
		}
		m_OnPlayerMoneyChanged.Invoke(persId, player.money);
	}
	
	void AddPlayerMoneyPersistentId(string persId, int amount)
	{
		RSTC_PlayerData player = m_Players.GetPlayer(persId);
		if(!player) return;
		
		player.money = player.money + amount;
		
		int playerId = RSTC_Global.GetPlayersManager().GetPlayerIDFromPersistentID(persId);
		if(playerId > -1){
			StreamPlayerMoney(playerId);	
		}
		m_OnPlayerMoneyChanged.Invoke(persId, player.money);
	}
	
	void DoTakePlayerMoney(int playerId, int amount)
	{
		string persId = RSTC_Global.GetPlayersManager().GetPersistentIDFromPlayerID(playerId);
		RSTC_PlayerData player = m_Players.GetPlayer(persId);
		if(!player) return;
		
		player.money = player.money - amount;
		if(player.money < 0) player.money = 0;
		StreamPlayerMoney(playerId);	
		m_OnPlayerMoneyChanged.Invoke(persId, player.money);
	}
	
	bool ResistanceHasMoney(int amount)
	{
		return m_iResistanceMoney >= amount;
	}
	
	int GetResistanceMoney()
	{
		return m_iResistanceMoney;
	}
	
	
	void Init(IEntity owner)
	{			
		float timeMul = 6;
		RSTC_TimeAndWeatherHandlerComponent tw = RSTC_TimeAndWeatherHandlerComponent.Cast(GetGame().GetGameMode().FindComponent(RSTC_TimeAndWeatherHandlerComponent));
		
		if(tw) timeMul = tw.GetDayTimeMultiplier();
		
		m_Cities = RSTC_Global.GetCitiesManager();
		m_Players = RSTC_Global.GetPlayersManager();
		
		GetGame().GetCallqueue().CallLater(AfterInit, 0);		
		
		if(!Replication.IsServer()) return;
		GetGame().GetCallqueue().CallLater(CheckUpdate, ECONOMY_UPDATE_FREQUENCY / timeMul, true, GetOwner());
		
	}
	
	protected void AfterInit()
	{

		if(!Replication.IsServer()) return;		
		// InitializeShops();		
	}
	
	
	void PostGameStart()
	{		
				
	}


	void ChargeRespawn(int playerId)
	{
		string persId = RSTC_Global.GetPlayersManager().GetPersistentIDFromPlayerID(playerId);
		int money = GetPlayerMoney(persId);
		if (money > 500) {
			int cost = RSTC_Global().GetConfig().GetRespawnCharge();
			TakePlayerMoney(playerId, cost);
		}
	}
	
	//RPC Methods
	
	
	override bool RplSave(ScriptBitWriter writer)
	{		
		writer.WriteInt(m_iResistanceMoney);
		
		return true;
	}
	
	override bool RplLoad(ScriptBitReader reader)
	{		
		if (!reader.ReadInt(m_iResistanceMoney)) return false;

		return true;
	}
	
	
	void StreamPlayerMoney(int playerId)
	{
		string persId = RSTC_Global.GetPlayersManager().GetPersistentIDFromPlayerID(playerId);
		RSTC_PlayerData player = m_Players.GetPlayer(persId);
		if(!player) return;
		
		Rpc(RpcDo_SetPlayerMoney, playerId, player.money);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_SetPlayerMoney(int playerId, int amount)
	{
		string persId = RSTC_Global.GetPlayersManager().GetPersistentIDFromPlayerID(playerId);
		RSTC_PlayerData player = m_Players.GetPlayer(persId);
		if(!player) return;
		player.money = amount;
		m_OnPlayerMoneyChanged.Invoke(persId, player.money);
	}
	
	protected void StreamResistanceMoney()
	{
		Rpc(RpcDo_SetResistanceMoney, m_iResistanceMoney);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_SetResistanceMoney(int amount)
	{
		m_iResistanceMoney = amount;
		m_OnResistanceMoneyChanged.Invoke(m_iResistanceMoney);
	}

		
	void ~RSTC_EconomyManagerComponent()
	{
		GetGame().GetCallqueue().Remove(CheckUpdate);	
	}
}