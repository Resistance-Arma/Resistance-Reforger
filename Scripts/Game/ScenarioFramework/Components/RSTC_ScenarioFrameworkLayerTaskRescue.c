[EntityEditorProps(category: "GameScripted/ScenarioFramework/Layer", description: "Layer for Rescue Task.")]
class RSTC_ScenarioFrameworkLayerTaskRescueClass : SCR_ScenarioFrameworkLayerTaskClass
{
}

class RSTC_ScenarioFrameworkLayerTaskRescue : SCR_ScenarioFrameworkLayerTask
{
	[Attribute(desc: "Name of the task in list of tasks after meeting the rescue point.", category: "Task")]		
	protected string m_sTaskTitleAfterRescuePoint;
	
	[Attribute(desc: "Description of the task after meeting the rescue point", category: "Task")]
	protected string m_sTaskDescriptionAfterRescuePoint;
	
	
    protected SCR_ScenarioFrameworkTriggerEntity m_RescuePointTrigger;
    protected SCR_ScenarioFrameworkTriggerEntity m_ExtractionPointTrigger;
    protected IEntity m_AIEntity;

	void UpdateTaskTitleAndDescription()
	{
		if (!m_sTaskTitleAfterRescuePoint.IsEmpty()) 
			m_Task.SetTitle(m_sTaskTitleAfterRescuePoint);
		if (!m_sTaskDescriptionAfterRescuePoint.IsEmpty()) 
			m_Task.SetDescription(m_sTaskDescriptionAfterRescuePoint);
	}
	
    override void SetupTask()
    {
        super.SetupTask();

        if (!m_Task)
        {
            Print("RescueTask => Rescue Task is not properly initialized.", LogLevel.ERROR);
            return;
        }

		array<SCR_ScenarioFrameworkSlotTask> slotTasks = {};
		
		GetAllSlotTasks(slotTasks);
		
        // Parcourt les enfants pour identifier RescuePoint, RescueExtraction, et l'entité AI
        foreach (SCR_ScenarioFrameworkLayerBase slotTask : slotTasks)
        {
            if (!slotTask)
                continue;

            string entityName = slotTask.GetName();

            if (entityName.Contains("RescuePoint"))
            {
                m_RescuePointTrigger = SCR_ScenarioFrameworkTriggerEntity.Cast(slotTask.GetSpawnedEntity());
            }
            else if (entityName.Contains("RescueExtraction"))
            {
                m_ExtractionPointTrigger = SCR_ScenarioFrameworkTriggerEntity.Cast(slotTask.GetSpawnedEntity());
            }
            else if (entityName.Contains("RescueAi"))
            {
                m_AIEntity = slotTask.GetSpawnedEntity();
            }
        }

        // Valider la configuration des triggers et de l'AI
        if (!m_RescuePointTrigger)
        {
            Print("RescueTask => Missing Rescue Point Trigger. Ensure a trigger with 'RescuePoint' is configured.", LogLevel.ERROR);
			return;
        }

        if (!m_ExtractionPointTrigger)
        {
            Print("RescueTask => Missing Rescue Extraction Trigger. Ensure a trigger with 'RescueExtraction' is configured.", LogLevel.ERROR);
			return;
        }

        if (!m_AIEntity)
        {
            Print("RescueTask => Missing AI Entity. Ensure an entity with 'AIEntity' in its name is configured.", LogLevel.ERROR);
			return;
        }

        // Configurer la tâche
        RSTC_ScenarioFrameworkTaskRescue rescueTask = RSTC_ScenarioFrameworkTaskRescue.Cast(m_Task);
        if (!rescueTask)
        {
			Print("RescueTask => Missing RSTC_ScenarioFrameworkTaskRescue", LogLevel.ERROR);
			return;
        }

         rescueTask.SetRescuePointTrigger(m_RescuePointTrigger);
         rescueTask.SetExtractionTrigger(m_ExtractionPointTrigger);
         rescueTask.SetAIEntity(m_AIEntity);
        
    }
	
}
