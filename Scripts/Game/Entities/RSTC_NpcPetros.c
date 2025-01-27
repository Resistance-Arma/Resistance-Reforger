class RSTC_NpcPetrosClass: SCR_ChimeraCharacterClass
{
};

class RSTC_NpcPetros: SCR_ChimeraCharacter
{

	static void SpawnPetros(vector position)
	{
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
		
		RSTC_NpcPetros petros = RSTC_NpcPetros.Cast(spawnedEntity);
		
		if(!petros)
			return;
		
		petros.m_pFactionComponent.SetAffiliatedFaction(RSTC_Global().GetFactionManager().GetResistanceFaction());
	}
}

