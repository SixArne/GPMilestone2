#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
	m_pMeshFilter{pMeshFilter}
{
	SetAnimation(0);
}

void ModelAnimator::Update(const SceneContext& sceneContext)
{
	TODO_W6_();

	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		//auto passedTicks = ...
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		auto passedTicks = sceneContext.pGameTime->GetElapsed() * m_CurrentClip.ticksPerSecond * m_AnimationSpeed;
		passedTicks = fmod(passedTicks, m_CurrentClip.duration);

		//2. 
		//IF m_Reversed is true
		//	Subtract passedTicks from m_TickCount
		//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
		//ELSE
		//	Add passedTicks to m_TickCount
		//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
		if (m_Reversed)
		{
			m_TickCount -= passedTicks;
			if (m_TickCount < 0.f)
			{
				m_TickCount += m_CurrentClip.duration;
			}
		}
		else
		{
			m_TickCount += passedTicks;
			if (m_TickCount > m_CurrentClip.duration)
			{
				m_TickCount -= m_CurrentClip.duration;
			}
		}

		//3.
		//Find the enclosing keys
		AnimationKey keyA{m_CurrentClip.keys.front()};
		AnimationKey keyB{m_CurrentClip.keys.front()};
		//Iterate all the keys of the clip and find the following keys:
		//keyA > Closest Key with Tick before/smaller than m_TickCount
		//keyB > Closest Key with Tick after/bigger than m_TickCount
		for (auto& key : m_CurrentClip.keys)
		{
			if (key.tick > m_TickCount)
			{
				keyB = key;
				break;
			}

			keyA = key;
		}


		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		auto blendFactor = (m_TickCount - keyA.tick) / (keyB.tick - keyA.tick);


		//Clear the m_Transforms vector
		m_Transforms.clear();
		//FOR every boneTransform in a key (So for every bone)
		//	Retrieve the transform from keyA (transformA)
		//	auto transformA = ...
		// 	Retrieve the transform from keyB (transformB)
		//	auto transformB = ...
		//	Decompose both transforms
		//	Lerp between all the transformations (Position, Scale, Rotation)
		//	Compose a transformation matrix with the lerp-results
		//	Add the resulting matrix to the m_Transforms vector
		auto boneCount = keyA.boneTransforms.size();
		for (auto bone{ 0 }; bone < boneCount; ++bone)
		{
			auto transformA = keyA.boneTransforms[bone];
			auto transformB = keyB.boneTransforms[bone];

			auto matrixA = XMLoadFloat4x4(&transformA);
			XMVECTOR vecScaleA{}, vecRotA{}, vecTransA{};
			XMMatrixDecompose(&vecScaleA, &vecRotA, &vecTransA, matrixA);

			auto matrixB = XMLoadFloat4x4(&transformB);
			XMVECTOR vecScaleB{}, vecRotB{}, vecTransB{};
			XMMatrixDecompose(&vecScaleB, &vecRotB, &vecTransB, matrixB);

			auto vecScaleAB = XMVectorLerp(vecScaleA, vecScaleB, blendFactor);
			auto vecRotAB = XMQuaternionSlerp(vecRotA, vecRotB, blendFactor);
			auto vecTransAB = XMVectorLerp(vecTransA, vecTransB, blendFactor);
			auto transformationMatrix = XMMatrixTransformation(
				FXMVECTOR{},
				FXMVECTOR{},
				vecScaleAB,
				FXMVECTOR{},
				vecRotAB,
				vecTransAB
			);

			XMFLOAT4X4 result{};
			XMStoreFloat4x4(&result, transformationMatrix);
			m_Transforms.push_back(result);
		}
	}
}

void ModelAnimator::SetAnimation(const std::wstring& clipName)
{
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	auto clipIt = std::find_if(
		m_pMeshFilter->m_AnimationClips.begin(),
		m_pMeshFilter->m_AnimationClips.end(), [clipName](auto clip) {
			return clip.name.compare(clipName) == 0;
	});
	//If found,
	//	Call SetAnimation(Animation Clip) with the found clip
	if (clipIt != m_pMeshFilter->m_AnimationClips.end())
	{
		SetAnimation(*clipIt);
		return;
	}

	Reset();
	Logger::LogWarning(L"Clipname {} not found", clipName);
	//Else
	//	Call Reset
	//	Log a warning with an appropriate message
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	
	if (clipNumber < m_pMeshFilter->m_AnimationClips.size())
	{
		SetAnimation(m_pMeshFilter->m_AnimationClips[clipNumber]);
		return;
	}

	Reset();
	Logger::LogWarning(L"Clipnumber {} not found", clipNumber);
	//If not,
		//	Call Reset
		//	Log a warning with an appropriate message
		//	return
	//else
		//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
		//	Call SetAnimation(AnimationClip clip)
}

void ModelAnimator::SetAnimation(const AnimationClip& clip)
{
	//Set m_ClipSet to true
	//Set m_CurrentClip
	m_ClipSet = true;
	m_CurrentClip = clip;
	
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	//If pause is true, set m_IsPlaying to false
	if (pause)
	{
		m_IsPlaying = false;
	}

	//Set m_TickCount to zero
	//Set m_AnimationSpeed to 1.0f
	m_TickCount = 0;
	m_AnimationSpeed = 1.0f;


	//If m_ClipSet is true
	//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
	//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
	if (m_ClipSet)
	{
		m_Transforms.assign(m_CurrentClip.keys[0].boneTransforms.begin(), m_CurrentClip.keys[0].boneTransforms.end());
	}
	//Else
	//	Create an IdentityMatrix 
	//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
	else
	{
		XMFLOAT4X4 identity{
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f
		};

		m_Transforms.assign(m_pMeshFilter->m_BoneCount, identity);
	}
}
