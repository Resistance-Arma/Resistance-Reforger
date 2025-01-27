[EntityEditorProps(category: "GameScripted/ScenarioFramework", description: "City with dynamic faction support.")]
class RSTC_CityComponentClass : SCR_ScenarioFrameworkAreaClass {}

class RSTC_CityComponent : SCR_ScenarioFrameworkArea
{
    [Attribute(defvalue: "650.0", UIWidgets.Slider, params: "0 inf", desc: "Default Supported faction reputation", category: "Faction")]
    float m_fDefaultSupportedFactionReputation;
    
    [Attribute(defvalue: "250.0", UIWidgets.Slider, params: "0 inf", desc: "Default Player faction reputation", category: "Faction")]
    float m_fDefaultPlayerFactionReputation;
    
    //! Map to store faction support levels.
    protected ref map<FactionKey, float> m_FactionSupport = new map<FactionKey, float>();
    protected FactionKey m_supportedFactionKey;
    protected vector m_location;
    protected int m_id;


    override void AfterAllChildrenSpawned(SCR_ScenarioFrameworkLayerBase layer)
    {
        super.AfterAllChildrenSpawned(layer);	
    }
    
    void SpawnAllCivils()
    {	
        ResourceName slotCivilPrefabToSpawn = RSTC_Global.GetConfig().GetSlotCivilPrefabToSpawn();
    
        if (slotCivilPrefabToSpawn.IsEmpty())
        {
            PrintFormat("RSTC_CityComponent:: SlotCivil prefab to spawn is empty.", LogLevel.ERROR);
            return;
        }
    
        int numberCivilInCity = GetNumberCivilInCity();
    
        if (numberCivilInCity >= RSTC_Global.GetConfig().GetMaxCivilPerCity())
            return;

        int civilsToSpawn = RSTC_Global.GetConfig().GetMaxCivilPerCity() - numberCivilInCity;
        for (int i = 0; i < civilsToSpawn; i++)
        {
            EntitySpawnParams spawnParams = new EntitySpawnParams();
            spawnParams.TransformMode = ETransformMode.LOCAL;
            spawnParams.Transform[3] = vector.Zero;
            spawnParams.Parent = GetOwner();

            IEntity civilEntity = GetGame().SpawnEntityPrefab(Resource.Load(slotCivilPrefabToSpawn), null, spawnParams);

            if (civilEntity)
            {
                RSTC_ScenarioFrameworkSlotCivil slotCivil = RSTC_ScenarioFrameworkSlotCivil.Cast(civilEntity.FindComponent(RSTC_ScenarioFrameworkSlotCivil));
                if (!slotCivil)
                    PrintFormat("RSTC_CityComponent:: Failed to initialize RSTC_ScenarioFrameworkSlotCivil for spawned entity", LogLevel.ERROR);
                else
                    slotCivil.Init(this, SCR_ScenarioFrameworkEActivationType.ON_INIT);
            }
            else
            {
                PrintFormat("RSTC_CityComponent:: Failed to spawn civilian entity at iteration: %1", i, LogLevel.ERROR);
            }
        }
    }
    
    int GetNumberCivilInCity()
    {
        int civilInCity = 0;
        IEntity child = GetOwner().GetChildren();

        while (child)
        {
            RSTC_ScenarioFrameworkSlotCivil slotCivil = RSTC_ScenarioFrameworkSlotCivil.Cast(child.FindComponent(RSTC_ScenarioFrameworkSlotCivil));
            if (slotCivil)
            {
                civilInCity++;
            }

            child = child.GetSibling();
        }

        return civilInCity;
    }
    
    void Setup(int id, array<FactionKey> factionKeys)
    {
        if (factionKeys.IsEmpty())
        {
            PrintFormat("RSTC_CityComponent:: No faction keys provided", LogLevel.WARNING);
            return;
        }

        foreach (FactionKey factionKey : factionKeys)
        {
            if (factionKey.IsEmpty())
            {
                PrintFormat("RSTC_CityComponent:: Invalid FactionKey detected. Skipping.", LogLevel.WARNING);
                continue;
            }

            // Define the initial support based on the faction
            if (factionKey == RSTC_Global.GetConfig().GetDefaultSupportedFactionKey())
                m_FactionSupport[factionKey] = m_fDefaultSupportedFactionReputation;
            else
                m_FactionSupport[factionKey] = m_fDefaultPlayerFactionReputation;
        }

        m_id = id;
        m_location = GetOwner().GetOrigin();
        UpdateCityFaction();
    }

