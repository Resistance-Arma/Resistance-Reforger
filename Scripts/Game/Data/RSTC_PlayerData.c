class RSTC_PlayerData : Managed
{
	[NonSerialized()]
	int id=0;
	
	[NonSerialized()]
	bool firstSpawn = true;
	
	//Persisted
	string name="";	
	int money = 0;
	bool initialized = false;	
	bool isOfficer = false;
	int xp = 0;
	
	
	bool IsOffline()
	{
		return id == -1;
	}
	
	float GetRawLevel()
	{
		return 1 + (0.1 * Math.Sqrt(xp));
	}
	
	int GetLevel()
	{		
		return Math.Floor(GetRawLevel());
	}		
	
	static RSTC_PlayerData Get(string persId)
	{
		return RSTC_Global.GetPlayersManager().GetPlayer(persId);
	}	
	
	static RSTC_PlayerData Get(int playerId)
	{
		RSTC_PlayerManagerComponent pm = RSTC_Global.GetPlayersManager();
		string persId = pm.GetPersistentIDFromPlayerID(playerId);
		return pm.GetPlayer(persId);
	}
}