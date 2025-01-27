[EntityEditorProps(category: "GameScripted/UserActions", description: "Action pour lancer une quête aléatoire depuis un PNJ.")]
class RSTC_LaunchRandomCityQuestAction : ScriptedUserAction
{
	[Attribute("Type of task", UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(RSTC_ESFTaskType))]
	protected SCR_ESFTaskType m_taskType;

    // Méthode appelée lors de l'exécution de l'action
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
    {
        if (!pOwnerEntity)
        {
            Print("Aucun propriétaire valide pour cette action.", LogLevel.ERROR);
            return;
        }

        // Position du PNJ
        vector npcPosition = pOwnerEntity.GetOrigin();
		
		RSTC_CitiesManagerComponent citiesManager = RSTC_Global.GetCitiesManager();
		
		float searchRadius = RSTC_Global.GetConfig().GetPretrosSearchCityForTaskRadius();
		array<RSTC_CityComponent> nearbyCities = citiesManager.FindNearbyCitiesWithTaskTypeInside(npcPosition, searchRadius, m_taskType);
		
        if (nearbyCities.IsEmpty())
        {
            PrintFormat("Aucune ville avec le task type: %1 trouvée dans le rayon spécifié.",m_taskType,  LogLevel.ERROR);
            return;
        }

        RSTC_CityComponent randomCity = citiesManager.GetRandomOccupiedCityInList(nearbyCities);
        if (!randomCity)
        {
            Print("No occuputed city in the area ", LogLevel.ERROR);
            return;
        }
		
		randomCity.ActiveTaskWithRandomSubTypeFromTaskType(m_taskType);

    }


    // Définir si l'action peut être affichée dans l'interface utilisateur
    override bool CanBeShownScript(IEntity user)
    {
        return true; // Toujours affiché pour cet exemple
    }

    // Définir si l'action peut être exécutée par l'utilisateur
    override bool CanBePerformedScript(IEntity user)
    {
        return true; // Toujours exécutable pour cet exemple
    }
}
