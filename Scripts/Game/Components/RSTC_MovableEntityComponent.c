[EntityEditorProps(category: "GameScripted/Components", description: "")]
class RSTC_MovableEntityComponentClass : ScriptComponentClass
{
}

class RSTC_MovableEntityComponent : ScriptComponent
{
    [Attribute("{56EBF5038622AC95}Assets/Conflict/CanBuild.emat")]
    protected ResourceName m_canBuildMaterial;

    [Attribute("{14A9DCEA57D1C381}Assets/Conflict/CannotBuild.emat")]
    protected ResourceName m_cannotBuildMaterial;

    [Attribute("3", desc: "Max placement distance in meters.")]
    protected float m_maxPlacementDistance;

    [Attribute(params: "xob")]
    protected ResourceName m_previewObject;

    [Attribute(
        uiwidget: UIWidgets.Flags,
        desc: "Set of flags to ignore objects based on their physics layer.\nWARNING: To prevent players from attaching objects to weapons, use Ignored Components list as checking weapon layer will make it impossible to attach the object to armed vehicles!",
        enums: ParamEnumArray.FromEnum(EPhysicsLayerDefs)
    )]
    protected EPhysicsLayerDefs m_ignoredPhysicsLayers;

    [Attribute(
        SCR_ECharacterDistanceMeasurementMethod.FROM_EYES.ToString(),
        UIWidgets.ComboBox,
        "How the distance between player and placement position should be calculated",
        enums: ParamEnumArray.FromEnum(SCR_ECharacterDistanceMeasurementMethod)
    )]
    protected SCR_ECharacterDistanceMeasurementMethod m_measurementMethod;

    [Attribute(
        uiwidget: UIWidgets.ComboBox,
        enums: ParamEnumArray.FromEnum(SCR_EPlacementType)
    )]
    protected SCR_EPlacementType m_placementType;

    [Attribute(
        uiwidget: UIWidgets.CheckBox,
        desc: "Can this entity be attached to dynamic objects."
    )]
    protected bool m_canAttachToDynamicObject;

    [Attribute(
        uiwidget: UIWidgets.CheckBox,
        desc: "Can this entity be attached even when it is not upright."
    )]
    protected bool m_canAttachAngled;

    protected bool m_isMoving;
    protected int m_targetEntityNodeID;
    protected bool m_canPlace = true;
    protected bool m_isBeingAttachedToEntity;
    protected vector m_currentMat[4];
    protected vector m_camDeploymentPosition[4];
    protected IEntity m_previewEntity;
    protected RplId m_targetId;
    protected IEntity m_targetEntity;

    bool IsMoving()
    {
        return m_isMoving;
    }

    //------------------------------------------------------------------------------------------------
    protected SCR_CharacterControllerComponent GetCharacterController(IEntity from)
    {
        if (!from)
            return null;

        ChimeraCharacter character = ChimeraCharacter.Cast(from);
        if (!character)
            return SCR_CharacterControllerComponent.Cast(from.FindComponent(SCR_CharacterControllerComponent));

        return SCR_CharacterControllerComponent.Cast(character.GetCharacterController());
    }

    //------------------------------------------------------------------------------------------------
    void PlaceItem()
    {
        IEntity controlledEntity = SCR_PlayerController.GetLocalControlledEntity();
        if (!controlledEntity)
            return;

        if (!m_canPlace)
        {
            SCR_NotificationsComponent.SendToPlayer(
                GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(controlledEntity),
                ENotification.EDITOR_PLACING_BLOCKED
            );
            return;
        }

        ChimeraCharacter character = ChimeraCharacter.Cast(controlledEntity);
        vector newPosition = m_currentMat[3]; // Calculated placement position
        GetOwner().SetOrigin(newPosition);

        DisablePreview();
    }

    //------------------------------------------------------------------------------------------------
    void ValidateTargetEntityExistence()
    {
        TraceParam param = new TraceParam();
        param.Start = m_camDeploymentPosition[3];
        param.End = param.Start + m_camDeploymentPosition[2] * m_maxPlacementDistance;
        param.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
        param.Exclude = SCR_PlayerController.GetLocalControlledEntity();
        param.LayerMask = EPhysicsLayerPresets.Projectile;

        BaseWorld world = GetOwner().GetWorld();
        float traceDistance = world.TraceMove(param, FilterCallback);
        IEntity tracedEntity = param.TraceEnt;

        if (!tracedEntity && m_targetId.IsValid())
        {
            m_canPlace = false;
        }
        else if (tracedEntity)
        {
            RplComponent rplComp = RplComponent.Cast(tracedEntity.FindComponent(RplComponent));
            if (!rplComp || rplComp.Id() != m_targetId)
                m_canPlace = false;
        }

        if (m_targetEntityNodeID != param.NodeIndex)
            m_canPlace = false;

        if (!m_canPlace)
        {
            IEntity controlledEntity = SCR_PlayerController.GetLocalControlledEntity();
            if (!controlledEntity)
                return;

            SCR_CharacterControllerComponent characterController = GetCharacterController(controlledEntity);
            if (!characterController)
                return;

            if (!characterController.IsUsingItem())
            {
                GetGame().GetCallqueue().Remove(ValidateTargetEntityExistence);
            }
            else
            {
                characterController.ActionUnequipItem();
                GetGame().GetCallqueue().Remove(ValidateTargetEntityExistence);
            }
        }
    }

