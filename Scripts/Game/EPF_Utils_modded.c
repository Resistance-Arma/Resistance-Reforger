modded class EPF_Utils
{
	//------------------------------------------------------------------------------------------------
	//! Gets the Bohemia UID
	//! \param playerId Index of the player inside player manager
	//! \return the uid as string
	override static string GetPlayerUID(int playerId)
	{
		if (!Replication.IsServer())
		{
			Debug.Error("GetPlayerUID can only be used on the server and after OnPlayerAuditSuccess.");
			return string.Empty;
		}

		string uid = GetGame().GetBackendApi().GetPlayerIdentityId(playerId);
		
		Print("DEBUG LINE | " + FilePath.StripPath(__FILE__) + ":" + __LINE__, LogLevel.DEBUG);		
		if (!uid)
		{
			if (RplSession.Mode() != RplMode.Dedicated || GetGame().GetServerInfo().GetName().IsEmpty())
			{
				// Peer tool supportR
				uid = string.Format("bbbbdddd-0000-0000-0000-%1", playerId.ToString(12));
			}
			else
			{
				Debug.Error("Dedicated server is not correctly configured to connect to the BI backend.\nSee https://community.bistudio.com/wiki/Arma_Reforger:Server_Hosting");
			}
		}

		return uid;
	}
}
