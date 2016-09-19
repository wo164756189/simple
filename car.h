#pragma once
#include <string>
#include <vector>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <iostream>
#include <time.h>
using std::string;
using std::vector;

class Car
{
public :
	int position[2];
	int direction[2];
	float speed;
	int meshID;
};

class Cross
{
public :
	int position[2];
	int orientation;
	string type;
};

class CarGenerator
{
public :
	CarGenerator()
	{
		srand(time(NULL));
	}
	vector<Cross> crosses;
	void GenerateCars(rapidjson::Value& carArray, rapidjson::Document::AllocatorType& allocator)
	{
		printf("cross size %d \n", crosses.size());
		for (int i = 0; i < crosses.size(); ++i)//crosses(random)
		{
			rapidjson::Value car(rapidjson::kObjectType);
			//position
			rapidjson::Value position(rapidjson::kObjectType);
			rapidjson::Value x(crosses[i].position[0]);
			rapidjson::Value z(crosses[i].position[1]);
			position.AddMember("x", x, allocator);
			position.AddMember("z", z, allocator);
			car.AddMember("position", position, allocator);
			//direction (random)
			{
				int randDir, randSize;
				if ("tcross" == crosses[i].type || "tcross_greenlight" == crosses[i].type)
					randSize = 3;
				else if ("lcross" == crosses[i].type || "lcross_greenlight" == crosses[i].type)
					randSize = 2;
				else if ("xcross" == crosses[i].type || "xcross_greenlight" == crosses[i].type)
					randSize = 4;

				randDir = randSize * rand() * 1.0f / RAND_MAX;
				if (randDir == randSize) randDir--;
				int dir = (crosses[i].orientation + randDir) % 4;
				if (!dir) dir = 4;

				rapidjson::Value direction(dir);
				car.AddMember("direction", direction, allocator);
			}
			//crossType
			rapidjson::Value s;
			s.SetString(rapidjson::StringRef(crosses[i].type.c_str()));
			car.AddMember("crossType", s, allocator);
			//car mesh (random)
			int carMeshId = 11 * rand() * 1.0f / RAND_MAX;
			if (carMeshId == 11) carMeshId--;
			rapidjson::Value meshID(carMeshId);
			car.AddMember("mesh", meshID, allocator);
			//car speed
			rapidjson::Value speed(1.0f);
			car.AddMember("speed", speed, allocator);

			carArray.PushBack(car, allocator);
		}
	}
};