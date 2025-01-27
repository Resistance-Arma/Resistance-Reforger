[EntityEditorProps(category: "GameScripted/ScenarioFramework/Slot", description: "SlotPick that always spawns in front of Petros.")]
class RSTC_ScenarioFrameworkSlotExtractionToPetrosClass : SCR_ScenarioFrameworkSlotExtractionClass
{
}

class RSTC_ScenarioFrameworkSlotExtractionToPetros : SCR_ScenarioFrameworkSlotExtraction
{
	override bool InitOtherThings()
    {
		RSTC_NpcPetros petros = RSTC_Global.GetPetros();
		if(!petros)
			return false;
		
	    vector petrosPosition = petros.GetOrigin();
		
		if(!petrosPosition)
			return false;
		
        vector forwardDirection = vector.Direction(Vector(0, 0, 0), petros.GetTransformAxis(2));
        vector newPosition = petrosPosition + (forwardDirection); 

        GetOwner().SetOrigin(newPosition);

        return super.InitOtherThings();
    }
}