    vector GetCityLocation()
    {
        return m_location;
    }
    
    int GetCityId()
    {
        return m_id;
    }

    float GetFactionSupport(FactionKey factionKey)
    {
        if (factionKey.IsEmpty() || !m_FactionSupport.Contains(factionKey))
        {
            PrintFormat("RSTC_CityComponent:: FactionKey not found or invalid. Returning default support (0.0).");
            return 0.0;
        }
        return m_FactionSupport[factionKey];
    }

    // Adjust the support level for a faction by a delta value using FactionKey
    void AdjustFactionSupport(FactionKey factionKey, float delta)
    {
        if (factionKey.IsEmpty())
        {
            PrintFormat("RSTC_CityComponent:: Invalid FactionKey. Skipping adjustment.", LogLevel.WARNING);
            return;
        }

        float currentSupport = GetFactionSupport(factionKey);
        float newSupport = currentSupport + delta;

        if (newSupport != currentSupport)
        {
            PrintFormat("RSTC_CityComponent:: Adjusting support for FactionKey: %1, Delta: %2, New Support: %3", factionKey, delta, newSupport);
            m_FactionSupport[factionKey] = newSupport;
            OnFactionSupportChanged(factionKey, newSupport);
            UpdateCityFaction(); // Update the supported faction after each change
        }
    }
    
    // Set the support level for a faction explicitly using FactionKey
    void SetFactionSupport(FactionKey factionKey, float value)
    {
        if (factionKey.IsEmpty())
        {
            PrintFormat("RSTC_CityComponent:: Invalid FactionKey. Skipping support set.", LogLevel.WARNING);
            return;
        }

        m_FactionSupport[factionKey] = value;
        OnFactionSupportChanged(factionKey, value);
        UpdateCityFaction(); // Update the supported faction after each change
    }

    // Triggered when faction support changes (now using FactionKey)
    protected void OnFactionSupportChanged(FactionKey factionKey, float newSupport)
    {
        PrintFormat("RSTC_CityComponent:: FactionKey support changed. FactionKey: %1, New Support: %2", factionKey, newSupport);
    }

    // Update the supported faction based on FactionKey
    void UpdateCityFaction()
    {
        float maxSupport = -1e9; // A very low number
        FactionKey topFactionKey = "";

        foreach (FactionKey factionKey, float support : m_FactionSupport)
        {
            if (support > maxSupport)
            {
                maxSupport = support;
                topFactionKey = factionKey;
            }
        }

        if (!topFactionKey.IsEmpty())
            SetCityFaction(topFactionKey);
        else
            PrintFormat("RSTC_CityComponent:: No faction determined as supported.", LogLevel.WARNING);
    }

    //! Explicitly sets the supported faction of the city using FactionKey.
    void SetCityFaction(FactionKey factionKey)
    {
        if (factionKey.IsEmpty())
        {
            PrintFormat("RSTC_CityComponent:: Invalid FactionKey provided for SetCityFaction.", LogLevel.ERROR);
            return;
        }

        if (m_supportedFactionKey == factionKey)
            return;

        m_supportedFactionKey = factionKey;

        PrintFormat("RSTC_CityComponent:: The supported faction of the city: %1 is now: %2", GetName(),  factionKey);
    }

    //! Modifies the reputation of a faction in the city.
    //! \param factionKey The faction whose reputation is being modified.
    //! \param delta The amount to add or subtract from the faction's reputation. Use positive values to add and negative values to subtract.
    void ModifyFactionReputation(FactionKey factionKey, float delta)
    {
        if (factionKey.IsEmpty())
        {
            PrintFormat("RSTC_CityComponent:: Invalid FactionKey. Cannot modify reputation.", LogLevel.WARNING);
            return;
        }

        float currentReputation = GetFactionSupport(factionKey);
        float newReputation = currentReputation + delta;

        if (newReputation < 0.0)
            newReputation = 0.0; // Clamp to avoid negative reputation.

        m_FactionSupport[factionKey] = newReputation;

        PrintFormat("RSTC_CityComponent:: Modified reputation for FactionKey: %1 by delta: %2. New reputation: %3", factionKey, delta, newReputation);
        UpdateCityFaction();
    }
        