    //------------------------------------------------------------------------------------------------
    protected void DisablePreview()
    {
        GetGame().GetInputManager().RemoveActionListener("CharacterFire", EActionTrigger.DOWN, PlaceItem);
        ClearEventMask(GetOwner(), EntityEvent.FRAME);
        delete m_previewEntity;
        m_isMoving = false;
    }

    VObject GetPreviewVObject()
	{
		if (m_previewObject.IsEmpty())
			return GetOwner().GetVObject();
		
		Resource resource = Resource.Load(m_previewObject);
		if (!resource.IsValid())
			return GetOwner().GetVObject();
		
		BaseResourceObject resourceObject = resource.GetResource();
		if (!resourceObject)
			return GetOwner().GetVObject();
		
		return resourceObject.ToVObject();
	}

    void StartMove()
    {
        EnablePreview();
    }

    //------------------------------------------------------------------------------------------------
    protected void EnablePreview()
    {
        EntityPrefabData prefabData = GetOwner().GetPrefabData();
        if (!prefabData)
            return;

        GetGame().GetInputManager().AddActionListener("CharacterFire", EActionTrigger.DOWN, PlaceItem);

        SetEventMask(GetOwner(), EntityEvent.FRAME);
        if (!m_previewEntity)
            m_previewEntity = GetGame().SpawnEntity(GenericEntity, GetOwner().GetWorld());

        IEntity controlledEntity = SCR_PlayerController.GetLocalControlledEntity();
        if (!controlledEntity)
            return;

        m_previewEntity.SetObject(GetPreviewVObject(), "");
        m_isMoving = true;
    }

    //------------------------------------------------------------------------------------------------
    protected override void EOnFrame(IEntity owner, float timeSlice)
    {
        if (!m_isMoving)
            return;

        CameraManager cameraManager = GetGame().GetCameraManager();
        if (!cameraManager)
            return;

        CameraBase currentCamera = cameraManager.CurrentCamera();
        if (!currentCamera)
            return;

        IEntity controlledEntity = SCR_PlayerController.GetLocalControlledEntity();
        if (!controlledEntity)
            return;

        vector cameraMat[4];
        currentCamera.GetTransform(cameraMat);
        float maxPlacementDistance = m_maxPlacementDistance;
        SCR_EPlacementType placementType = m_placementType;

        switch (placementType)
        {
            case SCR_EPlacementType.XZ_FIXED:
                UseXZFixedPlacement(owner, maxPlacementDistance, cameraMat);
                break;

            case SCR_EPlacementType.XYZ:
                UseXYZPlacement(owner, maxPlacementDistance, cameraMat);
                break;
        }

        if (m_canPlace)
            SCR_Global.SetMaterial(m_previewEntity, m_canBuildMaterial);
        else
            SCR_Global.SetMaterial(m_previewEntity, m_cannotBuildMaterial);
    }

    //------------------------------------------------------------------------------------------------
    void UseXZFixedPlacement(IEntity owner, float maxPlacementDistance, vector cameraMat[4])
    {
        vector direction = cameraMat[2];
        direction[1] = 0;
        direction.Normalize();

        IEntity character = SCR_PlayerController.GetLocalControlledEntity();

        // Trace against terrain and entities to detect item placement position
        TraceParam param = new TraceParam();
        param.Start = character.GetOrigin() + maxPlacementDistance * direction + vector.Up;
        param.End = param.Start - 5 * vector.Up;
        param.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
        param.Exclude = character;
        param.LayerMask = EPhysicsLayerPresets.Projectile;

        BaseWorld world = owner.GetWorld();
        float traceDistance = world.TraceMove(param, FilterCallback);
        m_previewEntity.GetTransform(m_currentMat);
        m_currentMat[3] = param.Start + ((param.End - param.Start) * traceDistance);
        vector up = param.TraceNorm;

        SCR_EntityHelper.OrientUpToVector(up, m_currentMat);
        m_previewEntity.SetTransform(m_currentMat);
        m_previewEntity.Update();

        m_canPlace = ValidatePlacement(up, param.TraceEnt, world, character);
    }

