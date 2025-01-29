[ComponentEditorProps(category: "Overthrow/Components/Player", description: "")]
class RSTC_UIManagerComponentClass: RSTC_ComponentClass
{}

class RSTC_UIManagerComponent: RSTC_Component
{
	[Attribute("", UIWidgets.Object)]
	ref array<ref RSTC_UIContext> m_aContexts;
	
	SCR_CharacterControllerComponent m_Controller;
					
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);	
				
		if(SCR_Global.IsEditMode()) return;	
		
		SCR_CharacterControllerComponent controller = SCR_CharacterControllerComponent.Cast(owner.FindComponent(SCR_CharacterControllerComponent) );		
				
		if(controller){
			m_Controller = controller;
			
			m_Controller.m_OnControlledByPlayer.Insert(this.OnControlledByPlayer);
			m_Controller.m_OnPlayerDeath.Insert(this.OnPlayerDeath);
		}
		
		foreach(RSTC_UIContext context : m_aContexts)
		{
			context.Init(owner, this);				
		}
	}
	
	void ShowContext(typename typeName)
	{
		foreach(RSTC_UIContext context : m_aContexts)
		{
			if(context.ClassName() == typeName.ToString())
			{
				context.ShowLayout();
				break;
			}
		}
	}
	
	RSTC_UIContext GetContext(typename typeName)
	{
		foreach(RSTC_UIContext context : m_aContexts)
		{
			if(context.ClassName() == typeName.ToString())
			{
				return context;
			}
		}
		
		return null;
	}
	
	RSTC_UIContext GetContextByString(string typeName)
	{
		foreach(RSTC_UIContext context : m_aContexts)
		{
			if(context.ClassName() == typeName)
			{
				return context;
			}
		}
		
		return null;
	}
			
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		GetGame().GetInputManager().ActivateContext("OverthrowGeneralContext");
		
		foreach(RSTC_UIContext context : m_aContexts)
		{
			context.EOnFrame(owner, timeSlice);
		}	
	}
	
	protected void OnPlayerDeath()
	{		
		foreach(RSTC_UIContext context : m_aContexts)
		{
			context.CloseLayout();
			context.UnregisterInputs();
		}
	}
	
	protected void AfterControlledByPlayer(IEntity owner, bool controlled)
	{
		if (!controlled)
		{			
			ClearEventMask(owner, EntityEvent.FRAME);
						
			foreach(RSTC_UIContext context : m_aContexts)
			{
				context.UnregisterInputs();
			}			
		}
		else if (owner)
		{			
			SetEventMask(owner, EntityEvent.FRAME);			
			
			foreach(RSTC_UIContext context : m_aContexts)
			{
				context.OnControlledByPlayer();
				context.RegisterInputs();				
			}
			
			RSTC_GameMode gameMode = RSTC_GameMode.Cast(GetGame().GetGameMode());
			int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(owner);
			
 			if(gameMode) gameMode.OnPlayerSpawnedLocal(RSTC_Global.GetPlayersManager().GetPersistentIDFromPlayerID(playerId));
		}
	}
	

	protected void OnControlledByPlayer(IEntity owner, bool controlled)
	{		
		GetGame().GetCallqueue().CallLater(AfterControlledByPlayer, 0, false, owner, controlled);
	}
	
	void ~RSTC_UIManagerComponent()
	{
		if(m_aContexts)
		{	
			foreach(RSTC_UIContext context : m_aContexts)
			{
				context.UnregisterInputs();
			}
			m_aContexts.Clear();
			m_aContexts = null;
		}
		if(m_Controller)
		{
			m_Controller.m_OnControlledByPlayer.Remove(this.OnControlledByPlayer);
			m_Controller.m_OnPlayerDeath.Remove(this.OnPlayerDeath);
		}
		
		
	}
}