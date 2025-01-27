class RSTC_CitiesManagerComponentClass : RSTC_ComponentClass
{
};

[EntityEditorProps(category: "GameScripted/ScenarioFramework", description: "Manages RSTC_City entities.")]
class RSTC_CitiesManagerComponent : RSTC_Component
{
    protected ref array<RSTC_CityComponent> m_Cities = {};
    
    static RSTC_CitiesManagerComponent s_Instance;	
    static RSTC_CitiesManagerComponent GetInstance()
    {
        if (!s_Instance)
        {
            BaseGameMode pGameMode = GetGame().GetGameMode();
            if (pGameMode)
                s_Instance = RSTC_CitiesManagerComponent.Cast(pGameMode.FindComponent(RSTC_CitiesManagerComponent));
        }

        return s_Instance;
    }
    
	
	void PostGameStart()
	{		
		IEntity citiesEntity = GetGame().FindEntity("Cities");
        if (!citiesEntity)
        {
            PrintFormat("RSTC_CitiesManagerComponent:: Entity 'Cities' not found in the map!", LogLevel.ERROR);
            return;
        }
    
        IEntity child = citiesEntity.GetChildren();
        int cityCount = 0;
    
		RSTC_FactionManager factionManager = RSTC_Global().GetFactionManager();
		if (!factionManager)
			return;
		
        array<FactionKey> availableFactions = factionManager.GetAvailableRSTCFactionsKey();
    
        while (child)
        {
            RSTC_CityComponent city = RSTC_CityComponent.Cast(child.FindComponent(RSTC_CityComponent));
            if (city)
            {
                m_Cities.Insert(city);
                PrintFormat("RSTC_CitiesManagerComponent:: City added: %1", city.GetOwner().GetName());
                city.Setup(cityCount, availableFactions);
                cityCount++;
            }
            child = child.GetSibling();
        }
    
        PrintFormat("RSTC_CitiesManagerComponent:: City fully ready. Total: %1", m_Cities.Count());	
	}
	
	
    //! Initializes all available cities in the hierarchy
    void Init(IEntity owner)
    {
       
    }
    
    void UpdateCities()
    {
        foreach (RSTC_CityComponent city : m_Cities)
        {
            if (!city) continue;

            city.UpdateCityFaction();
        }
    }
    
    array<RSTC_CityComponent> GetAllCities()
    {
        return m_Cities;
    }

    void CleanupRemovedCities()
    {
        for (int i = m_Cities.Count() - 1; i >= 0; i--)
        {
            if (!m_Cities[i] || !m_Cities[i].GetOwner())
                m_Cities.Remove(i);
        }
    }
    
    array<RSTC_CityComponent> FindNearbyCities(vector center, float radius)
    {
        array<RSTC_CityComponent> nearbyCities = {};
    
        foreach (RSTC_CityComponent city : m_Cities)
        {
            if (!city || !city.GetOwner())
                continue;
    
            vector cityPosition = city.GetCityLocation();
            float distance = vector.Distance(center, cityPosition);
    
            if (distance <= radius)
                nearbyCities.Insert(city);
        }
    
        return nearbyCities;
    }
    
    array<RSTC_CityComponent> FindNearbyCitiesWithTaskTypeInside(vector center, float radius, RSTC_ESFTaskType taskType)
    {
        array<RSTC_CityComponent> citiesWithTaskType = {};
    
        foreach (RSTC_CityComponent city : m_Cities)
        {
            if (!city || !city.GetOwner())
                continue;
            
            vector cityPosition = city.GetCityLocation();
            float distance = vector.Distance(center, cityPosition);
    
            if (distance > radius)
                continue;
    
            if(city.IsValidForTaskType(taskType)) 
                citiesWithTaskType.Insert(city);
        }

        return citiesWithTaskType;
    }

    RSTC_CityComponent GetRandomCityInList(array<RSTC_CityComponent> cities)
    {
        if (cities.IsEmpty())
        {
            PrintFormat("RSTC_CitiesManagerComponent:: No cities available in the provided list.", LogLevel.WARNING);
            return null;
        }
    
        Math.Randomize(-1);
        int randomIndex = Math.RandomInt(0, cities.Count());
        RSTC_CityComponent randomCity = cities[randomIndex];
    
        return randomCity;
    }
    
    //! Returns a random occupied city from a given list.
    RSTC_CityComponent GetRandomOccupiedCityInList(array<RSTC_CityComponent> cities)
    {
        if (cities.IsEmpty())
        {
            PrintFormat("RSTC_CitiesManagerComponent:: No cities available in the provided list.", LogLevel.WARNING);
            return null;
        }
    
        array<RSTC_CityComponent> occupiedCities = {};
    
        foreach (RSTC_CityComponent city : cities)
        {
            if (!city)
                continue;
    
            if (city.GetCitySupportedFactionKey() != RSTC_Global.GetConfig().GetResistanceFactionKey()) 
                occupiedCities.Insert(city);
        }
    
        if (occupiedCities.IsEmpty())
        {
            PrintFormat("RSTC_CitiesManagerComponent:: No occupied cities found in the list.", LogLevel.WARNING);
            return null;
        }
    
        Math.Randomize(-1);
        int randomIndex = Math.RandomInt(0, occupiedCities.Count());
        RSTC_CityComponent randomOccupiedCity = occupiedCities[randomIndex];

        return randomOccupiedCity;
    }
}
