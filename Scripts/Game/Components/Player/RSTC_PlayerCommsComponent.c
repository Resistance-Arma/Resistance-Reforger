class RSTC_PlayerCommsComponentClass: RSTC_ComponentClass
{
};

class RSTC_PlayerCommsComponent: RSTC_Component
{
	
	void RequestSave()
	{
		Rpc(RpcAsk_RequestSave);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_RequestSave()
	{
		RSTC_GameMode gameMode = RSTC_GameMode.Cast(GetGame().GetGameMode());
		if(!gameMode) return;
		RSTC_PersistenceManagerComponent persistence = gameMode.GetPersistence();
		if(persistence && persistence.IsActive())
		{
			persistence.SaveGame();
		}
	}
	

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    void Rpc_ShowStartGameUI()
    {
		 Print("[RSTC_PlayerCommsComponent] Rpc_ShowStartGameUI received on admin client!");
    
        RSTC_UIManagerComponent uimanager = RSTC_UIManagerComponent.Cast(GetOwner().FindComponent(RSTC_UIManagerComponent));
        if (!uimanager)
        {
            Print("[RSTC_PlayerCommsComponent] RSTC_UIManagerComponent not found!");
            return;
        }
    
        RSTC_StartGameContext context = RSTC_StartGameContext.Cast(uimanager.GetContext(RSTC_StartGameContext));
        if (!context)
        {
            Print("[RSTC_PlayerCommsComponent] RSTC_StartGameContext not found!");
            return;
        }
    
        uimanager.ShowContext(RSTC_StartGameContext);
		
        // This will execute on the client's machine
      
    }

    void ShowStartGameUI()
    {
         Rpc(Rpc_ShowStartGameUI);
    }

	//ECONOMY
	
	void AddPlayerMoney(int playerId, int amount, bool doEvent=false)
	{
		Rpc(RpcAsk_AddPlayerMoney, playerId, amount, doEvent);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_AddPlayerMoney(int playerId, int amount, bool doEvent)
	{
		RSTC_Global.GetEconomyManager().DoAddPlayerMoney(playerId, amount);
		if(doEvent)
		{
			RSTC_Global.GetEconomyManager().m_OnPlayerSell.Invoke(playerId, amount);
		}		
	}
	
	void TakePlayerMoney(int playerId, int amount)
	{
		Rpc(RpcAsk_TakePlayerMoney, playerId, amount);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_TakePlayerMoney(int playerId, int amount)
	{
		RSTC_Global.GetEconomyManager().DoTakePlayerMoney(playerId, amount);		
	}
	
	void AddResistanceMoney(int amount)
	{
		Rpc(RpcAsk_AddResistanceMoney, amount);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_AddResistanceMoney(int amount)
	{
		RSTC_Global.GetEconomyManager().DoAddResistanceMoney(amount);		
	}
	
	
	void TakeResistanceMoney(int amount)
	{
		Rpc(RpcAsk_TakeResistanceMoney, amount);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_TakeResistanceMoney(int amount)
	{
		RSTC_Global.GetEconomyManager().DoTakeResistanceMoney(amount);		
	}
	
	void RequestFastTravel(int playerId, vector pos)	
	{		
		Rpc(RpcAsk_RequestFastTravel, playerId, pos);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_RequestFastTravel(int playerId, vector pos)	
	{
		SCR_Global.TeleportPlayer(playerId, pos);
	}	
	
}