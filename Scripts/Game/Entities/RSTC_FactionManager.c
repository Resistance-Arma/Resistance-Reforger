class RSTC_FactionManagerClass: SCR_FactionManagerClass
{
};

class RSTC_FactionManager : SCR_FactionManager
{
	protected ref array<ref Faction> m_aFactions;
	
	Faction GetRSTCFactionByIndex(int index)
	{
		string key = GetFactionByIndex(index).GetFactionKey();
		return GetRSTCFactionByKey(key);
	}
	
	Faction GetRSTCFactionByKey(string key)
	{
		foreach(Faction faction : m_aFactions)
		{
			if(faction.GetFactionKey() == key) return faction;
		}
		return null;
	}
	
	Faction GetInvaderFaction()
	{
		Faction invaderFaction = GetFactionByKey(RSTC_Global.GetConfig().GetInvaderFactionKey());
		if(!invaderFaction)
			return null;
		
		return invaderFaction;
	}
	
	Faction GetOccupyingFaction()
	{
		Faction occupyingFaction = GetFactionByKey(RSTC_Global.GetConfig().GetOccupyingFactionKey());
		if(!occupyingFaction)
			return null;
		
		return occupyingFaction;
	}
	
	Faction GetResistanceFaction()
	{
		Faction resistanceFaction = GetFactionByKey(RSTC_Global.GetConfig().GetResistanceFactionKey());
		if(!resistanceFaction)
		{
			PrintFormat("No Resistance Faction foud in : GetResistanceFaction");
			return null;
		}
		
		return resistanceFaction;
	}
	
	array<FactionKey> GetAvailableRSTCFactionsKey()
	{
		array<FactionKey> factionKeys = {};
		foreach (Faction faction : m_aFactions)
		{
			if (faction)
			{
				factionKeys.Insert(faction.GetFactionKey());
			}
		}
		return factionKeys;
	}
	
	void Init(IEntity owner)
	{
	
	}
	
	void PostGameStart()
	{
		m_aFactions = {};
		
		Faction resistanceFaction = GetFactionByKey(RSTC_Global.GetConfig().GetResistanceFactionKey());
		if(!resistanceFaction)
		{
			PrintFormat("Start Faction : No resistance faction Found with this  factionKey : %1 ", RSTC_Global.GetConfig().GetResistanceFactionKey(), LogLevel.ERROR);
			return;
		}
		
		Faction occupyingFaction = GetFactionByKey(RSTC_Global.GetConfig().GetOccupyingFactionKey());
		if(!occupyingFaction)
		{
			PrintFormat("Start Faction : No occupying faction Found with this  factionKey : %1 ", RSTC_Global.GetConfig().GetResistanceFactionKey(), LogLevel.ERROR);
			return;
		}
		
		
		Faction invaderFaction = GetFactionByKey(RSTC_Global.GetConfig().GetInvaderFactionKey());
		if(!invaderFaction)
		{
			PrintFormat("Start Faction : No invader faction Found with this  factionKey : %1 ", RSTC_Global.GetConfig().GetResistanceFactionKey(), LogLevel.ERROR);
			return;
		}
		
		
		PrintFormat("Start Faction, Ready with faction %1 , %2 , %3", resistanceFaction.GetFactionKey(), occupyingFaction.GetFactionKey(), invaderFaction.GetFactionKey());
		
		m_aFactions.Insert(resistanceFaction);
		m_aFactions.Insert(occupyingFaction);
		m_aFactions.Insert(invaderFaction);
	}
}

