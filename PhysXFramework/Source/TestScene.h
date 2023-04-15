#pragma once
#include "GameScene.h"

class CubePosColorNorm;
//
//enum InputIDs
//{
//	MoveLeft,
//	MoveRight
//};

class TestScene final: public GameScene
{
public:
	TestScene():GameScene(L"TestScene"){}
	~TestScene() override = default;
	TestScene(const TestScene& other) = delete;
	TestScene(TestScene&& other) noexcept = delete;
	TestScene& operator=(const TestScene& other) = delete;
	TestScene& operator=(TestScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void OnSceneActivated() override;
	void OnSceneDeactivated() override;

private:
	CubePosColorNorm* m_pBox{};
	CubePosColorNorm* m_pBox2{};
	GameObject* m_pTorus{};
	GameObject* m_pCylinder{};
};

