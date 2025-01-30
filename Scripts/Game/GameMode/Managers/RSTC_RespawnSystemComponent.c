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
	
	protected override void GetCreationPosition(int playerId, string characterPersistenceId, out vector position, out vector yawPitchRoll)
	{
		SCR_ChimeraCharacter petros = RSTC_Global.GetPetros();
		if(!petros)
		{
			position = "100 0 100";
			yawPitchRoll = "0 0 0";
			return;
		}

	    vector petrosPosition = petros.GetOrigin();
	    if (petrosPosition == vector.Zero)
	        return;
		
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
		
		RSTC_Global.GetPlayersManager().PreparePlayerForGame(playerId, characterPersistenceId);
	}
	
	override void OnPlayerKilled_S(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{
		super.OnPlayerKilled_S(playerId, playerEntity, killerEntity, killer);
		
		if (!Replication.IsServer()) return;
		RSTC_Global.GetEconomyManager().ChargeRespawn(playerId);
	}
}