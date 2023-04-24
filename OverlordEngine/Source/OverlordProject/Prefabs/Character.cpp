#include "stdafx.h"
#include "Character.h"

Character::Character(const CharacterDesc& characterDesc) :
	m_CharacterDesc{ characterDesc },
	m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime),
	m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime)
{}

void Character::Initialize(const SceneContext& /*sceneContext*/)
{

	m_CharacterDesc.controller.contactOffset = 0.3f;
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));

	//Camera
	const auto pCamera = AddChild(new FixedCamera());
	m_pCameraComponent = pCamera->GetComponent<CameraComponent>();
	m_pCameraComponent->SetActive(true);

	pCamera->GetTransform()->Translate(0.f, m_CharacterDesc.controller.height * .5f, 0.f);
}

void Character::Update(const SceneContext& sceneContext)
{

	if (m_pCameraComponent->IsActive())
	{
		constexpr float epsilon{ 0.01f }; //Constant that can be used to compare if a float is near zero

		//***************
		//HANDLE INPUT

		//## Input Gathering (move)
		XMFLOAT2 move{0.f, 0.f};
		//move.y should contain a 1 (Forward) or -1 (Backward) based on the active input (check corresponding actionId in m_CharacterDesc)
		//Optional: if move.y is near zero (abs(move.y) < epsilon), you could use the ThumbStickPosition.y for movement
		if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveForward)) //forward
		{
			move.y = 1;
		}
		else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveBackward)) //backward
		{
			move.y = -1;
		}
		if (abs(move.y) < epsilon)
		{
			move.y = sceneContext.pInput->GetThumbstickPosition().y;
		}


		//move.x should contain a 1 (Right) or -1 (Left) based on the active input (check corresponding actionId in m_CharacterDesc)
		//Optional: if move.x is near zero (abs(move.x) < epsilon), you could use the Left ThumbStickPosition.x for movement
		if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveRight)) //right
		{
			move.x = 1;
		}
		else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveLeft)) //left
		{
			move.x = -1;
		}
		if (abs(move.x) < epsilon)
		{
			move.x = sceneContext.pInput->GetThumbstickPosition().x;
		}

		//## Input Gathering (look)
		XMFLOAT2 look{ 0.f, 0.f }; //Uncomment
		//Only if the Left Mouse Button is Down >
			// Store the MouseMovement in the local 'look' variable (cast is required)
		//Optional: in case look.x AND look.y are near zero, you could use the Right ThumbStickPosition for look
		auto mouseMovement = sceneContext.pInput->GetMouseMovement();
		if (sceneContext.pInput->IsMouseButton(InputState::down, 0))
		{
			look = DirectX::XMFLOAT2{ (float)mouseMovement.x, (float)mouseMovement.y };
		}

		if (abs(look.x) < epsilon && abs(look.y) < epsilon)
		{
			look = sceneContext.pInput->GetThumbstickPosition(false);
		}


		//************************
		//GATHERING TRANSFORM INFO

		//Retrieve the TransformComponent
		//Retrieve the forward & right vector (as XMVECTOR) from the TransformComponent
		XMVECTOR forward = DirectX::XMLoadFloat3(&m_pCameraComponent->GetGameObject()->GetTransform()->GetForward()) * move.y;
		XMVECTOR right = DirectX::XMLoadFloat3(&m_pCameraComponent->GetGameObject()->GetTransform()->GetRight()) * move.x;

		//***************
		//CAMERA ROTATION

		//Adjust the TotalYaw (m_TotalYaw) & TotalPitch (m_TotalPitch) based on the local 'look' variable
		//Make sure this calculated on a framerate independent way and uses CharacterDesc::rotationSpeed.
		//Rotate this character based on the TotalPitch (X) and TotalYaw (Y)
		float elapsedTime = sceneContext.pGameTime->GetElapsed();

		m_TotalYaw += look.x * m_CharacterDesc.rotationSpeed * elapsedTime;
		m_TotalPitch -= look.y * m_CharacterDesc.rotationSpeed * elapsedTime;

		GetTransform()->Rotate(m_TotalPitch, m_TotalYaw, 0);
		//m_pVisuals->GetTransform()->Rotate(-m_TotalPitch, 0, 0);

		//********
		//MOVEMENT

		//## Horizontal Velocity (Forward/Backward/Right/Left)
		//Calculate the current move acceleration for this frame (m_MoveAcceleration * ElapsedTime)
		//If the character is moving (= input is pressed)
			//Calculate & Store the current direction (m_CurrentDirection) >> based on the forward/right vectors and the pressed input
			//Increase the current MoveSpeed with the current Acceleration (m_MoveSpeed)
			//Make sure the current MoveSpeed stays below the maximum MoveSpeed (CharacterDesc::maxMoveSpeed)
		//Else (character is not moving, or stopped moving)
			//Decrease the current MoveSpeed with the current Acceleration (m_MoveSpeed)
			//Make sure the current MoveSpeed doesn't get smaller than zero

		m_MoveAcceleration += m_MoveAcceleration * elapsedTime;
		if (abs(move.x) > epsilon || abs(move.y) > epsilon)
		{
			XMStoreFloat3(&m_CurrentDirection, (forward + right));

			m_MoveSpeed += m_MoveAcceleration * elapsedTime;
			if (m_MoveSpeed > m_CharacterDesc.maxMoveSpeed)
			{
				m_MoveSpeed = m_CharacterDesc.maxMoveSpeed;
			}
		}
		else
		{
			m_MoveSpeed -= m_MoveAcceleration * elapsedTime;
			if (m_MoveSpeed < epsilon)
			{
				m_MoveSpeed = 0;
			}
		}

		//Now we can calculate the Horizontal Velocity which should be stored in m_TotalVelocity.xz
		//Calculate the horizontal velocity (m_CurrentDirection * MoveSpeed)
		//Set the x/z component of m_TotalVelocity (horizontal_velocity x/z)
		//It's important that you don't overwrite the y component of m_TotalVelocity (contains the vertical velocity)
		m_TotalVelocity.x = m_CurrentDirection.x * m_MoveSpeed;
		m_TotalVelocity.z = m_CurrentDirection.z * m_MoveSpeed;

		//## Vertical Movement (Jump/Fall)
		//If the Controller Component is NOT grounded (= freefall)
			//Decrease the y component of m_TotalVelocity with a fraction (ElapsedTime) of the Fall Acceleration (m_FallAcceleration)
			//Make sure that the minimum speed stays above -CharacterDesc::maxFallSpeed (negative!)
		//Else If the jump action is triggered
			//Set m_TotalVelocity.y equal to CharacterDesc::JumpSpeed
		//Else (=Character is grounded, no input pressed)
			//m_TotalVelocity.y is zero
		if (!m_pControllerComponent->GetCollisionFlags().isSet(PxControllerCollisionFlag::eCOLLISION_DOWN))
		{

			m_TotalVelocity.y -= m_FallAcceleration * elapsedTime;
			if (m_TotalVelocity.y < -m_CharacterDesc.maxFallSpeed)
			{
				m_TotalVelocity.y = -m_CharacterDesc.maxFallSpeed;
			}
		}
		else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_Jump))
		{
			m_TotalVelocity.y = m_CharacterDesc.JumpSpeed;
		}
		else
		{
			m_TotalVelocity.y = -0.1f;
		}
		Logger::LogDebug(L"Grounded, {}", m_pControllerComponent->GetCollisionFlags().isSet(PxControllerCollisionFlag::eCOLLISION_DOWN));


		//************
		//DISPLACEMENT

		//The displacement required to move the Character Controller (ControllerComponent::Move) can be calculated using our TotalVelocity (m/s)
		//Calculate the displacement (m) for the current frame and move the ControllerComponent

		//The above is a simple implementation of Movement Dynamics, adjust the code to further improve the movement logic and behaviour.
		//Also, it can be usefull to use a seperate RayCast to check if the character is grounded (more responsive)

		DirectX::XMFLOAT3 displacementFloat3;
		DirectX::XMStoreFloat3(&displacementFloat3, DirectX::XMLoadFloat3(&m_TotalVelocity)* sceneContext.pGameTime->GetElapsed());
		m_pControllerComponent->Move(displacementFloat3);
		
		//physx::PxQueryFilterData filterData{};
		//// ignore self
		//filterData.data.word0 = ~UINT(m_pControllerComponent->GetCollisionGroup().word0);
		//
		//physx::PxRaycastBuffer hit{};

		//auto activeScene = SceneManager::Get()->GetActiveScene();
		//auto startPosition = m_pControllerComponent->GetTransform()->GetWorldPosition();

		//PxVec3 start = {startPosition.x, startPosition.y + 5.f, startPosition.z};
		//PxVec3 end = start + PxVec3{ 0, -startPosition.y - 10.f, 0 };
		//auto direction = (end - start).getNormalized();
		//auto distance = (end - start).magnitude();


		//auto raycast = activeScene->GetPhysxProxy()->Raycast(
		//	start,
		//	direction,
		//	distance,
		//	hit,
		//	physx::PxHitFlag::eDEFAULT,
		//	filterData);

		//if (raycast)
		//{
		//	// Setting is grounded to true
		//	m_IsGrounded = true;
		//}
		//else
		//{
		//	m_IsGrounded = false;
		//}
	}
}

