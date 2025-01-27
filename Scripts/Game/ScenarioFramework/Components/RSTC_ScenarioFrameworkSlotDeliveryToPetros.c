[EntityEditorProps(category: "GameScripted/ScenarioFramework/Slot", description: "SlotPick that always spawns in front of Petros.")]
class RSTC_ScenarioFrameworkSlotDeliveryToPetrosClass : SCR_ScenarioFrameworkSlotDeliveryClass
{
}

class RSTC_ScenarioFrameworkSlotDeliveryToPetros : SCR_ScenarioFrameworkSlotDelivery
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
        vector newPosition = petrosPosition + (forwardDirection * 2.0); 

        GetOwner().SetOrigin(newPosition);

        return super.InitOtherThings();
    }
	
}

