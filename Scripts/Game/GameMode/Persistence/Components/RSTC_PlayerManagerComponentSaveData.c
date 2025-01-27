[EPF_ComponentSaveDataType(RSTC_PlayerManagerComponent), BaseContainerProps()]
class RSTC_PlayerManagerComponentSaveDataClass : EPF_ComponentSaveDataClass
{
};

[EDF_DbName.Automatic()]
class RSTC_PlayerManagerComponentSaveData : EPF_ComponentSaveData
{
	ref map<string, ref RSTC_PlayerData> m_mPlayers;
	
	override EPF_EReadResult ReadFrom(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
	{		
		RSTC_PlayerManagerComponent players = RSTC_PlayerManagerComponent.Cast(component);
		
		m_mPlayers = players.m_mPlayers;
		
		return EPF_EReadResult.OK;
	}
	
	override EPF_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
	{
		RSTC_PlayerManagerComponent players = RSTC_PlayerManagerComponent.Cast(component);
		
		if(!m_mPlayers) return EPF_EApplyResult.OK;
		
		for(int t=0; t< m_mPlayers.Count(); t++)
		{
			string persId = m_mPlayers.GetKey(t);
			RSTC_PlayerData player = m_mPlayers.GetElement(t);
			players.m_mPlayers[persId] = player;
			players.m_OnPlayerDataLoaded.Invoke(player, persId);
		}
				
		return EPF_EApplyResult.OK;
	}
}