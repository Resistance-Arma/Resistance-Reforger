class RSTC_ResistanceFactionManagerClass: RSTC_ComponentClass
{	
};


class RSTC_ResistanceFactionManager: RSTC_Component
{	
	[Attribute("", UIWidgets.ResourceNamePicker, desc: "List of prefabs that will be used for the HQ", params: "et")]
    protected ref array<ResourceName> m_aHQPrefabsList;

	protected static const int SPAWNING_RADIUS = 5;
	protected static const int SPAWNING_HEIGHT = 3;
		
	RSTC_PlayerManagerComponent m_Players;

	static RSTC_ResistanceFactionManager s_Instance;
	
	static RSTC_ResistanceFactionManager GetInstance()
	{
		if (!s_Instance)
		{
			BaseGameMode pGameMode = GetGame().GetGameMode();
			if (pGameMode)
				s_Instance = RSTC_ResistanceFactionManager.Cast(pGameMode.FindComponent(RSTC_ResistanceFactionManager));
		}

		return s_Instance;
	}
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		m_Players = RSTC_Global.GetPlayersManager();
	}
	
	void Init(IEntity owner)
	{
			
	}
	
	void PostGameStart()
	{
	 	vector postion = RSTC_Global.GetConfig().GetInitialPetrosPosition();
		if(!postion)
			postion = vector.Zero;
		
		RSTC_NpcPetros.SpawnPetros(postion);
		PlaceHQAtPetrosPosition();
	}	
	
	bool IsOfficer(int playerId)
	{
		string persId = RSTC_Global.GetPlayersManager().GetPersistentIDFromPlayerID(playerId);
		RSTC_PlayerData player = RSTC_Global.GetPlayersManager().GetPlayer(persId);
		return player.isOfficer;
	}
	
	bool IsLocalPlayerOfficer()
	{
		return IsOfficer(SCR_PlayerController.GetLocalPlayerId());
	}
	
	void AddOfficer(int playerId)
	{
		RpcDo_AddOfficer(playerId);
		Rpc(RpcDo_AddOfficer, playerId);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_AddOfficer(int playerId)
	{
		string persId = RSTC_Global.GetPlayersManager().GetPersistentIDFromPlayerID(playerId);
		if(IsOfficer(playerId)) return;
		
		RSTC_PlayerData player = RSTC_Global.GetPlayersManager().GetPlayer(persId);
		if(!player) return;
		
		player.isOfficer = true;
		if(playerId == SCR_PlayerController.GetLocalPlayerId())
		{
			SCR_HintManagerComponent.GetInstance().ShowCustom("#OVT-NewOfficerYou", "", 10, true);
		}else{
			string playerName = GetGame().GetPlayerManager().GetPlayerName(playerId);
			SCR_HintManagerComponent.GetInstance().ShowCustom(playerName + " #OVT-NewOfficer", "", 10, true);
		}
	}
	
	void PlaceHQAtPetrosPosition()
	{
	    Print("RSTC_HQManagerComponent::PlaceHQNearPetros started.", LogLevel.DEBUG);
	
		SCR_ChimeraCharacter petros = RSTC_Global.GetPetros();
		if(!petros)
			return;
		
	    vector petrosPosition = petros.GetOrigin();
	    if (!petrosPosition || petrosPosition == vector.Zero)
	    {
	        Print("RSTC_HQManagerComponent::Petros position is invalid. Aborting spawn.", LogLevel.ERROR);
	        return;
	    }
	
	    vector pos, oldPos;
	    IEntity spawnedEntity;
	    Physics physicsComponent;
	    EntitySpawnParams spawnParams = new EntitySpawnParams();
	    spawnParams.TransformMode = ETransformMode.WORLD;


	    for (int i = 0; i < m_aHQPrefabsList.Count(); i++)
	    {
	        World world = GetGame().GetWorld();
	        if(!world)
	            return;
	
	        ResourceName prefab = m_aHQPrefabsList[i];
	        if (prefab.IsEmpty())
	        {
	            PrintFormat("RSTC_HQManagerComponent::Prefab at index %1 is empty. Skipping.", i, LogLevel.WARNING);
	            continue;
	        }
	
	        oldPos = pos;
	        bool hasSafeSpawn = SCR_WorldTools.FindEmptyTerrainPosition(
	            pos,      
	            petrosPosition,         
	            RSTC_Global.GetConfig().GetHQRadius() / 2,            
	            SPAWNING_RADIUS,
	            SPAWNING_HEIGHT,
	            TraceFlags.ENTS | TraceFlags.OCEAN,
	            world
	        );
	
	        if (vector.DistanceSqXZ(pos, oldPos) < 2)
	            hasSafeSpawn = SCR_WorldTools.FindEmptyTerrainPosition(
	                pos, 
	                oldPos,  
	                RSTC_Global.GetConfig().GetHQRadius() / 2,
	                SPAWNING_RADIUS, SPAWNING_HEIGHT,
	                TraceFlags.ENTS | TraceFlags.OCEAN,
	                world
	            );
	
	        if (!hasSafeSpawn)
	        {
	            PrintFormat("RSTC_HQManagerComponent::No safe position found for prefab at index: %1", i);
	            continue;
	        }
	
	        spawnParams.Transform[3] = pos;
	
	        spawnedEntity = GetGame().SpawnEntityPrefab(Resource.Load(prefab), GetGame().GetWorld(), spawnParams);
	
	        if (!spawnedEntity)
	        {
	            PrintFormat("RSTC_HQManagerComponent::Failed to spawn entity for prefab at index: %1", i, LogLevel.ERROR);
	            continue;
	        }
	
	        physicsComponent = spawnedEntity.GetPhysics();
	
	        if (physicsComponent)
	            physicsComponent.SetVelocity("0 -0.1 0");
	
			
	        PrintFormat("RSTC_HQManagerComponent::HQ spawned at position: %1", pos, LogLevel.DEBUG);
	    }
	
	    Print("RSTC_HQManagerComponent::PlaceHQNearPetros completed.", LogLevel.DEBUG);
	}
	
	void ~RSTC_ResistanceFactionManager()
	{		
		
	}
}