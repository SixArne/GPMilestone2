#pragma once
class SkyBox : public GameObject
{
public:
	SkyBox() = default;
	~SkyBox() override = default;
	SkyBox(const SkyBox& other) = delete;
	SkyBox(SkyBox&& other) noexcept = delete;
	SkyBox& operator=(const SkyBox& other) = delete;
	SkyBox& operator=(SkyBox&& other) noexcept = delete;

protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;
};

