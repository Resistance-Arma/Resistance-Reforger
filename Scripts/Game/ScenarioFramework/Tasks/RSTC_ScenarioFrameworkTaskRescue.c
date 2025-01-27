class RSTC_ScenarioFrameworkTaskRescueClass : SCR_ScenarioFrameworkTaskClass
{
};

[EntityEditorProps(category: "GameScripted/ScenarioFramework/Task", description: "Rescue task.")]
class RSTC_ScenarioFrameworkTaskRescue : SCR_ScenarioFrameworkTask
{
    protected IEntity m_AIEntity;
    protected SCR_ScenarioFrameworkTriggerEntity m_RescuePointTrigger;
    protected SCR_ScenarioFrameworkTriggerEntity m_ExtractionTrigger;
    protected bool m_bAIRescued = false;

    // Setter pour définir l'entité AI
    void SetAIEntity(IEntity aiEntity)
    {
        if (!aiEntity)
        {
            Print("RescueTask => AI entity is null!", LogLevel.ERROR);
            return;
        }

        m_AIEntity = aiEntity;

        // Attache des listeners pour détecter la destruction
        SCR_DamageManagerComponent dmgManager = SCR_DamageManagerComponent.GetDamageManager(m_AIEntity);
        if (dmgManager)
        {
            dmgManager.GetOnDamageStateChanged().Insert(OnAIDestroyed);
        }
    }

    void OnAIDestroyed(EDamageState state)
    {
        if (state == EDamageState.DESTROYED)
        {
            Print("RescueTask => Rescue mission failed! The AI has been destroyed.", LogLevel.ERROR);
            Fail();
        }
    }

    void SetRescuePointTrigger(SCR_ScenarioFrameworkTriggerEntity trigger)
    {
        if (!trigger)
        {
            Print("RescueTask => Rescue Point Trigger not defined!", LogLevel.ERROR);
            return;
        }

        m_RescuePointTrigger = trigger;
        m_RescuePointTrigger.GetOnActivate().Insert(OnRescuePointReached);
    }

    void SetExtractionTrigger(SCR_ScenarioFrameworkTriggerEntity trigger)
    {
        if (!trigger)
        {
            Print("RescueTask => Extraction Trigger not defined!", LogLevel.ERROR);
            return;
        }

        m_ExtractionTrigger = trigger;
        m_ExtractionTrigger.GetOnActivate().Insert(OnExtractionPointReached);
    }

    void OnRescuePointReached(SCR_ScenarioFrameworkTriggerEntity trigger)
    {
        if (m_bAIRescued)
            return;

        m_bAIRescued = true;
		
		RSTC_ScenarioFrameworkLayerTaskRescue layerTaskRescue = RSTC_ScenarioFrameworkLayerTaskRescue.Cast(m_LayerTask);
		if (!layerTaskRescue)
		{
			Print("ScenarioFramework: Task Rescue failed to access LayerTask", LogLevel.ERROR);
			return;
		}
		
		layerTaskRescue.UpdateTaskTitleAndDescription();
		
		if (m_Asset)
        {
			m_Asset.SetOrigin(m_ExtractionTrigger.GetOrigin());
			m_Asset.Update();
            MoveTaskMarkerPosition()
        }
    }
	

    void OnExtractionPointReached(SCR_ScenarioFrameworkTriggerEntity trigger)
    {
        if (!m_AIEntity || !m_bAIRescued)
            return;

        array<IEntity> entitiesInside = {};
        trigger.GetEntitiesInside(entitiesInside);

        foreach (IEntity entity : entitiesInside)
        {
            if (entity == m_AIEntity)
            {
				trigger.Deactivate();
                Finish();
                return;
            }
        }
    }

	protected void MoveTaskMarkerPosition()
	{
		if (m_Asset)
			m_SupportEntity.MoveTask(m_Asset.GetOrigin(), this.GetTaskID());
		else
			Print("ScenarioFramework: Task Deliver does not have m_Asset properly assigned for MoveTaskMarkerPosition", LogLevel.ERROR);
	}
}
