#pragma once
#include <fstream>
#include <sstream>
#include <DirectXMath.h>
#include <string>
#include <vector>

class LocationWriter
{
public:
	LocationWriter() = default;
	LocationWriter(const std::string& filename)
		: m_Filename(filename)
	{
	}

	void WriteLocation(DirectX::XMFLOAT3 location)
	{
		std::ofstream file(m_Filename, std::ios::app);
		if (file)
		{
			file << "Location: " << location.x << "," << location.y << "," << location.z << "\n";
		}
	}

	void WriteRotation(DirectX::XMFLOAT3 rotation)
	{
		std::ofstream file(m_Filename, std::ios::app);
		if (file)
		{
			file << "Rotation: " << rotation.x << "," << rotation.y << "," << rotation.z << "\n";
		}
	}

	void SetFile(const std::string& file)
	{
		m_Filename = std::string(file);
	}

private:
	std::string m_Filename{};
};

class LocationReader
{
public:
	LocationReader() = default;

	LocationReader(const std::string& filename)
		: m_Filename(filename)
	{
	}

	std::vector<DirectX::XMFLOAT3> ReadLocations()
	{
		std::vector<DirectX::XMFLOAT3> locations;
		std::ifstream file(m_Filename);
		if (file)
		{
			std::string line;
			while (std::getline(file, line))
			{
				std::istringstream ss(line);
				std::string token;
				std::getline(ss, token, ':');

				if (token == "Location")
				{
					std::getline(ss, token, ',');
					float x = std::stof(token);
					std::getline(ss, token, ',');
					float y = std::stof(token);
					std::getline(ss, token, ',');
					float z = std::stof(token);

					locations.push_back(DirectX::XMFLOAT3(x, y, z));
				}
			}
		}
		return locations;
	}

	std::vector<DirectX::XMFLOAT3> ReadRotations()
	{
		std::vector<DirectX::XMFLOAT3> rotations;
		std::ifstream file(m_Filename);
		if (file)
		{
			std::string line;
			while (std::getline(file, line))
			{
				std::istringstream ss(line);
				std::string token;
				std::getline(ss, token, ':');

				if (token == "Rotation")
				{
					std::getline(ss, token, ',');
					float x = std::stof(token);
					std::getline(ss, token, ',');
					float y = std::stof(token);
					std::getline(ss, token, ',');
					float z = std::stof(token);

					rotations.push_back(DirectX::XMFLOAT3(x, y, z));
				}
			}
		}
		return rotations;
	}

private:
	std::string m_Filename{};
};
