class RSTC_GameModeClass: SCR_BaseGameModeClass
{
};

class RSTC_GameMode : SCR_BaseGameMode
{
	protected RSTC_ConfigManagerComponent m_configManager;
    protected RSTC_CitiesManagerComponent m_citiesManager;
	protected RSTC_TasksManagerComponent m_tasksManager;
	protected RSTC_EconomyManagerComponent m_economyManager;
	protected RSTC_PlayerManagerComponent m_playerManager;
	protected RSTC_ResistanceFactionManager m_resistanceFactionManager;
	protected RSTC_PersistenceManagerComponent m_persistence;
	protected RSTC_FactionManager m_factionManager;


  	protected bool m_bGameInitialized = false;
	protected bool m_bGameStarted = false;
	protected bool m_bRequestStartOnPostProcess = false;
	protected bool m_bWaitingAdminToStartGame = false;

	ref set<string> m_aInitializedPlayers;
	ref set<string> m_aHintedPlayers;
	
	static RSTC_GameMode GetInstance()
	{
		return RSTC_GameMode.Cast(GetGame().GetGameMode());
	}
	
	bool IsInitialized()
	{
		return m_bGameInitialized;
	}
	
	bool IsWaitingForAdminToStart()
	{
		return m_bWaitingAdminToStartGame; 
	}
	
	override void OnPlayerAuditSuccess(int iPlayerID)
	{

		Print("SCR_BaseGameMode::OnPlayerAuditSuccess - playerId: " + iPlayerID, LogLevel.DEBUG);

		super.OnPlayerAuditSuccess(iPlayerID);
		m_OnPlayerAuditSuccess.Invoke(iPlayerID);
		
		// Dispatch event to child components
		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
		{
			comp.OnPlayerAuditSuccess(iPlayerID);
		}
	}
	
    // Surcharge de la méthode EOnInit pour initialiser les composants
    override void EOnInit(IEntity owner)
    {
        super.EOnInit(owner);

		m_persistence = RSTC_PersistenceManagerComponent.Cast(FindComponent(RSTC_PersistenceManagerComponent));
		if(m_persistence)
		{
			 m_persistence.WipeSave()
		}
        InitializeGame(owner);
    }

	
    protected void InitializeGame(IEntity owner)
    {		
		m_aInitializedPlayers = new set<string>;
		m_aHintedPlayers = new set<string>;
		
		m_playerManager = RSTC_Global.GetPlayersManager();
		if(!m_playerManager)
			return;
		Print("[Resistance] Initializing Players");
		m_playerManager.Init(this);
		
		m_factionManager = RSTC_Global.GetFactionManager();
		if(!m_factionManager)
			return;
		Print("[Resistance] Initializing Resistance Faction");
		m_factionManager.Init(this);
		
		m_resistanceFactionManager = RSTC_Global.GetResistanceFactionManager();
		if(!m_resistanceFactionManager)
			return;
		Print("[Resistance] Initializing Resistance Faction");
		m_resistanceFactionManager.Init(this);
		
        m_citiesManager = RSTC_Global.GetCitiesManager();
        if (!m_citiesManager)
			return;
		Print("[Resistance] Initializing Cities...");
		m_citiesManager.Init(this);
		
		m_tasksManager = RSTC_Global.GetTasksManager();
        if (!m_tasksManager)
    		return;
		Print("[Resistance] Initializing Task...");
		m_tasksManager.Init(this);
    
		m_economyManager = RSTC_EconomyManagerComponent.Cast(FindComponent(RSTC_EconomyManagerComponent));
		if(!m_economyManager)
			return
		Print("[Resistance] Initializing Economy");
		m_economyManager.Init(this);
				
		if(!IsMaster()) {			
			return;
		}
				
		m_persistence = RSTC_PersistenceManagerComponent.Cast(FindComponent(RSTC_PersistenceManagerComponent));
		if(m_persistence)
		{
			Print("[Resistance] Initializing Persistence");
			if(m_persistence.HasSaveGame())
			{
				m_bRequestStartOnPostProcess = true;
			}
			else
			{
				Print("[Resistance] No save game detected");
				m_bWaitingAdminToStartGame = true;				
			}
		}		
    }
	

	override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsProxy()
	{
		return (m_RplComponent && m_RplComponent.IsProxy());
	}
	
	RSTC_PersistenceManagerComponent GetPersistence()
	{
		return m_persistence;
	}
	
	void DoStartNewGame()
	{
		RestartSession();
	}
	
	
	void DoStartGame()
	{
		Print("[Resistance] Resistance Starting");		
		
		m_bWaitingAdminToStartGame = false;
		m_bGameStarted = true;

		if(m_factionManager)
		{
			Print("[Resistance] Starting Faction");
			m_factionManager.PostGameStart();
		}
		
		if(m_economyManager)
		{
			Print("[Resistance] Starting Economy");
			m_economyManager.PostGameStart();
		}

		if(m_citiesManager)
		{
			Print("[Resistance] Starting Cities");
			m_citiesManager.PostGameStart();
		}

		if(m_resistanceFactionManager)
		{
			Print("[Resistance] Starting Resistance Faction");
			m_resistanceFactionManager.PostGameStart();
		}

		m_bGameInitialized = true;

		
		Print("[Resistance] Resistance correctly started");		
	}
	
