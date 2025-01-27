class RSTC_MapCitiesLocation : SCR_MapUIBaseComponent
{
	protected ref map<int, ref Widget> m_Widgets; // MapID -> Widget
	
	[Attribute()]
	protected ResourceName m_Layout; // Layout du widget pour une ville
	
	override void Init()
	{
		super.Init();
		m_Widgets = new map<int, ref Widget>;
	}
	
	override void OnMapOpen(MapConfiguration config)
	{
		super.OnMapOpen(config);
				
		FactionManager factionManager = GetGame().GetFactionManager();
		if(!factionManager)
		{
			return;
		}
		
		RSTC_CitiesManagerComponent citiesManager = RSTC_Global.GetCitiesManager();
		if (!citiesManager)
		{
			return;
		}
		
		ref array<RSTC_CityComponent> cities = citiesManager.GetAllCities();
		
				
		foreach (RSTC_CityComponent city : cities)
		{
		
			int cityId = city.GetCityId();
			if (cityId == -1) continue; // Vérifie si l'ID de la ville est valide
			
			Widget widget = GetGame().GetWorkspace().CreateWidgets(m_Layout, m_RootWidget);
			ImageWidget img = ImageWidget.Cast(widget.FindAnyWidget("Image"));
			if (img)
			{
                FactionKey factionKey = city.GetCitySupportedFactionKey();
				
				Faction faction = factionManager.GetFactionByKey(factionKey);
		
				
                if (faction)
                {
                    img.SetColor(faction.GetFactionColor());
                }
                else
                {
                    img.SetColor(Color.Gray);
                }
				
				img.SetOpacity(0.8);
			}
			
			m_Widgets[cityId] = widget;
		}
	}
	
	override void Update(float timeSlice)
	{
		if (!m_Widgets || m_Widgets.Count() == 0)
			return;
		
		RSTC_CitiesManagerComponent citiesManager = RSTC_Global.GetCitiesManager();
		if (!citiesManager)
			return;
		
		ref array<RSTC_CityComponent> cities = citiesManager.GetAllCities();
		foreach (RSTC_CityComponent city : cities)
		{
			int cityId = city.GetCityId();
			if (cityId == -1 || !m_Widgets.Contains(cityId))
				continue;
			
			Widget w = m_Widgets[cityId];
			if (!w)
				continue;
			
			vector posTown = city.GetCityLocation();
			float x, y;
			m_MapEntity.WorldToScreen(posTown[0], posTown[2], x, y, true);
			
			x = GetGame().GetWorkspace().DPIUnscale(x);
			y = GetGame().GetWorkspace().DPIUnscale(y);
			
			FrameSlot.SetPos(w, x, y);
		}
	}
	
	override void OnMapClose(MapConfiguration config)
	{
		super.OnMapClose(config);
		
		if (m_Widgets)
		{
			m_Widgets.Clear();
		}
	}
}
