class RSTC_NpcPetrosClass: SCR_ChimeraCharacterClass
{
};

class RSTC_NpcPetros: SCR_ChimeraCharacter
{
	private static RSTC_NpcPetros m_pPetrosInstance;

	protected override void EOnInit(IEntity owner)
	{
		m_pPetrosInstance = RSTC_NpcPetros.Cast(owner)	;
		super.EOnInit(owner);
	}

    static bool IsPetrosSpawned()
    {
        return m_pPetrosInstance != null;
    }

    static RSTC_NpcPetros GetPetros()
    {
        return m_pPetrosInstance;
    }
	
	
	static void SpawnPetros(vector position)
	{
		if(IsPetrosSpawned())
			return;
		
		PrintFormat("RSTC_NpcPetros:: SPAWNING PETROS.", LogLevel.WARNING);		
		ResourceName petrosPrefab = RSTC_Global.GetConfig().GetPretrosPrefab();
	
		if (petrosPrefab.IsEmpty())
        {
            PrintFormat("RSTC_NpcPetros:: Petros Prefab is empty.", LogLevel.WARNING);
            return;
        }
		
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
	   	spawnParams.Transform[3] = position;

       	IEntity spawnedEntity = GetGame().SpawnEntityPrefab(Resource.Load(petrosPrefab), GetGame().GetWorld(), spawnParams);
		
		spawnedEntity.SetName("Petros");

		
		SCR_ChimeraCharacter petros = SCR_ChimeraCharacter.Cast(spawnedEntity);
		
		if(!petros)
			return;
		
		petros.m_pFactionComponent.SetAffiliatedFaction(RSTC_Global().GetFactionManager().GetResistanceFaction());
	}
	

}

