modded class SCR_ScenarioFrameworkLayerBase
{
	protected RSTC_CityComponent m_city;
	
	override protected bool InitFactionSettings()
	{
	    if (m_sFactionKey == "OCCUPANT")
	    {
	        IEntity currentEntity = GetOwner();
			
			RSTC_CityComponent parentCity = GetParentCity();
			
			if(!parentCity)
				return false;
	
			FactionKey cityFactionKey = parentCity.GetCitySupportedFactionKey();
            if (cityFactionKey)
            {
                SetFactionKey(cityFactionKey);
                return true;
            }

	        Print("Aucun RSTC_CityComponent trouvé dans la hiérarchie des parents pour la faction OCCUPANT.", LogLevel.ERROR);
	        return false;
	    }
		
		if (m_sFactionKey == "PLAYER")
		{
			SetFactionKey(RSTC_Global.GetConfig().GetResistanceFactionKey());
			return true;
		}
	
	    if (SCR_StringHelper.IsEmptyOrWhiteSpace(m_sFactionKey))
	    {
	        if (!m_ParentLayer)
	        {
	            Print("m_ParentLayer est NULL. Impossible d'hériter de la faction.", LogLevel.WARNING);
	            return true;
	        }
	
	        FactionKey parentFactionKey = m_ParentLayer.GetFactionKey();
	        if (!SCR_StringHelper.IsEmptyOrWhiteSpace(parentFactionKey))
	        {
	            SetFactionKey(parentFactionKey);
	        }
	        return true;
	    }
	
	    FactionManager factionManager = GetGame().GetFactionManager();
	    if (!factionManager)
	    {
	        return false;
	    }
	
	    SCR_FactionAliasComponent factionAliasComponent = SCR_FactionAliasComponent.Cast(factionManager.FindComponent(SCR_FactionAliasComponent));
	    if (factionAliasComponent)
	    {
	        SetFactionKey(factionAliasComponent.ResolveFactionAlias(GetFactionKey()));
	    }
	    return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! \return parent city of the entity or null if not found.
	RSTC_CityComponent GetParentCity()
	{
		if (m_city)
			return m_city;

		IEntity entity = GetOwner().GetParent();
		while (entity)
		{
			m_city = RSTC_CityComponent.Cast(entity.FindComponent(RSTC_CityComponent));
			if (m_city)
				return m_city;

			entity = entity.GetParent();
		}

		return null;
	}


}