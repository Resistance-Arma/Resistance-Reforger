modded class EPF_BaseRespawnSystemComponent
{	
	override void OnPlayerRegistered_S(int playerId)
	{
		//PrintFormat("EPF_BaseRespawnSystemComponent.OnPlayerRegistered_S(%1)", playerId);

		if (RplSession.Mode() != RplMode.Dedicated || GetGame().GetBackendApi().GetBackendEnv() == "invalid")
		{
			WaitForUid(playerId);
		}
		else
		{
			EDF_ScriptInvokerCallback1<int> callback(this, "WaitForUid");
			m_pGameMode.GetOnPlayerAuditSuccess().Insert(callback.Invoke);
		}
	}
	
}