    //! Get the FactionKey associated with the city.
    FactionKey GetCitySupportedFactionKey()
    {
        return m_supportedFactionKey;
    }

    bool IsValidForTaskType(RSTC_ESFTaskType taskType)
    {
        array<SCR_ScenarioFrameworkLayerTask> layerTasks = {};
        GetAllLayerTasks(layerTasks);

        if (layerTasks.IsEmpty())
            return false;
        
        foreach (SCR_ScenarioFrameworkLayerTask layerTask : layerTasks)
        {
            if (layerTask.GetTaskType() == taskType && layerTask.GetLayerTaskState() == SCR_TaskState.OPENED)
            {
                return true;
            }
        }
        
        return false;
    }
    
    bool IsValidForTaskSubType(RSTC_ESFTaskSubType taskSubType)
    {
        array<SCR_ScenarioFrameworkLayerTask> layerTasks = {};
        GetAllLayerTasks(layerTasks);
        
        if (layerTasks.IsEmpty())
            return false;
        
        foreach (SCR_ScenarioFrameworkLayerTask layerTask : layerTasks)
        {
            if (layerTask.GetTaskSubType() == taskSubType)
            {
                return true;
            }
        }
        
        return false;	
    }
    
    array<SCR_ScenarioFrameworkLayerTask> GetAllTaskLayerWithTaskType(RSTC_ESFTaskType taskType)
    {
        array<SCR_ScenarioFrameworkLayerTask> layerTasks = {};
        GetAllLayerTasks(layerTasks);
        
        if(layerTasks.IsEmpty())
            return null;
        
        array<SCR_ScenarioFrameworkLayerTask> layerTasksWithTaskType = {};
        
        foreach (SCR_ScenarioFrameworkLayerTask layerTask : layerTasks)
        {
            if (layerTask.GetTaskType() == taskType)
            {
                layerTasksWithTaskType.Insert(layerTask);
            }
        }

        return layerTasksWithTaskType;
    }
    
    SCR_ScenarioFrameworkLayerTask GetRandomTaskLayerFromTaskType(RSTC_ESFTaskType taskType)
    {
        array<SCR_ScenarioFrameworkLayerTask> layerTasks = {};
        GetAllLayerTasks(layerTasks);
        
        if(layerTasks.IsEmpty())
            return null;
        
        array<SCR_ScenarioFrameworkLayerTask> layerTasksWithTaskType = GetAllTaskLayerWithTaskType(taskType);
        
        if(layerTasksWithTaskType.IsEmpty())
            return null;
        
        Math.Randomize(-1);
        int randomIndex = Math.RandomInt(0, layerTasksWithTaskType.Count());
        SCR_ScenarioFrameworkLayerTask randomLayerWithTaskType = layerTasksWithTaskType[randomIndex];

        return randomLayerWithTaskType;
    }
    
    void ActiveTaskWithRandomSubTypeFromTaskType(RSTC_ESFTaskType taskType)
    {
        SCR_ScenarioFrameworkLayerTask layerTask = GetRandomTaskLayerFromTaskType(taskType);
        
        if(layerTask == null)
        {
            PrintFormat("RSTC_CityComponent:: No task layer found for TaskType: %1", taskType, LogLevel.WARNING);
            return;
        }

        layerTask.Init(this, SCR_ScenarioFrameworkEActivationType.ON_TASKS_INIT);
        layerTask.SetLayerTaskState(SCR_TaskState.PROGRESSED);
        
        PrintFormat("RSTC_CityComponent:: Task of type %1 successfully spawned and initialized at %2", taskType, GetOwner().GetName());
    }
    
    override void Init(SCR_ScenarioFrameworkArea area = null, SCR_ScenarioFrameworkEActivationType activation = SCR_ScenarioFrameworkEActivationType.SAME_AS_PARENT)
    {
        super.Init(this, SCR_ScenarioFrameworkEActivationType.ON_INIT);
        SpawnAllCivils();
    }
}
