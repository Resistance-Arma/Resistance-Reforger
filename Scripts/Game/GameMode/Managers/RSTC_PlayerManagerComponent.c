class RSTC_PlayerManagerComponentClass: RSTC_ComponentClass
{
};

class RSTC_PlayerManagerComponent: RSTC_Component
{		
	static RSTC_PlayerManagerComponent s_Instance;
	static RSTC_PlayerManagerComponent GetInstance()
	{
		if (!s_Instance)
		{
			BaseGameMode pGameMode = GetGame().GetGameMode();
			if (pGameMode)
				s_Instance = RSTC_PlayerManagerComponent.Cast(pGameMode.FindComponent(RSTC_PlayerManagerComponent));
		}

		return s_Instance;
	}
	
	ref ScriptInvoker m_OnPlayerDataLoaded = new ScriptInvoker();
	
	protected ref map<int, string> m_mPersistentIDs;
	protected ref map<string, int> m_mPlayerIDs;
	ref map<string, ref RSTC_PlayerData> m_mPlayers;
	
	void Init(IEntity owner)
	{
		m_mPersistentIDs = new map<int, string>;
		m_mPlayerIDs = new map<string, int>;
		m_mPlayers = new map<string, ref RSTC_PlayerData>;
	}
	
	RSTC_PlayerData GetPlayer(string persId)
	{
		if(m_mPlayers.Contains(persId)) return m_mPlayers[persId];
		return null;
	}
	
	bool LocalPlayerIsOfficer()
	{
		int localId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(SCR_PlayerController.GetLocalControlledEntity());
		string persId = GetPersistentIDFromPlayerID(localId);
		RSTC_PlayerData player = GetPlayer(persId);
		return player.isOfficer;
	}
	
	RSTC_PlayerData GetPlayer(int playerId)
	{		
		return GetPlayer(GetPersistentIDFromPlayerID(playerId));
	}
	
	string GetPlayerName(string persId)
	{
		RSTC_PlayerData player = GetPlayer(persId);
		if(player) return player.name;
		return "";
	}
	
	string GetPlayerName(int playerId)
	{
		return GetPlayerName(GetPersistentIDFromPlayerID(playerId));
	}
	
	string GetPersistentIDFromPlayerID(int playerId)
	{
		if(!m_mPersistentIDs.Contains(playerId)) {
			string persistentId = EPF_Utils.GetPlayerUID(playerId);
#ifdef WORKBENCH
			//Force only two players in workbench to test reconnection
			if(playerId > 2)
			{
				persistentId = EPF_Utils.GetPlayerUID(2);
			}
#endif
			SetupPlayer(playerId, persistentId);
			return persistentId;
		}
		return m_mPersistentIDs[playerId];
	}
	
	string GetPersistentIDFromControlledEntity(IEntity controlled)
	{
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(controlled);
		return GetPersistentIDFromPlayerID(playerId);
	}
	
	int GetPlayerIDFromPersistentID(string id)
	{
		if(!m_mPlayerIDs.Contains(id)) return -1;
		return m_mPlayerIDs[id];
	}
	
	void SetupPlayer(int playerId, string persistentId)
	{
		Print("Setting up player: " + persistentId);
		m_mPersistentIDs[playerId] = persistentId;
		m_mPlayerIDs[persistentId] = playerId;
		
		RSTC_PlayerData player = GetPlayer(persistentId);		
					
		if(!player)
		{
			player = new RSTC_PlayerData;
			m_mPlayers[persistentId] = player;			
		}
		
		player.name = GetGame().GetPlayerManager().GetPlayerName(playerId);
		
		player.id = playerId;
		
		if(Replication.IsServer())	
			Rpc(RpcDo_RegisterPlayer, playerId, persistentId);
	}
	
	//RPC Methods
	
	override bool RplSave(ScriptBitWriter writer)
	{
		//Send JIP Players
		writer.WriteInt(m_mPlayers.Count());
		for(int i=0; i<m_mPlayers.Count(); i++)
		{
			RSTC_PlayerData player = m_mPlayers.GetElement(i);
			writer.WriteString(m_mPlayers.GetKey(i));
			writer.WriteInt(player.id);
			writer.WriteInt(player.money);
			writer.WriteString(player.name);
			writer.WriteBool(player.isOfficer);
	

			writer.WriteInt(player.xp);
		}		
		return true;
	}
	
	override bool RplLoad(ScriptBitReader reader)
	{		
		
		int length, playerId;
		string persId;
		
		//Recieve JIP players
		if (!reader.ReadInt(length)) return false;
		for(int i=0; i<length; i++)
		{
			if (!reader.ReadString(persId)) return false;
			if (!reader.ReadInt(playerId)) return false;
			RSTC_PlayerData player = GetPlayer(persId);
			if(!player)
			{
				player = new RSTC_PlayerData;
				m_mPlayers[persId] = player;
				player.id = playerId;		
			}
			m_mPersistentIDs[playerId] = persId;
			m_mPlayerIDs[persId] = playerId;
			
			if (!reader.ReadInt(player.money)) return false;
			if (!reader.ReadString(player.name)) return false;
			if (!reader.ReadBool(player.isOfficer)) return false;
			if(!reader.ReadInt(player.xp)) return false;
			
		}
		return true;
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_RegisterPlayer(int playerId, string s)
	{
		SetupPlayer(playerId, s);
	}
	
	void ~RSTC_PlayerManagerComponent()
	{
		if(m_mPersistentIDs)
		{
			m_mPersistentIDs.Clear();
			m_mPersistentIDs = null;
		}
		if(m_mPlayerIDs)
		{
			m_mPlayerIDs.Clear();
			m_mPlayerIDs = null;
		}
	}
}