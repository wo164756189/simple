#pragma once
#include <string>
#include <vector>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
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
	vector<Cross> crosses;
	void GenerateCars(rapidjson::Value& carArray, rapidjson::Document::AllocatorType& allocator)
	{
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
			int randRange;
			if (crosses[i].type == "lcross" || crosses[i].type == "lcross_greenlight")
				randRange = 2;
			else if (crosses[i].type == "tcross" || crosses[i].type == "tcross_greenlight")
				randRange = 3;
			else if (crosses[i].type == "xcross" || crosses[i].type == "xcross_greenlight")
				randRange = 4;

			int randDir = randRange * rand() * 1.0f / RAND_MAX;
			if (randRange == randDir) randDir = 0;

			int dir = (crosses[i].orientation + randDir) % 4;
			if (0 == dir) dir = 4;

			rapidjson::Value direction((crosses[i].orientation + randDir) % 4);
			car.AddMember("direction", direction, allocator);
			//crossType
			rapidjson::Value s;
			s.SetString(rapidjson::StringRef(crosses[i].type.c_str()));
			car.AddMember("crossType", s, allocator);
			//car mesh (random)
			int carMesh = 11 * rand() * 1.0f / RAND_MAX;
			if (11 == carMesh) carMesh--;
			rapidjson::Value meshID(carMesh);
			car.AddMember("mesh", meshID, allocator);
			//car speed
			rapidjson::Value speed(1.0f);
			car.AddMember("speed", speed, allocator);

			carArray.PushBack(car, allocator);
		}
	}
};