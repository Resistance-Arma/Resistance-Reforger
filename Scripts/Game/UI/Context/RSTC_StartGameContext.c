class RSTC_StartGameContext : RSTC_UIContext
{
	protected FactionManager m_Factions;
	
	protected ref array<FactionKey> m_FactionKeys;
	protected ref array<SCR_ButtonComponent> m_FactionCards;
	
	override void OnShow()
	{		
#ifdef PLATFORM_XBOX		
		Widget xbox = m_wRoot.FindAnyWidget("XBOXWarning");
		xbox.SetVisible(true);
#endif
		
		IEntity mode = GetGame().GetGameMode();
		SCR_SaveLoadComponent saveload = SCR_SaveLoadComponent.Cast(mode.FindComponent(SCR_SaveLoadComponent));
		
		Widget startButton = m_wRoot.FindAnyWidget("StartGameButton");
		SCR_InputButtonComponent action = SCR_InputButtonComponent.Cast(startButton.FindHandler(SCR_InputButtonComponent));
		
		if(action)
			action.m_OnActivated.Insert(StartGame);
											
		m_Factions = GetGame().GetFactionManager();
		
		autoptr array<Faction> factions = new array<Faction>;
		m_Factions.GetFactionsList(factions);
		
		Widget occupyingFactionSpinner = m_wRoot.FindAnyWidget("OccupyingFactionSpinner");
		SCR_SpinBoxComponent occupyingFactionSpinnerBox = SCR_SpinBoxComponent.Cast(occupyingFactionSpinner.FindHandler(SCR_SpinBoxComponent));
		occupyingFactionSpinnerBox.m_OnChanged.Insert(OnSpinOccupyingFaction);
		
		Widget resistanceFactionSpinner = m_wRoot.FindAnyWidget("ResistanceFactionSpinner");
		SCR_SpinBoxComponent resistanceFactionSpinnerBox = SCR_SpinBoxComponent.Cast(resistanceFactionSpinner.FindHandler(SCR_SpinBoxComponent));
		resistanceFactionSpinnerBox.m_OnChanged.Insert(OnSpinResistanceFaction);
		
		Widget invaderFactionSpinner = m_wRoot.FindAnyWidget("InvaderFactionSpinner");
		SCR_SpinBoxComponent invaderFactionSpinnerBox = SCR_SpinBoxComponent.Cast(invaderFactionSpinner.FindHandler(SCR_SpinBoxComponent));
		invaderFactionSpinnerBox.m_OnChanged.Insert(OnSpinInvaderFaction);
		
		int i = 0;
		protected int selectedResistanceFaction = 0;
		protected int selectedInvaderFaction = 0;
		protected int selectedOccupyingFaction = 0;	
		
		foreach(Faction faction : factions)
		{
			if(!faction) continue;
			SCR_Faction factionCast = SCR_Faction.Cast(faction);


			if(!factionCast) continue;
			if(!factionCast.IsPlayable()) continue;
						
			occupyingFactionSpinnerBox.AddItem(faction.GetUIInfo().GetName(),false,faction);
			resistanceFactionSpinnerBox.AddItem(faction.GetUIInfo().GetName(),false,faction);
			invaderFactionSpinnerBox.AddItem(faction.GetUIInfo().GetName(),false,faction);
						
			if(faction.GetFactionKey() == RSTC_Global.GetConfig().GetResistanceFactionKey()) selectedResistanceFaction = i;
			if(faction.GetFactionKey() == RSTC_Global.GetConfig().GetInvaderFactionKey()) selectedInvaderFaction = i;
			if(faction.GetFactionKey() == RSTC_Global.GetConfig().GetOccupyingFactionKey()) selectedOccupyingFaction = i;
			
			i++;
		}
		resistanceFactionSpinnerBox.SetCurrentItem(selectedResistanceFaction);
		invaderFactionSpinnerBox.SetCurrentItem(selectedInvaderFaction);
		occupyingFactionSpinnerBox.SetCurrentItem(selectedOccupyingFaction);
	}
	
	protected void OnSpinOccupyingFaction(SCR_SpinBoxComponent spinner, int index)
	{
		Faction data = Faction.Cast(spinner.GetItemData(index));
		if(!data) return;
		RSTC_Global.GetServer().SetOccupyingFactionKey(data.GetFactionKey());	
	}
	
	protected void OnSpinResistanceFaction(SCR_SpinBoxComponent spinner, int index)
	{
		Faction data = Faction.Cast(spinner.GetItemData(index));
		if(!data) return;
		RSTC_Global.GetServer().SetResistanceFactionKey(data.GetFactionKey());	
	}
	
	protected void OnSpinInvaderFaction(SCR_SpinBoxComponent spinner, int index)
	{
		Faction data = Faction.Cast(spinner.GetItemData(index));
		if(!data) return;
		RSTC_Global.GetServer().SetInvaderFactionKey(data.GetFactionKey());	
	}
	
	protected void StartGame()
    {
        FactionKey resistanceFaction = RSTC_Global.GetConfig().GetResistanceFactionKey();
        FactionKey invaderFaction = RSTC_Global.GetConfig().GetInvaderFactionKey();
        FactionKey occupyingFaction = RSTC_Global.GetConfig().GetOccupyingFactionKey();

        if (resistanceFaction == invaderFaction || resistanceFaction == occupyingFaction || invaderFaction == occupyingFaction)
        {
            SCR_HintManagerComponent.GetInstance().ShowCustom("Les factions doivent être différentes !", "Erreur", 5);
            return;
        }

        CloseLayout();
    
        RSTC_Global.GetServer().StartNewGame();
    }
}