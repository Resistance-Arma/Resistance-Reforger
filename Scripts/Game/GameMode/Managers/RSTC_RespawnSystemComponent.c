//------------------------------------------------------------------------------------------------
class RSTC_RespawnSystemComponentClass : EPF_BaseRespawnSystemComponentClass
{
};

//! Scripted implementation that handles spawning and respawning of players.
//! Should be attached to a GameMode entity.
[ComponentEditorProps(icon: HYBRID_COMPONENT_ICON)]
class RSTC_RespawnSystemComponent : EPF_BaseRespawnSystemComponent
{	
	protected RSTC_GameMode m_GameMode;
	
	[Attribute(defvalue: "{3A99A99836F6B3DC}Prefabs/Characters/Factions/INDFOR/FIA/Character_Player.et")]
	ResourceName m_rDefaultPrefab;
	
	override protected ResourceName GetCreationPrefab(int playerId, string characterPersistenceId)
	{
		return m_rDefaultPrefab;
	}
	
	protected override void OnUidAvailable(int playerId)
	{
		RSTC_GameMode mode = RSTC_GameMode.Cast(GetGame().GetGameMode());
		
		if(!mode.IsInitialized() && !mode.IsWaitingForAdminToStart())
		{
			//Game has not started yet
			OnPlayerRegisterFailed(playerId);
			return;
		}
		
		string playerUid = EPF_Utils.GetPlayerUID(playerId);
		if (!playerUid)
		{
			//Still no player UID?
			Print("WARNING: Early OnUidAvailable detected. Retrying...", LogLevel.WARNING);
			OnPlayerRegisterFailed(playerId);
			return;
		}
		
		mode.PreparePlayer(playerId, playerUid);
		
		super.OnUidAvailable(playerId);		
	}
	
	void OnPlayerRegisterFailed(int playerId)
	{
		GetGame().GetCallqueue().CallLater(OnPlayerRegistered_S, 1000, false, playerId);
	}
	
	protected override void GetCreationPosition(int playerId, string characterPersistenceId, out vector position, out vector yawPitchRoll)
	{
		RSTC_NpcPetros petros = RSTC_Global.GetPetros();
		if(!petros)
		{
			position = "100 0 100";
			yawPitchRoll = "0 0 0";
			return;
		}

	    vector petrosPosition = petros.GetOrigin();
	    if (petrosPosition == vector.Zero)
	    {
	        Print("RSTC_HQManagerComponent::Petros position is invalid. Aborting spawn.", LogLevel.ERROR);
	        return;
	    }
		
		RSTC_PlayerData player = RSTC_PlayerData.Get(characterPersistenceId);

		if(player)
		{
			position =  petrosPosition + "1 0 0";
			yawPitchRoll = "0 0 0";
		}
	}
	
	
	protected override void OnCharacterCreated(int playerId, string characterPersistenceId, IEntity character)
	{
		super.OnCharacterCreated(playerId, characterPersistenceId, character);
		
		InventoryStorageManagerComponent storageManager = EPF_Component<InventoryStorageManagerComponent>.Find(character);
		FactionAffiliationComponent factionAffiliationManager = EPF_Component<FactionAffiliationComponent>.Find(character);
		
		array<ResourceName> doneStartingItems = {};
		RSTC_PlayerData player = RSTC_PlayerData.Get(characterPersistenceId);
			
		if(storageManager && factionAffiliationManager && player)
		{
			PrintFormat(" === OnCharacterCreated %1 : ", RSTC_Global().GetFactionManager().GetResistanceFaction().GetFactionKey());
			factionAffiliationManager.SetAffiliatedFaction(RSTC_Global().GetFactionManager().GetResistanceFaction());	
			player.firstSpawn = false;
		}		
	}
	
	override void OnPlayerKilled_S(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{
		super.OnPlayerKilled_S(playerId, playerEntity, killerEntity, killer);
		
		if (!Replication.IsServer()) return;
		RSTC_Global.GetEconomyManager().ChargeRespawn(playerId);
	}
}