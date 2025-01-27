class RSTC_TasksManagerComponentClass : RSTC_ComponentClass
{
};

[EntityEditorProps(category: "GameScripted/ScenarioFramework", description: "Manages RSTC_City entities.")]
class RSTC_TasksManagerComponent : RSTC_Component
{
    protected ref map<RSTC_ESFTaskType, ref array<RSTC_ESFTaskSubType>> m_TaskTypeMapping = new map<RSTC_ESFTaskType, ref array<RSTC_ESFTaskSubType>>();

    static RSTC_TasksManagerComponent s_Instance;

    static RSTC_TasksManagerComponent GetInstance()
    {
        if (!s_Instance)
        {
            BaseGameMode pGameMode = GetGame().GetGameMode();
            if (pGameMode)
                s_Instance = RSTC_TasksManagerComponent.Cast(pGameMode.FindComponent(RSTC_TasksManagerComponent));
        }

        return s_Instance;
    }

    void Init(IEntity owner)
    {
        InitializeTaskMappings();
    }

    void InitializeTaskMappings()
    {
        m_TaskTypeMapping.Insert(RSTC_ESFTaskType.SEARCH, {
            RSTC_ESFTaskSubType.SEARCH_INTEL
        });

        m_TaskTypeMapping.Insert(RSTC_ESFTaskType.DELIVER, {
            RSTC_ESFTaskSubType.DELIVER_FUEL
        });

        m_TaskTypeMapping.Insert(RSTC_ESFTaskType.DESTROY, {
            RSTC_ESFTaskSubType.DESTROY_STEAL_VEHICLE
        });

        m_TaskTypeMapping.Insert(RSTC_ESFTaskType.KILL, {
            RSTC_ESFTaskSubType.KILL_LEADER
        });

        m_TaskTypeMapping.Insert(RSTC_ESFTaskType.CLEAR, {
            RSTC_ESFTaskSubType.CLEAR_SMALL_AREA,
            RSTC_ESFTaskSubType.CLEAR_BIG_AREA,
            RSTC_ESFTaskSubType.CLEAR_AREA_ROADBLOCK
        });

        m_TaskTypeMapping.Insert(RSTC_ESFTaskType.EXTRACTION, {
            RSTC_ESFTaskSubType.EXTRACTION_SOLDIER
        });
    }

    array<RSTC_ESFTaskSubType> GetSubTypes(RSTC_ESFTaskType type)
    {
        if (m_TaskTypeMapping.Contains(type))
        {
            PrintFormat("GetSubTypes for type: %1", typename.EnumToString(RSTC_ESFTaskType, type));
            return m_TaskTypeMapping[type];
        }

        PrintFormat("No subtypes found for type: %1", typename.EnumToString(RSTC_ESFTaskType, type));
        return {};
    }

    RSTC_ESFTaskType GetMainType(RSTC_ESFTaskSubType subType)
    {
        foreach (RSTC_ESFTaskType mainType, array<RSTC_ESFTaskSubType> subTypes : m_TaskTypeMapping)
        {
            if (subTypes.Contains(subType))
                return mainType;
        }

        return RSTC_ESFTaskType.NONE;
    }

    RSTC_ESFTaskSubType GetRandomSubTypeFromType(RSTC_ESFTaskType type)
    {
        array<RSTC_ESFTaskSubType> subTypes = GetSubTypes(type);

        if (subTypes.IsEmpty())
        {
            PrintFormat("No subtypes available for type: %1", typename.EnumToString(RSTC_ESFTaskType, type));
            return RSTC_ESFTaskSubType.NONE;
        }

        int randomIndex = Math.RandomInt(0, subTypes.Count());
        RSTC_ESFTaskSubType chosenSubType = subTypes[randomIndex];
        return chosenSubType;
    }
}