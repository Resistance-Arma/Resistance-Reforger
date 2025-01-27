
modded class SCR_ScenarioFrameworkLayerTask 
{
	[Attribute("0", UIWidgets.ComboBox, "Task  type", enums: ParamEnumArray.FromEnum(RSTC_ESFTaskType), category: "Activation")]
	RSTC_ESFTaskSubType m_TaskType;

	
	[Attribute("0", UIWidgets.ComboBox, "Task sub type", enums: ParamEnumArray.FromEnum(RSTC_ESFTaskSubType), category: "Activation")]
	RSTC_ESFTaskSubType m_TaskSubType;
	
	[Attribute(desc: "Restore to Default when task Finshed", category: "Activation")]
	bool m_RestoreToDefaultWhenFinished;
	
	[Attribute(defvalue: "0.0", UIWidgets.Slider, params: "0 inf", category: "Activation")]
	float m_ReputationWinOnSuccess;
	
	[Attribute(defvalue: "0.0", UIWidgets.Slider, params: "0 inf", category: "Activation")]
	float m_ReputationLossOnFailed;
	
	override SCR_ESFTaskType GetTaskType()
	{
		return m_TaskType;
	}
	
	SCR_ESFTaskType GetTaskSubType()
	{
		return m_TaskSubType;
	}
	
	
	//------------------------------------------------------------------------------------------------
	//! Triggers actions on task finish if not resolved before load.
	//! \param[in] previousState represents the current state of the task before it transitions to new state.
	//! \param[in] newState Triggers actions on task finish state change.
	override void OnTaskStateChanged(SCR_TaskState previousState, SCR_TaskState newState)
	{
		m_eLayerTaskState = newState;

		if (newState == SCR_TaskState.FINISHED && !m_bTaskResolvedBeforeLoad)
		{
			foreach (SCR_ScenarioFrameworkActionBase triggerAction : m_aTriggerActionsOnFinish)
			{
				triggerAction.OnActivate(GetOwner());
			}
			
			if(m_ReputationWinOnSuccess > 0.0)
			{
				RSTC_CityComponent city = RSTC_CityComponent.Cast(GetOwner().GetParent().FindComponent(RSTC_CityComponent));
				if (!city)
					return;
				city.ModifyFactionReputation(GetFactionKey(), m_ReputationWinOnSuccess)
			}			
			
			if(m_RestoreToDefaultWhenFinished)
				RestoreToDefault(true);
		}
		
		if (newState == SCR_TaskState.CANCELLED)
		{
		
			if(m_ReputationLossOnFailed > 0.0)
			{
				RSTC_CityComponent city = RSTC_CityComponent.Cast(GetOwner().GetParent().FindComponent(RSTC_CityComponent));
				if (!city)
					return;
				city.ModifyFactionReputation(GetFactionKey(), m_ReputationWinOnSuccess)
			}
		}
	}
	
	

}
