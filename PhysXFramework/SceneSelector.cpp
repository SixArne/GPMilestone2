#include "stdafx.h"
#include "SceneSelector.h"
#include "SceneManager.h"
#include "W1_AssignmentScene.h"

//Change this define to activate/deactive the corresponding scenes
// W01 - W02  (#define ...)

//#define W01
#define W02

#ifdef W01
#include "TestScene.h"
#include "PhysicsTestScene.h"
#endif

#ifdef W02
#include "PhysicsTestScene.h"
#include "TriggerScene.h"
#include "AudioScene.h"
#include "W2_AssignmentScene.h"
#endif

namespace dae
{
	void AddScenes(SceneManager * pSceneManager)
	{

#ifdef W01
		pSceneManager->AddGameScene(new W1_AssignmentScene());
		//pSceneManager->AddGameScene(new TestScene());
#endif

#ifdef W02
		//pSceneManager->AddGameScene(new PhysicsTestScene());
		//pSceneManager->AddGameScene(new TriggerScene());
		//pSceneManager->AddGameScene(new AudioScene());
		pSceneManager->AddGameScene(new W2_AssignmentScene());
#endif

	}
}

