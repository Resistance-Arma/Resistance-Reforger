modded class SCR_ScenarioFrameworkSystem : GameSystem
{	
	
	
	//------------------------------------------------------------------------------------------------
	//! Loads component settings from SCR_GameModeSFManager
	override void LoadComponentSettings()
	{
		m_iMaxNumberOfTasks = RSTC_Global.GetConfig().GetMaxNumberOfTasks();
		m_bDynamicDespawn = RSTC_Global.GetConfig().GetDynamicDespawn();

	}
}
