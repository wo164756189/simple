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
			rapidjson::Value direction(crosses[i].orientation);
			car.AddMember("direction", direction, allocator);
			//car mesh (random)
			rapidjson::Value meshID(1);
			car.AddMember("mesh", meshID, allocator);
			//car speed
			rapidjson::Value speed(20.0f);
			car.AddMember("speed", speed, allocator);
		}
	}
};