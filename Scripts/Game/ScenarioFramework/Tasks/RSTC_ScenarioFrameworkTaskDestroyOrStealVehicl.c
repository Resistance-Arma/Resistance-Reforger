[EntityEditorProps(category: "GameScripted/ScenarioFramework/Task", description: "Task to either destroy or check if the vehicle is stolen.")]
class RSTC_TaskDestroyOrStealVehicleClass : SCR_ScenarioFrameworkTaskClass
{
}

class RSTC_TaskDestroyOrStealVehicle : SCR_ScenarioFrameworkTask
{
    protected bool m_bVehicleDestroyed = false;
    protected bool m_bVehicleOccupied = false;

    //------------------------------------------------------------------------------------------------
    //! Callback when the vehicle is destroyed.
    void OnVehicleDestroyed(EDamageState state)
    {
        if (state != EDamageState.DESTROYED || m_bVehicleDestroyed)
            return;

        m_bVehicleDestroyed = true;
        CompleteTask("Vehicle destroyed!");
    }

    //------------------------------------------------------------------------------------------------
    //! Callback when a passenger or driver enters the vehicle.
   	void OnCompartmentEntered(IEntity vehicle, BaseCompartmentManagerComponent mgr, IEntity occupant, int managerId, int slotId)
	{
	   	if (vehicle != m_Asset || m_bVehicleOccupied)
			return;
	
	   	if (!EntityUtils.IsPlayer(occupant))
			return;
		
		m_bVehicleOccupied = true;
		Finish();
	}


    //------------------------------------------------------------------------------------------------
    //! Completes the task with a success message.
    void CompleteTask(string message)
    {
        Print(message, LogLevel.NORMAL);
		Finish();
    }

    //------------------------------------------------------------------------------------------------
    //! Initializes the task and sets up event listeners for destruction and occupancy.
    override void Init()
    {
        super.Init();

        if (!m_Asset)
        {
            Print("No vehicle asset assigned to the task.", LogLevel.ERROR);
            return;
        }

        // Listen for destruction events
        SCR_DamageManagerComponent damageManager = SCR_DamageManagerComponent.GetDamageManager(m_Asset);
        if (damageManager)
        {
            damageManager.GetOnDamageStateChanged().Insert(OnVehicleDestroyed);
        }

		if (Vehicle.Cast(m_Asset))
		{
			EventHandlerManagerComponent eventHandler = EventHandlerManagerComponent.Cast(m_Asset.FindComponent(EventHandlerManagerComponent));
			if (eventHandler)
				eventHandler.RegisterScriptHandler("OnCompartmentEntered", this, OnCompartmentEntered);
		}
    }

	
    //------------------------------------------------------------------------------------------------
    //! Cleans up listeners to avoid memory leaks.
    void ~RSTC_TaskDestroyOrStealVehicle()
    {
        if (m_Asset)
        {
            // Remove destruction listener
            SCR_DamageManagerComponent damageManager = SCR_DamageManagerComponent.GetDamageManager(m_Asset);
            if (damageManager)
            {
                damageManager.GetOnDamageStateChanged().Remove(OnVehicleDestroyed);
            }

            // Remove compartment entry listener
            EventHandlerManagerComponent eventHandler = EventHandlerManagerComponent.Cast(m_Asset.FindComponent(EventHandlerManagerComponent));
            if (eventHandler)
            {
                eventHandler.RemoveScriptHandler("OnCompartmentEntered", this, OnCompartmentEntered);
            }
        }
    }

    //------------------------------------------------------------------------------------------------
    //! Sets the support entity for this task.
    //! \return True if the support entity was successfully set, false otherwise.
    override bool SetSupportEntity()
    {
        m_SupportEntity = SCR_ScenarioFrameworkTaskDestroySupportEntity.Cast(GetTaskManager().FindSupportEntity(SCR_ScenarioFrameworkTaskDestroySupportEntity));
        if (!m_SupportEntity)
        {
            Print("Task support entity not found. Task will not function.", LogLevel.ERROR);
            return false;
        }
        return true;
    }
}
