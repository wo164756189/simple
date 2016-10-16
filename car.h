#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <AR/ar.h>
#include <AR/gsub.h>
#include <AR/video.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <sstream>
#include "connection.h"
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::stringstream;
using namespace std::chrono;

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

	void InitCrosses()//call only detection not available
	{
		Cross cross;
		cross.position[0] = 16;
		cross.position[1] = 8;
		cross.type = "xcross_greenlight";
		cross.orientation = 1;

		Cross cross1;
		cross1.position[0] = 24;
		cross1.position[1] = 8;
		cross1.type = "tcross_greenlight";
		cross1.orientation = 3;

		crosses.push_back(cross);
		crosses.push_back(cross1);
	}

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

//car
CarGenerator carGenerator;
milliseconds preMs, curMs;
milliseconds preMsFrame, curMsFrame;

//connection
Server carServer;

static void testLoop(void)
{
	curMsFrame = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	std::chrono::duration<double, std::milli> fp_msFrame = curMsFrame - preMsFrame;
	if (fp_msFrame < std::chrono::duration<double, std::milli>(33.3f))
		return;

	rapidjson::Document doc;
	doc.SetObject();

	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
	rapidjson::Value width(32);
	rapidjson::Value height(16);
	rapidjson::Value blockSize;
	blockSize.SetObject();
	blockSize.AddMember("width", width, allocator);
	blockSize.AddMember("height", height, allocator);
	doc.AddMember("blockSize", blockSize, allocator);

	rapidjson::Value mkArray(rapidjson::kArrayType);

	for (int i = 0; i < carGenerator.crosses.size(); ++i)
	{
		rapidjson::Value mk(rapidjson::kObjectType);
		rapidjson::Value span(rapidjson::kObjectType);
		rapidjson::Value spanX(1);
		rapidjson::Value spanY(1);
		rapidjson::Value x(carGenerator.crosses[i].position[0]);
		rapidjson::Value y(carGenerator.crosses[i].position[1]);
		rapidjson::Value type((int)(22));
		rapidjson::Value orientation((int)(carGenerator.crosses[i].orientation));

		mk.AddMember("x", x, allocator);
		mk.AddMember("y", y, allocator);
		mk.AddMember("type", "BLOCK", allocator);
		rapidjson::Value s;
		s.SetString(rapidjson::StringRef(carGenerator.crosses[i].type.c_str()));
		mk.AddMember("id", s, allocator);
		mk.AddMember("orientation", carGenerator.crosses[i].orientation, allocator);
		span.AddMember("x", spanX, allocator);
		span.AddMember("y", spanY, allocator);
		mk.AddMember("span", span, allocator);

		mkArray.PushBack(mk, allocator);
	}
	doc.AddMember("entry", mkArray, allocator);

	curMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	std::chrono::duration<double, std::milli> fp_ms = curMs - preMs;
	//cars
	bool flag = false;
	rapidjson::Value cars(rapidjson::kArrayType);
	if (fp_ms >= std::chrono::duration<double, std::milli>(2000.0f))
	{
		flag = !flag;
		printf("car gen\n");
		carGenerator.GenerateCars(cars, allocator);
		preMs = curMs;
	}
	doc.AddMember("cars", cars, allocator);

	//print json
	stringstream ss;
	ss.str("");
	rapidjson::StringBuffer strbuf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	doc.Accept(writer);

	const char* jstr = strbuf.GetString();
	ss << jstr;

	if (flag)
		printf("%s\n", ss.str().c_str());
	carServer.Send(ss.str());
	preMsFrame = curMsFrame;
}