    //------------------------------------------------------------------------------------------------
    protected bool FilterCallback(IEntity e)
    {
        Physics physics = e.GetPhysics();
        if (physics)
        {
            if ((physics.IsDynamic() || physics.IsKinematic()) && !m_canAttachToDynamicObject)
                return false;
        }

        return true;
    }

    //------------------------------------------------------------------------------------------------
    void UseXYZPlacement(IEntity owner, float maxPlacementDistance, vector cameraMat[4])
    {
        // Trace against terrain and entities to detect item placement position
        ChimeraCharacter character = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
        if (!character)
            return;

        TraceParam param = new TraceParam();
        param.Start = character.EyePosition();
        param.End = param.Start + cameraMat[2] * maxPlacementDistance;
        param.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
        param.Exclude = SCR_PlayerController.GetLocalControlledEntity();
        param.LayerMask = EPhysicsLayerPresets.Projectile;

        BaseWorld world = owner.GetWorld();
        float traceDistance = world.TraceMove(param, FilterCallback);
        m_previewEntity.GetTransform(m_currentMat);
        m_currentMat[3] = param.Start + ((param.End - param.Start) * traceDistance);
        vector up = param.TraceNorm;
		m_currentMat[3] = m_currentMat[3] + up * 0.01;

        IEntity tracedEntity = param.TraceEnt;

        if (traceDistance == 1) // Assume we didn't hit anything and snap item on the ground
        {
            // Trace against terrain and entities to detect new placement position
            TraceParam paramGround = new TraceParam();
            paramGround.Start = param.End + vector.Up;
            paramGround.End = paramGround.Start - vector.Up * 20;
            paramGround.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
            paramGround.Exclude = SCR_PlayerController.GetLocalControlledEntity();
            paramGround.LayerMask = EPhysicsLayerPresets.Projectile;

            float traceGroundDistance = world.TraceMove(paramGround, FilterCallback);
            m_previewEntity.GetTransform(m_currentMat);
            m_currentMat[3] = paramGround.Start + ((paramGround.End - paramGround.Start) * traceGroundDistance) + vector.Up * 0.01; // adding 1 cm to avoid collision with object under

            if (traceGroundDistance < 1)
                up = paramGround.TraceNorm;

            tracedEntity = paramGround.TraceEnt;
        }

        SCR_EntityHelper.OrientUpToVector(up, m_currentMat);
        vector right = up * cameraMat[0];
        vector forward = up * right;
        right.Normalize();
        forward.Normalize();
        m_currentMat[0] = forward;
        m_currentMat[2] = right;

        #ifdef WORKBENCH
            ShapeFlags shapeFlags = ShapeFlags.ONCE | ShapeFlags.NOZBUFFER | ShapeFlags.TRANSP | ShapeFlags.DOUBLESIDE | ShapeFlags.NOOUTLINE;
            Shape.CreateArrow(m_currentMat[3], m_currentMat[3] + up * 0.5, 0.1, Color.BLUE, shapeFlags);
            Shape.CreateArrow(m_currentMat[3], m_currentMat[3] + right * 0.5, 0.1, Color.ORANGE, shapeFlags);
            Shape.CreateArrow(m_currentMat[3], m_currentMat[3] + forward * 0.5, 0.1, Color.GREEN, shapeFlags);
        #endif

        m_previewEntity.SetTransform(m_currentMat);
        m_previewEntity.Update();

        // Reject based on distance from character
        if (GetDistanceFromCharacter(character, m_currentMat[3], m_measurementMethod) > maxPlacementDistance)
        {
            m_canPlace = false;
            return;
        }

        m_targetEntityNodeID = param.NodeIndex;
        m_camDeploymentPosition = cameraMat;
        m_camDeploymentPosition[3] = character.EyePosition();
        m_canPlace = ValidatePlacement(m_currentMat[1], tracedEntity, world, SCR_PlayerController.GetLocalControlledEntity());
    }

    static float GetDistanceFromCharacter(
        notnull ChimeraCharacter character,
        vector destination,
        SCR_ECharacterDistanceMeasurementMethod method = SCR_ECharacterDistanceMeasurementMethod.FROM_EYES
    )
    {
        vector pos;
        switch (method)
        {
            case SCR_ECharacterDistanceMeasurementMethod.FROM_ORIGIN:
                pos = character.GetOrigin();
                break;

            case SCR_ECharacterDistanceMeasurementMethod.FROM_CENTER_OF_MASS:
                pos = character.AimingPosition(); // Physics cannot be used for characters
                break;

            default: // FROM_EYES
                pos = character.EyePosition();
                break;
        }

        return vector.Distance(pos, destination);
    }

