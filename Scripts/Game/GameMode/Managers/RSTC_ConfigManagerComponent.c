class RSTC_ConfigManagerComponentClass : RSTC_ComponentClass
{
};

[EntityEditorProps(category: "GameScripted/Config", description: "Config Manager for the all game settings")]
class RSTC_ConfigManagerComponent : RSTC_Component
{

    [Attribute("10", desc: "Maximum number of civilian entities in the city", category: "City")]
    int m_fMaxCivilPerCity;
	
    [Attribute("10", desc: "Maximum number of ennemy entities in the city", category: "City")]
    int m_fMaxEnnemyPerCity;

	[Attribute(params: "et", desc: "Resource name of the Slot Civil to be spawned", category: "Prefabs")]
	ResourceName m_pSlotCivilPrefabToSpawn;
	
	[Attribute(defvalue: "RHS_USAF", desc: "Default Faction supported by a city", category: "City")]
	FactionKey m_sDefaultSupportedFactionKey;
	
	[Attribute( defvalue: "RHS_ION", uiwidget: UIWidgets.EditBox, desc: "Faction for player side", category: "Player")]
	FactionKey m_sResistanceFaction;
	
	[Attribute( defvalue: "RHS_USAF", uiwidget: UIWidgets.EditBox, desc: "Faction for player side", category: "Player")]
	FactionKey m_sOccupyingFaction;
	
	[Attribute( defvalue: "RHS_AFRF", uiwidget: UIWidgets.EditBox, desc: "Faction for player side", category: "Player")]
	FactionKey m_sInvaderFaction;
	
	[Attribute( defvalue: "3", desc: "Maximal number of tasks that can be generated", category: "Tasks" )];
	int m_iMaxNumberOfTasks;
	
	[Attribute( defvalue: "300", desc: "Starting game money", category: "Difficulty" )];
	int m_iStartingMoney;
	
	[Attribute( defvalue: "50", desc: "Price charge every Respawn", category: "Difficulty" )];
	int m_iRespawnCharge;
	
	[Attribute( defvalue: "60", desc: "Radius of the HQ Area", category: "HQ" )];
	float m_fHQRadius;
	
	[Attribute(params: "et", desc: "Petros Prefab", category: "Prefabs")]
	ResourceName m_pPetrosPrefab;
	
	[Attribute( defvalue: "0 0 0", desc: "Initial Petros position", category: "HQ" )];
	vector m_sInitialPetrosPosition;
	
	[Attribute(desc: "Should the dynamic Spawn/Despawn based on distance from player characters be enabled for the whole GameMode?", category: "Dynamic Spawn/Despawn")];
	bool m_bDynamicDespawn;
	
	[Attribute(defvalue: "1000", desc: "Petros search Radius for searching city and launch task", category: "Tasks" )];
    float m_fPretrosSearchCityForTaskRadius;
	
	[Attribute(defvalue: EAISkill.REGULAR.ToString(), UIWidgets.ComboBox, "AI skill in combat", "", ParamEnumArray.FromEnum(EAISkill), category: "Common")]
	EAISkill m_eAISkill;
	
	private static RSTC_ConfigManagerComponent s_Instance = null;

	static RSTC_ConfigManagerComponent GetInstance()
	{
		if (!s_Instance)
		{
			BaseGameMode pGameMode = GetGame().GetGameMode();
			if (pGameMode)
				s_Instance = RSTC_ConfigManagerComponent.Cast(pGameMode.FindComponent(RSTC_ConfigManagerComponent));
		}

		return s_Instance;
	}
	
	EAISkill GetAISkill()
	{
		return m_eAISkill;
	}
	
	int GetRespawnCharge()
	{
		return m_iRespawnCharge;
	}
	
	int GetStartingMoney()
	{
		return m_iStartingMoney;
	}
	
	int GetMaxCivilPerCity()
	{
		return m_fMaxCivilPerCity;
	}
	
	int GetMaxEnnemyPerCity()
	{
		return m_fMaxEnnemyPerCity;
	}
	
	int GetMaxNumberOfTasks()
	{
		return m_iMaxNumberOfTasks;
	}
	
	bool GetDynamicDespawn()
	{
		return m_bDynamicDespawn;
	}
	
	FactionKey GetDefaultSupportedFactionKey()
	{
		return m_sDefaultSupportedFactionKey;
	}
	
	float GetPretrosSearchCityForTaskRadius()
	{
		return m_fPretrosSearchCityForTaskRadius;
	}
	
	float GetHQRadius()
	{
		return m_fHQRadius;
	}
		
	FactionKey GetResistanceFactionKey()
	{
		return m_sResistanceFaction;
	}
	
	FactionKey GetInvaderFactionKey()
	{
		return m_sInvaderFaction;
	}
	
	FactionKey GetOccupyingFactionKey()
	{
		return m_sOccupyingFaction;
	}
	
	ResourceName GetPretrosPrefab()
	{
		return m_pPetrosPrefab;
	}
	
	vector GetInitialPetrosPosition()
	{
		return m_sInitialPetrosPosition;
	}
	
	ResourceName GetSlotCivilPrefabToSpawn()
	{
		return m_pSlotCivilPrefabToSpawn;
	}
	
	
	void SetResistanceFactionKey(FactionKey faction)
	{
		m_sResistanceFaction = faction;
	}
	
	void SetInvaderFactionKey(FactionKey faction)
	{
		m_sInvaderFaction = faction;
	}
	
	void SetOccupyingFactionKey(FactionKey faction)
	{
		m_sOccupyingFaction = faction;
	}
	
	  void SetMaxCivilPerCity(int value)
    {
        m_fMaxCivilPerCity = value;
    }

    void SetMaxEnnemyPerCity(int value)
    {
        m_fMaxEnnemyPerCity = value;
    }

    void SetResistanceFaction(FactionKey faction)
    {
        m_sResistanceFaction = faction;
    }

    void SetOccupyingFaction(FactionKey faction)
    {
        m_sOccupyingFaction = faction;
    }

    void SetInvaderFaction(FactionKey faction)
    {
        m_sInvaderFaction = faction;
    }

    void SetMaxNumberOfTasks(int value)
    {
        m_iMaxNumberOfTasks = value;
    }

    void SetRespawnCharge(int value)
    {
        m_iRespawnCharge = value;
    }

    void SetHQRadius(float value)
    {
        m_fHQRadius = value;
    }

    void SetPetrosSearchCityForTaskRadius(float value)
    {
        m_fPretrosSearchCityForTaskRadius = value;
    }

    void SetAISkill(EAISkill value)
    {
        m_eAISkill = value;
    }
	
	
	
}