void Character::DrawImGui()
{
	if (ImGui::CollapsingHeader("Character"))
	{
		ImGui::Text(std::format("Move Speed: {:0.1f} m/s", m_MoveSpeed).c_str());
		ImGui::Text(std::format("Fall Speed: {:0.1f} m/s", m_TotalVelocity.y).c_str());

		ImGui::Text(std::format("Move Acceleration: {:0.1f} m/s2", m_MoveAcceleration).c_str());
		ImGui::Text(std::format("Fall Acceleration: {:0.1f} m/s2", m_FallAcceleration).c_str());

		const float jumpMaxTime = m_CharacterDesc.JumpSpeed / m_FallAcceleration;
		const float jumpMaxHeight = (m_CharacterDesc.JumpSpeed * jumpMaxTime) - (0.5f * (m_FallAcceleration * powf(jumpMaxTime, 2)));
		ImGui::Text(std::format("Jump Height: {:0.1f} m", jumpMaxHeight).c_str());

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Move Speed (m/s)", &m_CharacterDesc.maxMoveSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Move Acceleration Time (s)", &m_CharacterDesc.moveAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_MoveAcceleration = m_CharacterDesc.maxMoveSpeed / m_CharacterDesc.moveAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Fall Speed (m/s)", &m_CharacterDesc.maxFallSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Fall Acceleration Time (s)", &m_CharacterDesc.fallAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_FallAcceleration = m_CharacterDesc.maxFallSpeed / m_CharacterDesc.fallAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		ImGui::DragFloat("Jump Speed", &m_CharacterDesc.JumpSpeed, 0.1f, 0.f, 0.f, "%.1f");
		ImGui::DragFloat("Rotation Speed (deg/s)", &m_CharacterDesc.rotationSpeed, 0.1f, 0.f, 0.f, "%.1f");

		bool isActive = m_pCameraComponent->IsActive();
		if(ImGui::Checkbox("Character Camera", &isActive))
		{
			m_pCameraComponent->SetActive(isActive);
		}
	}
}