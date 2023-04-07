#pragma once

class SpikyMaterial :public Material<SpikyMaterial>
{
public:
	SpikyMaterial();
	~SpikyMaterial() override = default;
	SpikyMaterial(const SpikyMaterial&) = delete;
	SpikyMaterial(SpikyMaterial&&) noexcept = delete;
	SpikyMaterial& operator=(const SpikyMaterial&) = delete;
	SpikyMaterial& operator=(SpikyMaterial&&) noexcept = delete;


protected:
	virtual void InitializeEffectVariables() override;
};

