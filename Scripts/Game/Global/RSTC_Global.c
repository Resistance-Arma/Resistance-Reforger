class RSTC_Global : Managed
{	
	static RSTC_PlayerCommsComponent GetServer()
	{		
		if(Replication.IsServer())
		{
			return RSTC_PlayerCommsComponent.Cast(GetGameMode().FindComponent(RSTC_PlayerCommsComponent));
		}		
		IEntity player = SCR_PlayerController.GetLocalControlledEntity();
		return RSTC_PlayerCommsComponent.Cast(player.FindComponent(RSTC_PlayerCommsComponent));
	}
	
	
	static RSTC_GameMode GetGameMode()
	{
		return RSTC_GameMode.Cast(GetGame().GetGameMode());
	}
	
	static RSTC_NpcPetros GetPetros()
	{
		return RSTC_NpcPetros.Cast(GetGame().GetWorld().FindEntityByName("Petros"));
	}
	
	static RSTC_FactionManager GetFactionManager()
	{
		return RSTC_FactionManager.Cast(GetGame().GetFactionManager());
	}
	
	static RSTC_ConfigManagerComponent GetConfig()
	{
		return RSTC_ConfigManagerComponent.GetInstance();
	}
	
	static RSTC_EconomyManagerComponent GetEconomyManager()
	{
		return RSTC_EconomyManagerComponent.GetInstance();
	}
	
	static RSTC_CitiesManagerComponent GetCitiesManager()
	{
		return RSTC_CitiesManagerComponent.GetInstance();
	}
	
	static RSTC_TasksManagerComponent GetTasksManager()
	{
		return RSTC_TasksManagerComponent.GetInstance();
	}
	
	static RSTC_PlayerManagerComponent GetPlayersManager()
	{
		return RSTC_PlayerManagerComponent.GetInstance();
	}
	
	static RSTC_ResistanceFactionManager GetResistanceFactionManager()
	{
		return RSTC_ResistanceFactionManager.GetInstance();
	}
	
}