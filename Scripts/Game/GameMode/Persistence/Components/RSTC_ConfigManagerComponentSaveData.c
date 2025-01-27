[EPF_ComponentSaveDataType(RSTC_ConfigManagerComponent), BaseContainerProps()]
class RSTC_ConfigManagerComponentSaveDataClass : EPF_ComponentSaveDataClass
{
};

[EDF_DbName.Automatic()]
class RSTC_ConfigManagerComponentSaveData : EPF_ComponentSaveData
{
	int m_fMaxCivilPerCity;
	int m_fMaxEnnemyPerCity;
	FactionKey m_sResistanceFaction;
	FactionKey m_sOccupyingFaction;
	FactionKey m_sInvaderFaction;
	int m_iMaxNumberOfTasks;
	int m_iRespawnCharge;
	float m_fHQRadius;
	float m_fPretrosSearchCityForTaskRadius;
	EAISkill m_eAISkill;
	
	override EPF_EReadResult ReadFrom(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
	{		
		RSTC_ConfigManagerComponent config = RSTC_ConfigManagerComponent.Cast(component);
		
		m_fMaxCivilPerCity = config.GetMaxCivilPerCity();
	    m_fMaxEnnemyPerCity = config.GetMaxEnnemyPerCity();
	    m_sResistanceFaction = config.GetResistanceFactionKey();
	    m_sOccupyingFaction = config.GetOccupyingFactionKey();
	    m_sInvaderFaction = config.GetInvaderFactionKey();
	    m_iMaxNumberOfTasks = config.GetMaxNumberOfTasks();
	    m_iRespawnCharge = config.GetRespawnCharge();
	    m_fHQRadius = config.GetHQRadius();
	    m_fPretrosSearchCityForTaskRadius = config.GetPretrosSearchCityForTaskRadius();
	    m_eAISkill = config.GetAISkill();
		
		
		return EPF_EReadResult.OK;
	}
	
	override EPF_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
	{
		RSTC_ConfigManagerComponent config = RSTC_ConfigManagerComponent.Cast(component);
		
		config.SetMaxCivilPerCity(m_fMaxCivilPerCity);
	    config.SetMaxEnnemyPerCity(m_fMaxEnnemyPerCity);
	    config.SetResistanceFactionKey(m_sResistanceFaction);
	    config.SetOccupyingFactionKey(m_sOccupyingFaction);
	    config.SetInvaderFactionKey(m_sInvaderFaction);
	    config.SetMaxNumberOfTasks(m_iMaxNumberOfTasks);
	    config.SetRespawnCharge(m_iRespawnCharge);
	    config.SetHQRadius(m_fHQRadius);
	    config.SetPetrosSearchCityForTaskRadius(m_fPretrosSearchCityForTaskRadius);
	    config.SetAISkill(m_eAISkill);
					
		return EPF_EApplyResult.OK;
	}
}