    //------------------------------------------------------------------------------------------------
    protected bool ValidateEntity(notnull IEntity entity)
    {
        Physics physics = entity.GetPhysics();
        if (physics && physics.IsDynamic() && !m_canAttachToDynamicObject)
            return false;

        // Check main parent entity
        IEntity mainEntity = entity.GetRootParent();
        if (mainEntity && mainEntity != entity)
        {
            physics = mainEntity.GetPhysics();
            if (physics && physics.IsDynamic() && !m_canAttachToDynamicObject)
                return false;
        }

        // Check ignored physics layers
        IEntity parent = entity;
        while (parent)
        {
            physics = parent.GetPhysics();
            if (physics && (physics.GetInteractionLayer() & m_ignoredPhysicsLayers))
                return false;

            parent = parent.GetParent();
        }

        return true;
    }

    //------------------------------------------------------------------------------------------------
    bool ValidatePlacement(vector up, IEntity tracedEntity, BaseWorld world, IEntity character)
    {
        if (!m_canAttachAngled && vector.Dot(up, vector.Up) < 0.5) // Early reject based on placement angle
        {
            SCR_Global.SetMaterial(m_previewEntity, m_cannotBuildMaterial);
            return false;
        }

        if (tracedEntity != m_targetEntity)
        {
            m_isBeingAttachedToEntity = false;
            m_targetEntity = null;
            m_targetId = -1;
            m_targetEntityNodeID = -1;

            if (!ValidateEntity(tracedEntity)) // Reject items with dynamic physics
                return false;

            Physics entPhysics = tracedEntity.GetPhysics();
            if (entPhysics && !(entPhysics.GetInteractionLayer() & EPhysicsLayerDefs.Terrain))
                m_isBeingAttachedToEntity = true;
        }

        array<IEntity> excludeArray = { m_previewEntity, character };

        // Reject based on bbox collision with surrounding objects
        TraceOBB paramOBB = new TraceOBB();
        Math3D.MatrixIdentity3(paramOBB.Mat);
        paramOBB.Mat[0] = m_currentMat[0];
        paramOBB.Mat[1] = m_currentMat[1];
        paramOBB.Mat[2] = m_currentMat[2];
        paramOBB.Start = m_currentMat[3] + 0.05 * paramOBB.Mat[1];
        paramOBB.Flags = TraceFlags.ENTS;
        paramOBB.ExcludeArray = excludeArray;
        paramOBB.LayerMask = EPhysicsLayerPresets.Projectile;

        m_previewEntity.GetBounds(paramOBB.Mins, paramOBB.Maxs);
        world.TracePosition(paramOBB, TraceEntitiesCallback);

        #ifdef WORKBENCH
            ShapeFlags shapeFlags = ShapeFlags.ONCE | ShapeFlags.NOZBUFFER | ShapeFlags.TRANSP | ShapeFlags.DOUBLESIDE | ShapeFlags.NOOUTLINE;
            Shape.CreateArrow(paramOBB.Start, paramOBB.Start + paramOBB.Mat[1] * 0.5, 0.1, Color.WHITE, shapeFlags);
            Shape.CreateArrow(paramOBB.Start, paramOBB.Start + paramOBB.Mat[2] * 0.5, 0.1, Color.PINK, shapeFlags);
            Shape.CreateArrow(paramOBB.Start, paramOBB.Start + paramOBB.Mat[0] * 0.5, 0.1, Color.YELLOW, shapeFlags);
        #endif

        // Collides with another entity
        if (paramOBB.TraceEnt)
            return false;

        if (tracedEntity && m_isBeingAttachedToEntity)
        {
            RplComponent rplComp = RplComponent.Cast(tracedEntity.FindComponent(RplComponent));
            if (rplComp)
                m_targetId = rplComp.Id();
            else
                m_isBeingAttachedToEntity = false;
        }
        m_targetEntity = tracedEntity;

        return true;
    }

    //------------------------------------------------------------------------------------------------
    protected override void OnPostInit(IEntity owner)
    {
        SetEventMask(owner, EntityEvent.INIT);
    }

    //------------------------------------------------------------------------------------------------
    protected override void EOnInit(IEntity owner)
    {
        Math3D.MatrixIdentity4(m_currentMat);
    }
}