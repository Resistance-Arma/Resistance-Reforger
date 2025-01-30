[EntityEditorProps(category: "GameScripted/ScenarioFramework/Slot", description: "SlotPick that always spawns in front of Petros.")]
class RSTC_ScenarioFrameworkSlotPickToPetrosClass : SCR_ScenarioFrameworkSlotPickClass
{
}

class RSTC_ScenarioFrameworkSlotPickToPetros : SCR_ScenarioFrameworkSlotPick
{
    override bool InitOtherThings()
    {
		SCR_ChimeraCharacter petros = RSTC_Global.GetPetros();
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

