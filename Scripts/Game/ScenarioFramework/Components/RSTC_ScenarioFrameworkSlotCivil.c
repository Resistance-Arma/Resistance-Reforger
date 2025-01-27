[EntityEditorProps(category: "GameScripted/ScenarioFramework/Slot", description: "")]
class RSTC_ScenarioFrameworkSlotCivilClass : SCR_ScenarioFrameworkSlotAIClass
{
}

class RSTC_ScenarioFrameworkSlotCivil : SCR_ScenarioFrameworkSlotAI
{	
	protected void OnDamageStateChanged()
	{
		if(!m_Entity)
			return;
		
		SCR_DamageManagerComponent damageManager = SCR_DamageManagerComponent.GetDamageManager(m_Entity);
		IEntity instigator;
		if (!damageManager)
			return;
		
		instigator = damageManager.GetInstigator().GetInstigatorEntity();
		
		if(!instigator)
			return;
		
		SCR_ChimeraCharacter chimeraCharacter = SCR_ChimeraCharacter.Cast(instigator);
		
		if (!chimeraCharacter)
			return;
				
		RSTC_CityComponent city = GetParentCity();
		
		if(!city)
			return;
		
		city.ModifyFactionReputation(chimeraCharacter.GetFactionKey(), -20)
	}
	
	
	override void FinishInit()
	{
		if(!m_Entity)
			return;
		
		SCR_DamageManagerComponent damageManager = SCR_DamageManagerComponent.GetDamageManager(m_Entity);
        if (damageManager)
        {
            damageManager.GetOnDamageStateChanged().Insert(OnDamageStateChanged);
        }

        super.FinishInit()
	}

}