	override event void OnWorldPostProcess(World world)
	{
		Print("[Resistance] World Post Processing complete..");
		super.OnWorldPostProcess(world);
		if(m_bRequestStartOnPostProcess)
		{
			GetGame().GetCallqueue().CallLater(DoStartGame);
		}
	};
	
	protected override void OnPlayerRoleChange(int playerId, EPlayerRole roleFlags)
	{
		super.OnPlayerRoleChange(playerId, roleFlags);

		if(SCR_Global.IsAdminRole(roleFlags))
		{	
			IEntity playerEntity = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
			if(!m_bGameInitialized && m_bWaitingAdminToStartGame && playerEntity)
			{					
				RSTC_PlayerCommsComponent playerCom = RSTC_PlayerCommsComponent.Cast(playerEntity.FindComponent(RSTC_PlayerCommsComponent));
		        if (playerCom)
		        {
					PrintFormat("playerCom playerCom playerCom: %1", playerId);
					
		            PrintFormat("Sent RPC_ShowStartGameUI to client for player ID: %1", playerId);
					playerCom.Rpc_ShowStartGameUI();
		        }
			}
			
			string persId = m_playerManager.GetPersistentIDFromPlayerID(playerId);
			RSTC_PlayerData player = m_playerManager.GetPlayer(persId);
			if(!player) return;
			if(!player.isOfficer)
			{
				m_resistanceFactionManager.AddOfficer(playerId);
			}
		}
	}
	
	void PreparePlayer(int playerId, string persistentId)
	{
		if(!Replication.IsServer()) return;

		m_playerManager.SetupPlayer(playerId, persistentId);
		RSTC_PlayerData player = m_playerManager.GetPlayer(persistentId);


		if(!player.isOfficer && RplSession.Mode() == RplMode.None)
		{
			m_resistanceFactionManager.AddOfficer(playerId);
		}

		if(player.initialized)
		{
			if(m_aInitializedPlayers.Contains(persistentId))
			{
				m_economyManager.ChargeRespawn(playerId);
			}
			else
			{
				Print("[Resistance] Preparing returning player: " + persistentId);
				m_aInitializedPlayers.Insert(persistentId);
			}
			player.firstSpawn = false;
		}
		else
		{
			//New player
			Print("[Resistance] Preparing NEW player: " + persistentId);
			int cash = RSTC_Global.GetConfig().GetStartingMoney();
			m_economyManager.AddPlayerMoney(playerId, cash);

			player.initialized = true;
			player.firstSpawn = true;
			m_aInitializedPlayers.Insert(persistentId);
		}
	}	
	
	protected override void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		string persId = m_playerManager.GetPersistentIDFromPlayerID(playerId);
		IEntity controlledEntity = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);

		if(controlledEntity)
		{
			RSTC_PlayerData player = m_playerManager.GetPlayer(persId);
			if(player)
			{
				player.id = -1;
			}

			EPF_PersistenceComponent persistence = EPF_PersistenceComponent.Cast(controlledEntity.FindComponent(EPF_PersistenceComponent));
			if(persistence)
			{
				persistence.PauseTracking();
				persistence.Save();
			}
		}

		int i = m_aInitializedPlayers.Find(persId);

		if(i > -1)
			m_aInitializedPlayers.Remove(i);

		super.OnPlayerDisconnected(playerId, cause, timeout);
	}
	
	protected override void OnPlayerSpawned(int playerId, IEntity controlledEntity)
	{
	
        SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if(playerController && playerController.HasRole(EPlayerRole.ADMINISTRATOR))
		{
			if(!m_bGameInitialized && m_bWaitingAdminToStartGame)
			{	
				RSTC_PlayerCommsComponent playerCom = RSTC_PlayerCommsComponent.Cast(controlledEntity.FindComponent(RSTC_PlayerCommsComponent));
		        if (playerCom)
		        {
		            // Envoie l'RPC pour afficher l'UI sur le client
		            PrintFormat("Sent RPC_ShowStartGameUI to client for player ID: %1", playerId);
					playerCom.Rpc_ShowStartGameUI();
		        }
			}
		}

		super.OnPlayerSpawned(playerId, controlledEntity)
		
	}
	
	void OnPlayerSpawnedLocal(string playerId)
	{
		if(!m_aHintedPlayers.Contains(playerId) && m_bGameInitialized)
		{
			SCR_HintManagerComponent.GetInstance().ShowCustom("Welcome","Welcome description !",20);
			m_aHintedPlayers.Insert(playerId);
		}
	}
}
