[EntityEditorProps(category: "GameScripted/UserActions", description: "Action pour lancer le déplacement d'une entité.")]
class RSTC_StartMoveEntityAction : ScriptedUserAction
{	
	protected RSTC_MovableEntityComponent m_MovableItem;

	
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_MovableItem= RSTC_MovableEntityComponent.Cast(pOwnerEntity.FindComponent(RSTC_MovableEntityComponent));
	}
	
    // Méthode appelée lors de l'exécution de l'action
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
    {			
		if (!m_MovableItem)
			return;

		m_MovableItem.StartMove();
    }

    // Définir si l'action peut être affichée dans l'interface utilisateur
    override bool CanBeShownScript(IEntity user)
    {
		if (!m_MovableItem)
			return false;

		if (m_MovableItem.IsMoving())
			return false;
		
        return true;
    }

    // Définir si l'action peut être exécutée par l'utilisateur
    override bool CanBePerformedScript(IEntity user)
    {
        return true; // Toujours exécutable pour cet exemple
    }
}
