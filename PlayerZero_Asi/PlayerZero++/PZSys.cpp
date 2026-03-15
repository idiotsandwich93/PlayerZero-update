/*
   Player Zero FunctionScript
	--By Adopocalipt 2023--
*/

#include "PZSys.h"
#include "keyboard.h"
#include "GtaVMenu.h"

using namespace PZClass;
using namespace PZData;

#include <cstdlib>		//sRand - Random Number Generator
#include <ctime>		//time_t - Get the current date/time
#include <cmath>		//Math Functions Cos Tan etc...
#include <filesystem>	//Directory control Add/Remove Folders
#include <fstream>		//ofstream read/write text documents
#include <iostream>		//Header that defines the standard input/output stream objects:

#include <stdio.h>		//Makes Stream Pointers to files
#include <wchar.h>		//Support for w_Chars
#include <locale>		//streamng Support for w_Chars
#include <random>		//Randomize vector lists

namespace PZSys
{
	Hash MyHashKey(const std::string& name)
	{
		return GAMEPLAY::GET_HASH_KEY((LPSTR)name.c_str());
	}

	int InGameTime()
	{
		return (int)GAMEPLAY::GET_GAME_TIMER();
	}

	std::string ConvertWideToANSI(const std::wstring& wstr)
	{
		int Count = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
		std::string Str(Count, 0);
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &Str[0], Count, NULL, NULL);
		return Str;
	}
	std::string ConvertWideToUtf8(const std::wstring& wstr)
	{
		int Count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
		std::string Str(Count, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &Str[0], Count, NULL, NULL);
		return Str;
	}

	std::string GetExeFileName()
	{
		WCHAR Buffer[MAX_PATH]{};
		GetModuleFileNameW(NULL, Buffer, ARRAYSIZE(Buffer));

		return ConvertWideToUtf8(std::wstring(Buffer));
	}
	std::string GetDir()
	{
		std::string File = GetExeFileName();
		std::string Dir = File.substr(0, File.find_last_of("\\/"));

		return Dir;
	}
	std::string TimeDate()
	{
		time_t CurTime;
		CurTime = time(NULL);
		std::string TimeStr = ctime(&CurTime);
		TimeStr[TimeStr.length() - 1] = char(32);
		return TimeStr;
	}
	
	void BuildMissingDirectory(std::string dir)
	{
		if (CreateDirectoryA((LPSTR)dir.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
		{		}
	}
	void DirectoryTest()
	{
		BuildMissingDirectory(DirectMain);
		BuildMissingDirectory(DirectRandNum);
		BuildMissingDirectory(DirectContacts);
		BuildMissingDirectory(DirectVehicles);
		BuildMissingDirectory(DirectMobileNet);
		BuildMissingDirectory(DirectTranslate);
		BuildMissingDirectory(DirectOutfitFolder);
		BuildMissingDirectory(DirectOutfitMale);
		BuildMissingDirectory(DirectOutfitFemale);
	}
	bool FileExists(const std::string& filename)
	{
		try
		{
			std::ifstream Infield(filename);
			return Infield.good();
		}
		catch (...)
		{
			return false;
		}
	}
	bool FileRemoval(const std::string& filename)
	{
		bool Removed = false;
		try 
		{
			if (std::filesystem::remove(filename))
				Removed = true;
		}
		catch (const std::filesystem::filesystem_error& err)
		{
			Removed = false;
		}
		return Removed;
	}
	void AmendFile(const std::string& file, const std::vector<std::string>& text)
	{
		std::ofstream FileSt(file, std::ios_base::app | std::ios_base::out);
		for (int i = 0; i < text.size(); i++)
		{
			FileSt << text[i];
			FileSt << "\n";
		}
		FileSt.close();
	}
	void WriteFile(const std::string& file, const std::vector<std::string>& text)
	{
		std::ofstream FileSt(file);
		for (int i = 0; i < text.size(); i++)
		{
			FileSt << text[i];
			FileSt << "\n";
		}
		FileSt.close();
	}
	std::vector<std::string> ReadDirectory(const std::string& dir)
	{
		std::vector<std::string> Files = {};

		for (const auto& entry : std::filesystem::directory_iterator(dir))
			Files.push_back(entry.path().string());

		return Files;
	}
	std::vector<std::string> ReadFile(const std::string& fileName)
	{
		std::string Text;
		std::vector<std::string> TextList = {};

		if (FileExists(fileName))
		{
			std::ifstream MyReadFile(fileName);

			while (getline(MyReadFile, Text))
			{
				TextList.push_back(Text);
			}
			MyReadFile.close();
		}
		return TextList;
	}
	
	bool BlogStart = true;
	void LoggerLight(const std::string& text)
	{
		if (PZData::MySettings.Debugger)
		{
			std::string Message = TimeDate() + text;
			if (BlogStart)
			{
				WriteFile(LogFile, { Message });
				BlogStart = false;
			}
			else
				AmendFile(LogFile, { Message });
		}
	}

	bool StringContains(const std::string& line, const std::string& wholeString)
	{
		bool Contain = false;
		int Line = 0;
		std::string Word = "";
		for (int i = 0; i < wholeString.length(); i++)
		{
			if (line[Line] == wholeString[i] && Line < line.length())
			{
				Contain = true;
				Word += wholeString[i];
				Line++;
			}
			else if (Line > 0)
			{
				if (line == Word)
					break;
				else
				{
					Line = 0;
					Word = "";

					if (line[0] == wholeString[i])
					{
						Contain = true;
						Word += wholeString[i];
						Line++;
					}
				}
			}
		}

		if (Contain)
		{
			if (line != Word)
				Contain = false;
		}

		return Contain;
	}
	bool StringContains(char line, const std::string& wholeString)
	{
		bool Contain = false;
		for (int i = 0; i < wholeString.length(); i++)
		{
			if (line == wholeString[i])
			{
				Contain = true;
				break;
			}
		}
		return Contain;
	}
	
	int FindCharicter(const std::string& chars, const std::string& wholeString)
	{
		int Line = -1;
		for (int i = 0; i < wholeString.length(); i++)
		{
			if (chars[0] == wholeString[i])
			{
				Line = i;
				break;
			}
		}
		return Line;
	}
	int FindCharicter(char chars, const std::string& wholeString)
	{
		int Line = -1;
		for (int i = 0; i < wholeString.length(); i++)
		{
			if (chars == wholeString[i])
			{
				Line = i;
				break;
			}
		}
		return Line;
	}
	
	int StingNumbersInt(const std::string& line)
	{
		int Num = 0;
		int Times = 0;
		bool Negative = false;
		for (int i = (int)line.size(); i > -1; i--)
		{
			bool Skip = false;
			int Add = 0;
			char Comp = line[i];

			if (Comp == char(45))
				Negative = true;
			else if (Comp == char(49))
			{
				Negative = false;
				Add = 1;
			}
			else if (Comp == char(50))
			{
				Negative = false;
				Add = 2;
			}
			else if (Comp == char(51))
			{
				Negative = false;
				Add = 3;
			}
			else if (Comp == char(52))
			{
				Negative = false;
				Add = 4;
			}
			else if (Comp == char(53))
			{
				Negative = false;
				Add = 5;
			}
			else if (Comp == char(54))
			{
				Negative = false;
				Add = 6;
			}
			else if (Comp == char(55))
			{
				Negative = false;
				Add = 7;
			}
			else if (Comp == char(56))
			{
				Negative = false;
				Add = 8;
			}
			else if (Comp == char(57))
			{
				Negative = false;
				Add = 9;
			}
			else if (Comp == char(48))
			{

			}
			else
				Skip = true;

			if (!Skip)
			{
				if (Times == 0)
				{
					Num = Add;
					Times = 1;
				}
				else
					Num += Add * Times;
				Times *= 10;
			}
		}

		if (Negative)
			Num = Num * (-1);

		return Num;
	}
	float StingNumbersFloat(const std::string& line)
	{
		float Num = 0;
		std::string DecOp = ".";
		int EndIndex = -1;
		for (int i = 0; i < line.length(); i++)
		{
			if (line[i] == DecOp[0])
			{
				EndIndex = i;
				break;
			}
		}

		if (EndIndex >= 0 && EndIndex <= line.length())
		{
			std::string AfterDec = line.substr(EndIndex + 1);
			std::string BigNum = line.substr(0, EndIndex) + AfterDec;
			Num = (float)StingNumbersInt(BigNum);

			for (int i = 0; i < AfterDec.length(); i++)
				Num = Num / 10;
		}
		else
			Num = (float)StingNumbersInt(line);

		return Num;
	}
	bool StringBool(const std::string& line)
	{
		if (StringContains("True", line))
			return true;
		else if (StringContains("true", line))
			return true;
		else if (StringContains("TRUE", line))
			return true;
		else
			return false;
	}

	std::string AfterEqual(const std::string& tag)
	{
		int Start = 0;
		std::string Output;
		for (int i = 0; i < tag.length(); i++)
		{
			if (tag[i] == char(61))
			{
				Start = i + 1;
				break;
			}
		}
		for (int i = Start; i < tag.length(); i++)
			Output += tag[i];

		return Output;
	}
	std::string BoolToString(bool b)
	{
		if (b)
			return "true";
		else
			return "false";
	}
	float TwoDecimal(const std::string& number)
	{
		int Size = (int)number.length();
		std::string Output = "";
		for (int i = 0; i < Size; i++)
		{
			Output += number[i];
			if (number[i] == char(46) && i + 3 < number.length())
				Size = i + 3;
		}
		return StingNumbersFloat(Output);
	}

	void RandomizeIntList(std::vector<int>* numbers)
	{
		std::random_device Randv;
		std::mt19937 Output(Randv());

		std::shuffle(numbers->begin(), numbers->end(), Output);
	}
	float RandomFloat(float min, float max)
	{
		if (min < max)
		{
			std::random_device RandoNum;
			std::mt19937 Mt(RandoNum());
			std::uniform_real_distribution<double> Output((double)min, (double)max);

			return (float)Output(Mt);
		}
		else
			return min;
	}
	int RandomInt(int min, int max)
	{
		if (min < max)
		{
			std::random_device RandoNum;
			std::mt19937 Mt(RandoNum());
			std::uniform_real_distribution<double> Output((double)min, (double)max);

			return (int)Output(Mt);
		}
		else
			return min;
	}
	void NewNums(std::vector<int>* numbers, int min, int max)
	{
		numbers->clear();
		for (int i = min; i < max + 1; i++)
			numbers->push_back(i);
		RandomizeIntList(numbers);
	}
	int LessRandomInt(const std::string& file, int min, int max)
	{
		if (min < max)
		{
			int Place = 0;
			int Num = min;
			std::string FileName = DirectRandNum + "/" + file + ".txt";
			std::vector<std::string> MyNums = ReadFile(FileName);
			std::vector<int> NumList = {};

			if (MyNums.size() < 2)
				NewNums(&NumList, min, max);
			else
			{
				bool NewWrite = false;
				if (StringContains("min=", MyNums[0]))
				{
					if (StingNumbersInt(MyNums[0]) != min)
						NewWrite = true;
				}
				else
					NewWrite = true;

				if (StringContains("max=", MyNums[1]))
				{
					if (StingNumbersInt(MyNums[1]) != max)
						NewWrite = true;
				}
				else
					NewWrite = true;

				for (int i = 2; i < MyNums.size(); i++)
					NumList.push_back(StingNumbersInt(MyNums[i]));

				if (NumList.size() == 0 || NewWrite)
					NewNums(&NumList, min, max);
			}

			Place = RandomInt(0, (int)NumList.size() - 1);
			Num = NumList[Place];
			NumList.erase(NumList.begin() + Place);

			std::vector<std::string> Output = { "min=" + std::to_string(min) , "max=" + std::to_string(max) };

			for (int i = 0; i < NumList.size(); i++)
				Output.push_back(std::to_string(NumList[i]));

			WriteFile(FileName, Output);

			LoggerLight("LessRandomInt, id == " + file + ", min == " + std::to_string(min) + ", max == " + std::to_string(max) + "Num == " + std::to_string(Num));

			return Num;
		}
		else
			return min;
	}

	float GetAngle(Vector3 postion1, Vector3 postion2)
	{
		double Ang = (postion1.x * postion2.x) + (postion1.y * postion2.y);
		double Output = 0;
		if (Ang != 0)
		{
			double Ang1 = sqrt((postion1.x * 2) + (postion1.y * 2)) / Ang;
			double Ang2 = sqrt((postion2.x * 2) + (postion2.y * 2)) / Ang;
			double Output = cos(Ang1 + Ang2 / Ang);
		}
		return (float)Output;
	}

	float DistanceTo(Vector3 postion1, Vector3 postion2)
	{
		double Num = postion2.x - postion1.x;
		double Num2 = postion2.y - postion1.y;
		double Num3 = postion2.z - postion1.z;
		return (float)sqrt(Num * Num + Num2 * Num2 + Num3 * Num3);
	}
	float DistanceTo(Entity entity1, Entity entity2)
	{
		Vector3 Postion1 = ENTITY::GET_ENTITY_COORDS(entity1, false);
		Vector3 Postion2 = ENTITY::GET_ENTITY_COORDS(entity2, false);

		return DistanceTo(Postion1, Postion2);
	}
	float DistanceTo(Entity entity1, Vector3 postion2)
	{
		Vector3 Postion1 = ENTITY::GET_ENTITY_COORDS(entity1, false);

		return DistanceTo(Postion1, postion2);
	}
	float DistanceTo(Vector4 postion1, Vector3 postion2)
	{
		double Num = postion2.x - postion1.X;
		double Num2 = postion2.y - postion1.Y;
		double Num3 = postion2.z - postion1.Z;
		return (float)sqrt(Num * Num + Num2 * Num2 + Num3 * Num3);
	}
	float DistanceTo(Vector4 postion1, Vector4 postion2)
	{
		double Num = postion2.X - postion1.X;
		double Num2 = postion2.Y - postion1.Y;
		double Num3 = postion2.Z - postion1.Z;
		return (float)sqrt(Num * Num + Num2 * Num2 + Num3 * Num3);
	}

	Vector3 NewVector3(float X, float Y, float Z)
	{
		Vector3 NewVec = Vector3();
		NewVec.x = X;
		NewVec.y = Y;
		NewVec.z = Z;

		return NewVec;
	}
	Vector3 NewVector3(Vector4 pos4)
	{
		Vector3 NewVec = Vector3();
		NewVec.x = pos4.X;
		NewVec.y = pos4.Y;
		NewVec.z = pos4.Z;

		return NewVec;
	}
	Vector3 EntityPosition(Entity ent)
	{
		return  ENTITY::GET_ENTITY_COORDS(ent, 0);
	}
	Vector3 PlayerPosi()
	{
		return EntityPosition(PLAYER::PLAYER_PED_ID());
	}
	Vector3 PlayerPosi(float above)
	{
		Vector3 HighAb = PlayerPosi();
		return NewVector3(HighAb.x, HighAb.y, HighAb.z + above);
	}
	Vector3 MyWayPoint()
	{
		return UI::GET_BLIP_INFO_ID_COORD(UI::GET_FIRST_BLIP_INFO_ID(8));
	}
	Vector3 RightOfEntity(Entity ent)
	{
		Vector3 Pos = ENTITY::GET_ENTITY_ROTATION(ent, 00);
		const double PI = 3.14259;
		double Num = cos((double)Pos.y * (PI / 180.0));
		double Num2 = cos((double)(0 - Pos.z) * (PI / 180.0)) * Num;
		double Num3 = sin((double)Pos.z * (PI / 180.0)) * Num;
		double Num4 = sin((double)(0 - Pos.y) * (PI / 180.0));

		Pos.x = (float)Num2;
		Pos.y = (float)Num3;
		Pos.z = (float)Num4;

		return Pos;
	}
	Vector3 FowardOf(Entity ent, float multi, bool inFront)
	{
		Vector3 vec = ENTITY::GET_ENTITY_COORDS(ent, true);
		Vector3 FV = ENTITY::GET_ENTITY_FORWARD_VECTOR(ent);
		FV.x *= multi;
		FV.y *= multi;
		FV.z *= multi;

		if (inFront)
		{
			vec.x += FV.x;
			vec.y += FV.y;
			vec.z += FV.z;
		}
		else
		{
			vec.x -= FV.x;
			vec.y -= FV.y;
			vec.z -= FV.z;
		}

		return vec;
	}
	Vector3 RightOf(Entity ent, float multi, bool inFront)
	{
		Vector3 vec = ENTITY::GET_ENTITY_COORDS(ent, true);
		Vector3 FV = RightOfEntity(ent);
		FV.x *= multi;
		FV.y *= multi;
		FV.z *= multi;

		if (inFront)
		{
			vec.x += FV.x;
			vec.y += FV.y;
			vec.z += FV.z;
		}
		else
		{
			vec.x -= FV.x;
			vec.y -= FV.y;
			vec.z -= FV.z;
		}

		return vec;
	}

	Vector4 EntityPositionV4(Entity ent)
	{
		Vector3 Pos = EntityPosition(ent);
		return Vector4(Pos.x, Pos.y, Pos.z, ENTITY::GET_ENTITY_HEADING(ent));
	}
	
	float InAreaOf_fDir = 0.0f;
	Vector4 InAreaOf(Vector4 area, float minDist, float maxDist)
	{
		float X = RandomFloat(maxDist * -1, maxDist);
		float Y = RandomFloat(maxDist * -1, maxDist);

		if (X > -1.0 && X < minDist)
			X = minDist;
		else if (X < 1.0 && X > minDist * -1)
			X = minDist * -1;

		if (Y > -1.0 && Y < minDist)
			Y = minDist;
		else if (Y < 1.0 && Y > minDist * -1)
			Y = minDist * -1;

		float Ax = area.X + X;
		float Ay = area.Y + Y;
		float Az = area.Z;
		float Ar = area.R;

		return Vector4(Ax, Ay, Az, Ar);
	}
	Vector4 InAreaOf(Vector3 area, float minDist, float maxDist)
	{
		InAreaOf_fDir += (float)LessRandomInt("AngleSaxon", 20, 65);
		if (InAreaOf_fDir > 360.0f)
			InAreaOf_fDir = (float)LessRandomInt("AngleSaxon", 20, 65);

		return InAreaOf(Vector4(area.x, area.y, area.z, InAreaOf_fDir), minDist, maxDist);
	}
	Vector3 InAreaOfV3(Vector3 area, float minDist, float maxDist)
	{
		Vector4 VA = InAreaOf(Vector4(area.x, area.y, area.z, InAreaOf_fDir), minDist, maxDist);

		return NewVector3(VA);
	}

	const std::vector<Vector4> SanLoocIndex = {
		Vector4(-812.1608f, -2201.21f, 16.7955f, 0.0f),
		Vector4(211.7755f, -3045.787f, 5.089411f, 0.0f),
		Vector4(990.5366f, -3114.517f, 5.426713f, 0.0f),
		Vector4(-1230.986f, -1088.824f, 7.947563f, 0.0f),
		Vector4(592.1391f, -1825.843f, 24.36324f, 0.0f),
		Vector4(17.29186f, -805.5605f, 47.16209f, 0.0f),
		Vector4(2042.241f, -2126.156f, 116.9319f, 0.0f),
		Vector4(1602.962f, -550.4477f, 168.798f, 0.0f),
		Vector4(-747.7878f, 200.8658f, 75.26504f, 0.0f),
		Vector4(615.4108f, 266.308f, 102.7443f, 0.0f),
		Vector4(-2812.112f, 637.9633f, 104.826f, 0.0f),
		Vector4(-402.1792f, 2294.639f, 229.0287f, 0.0f),
		Vector4(2260.562f, 1439.681f, 138.9774f, 0.0f),
		Vector4(1622.083f, 4018.104f, 37.64009f, 0.0f),
		Vector4(-1027.057f, 5541.436f, 20.38375f, 0.0f),
		Vector4(1272.17f, 6244.464f, 115.8106f, 0.0f),
	};
	static std::vector<Vector3> LastDropVeh = {};
	static std::vector<Vector3> LastDropPed = {};
	bool ItsTooNear(const std::vector<Vector3>& checkList, Vector4 spot, float minDist)
	{
		LoggerLight("ItsTooNear");
		bool ItsNear = true;
		if ((int)checkList.size() > 0)
		{
			int NotNear = 0;
			for (int i = 0; i < (int)checkList.size(); i++)
			{
				if (DistanceTo(spot, checkList[i]) < minDist)
					break;
				else
					NotNear++;
			}

			if (NotNear == (int)checkList.size())
				ItsNear = false;
		}
		else
			ItsNear = false;

		return ItsNear;
	}
	void NearestToo(Vector4* pos, const std::vector<Vector4>& placeList, const std::vector<Vector3>& checkList, float minDist)
	{
		LoggerLight("NearestToo");

		if (pos != nullptr)
		{
			float FAr = 9000.0f;
			int Near = 0;
			for (int i = 0; i < (int)placeList.size(); i++)
			{
				if (DistanceTo(*pos, placeList[i]) < FAr && !ItsTooNear(checkList, placeList[i], minDist))
				{
					FAr = DistanceTo(*pos, placeList[i]);
					Near = i;
				}
			}

			if (Near < (int)placeList.size())
				*pos = placeList[Near];
		}
		else
			LoggerLight("NearestToo pos == nullptr");
	}	
	Vector4 VehPlace(Vector3 local)
	{
		LoggerLight("VehPlace");

		Vector4 Location = Vector4(local);
		int List = 0;
		float Dis = 4000.0f;
		for (int i = 0; i < SanLoocIndex.size(); i++)
		{
			if (DistanceTo(SanLoocIndex[i], local) < Dis)
			{
				Dis = DistanceTo(SanLoocIndex[i], local);
				List = i;
			}
		}
		
		if (List == 0)
			NearestToo(&Location, VehDrops01, LastDropVeh, 20.0f);
		else if (List == 1)
			NearestToo(&Location, VehDrops02, LastDropVeh, 20.0f);
		else if (List == 2)
			NearestToo(&Location, VehDrops03, LastDropVeh, 20.0f);
		else if (List == 3)
			NearestToo(&Location, VehDrops04, LastDropVeh, 20.0f);
		else if (List == 4)
			NearestToo(&Location, VehDrops05, LastDropVeh, 20.0f);
		else if (List == 5)
			NearestToo(&Location, VehDrops06, LastDropVeh, 20.0f);
		else if (List == 6)
			NearestToo(&Location, VehDrops07, LastDropVeh, 20.0f);
		else if (List == 7)
			NearestToo(&Location, VehDrops08, LastDropVeh, 20.0f);
		else if (List == 8)
			NearestToo(&Location, VehDrops09, LastDropVeh, 20.0f);
		else if (List == 9)
			NearestToo(&Location, VehDrops10, LastDropVeh, 20.0f);
		else if (List == 10)
			NearestToo(&Location, VehDrops11, LastDropVeh, 20.0f);
		else if (List == 11)
			NearestToo(&Location, VehDrops12, LastDropVeh, 20.0f);
		else if (List == 12)
			NearestToo(&Location, VehDrops13, LastDropVeh, 20.0f);
		else if (List == 13)
			NearestToo(&Location, VehDrops14, LastDropVeh, 20.0f);
		else if (List == 14)
			NearestToo(&Location, VehDrops15, LastDropVeh, 20.0f);
		else
			NearestToo(&Location, VehDrops16, LastDropVeh, 20.0f);

		return Location;
	}
	Vector4 PedPlace(Vector3 local)
	{
		LoggerLight("PedPlace");

		Vector4 Location = Vector4(local);
		int List = 0;
		float Dis = 4000.0f;
		for (int i = 0; i < SanLoocIndex.size(); i++)
		{
			if (DistanceTo(SanLoocIndex[i], local) < Dis)
			{
				Dis = DistanceTo(SanLoocIndex[i], local);
				List = i;
			}
		}
		
		if (List == 0)
			NearestToo(&Location, PedDrops01, LastDropPed, 10.0f);
		else if (List == 1)
			NearestToo(&Location, PedDrops02, LastDropPed, 10.0f);
		else if (List == 2)
			NearestToo(&Location, PedDrops03, LastDropPed, 10.0f);
		else if (List == 3)
			NearestToo(&Location, PedDrops04, LastDropPed, 10.0f);
		else if (List == 4)
			NearestToo(&Location, PedDrops05, LastDropPed, 10.0f);
		else if (List == 5)
			NearestToo(&Location, PedDrops06, LastDropPed, 10.0f);
		else if (List == 6)
			NearestToo(&Location, PedDrops07, LastDropPed, 10.0f);
		else if (List == 7)
			NearestToo(&Location, PedDrops08, LastDropPed, 10.0f);
		else if (List == 8)
			NearestToo(&Location, PedDrops09, LastDropPed, 10.0f);
		else if (List == 9)
			NearestToo(&Location, PedDrops10, LastDropPed, 10.0f);
		else if (List == 10)
			NearestToo(&Location, PedDrops11, LastDropPed, 10.0f);
		else if (List == 11)
			NearestToo(&Location, PedDrops12, LastDropPed, 10.0f);
		else if (List == 12)
			NearestToo(&Location, PedDrops13, LastDropPed, 10.0f);
		else if (List == 13)
			NearestToo(&Location, PedDrops14, LastDropPed, 10.0f);
		else if (List == 14)
			NearestToo(&Location, PedDrops15, LastDropPed, 10.0f);
		else
			NearestToo(&Location, PedDrops16, LastDropPed, 10.0f);

		return Location;
	}
	Vector4 FindPedSpPoint()
	{
		Vector4 Pos(0.0f, 0.0f, 0.0f, 0.0f);

		if (FileExists(ZeroYank))
		{
			Pos = InAreaOf(PlayerPosi(), 30.0f, 60.0f);
			NearestToo(&Pos, YankSpPoint, LastDropPed, 10.0f);
		}
		else if (FileExists(ZeroCayo))
		{
			Pos = InAreaOf(PlayerPosi(), 30.0f, 60.0f);
			NearestToo(&Pos, CayoSpPoint, LastDropPed, 10.0f);
		}
		else
		{
			// Cap near-player spawns at 5 on-foot peds within 80m.
			// This keeps the local area feeling alive without overwhelming the player,
			// and scales automatically regardless of the MaxPlayers setting.
			int nearCount = 0;
			Vector3 pPos = PlayerPosi();
			for (int i = 0; i < (int)PedList.size(); i++)
			{
				if (!PedList[i].Driver && !PedList[i].Passenger &&
					DistanceTo(PedList[i].ThisPed, pPos) < 80.0f)
				{
					nearCount++;
				}
			}
			if (nearCount < 5)
			{
				Pos = InAreaOf(PlayerPosi(), 30.0f, 60.0f);
			}
			else
			{
				int regionIdx = RandomInt(0, (int)SanLoocIndex.size() - 1);
				Vector3 RegionCenter = NewVector3(SanLoocIndex[regionIdx].X, SanLoocIndex[regionIdx].Y, SanLoocIndex[regionIdx].Z);
				Pos = PedPlace(RegionCenter);
			}
		}

		// Snap to a valid pedestrian-safe coord.
		// Flag 16 tells the pathfinder to prefer a sidewalk / pavement position,
		// keeping spawned peds off the centre of the road.
		Vector3 safeOut; safeOut.x = Pos.X; safeOut.y = Pos.Y; safeOut.z = Pos.Z;
		if (PATHFIND::GET_SAFE_COORD_FOR_PED(Pos.X, Pos.Y, Pos.Z, true, &safeOut, 16))
		{
			Pos.X = safeOut.x;
			Pos.Y = safeOut.y;
			Pos.Z = safeOut.z;
		}

		LastDropPed.push_back(NewVector3(Pos.X, Pos.Y, Pos.Z));
		if (LastDropPed.size() > 15)
			LastDropPed.erase(LastDropPed.begin());

		return Pos;
	}
	Vector4 FindVehSpPoint(int prefVeh)
	{
		Vector4 Pos = InAreaOf(PlayerPosi(), 45.0f, 90.0f);
		Vector3 PlayPos = NewVector3(Pos.X, Pos.Y, Pos.Z);

		if (prefVeh == 5 || prefVeh == 3 || prefVeh == 8 || prefVeh == 9)
		{
			PlayPos.z += 1200.0f;
			Pos = InAreaOf(PlayPos, 155.0f, 1400.0f);
			while (ItsTooNear(LastDropVeh, Pos, 50.0f))
			{
				Pos = InAreaOf(PlayPos, 155.0f, 1400.0f);
				WAIT(10);
			}
		}
		else if (prefVeh == 2 || prefVeh == 4)
		{
			PlayPos.z += 600.0f;
			Pos = InAreaOf(PlayPos, 155.0f, 500.0f);
			while (ItsTooNear(LastDropVeh, Pos, 30.0f))
			{
				Pos = InAreaOf(PlayPos, 155.0f, 1400.0f);
				WAIT(10);
			}
		}
		else if (FileExists(ZeroYank))
			NearestToo(&Pos, YankVhPoint, LastDropVeh, 20.0f);
		else if (FileExists(ZeroCayo))
			NearestToo(&Pos, CayoVhPoint, LastDropVeh, 20.0f);
		else
			Pos = VehPlace(PlayPos);

		LastDropVeh.push_back(NewVector3(Pos.X, Pos.Y, Pos.Z));
		if (LastDropVeh.size() > 15)
			LastDropVeh.erase(LastDropVeh.begin());

		return Pos;
	}

	Vector3 RandomLocation()
	{
		if (FileExists(ZeroYank))
			return NewVector3(YankVhPoint[LessRandomInt("YankLocRand", 0, (int)YankVhPoint.size() -1)]);
		else if (FileExists(ZeroCayo))
			return NewVector3(CayoVhPoint[LessRandomInt("CayoLocRand", 0, (int)CayoVhPoint.size() - 1)]);
		else
		{
			Vector3 V3P = PlayerPosi();
			V3P.x *= -1;
			V3P.y *= -1;
			Vector4 VP = VehPlace(V3P);
			return NewVector3(VP);
		}
	}
	
	Vehicle GetPedsVehicle(Ped peddy)
	{
		Vehicle Pv = NULL;
		if ((bool)PED::IS_PED_IN_ANY_VEHICLE(peddy, 0))
		{
			Pv = PED::GET_VEHICLE_PED_IS_IN(peddy, false);
		}
		return Pv;
	}
	Vehicle GetPlayersVehicle()
	{
		return GetPedsVehicle(PLAYER::PLAYER_PED_ID());
	}

	void GhostTown()
	{
		Vehicle PlayersVeh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false);

		const int VEH_ARR_SIZE = 1024;
		Vehicle VehArray[VEH_ARR_SIZE];
		int Vehcount = worldGetAllVehicles(VehArray, VEH_ARR_SIZE);

		for (int i = 0; i < Vehcount; i++)
		{
			Vehicle Vic = VehArray[i];
			if (Vic != NULL && ENTITY::DOES_ENTITY_EXIST(Vic))
			{
				if (ENTITY::IS_ENTITY_A_MISSION_ENTITY(Vic) || Vic == PlayersVeh)
				{

				}
				else
				{
					ENTITY::SET_ENTITY_AS_MISSION_ENTITY(Vic, true, true);
					VEHICLE::DELETE_VEHICLE(&Vic);
				}
			}
		}

		const int PED_ARR_SIZE = 1024;
		Ped PedArray[PED_ARR_SIZE];
		int Pedcount = worldGetAllPeds(PedArray, PED_ARR_SIZE);

		for (int i = 0; i < Pedcount; i++)
		{
			Ped Peddy = PedArray[i];
			if (Peddy != NULL && ENTITY::DOES_ENTITY_EXIST(Peddy))
			{
				if (ENTITY::IS_ENTITY_A_MISSION_ENTITY(Peddy))
				{

				}
				else
				{
					ENTITY::SET_ENTITY_AS_MISSION_ENTITY(Peddy, true, true);
					PED::DELETE_PED(&Peddy);
				}
			}
		}
	}

	void AddMarker(Entity ent)
	{
		Vector4 Entpos = EntityPositionV4(ent);
		GRAPHICS::DRAW_MARKER(2, Entpos.X, Entpos.Y, Entpos.Z + 1.0f, 0.0f, 0.0f, 0.0f, 180.0f, 0.0f, Entpos.R, 0.2f, 0.2f, 0.2f, 236, 240, 41, 255, false, true, 2, true, 0, 0, false);
		/*
		void DRAW_MARKER(int type, float posX, float posY, float posZ, float dirX, float dirY, float dirZ, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ, int red, int green, int blue, int alpha, BOOL bobUpAndDown, BOOL faceCamera, int p19, BOOL rotate, char* textureDict, char* textureName, BOOL drawOnEnts) // 28477EC23D892089 48D84A02
			enum MarkerTypes
		{
			MarkerTypeUpsideDownCone = 0,
			MarkerTypeVerticalCylinder = 1,
			MarkerTypeThickChevronUp = 2,
			MarkerTypeThinChevronUp = 3,
			MarkerTypeCheckeredFlagRect = 4,
			MarkerTypeCheckeredFlagCircle = 5,
			MarkerTypeVerticleCircle = 6,
			MarkerTypePlaneModel = 7,
			MarkerTypeLostMCDark = 8,
			MarkerTypeLostMCLight = 9,
			MarkerTypeNumber0 = 10,
			MarkerTypeNumber1 = 11,
			MarkerTypeNumber2 = 12,
			MarkerTypeNumber3 = 13,
			MarkerTypeNumber4 = 14,
			MarkerTypeNumber5 = 15,
			MarkerTypeNumber6 = 16,
			MarkerTypeNumber7 = 17,
			MarkerTypeNumber8 = 18,
			MarkerTypeNumber9 = 19,
			MarkerTypeChevronUpx1 = 20,
			MarkerTypeChevronUpx2 = 21,
			MarkerTypeChevronUpx3 = 22,
			MarkerTypeHorizontalCircleFat = 23,
			MarkerTypeReplayIcon = 24,
			MarkerTypeHorizontalCircleSkinny = 25,
			MarkerTypeHorizontalCircleSkinny_Arrow = 26,
			MarkerTypeHorizontalSplitArrowCircle = 27,
			MarkerTypeDebugSphere = 28,
			MarkerTypeDallorSign = 29,
			MarkerTypeHorizontalBars = 30,
			MarkerTypeWolfHead = 31,
			MarkerTypeQuestionMark = 32,
			MarkerTypePlaneUpwards = 33,
			MarkerTypeHelicopter = 34,
			MarkerTypeBoat = 35,
			MarkerTypeCar = 36,
			MarkerTypeMotorcycle = 37,
			MarkerTypeBicycle = 38,
			MarkerTypeTruck = 39,
			MarkerTypeParachute = 40,
			MarkerTypeJetpack = 41,
			MarkerTypeTwirlingStar = 42
		};

		dirX / Y / Z represent a heading on each axis in which the marker should face, alternatively you can rotate each axis independently with rotX / Y / Z(and set dirX / Y / Z all to 0).

			faceCamera - Rotates only the y - axis(the heading) towards the camera

			p19 - no effect, default value in script is 2

			rotate - Rotates only on the y - axis(the heading)

			textureDict - Name of texture dictionary to load texture from(e.g. "GolfPutting")

			textureName - Name of texture inside dictionary to load(e.g. "PuttingMarker")

			drawOnEnts - Draws the marker onto any entities that intersect it

			basically what he said, except textureDict and textureName are totally not char*, or if so, then they are always set to 0 / NULL / nullptr in every script I checked, eg:

		bj.c : graphics::draw_marker(6, vParam0, 0f, 0f, 1f, 0f, 0f, 0f, 4f, 4f, 4f, 240, 200, 80, iVar1, 0, 0, 2, 0, 0, 0, false);

		his is what I used to draw an amber downward pointing chevron "V", has to be redrawn every frame.The 180 is for 180 degrees rotation around the Y axis, the 50 is alpha, assuming max is 100, but it will accept 255.
		*/
	}

	int ReteaveAfk(const std::string& id)
	{
		LoggerLight("ReteaveAfk, id == " + id);

		int Id = -1;
		for (int i = 0; i < (int)AFKList.size(); i++)
		{
			if (AFKList[i].MyIdentity == id)
			{
				Id = i;
				break;
			}
		}
		return Id;
	}
	int ReteaveBrain(const std::string& id)
	{
		LoggerLight("ReteaveBrain, id == " + id);

		int Id = -1;
		for (int i = 0; i < (int)PedList.size(); i++)
		{
			if (PedList[i].MyIdentity == id)
			{
				Id = i;
				break;
			}
		}
		return Id;
	}
	int RetreaveCont(const std::string& id)
	{
		LoggerLight("RetreaveCont, id == " + id);

		int Id = -1;
		for (int i = 0; i < (int)PhoneContacts.size(); i++)
		{
			if (PhoneContacts[i].YourFriend.MyIdentity == id)
			{
				Id = i;
				break;
			}
		}
		return Id;
	}
	int ReteaveBrain(const Ped& ped)
	{
		LoggerLight("ReteaveBrain, ped");

		int Id = -1;
		for (int i = 0; i < (int)PedList.size(); i++)
		{
			if (PedList[i].ThisPed == ped)
			{
				Id = i;
				break;
			}
		}
		return Id;
	}

	void AddSprite(const std::string& spriteLocation, const std::string& spriteName, float posX, float posY, float sizeX, float sizeY, float heading)
	{
		if (!GRAPHICS::HAS_STREAMED_TEXTURE_DICT_LOADED((LPSTR)spriteLocation.c_str()))
			GRAPHICS::REQUEST_STREAMED_TEXTURE_DICT((LPSTR)spriteLocation.c_str(), true);

		int red = 101;
		int blue = 185;
		int green = 231;
		int alpha = 120;

		int iWidth, iHeight;
		GRAPHICS::GET_SCREEN_RESOLUTION(&iWidth, &iHeight);

		float num = (float)iWidth / (float)iHeight;
		float num2 = (float)1080.0 * num;
		float width = posX / num2;
		float height = posY / 1080;
		float screenX = sizeX / num2 + width * 0.5f;
		float screenY = sizeY / 1080 + height * 0.5f;
		GRAPHICS::DRAW_SPRITE((LPSTR)spriteLocation.c_str(), (LPSTR)spriteName.c_str(), screenX, screenY, width, height, heading, red, green, blue, alpha);
	}
	void DrawSessionList(const std::string& caption, const std::string& level, float lineWidth, float lineHeight, float lineTop, float lineLeft, float textLeft, float textLeft2, bool active, bool title, int position)
	{
		// default values
		int text_col[4] = { 255, 255, 255, 255 },
			rect_col[4] = { 0, 0, 0, 120 };
		float text_scale = 0.50;
		int font = 4;

		// correcting values for active line
		if (title)
		{
			rect_col[0] = 0;
			rect_col[1] = 0;
			rect_col[2] = 0;
			rect_col[3] = 250;
		}

		int screen_w, screen_h;
		GRAPHICS::GET_SCREEN_RESOLUTION(&screen_w, &screen_h);

		textLeft += lineLeft;
		textLeft2 += lineLeft;

		float lineWidthScaled = lineWidth / (float)screen_w; // line width
		float lineTopScaled = lineTop / (float)screen_h; // line top offset
		float textLeftScaled = textLeft / (float)screen_w; // text left offset
		float textLeftScaled2 = textLeft2 / (float)screen_w; // text left offset
		float lineHeightScaled = lineHeight / (float)screen_h; // line height

		float lineLeftScaled = lineLeft / (float)screen_w;

		// this is how it's done in original scripts

		// text upper part
		int num25 = GVM::AddScreenTextCenter(font, caption, textLeftScaled, lineTopScaled, lineHeightScaled, text_scale, GVM::RGBA(text_col[0], text_col[1], text_col[2], text_col[3]));
		int num26 = GVM::AddScreenTextCenter(font, level, textLeftScaled2, lineTopScaled, lineHeightScaled, text_scale, GVM::RGBA(text_col[0], text_col[1], text_col[2], text_col[3]));
		// rect
		GVM::Draw_rect(lineLeftScaled, lineTopScaled + (0.00278f), lineWidthScaled, ((((float)(num25)*UI::_0xDB88A37483346780(text_scale, 0)) + (lineHeightScaled * 2.0f)) + 0.005f), rect_col[0], rect_col[1], rect_col[2], rect_col[3]);

		if (active)
		{
			float PosX = 55.5;
			float PosY = 62.0;
			float sizeX = 352.0;
			float sizeY = 51.0;
			float Heading = 0.0;

			float iTimes1 = 1.0;
			float iTimes2 = 33.0;

			AddSprite("mpleaderboard", "leaderboard_globe_icon", PosX, PosY + (float)position * iTimes1, sizeX, sizeY + (float)position * iTimes2, 0.0);
		}
	}

	void showText(float x, float y, float scale, const std::string& text, int font, RGBA rgba, bool outline)
	{
		UI::SET_TEXT_FONT(font);
		UI::SET_TEXT_SCALE(scale, scale);
		UI::SET_TEXT_COLOUR(rgba.R, rgba.G, rgba.B, rgba.A);
		UI::SET_TEXT_WRAP(0.0, 1.0);
		UI::SET_TEXT_CENTRE(0);
		if (outline)
			UI::SET_TEXT_OUTLINE();
		UI::_SET_TEXT_ENTRY("STRING");
		UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)text.c_str());
		UI::_DRAW_TEXT(x, y);
	}
	void showPlayerLabel(Vector3 location, float baseSize, const std::vector<std::string>& textLines)
	{
		Vector3 cameraPos = CAM::GET_GAMEPLAY_CAM_COORD();
		float distance = DistanceTo(cameraPos, location);
		float totalMult = baseSize / (distance * (CAM::GET_GAMEPLAY_CAM_FOV() / 60.0f));

		float height = 0.0125f * totalMult;
		RGBA fontColor = RGBA(255, 255, 255, 255);
		GRAPHICS::SET_DRAW_ORIGIN(location.x, location.y, location.z, 0);
		int i = 0;

		for (auto line : textLines) {

			showText(0.0f, 0.0f + height * i, 0.2f * totalMult, line, 0, fontColor, true);
			i++;
		}

		GRAPHICS::CLEAR_DRAW_ORIGIN();
	}

	std::string GetVehName(std::string vic)
	{
		char* VehName = VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(MyHashKey(vic));

		if (UI::DOES_TEXT_LABEL_EXIST(VehName))
			VehName = UI::_GET_LABEL_TEXT(VehName);
		else
			VehName = "";

		std::string VehStr(VehName);

		return VehStr;
	}

	std::vector<std::string> LastMessage = {};

	void CleanOutOldCalls()
	{
		std::vector<std::string> Files = ReadDirectory(DirectMain);

		for (int i = 0; i < Files.size(); i++)
		{
			if (StringContains("PzContacts", Files[i]))
				FileRemoval(Files[i]);
		}
	}
	void AddContacttoList(const PhoneContact& cont)
	{
		std::vector<std::string> AddSetts = {
			"[Phone]",
			"ContactName=" + cont.Name,
			"ContactAddress=" + cont.ConReturnAdd,
			"Icon=CHAR_BLANK_ENTRY",
			"[Phone/End]"
		};
		WriteFile(cont.ConMobileAdd, AddSetts);
	}
	void AddPhoneFriends()
	{
		std::vector<std::string> PhoneFr = ReadDirectory(DirectMobileNet);

		for (int i = 0; i < PhoneFr.size(); i++)
		{
			if (StringContains("PzContacts", PhoneFr[i]))
				FileRemoval(PhoneFr[i]);
		}

		for (int i = 0; i < (int)PhoneContacts.size(); i++)
		{
			if (PhoneContacts[i].YourFriend.IsMobileCont && !PhoneContacts[i].InSession)
				AddContacttoList(PhoneContacts[i]);
		}		
	}
	void CleanNameList()
	{
		PlayerNames.clear();

		for (int i = 0; i < PedList.size(); i++)
			PlayerNames.push_back(PedList[i].MyName);

		for (int i = 0; i < AFKList.size(); i++)
			PlayerNames.push_back(AFKList[i].MyName);
	}
	void LoadContacts()
	{
		LoggerLight("LoadContacts");

		CleanNameList();

		std::vector<std::string> Files = ReadDirectory(DirectContacts);

		for (int i = 0; i < Files.size(); i++)
		{
			LoggerLight("LoadContacts == " + Files[i]);

			int intList = 0;
			int Level = 101; std::string Name = "Bob", Id = "Bob101"; int iNat = 1, Gun = -1, Rad = -2, PreVeh = 0;
			std::string model = "mp_m_freemode_01", faveVehicle = "";
			int shapeFirstID = 5, shapeSecondID = 5; float shapeMix = 0.0, skinMix = 0.92, thirdMix = 0.0;
			bool male = true, mobCon = false; int hairColour = 7, hairStreaks = 7, eyeColour = 0;
			int comp = 3, text = 4; std::string HairTag = "H_FMM_3_4", HairName = ""; int overLib = -1, over = 1;
			std::string Cloths = ""; std::vector<int> ClothA = {}, ClothB = {}, ExtraA = {}, ExtraB = {};

			std::vector<FreeOverLay> myOverlay = {};
			std::vector<Tattoo> myTattoo = {};

			std::vector<std::string> MyColect = ReadFile(Files[i]);

			for (int i = 0; i < MyColect.size(); i++)
			{
				std::string line = MyColect[i];
				if (StringContains("National", line))
					iNat = StingNumbersInt(line);
				else if (StringContains("Level", line))
					Level = StingNumbersInt(line);
				else if (StringContains("IsMobileCont", line))
					mobCon = StringBool(line);
				else if (StringContains("GunSelect", line))
					Gun = StingNumbersInt(line);
				else if (StringContains("FaveRadio", line))
					Rad = StingNumbersInt(line);
				else if (StringContains("PrefredVehicle", line))
					PreVeh = StingNumbersInt(line);
				else if (StringContains("FaveVehicle", line))
					faveVehicle = AfterEqual(line);
				else if (StringContains("PlayersName", line))
					Name = AfterEqual(line);
				else if (StringContains("PlayersId", line))
					Id = AfterEqual(line);
				else if (StringContains("Model", line))
					model = AfterEqual(line);
				else if (StringContains("ShapeFirstID", line))
					shapeFirstID = StingNumbersInt(line);
				else if (StringContains("ShapeSecondID", line))
					shapeSecondID = StingNumbersInt(line);
				else if (StringContains("ShapeMix", line))
					shapeMix = StingNumbersFloat(line);
				else if (StringContains("SkinMix", line))
					skinMix = StingNumbersFloat(line);
				else if (StringContains("ThirdMix", line))
					thirdMix = StingNumbersFloat(line);
				else if (StringContains("Male", line))
					male = StringBool(line);
				else if (StringContains("HairColour", line))
					hairColour = StingNumbersInt(line);
				else if (StringContains("HairStreaks", line))
					hairStreaks = StingNumbersInt(line);
				else if (StringContains("EyeColour", line))
					eyeColour = StingNumbersInt(line);
				else if (StringContains("Comp", line))
					comp = StingNumbersInt(line);
				else if (StringContains("Text", line))
					text = StingNumbersInt(line);
				else if (StringContains("HairTag", line))
					HairTag = AfterEqual(line);
				else if (StringContains("HairName", line))
					HairName = AfterEqual(line);
				else if (StringContains("OverLib", line))
					overLib = StingNumbersInt(line);
				else if (StringContains("Over", line))
					over = StingNumbersInt(line);
				else if (StringContains("Title", line))
					Cloths = StingNumbersInt(line);
				else if (StringContains("[ClothA]", line))
					intList = 1;
				else if (StringContains("[ClothB]", line))
					intList = 2;
				else if (StringContains("[ExtraA]", line))
					intList = 3;
				else if (StringContains("[ExtraB]", line))
					intList = 4;
				else if (StringContains("[FreeOverLay]", line))
					intList = 5;
				else if (StringContains("[Tattoo]", line))
					intList = 6;
				else if (intList == 1)
				{
					if (StringContains("ClothA", line))
						ClothA.push_back(StingNumbersInt(line));
				}
				else if (intList == 2)
				{
					if (StringContains("ClothB", line))
						ClothB.push_back(StingNumbersInt(line));
				}
				else if (intList == 3)
				{
					if (StringContains("ExtraA", line))
						ExtraA.push_back(StingNumbersInt(line));
				}
				else if (intList == 4)
				{
					if (StringContains("ExtraB", line))
						ExtraB.push_back(StingNumbersInt(line));
				}
				else if (intList == 5)
				{
					if (StringContains("Overlay", line))
						myOverlay.push_back(FreeOverLay(StingNumbersInt(line), StingNumbersInt(MyColect[i + 1]), StingNumbersFloat(MyColect[i + 2])));
				}
				else if (intList == 6)
				{
					if (StringContains("BaseName", line))
						myTattoo.push_back(Tattoo(AfterEqual(line), AfterEqual(MyColect[i + 1]), AfterEqual(MyColect[i + 2])));
				}
			}

			bool bReBuild = false;

			if (Gun == -1)
			{
				Gun = RandomInt(1, 7);
				PreVeh = 0;
				bReBuild = true;
			}
			
			if (Rad == -2)
			{
				Rad = LessRandomInt("RadioSet", -1, (int)RadioGaGa.size() - 1);
				if (LessRandomInt("RadioFlik", 0, 20) < 3)
					Rad = 99;
				bReBuild = true;
			}
			
			FaceBank myFaces = FaceBank(shapeFirstID, shapeSecondID, shapeMix, skinMix, thirdMix);
			ClothX cothing = ClothX(Cloths, ClothA, ClothB, ExtraA, ExtraB);
			HairSets myHair = HairSets(comp, text, HairTag, HairName, overLib, over);
			std::string ConMobAdd = DirectMobileNet + "/PzContacts" + Name + ".ini";
			std::string ConRetAdd = DirectMain + "/PzContacts" + Name + ".ini";
			PhoneContact ThisCon = PhoneContact(Name, ConMobAdd, ConRetAdd, PlayerBrain(Name, Id, ClothBank(Name, model, cothing, myFaces, male, myHair, hairColour, hairStreaks, eyeColour, myOverlay, myTattoo), 0, Level, true, mobCon, iNat, Gun, PreVeh, faveVehicle, Rad));

			if (ReteaveBrain(Id) != -1)
				ThisCon.InSession = true;

			PhoneContacts.push_back(ThisCon);

			PlayerNames.push_back(Name);

			if (bReBuild)
				SaveContacts(ThisCon);
		}
		AddPhoneFriends();
	}
	void SaveContacts(const PhoneContact& contact)
	{
		LoggerLight("SaveContacts == " + contact.YourFriend.MyName);

		std::vector<std::string> MyCon = {
			"[PlayerBrain]",
			"National=" + std::to_string(contact.YourFriend.Nationality),
			"Level=" + std::to_string(contact.YourFriend.Level),
			"IsMobileCont=" + BoolToString(contact.YourFriend.IsMobileCont),
			"GunSelect=" + std::to_string(contact.YourFriend.GunSelect),
			"FaveRadio=" + std::to_string(contact.YourFriend.FaveRadio),
			"PrefredVehicle=" + std::to_string(contact.YourFriend.PrefredVehicle),
			"FaveVehicle=" + contact.YourFriend.FaveVehicle,
			"PlayersName=" + contact.YourFriend.MyName,
			"PlayersId=" + contact.YourFriend.MyIdentity,
			"[ClothBank]",
			"Model=" + contact.YourFriend.PFMySetting.Model,
			"[FaceBank]",
			"ShapeFirstID=" + std::to_string(contact.YourFriend.PFMySetting.MyFaces.ShapeFirstID),
			"ShapeSecondID=" + std::to_string(contact.YourFriend.PFMySetting.MyFaces.ShapeSecondID),
			"ShapeMix=" + std::to_string(contact.YourFriend.PFMySetting.MyFaces.ShapeMix),
			"SkinMix=" + std::to_string(contact.YourFriend.PFMySetting.MyFaces.SkinMix),
			"ThirdMix=" + std::to_string(contact.YourFriend.PFMySetting.MyFaces.ThirdMix),
			"Male=" + BoolToString(contact.YourFriend.PFMySetting.Male),
			"HairColour=" + std::to_string(contact.YourFriend.PFMySetting.HairColour),
			"HairStreaks=" + std::to_string(contact.YourFriend.PFMySetting.HairStreaks),
			"EyeColour=" + std::to_string(contact.YourFriend.PFMySetting.EyeColour),
			"[Hair]",
			"Comp=" + std::to_string(contact.YourFriend.PFMySetting.MyHair.Comp),
			"Text=" + std::to_string(contact.YourFriend.PFMySetting.MyHair.Text),
			"HairTag=" + contact.YourFriend.PFMySetting.MyHair.HandName,
			"HairName=" + contact.YourFriend.PFMySetting.MyHair.Name,
			"OverLib=" + std::to_string(contact.YourFriend.PFMySetting.MyHair.OverLib),
			"Over=" + std::to_string(contact.YourFriend.PFMySetting.MyHair.Over),
			"[ClothX]",
			"Title= " + contact.YourFriend.PFMySetting.Cothing.Title
		};

		MyCon.push_back("[ClothA]");

		for (int i = 0; i < contact.YourFriend.PFMySetting.Cothing.ClothA.size(); i++)
			MyCon.push_back("ClothA=" + std::to_string(contact.YourFriend.PFMySetting.Cothing.ClothA[i]));

		MyCon.push_back("[ClothB]");

		for (int i = 0; i < contact.YourFriend.PFMySetting.Cothing.ClothB.size(); i++)
			MyCon.push_back("ClothB=" + std::to_string(contact.YourFriend.PFMySetting.Cothing.ClothB[i]));

		MyCon.push_back("[ExtraA]");

		for (int i = 0; i < contact.YourFriend.PFMySetting.Cothing.ExtraA.size(); i++)
			MyCon.push_back("ExtraA=" + std::to_string(contact.YourFriend.PFMySetting.Cothing.ExtraA[i]));

		MyCon.push_back("[ExtraB]");

		for (int i = 0; i < contact.YourFriend.PFMySetting.Cothing.ExtraB.size(); i++)
			MyCon.push_back("ExtraB=" + std::to_string(contact.YourFriend.PFMySetting.Cothing.ExtraB[i]));

		MyCon.push_back("[FreeOverLay]");
		for (int i = 0; i < contact.YourFriend.PFMySetting.MyOverlay.size(); i++)
		{
			MyCon.push_back("[OverLay]");
			MyCon.push_back("Overlay=" + std::to_string(contact.YourFriend.PFMySetting.MyOverlay[i].Overlay));
			MyCon.push_back("OverCol=" + std::to_string(contact.YourFriend.PFMySetting.MyOverlay[i].OverCol));
			MyCon.push_back("OverOpc=" + std::to_string(contact.YourFriend.PFMySetting.MyOverlay[i].OverOpc));
		}
		
		MyCon.push_back("[Tattoo]");
		if ((int)contact.YourFriend.PFMySetting.MyTattoo.size() > 0)
		{
			for (int i = 0; i < contact.YourFriend.PFMySetting.MyTattoo.size(); i++)
			{
				MyCon.push_back("[Tat]");
				MyCon.push_back("BaseName=" + contact.YourFriend.PFMySetting.MyTattoo[i].BaseName);
				MyCon.push_back("TatsDesc=" + contact.YourFriend.PFMySetting.MyTattoo[i].Name);
				MyCon.push_back("TatName=" + contact.YourFriend.PFMySetting.MyTattoo[i].TatName);
			}
		}

		WriteFile(DirectContacts + "/" + contact.YourFriend.MyName + ".ini", MyCon);
	}

	void SaveOutfits(ClothX cloths, const std::string& dir)
	{
		LoggerLight("Savecloths == " + cloths.Title);

		std::vector<std::string> MyCon = {
			"[ClothX]",
			"Title= " + cloths.Title
		};

		MyCon.push_back("[ClothA]");

		for (int i = 0; i < cloths.ClothA.size(); i++)
			MyCon.push_back("ClothA=" + std::to_string(cloths.ClothA[i]));

		MyCon.push_back("[ClothB]");

		for (int i = 0; i < cloths.ClothB.size(); i++)
			MyCon.push_back("ClothB=" + std::to_string(cloths.ClothB[i]));

		MyCon.push_back("[ExtraA]");

		for (int i = 0; i < cloths.ExtraA.size(); i++)
			MyCon.push_back("ExtraA=" + std::to_string(cloths.ExtraA[i]));

		MyCon.push_back("[ExtraB]");

		for (int i = 0; i < cloths.ExtraB.size(); i++)
			MyCon.push_back("ExtraB=" + std::to_string(cloths.ExtraB[i]));

		WriteFile(dir + "/" + cloths.Title + ".ini", MyCon);
	}
	void OutfitFolderTest()
	{
		std::vector<std::string> ReadMe = {
			"This txt file is a marker that this folder has Outfits to load.",
			"If the any of the two folders are empty delete this txt file.",
			"If You wish to manualy alter any of the files or add your own,",
			"Title does not have to match file name and the mod searches the",
			"the assoosiated folder for any .ini.",
			"While i've got youur attention consider subing my youtube channel as i get a lot of views but no subs means no revenue.",
			"the mods i've made have taken years of unpaid work to complete and the few coins from youtube would help a lot.",
			"",
			"",
			"",
			"",
			"https://www.youtube.com/@adopcalipt"
		};
		std::vector<ClothX> MaleOut = {
			ClothX("MaleBeachThe Bare Chest", { 0, 0, -1, 15, 16, 0, 1, 16, 15, 0, 0, 15 }, { 0, 0, 0, 0, 1, 0, 7, 2, 0, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleBeachThe Dude", { 0, 0, -1, 5, 15, 0, 16, 0, 15, 0, 0, 17 }, { 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 4 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleBeachThe Heat", { 0, 0, -1, 5, 18, 0, 16, 0, 15, 0, 0, 5 }, { 0, 0, 0, 0, 3, 0, 4, 0, 0, 0, 0, 7 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleBeachThe Paradise", { 0, 0, -1, 0, 18, 0, 1, 0, 15, 0, 0, 1 }, { 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleBeachThe Skimpy", { 0, 0, -1, 15, 18, 0, 5, 0, 15, 0, 0, 15 }, { 0, 0, 0, 0, 11, 0, 3, 0, 0, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleBeachThe Summer Rays", { 0, 0, -1, 5, 16, 0, 16, 17, 15, 0, 0, 17 }, { 0, 0, 0, 0, 2, 0, 8, 2, 0, 0, 0, 3 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleBeachThe Surfer", { 0, 0, -1, 5, 12, 0, 16, 0, 15, 0, 0, 17 }, { 0, 0, 0, 0, 4, 0, 6, 0, 0, 0, 0, 5 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleBeachThe Tidal Wave", { 0, 0, -1, 0, 6, 0, 16, 0, 15, 0, 0, 44 }, { 0, 0, 0, 0, 10, 0, 6, 0, 0, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleBikerAvarus", { 0, 0, -1, 19, 72, 0, 53, 0, 82, 0, 0, 157 }, { 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0 }, { 83, 24, -1, -1, -1 }, { 0, 1, 0, 0, 0 }),
			ClothX("MaleBikerBlazer", { 0, 0, -1, 14, 62, 0, 7, 0, 23, 0, 0, 167 }, { 0, 0, 0, 0, 2, 0, 15, 0, 2, 0, 0, 3 }, { 76, 0, -1, -1, -1 }, { 4, 0, 0, 0, 0 }),
			ClothX("MaleBikerChimera", { 0, 0, -1, 130, 73, 0, 56, 0, 15, 0, 0, 162 }, { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3 }, { 83, 8, -1, -1, -1 }, { 0, 6, 0, 0, 0 }),
			ClothX("MaleBikerFaggio", { 0, 0, 10, 6, 4, 0, 23, 0, 15, 0, 0, 184 }, { 0, 0, 0, 0, 4, 0, 9, 0, 0, 0, 0, 1 }, { 29, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleBikerHakuchou", { 0, 0, -1, 6, 9, 0, 51, 0, 15, 0, 0, 165 }, { 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 1 }, { 83, 25, -1, -1, -1 }, { 5, 7, 0, 0, 0 }),
			ClothX("MaleBikerManchez", { 0, 0, -1, 0, 69, 0, 7, 0, 83, 0, 0, 180 }, { 0, 0, 0, 0, 2, 0, 1, 0, 0, 0, 0, 2 }, { 77, 25, -1, -1, -1 }, { 18, 0, 0, 0, 0 }),
			ClothX("MaleBikerNightblade", { 0, 0, -1, 116, 74, 0, 52, 0, 15, 0, 0, 173 }, { 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 0, 2 }, { -1, 8, -1, -1, -1 }, { 0, 1, 0, 0, 0 }),
			ClothX("MaleBikerRat Bike", { 0, 0, -1, 30, 72, 0, 50, 0, 83, 0, 0, 158 }, { 0, 0, 0, 1, 4, 0, 0, 0, 8, 0, 0, 1 }, { -1, 24, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleBikerWolfsbane", { 0, 0, -1, 24, 76, 0, 56, 0, 15, 0, 0, 174 }, { 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0 }, { -1, 9, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleBikerZombie", { 0, 0, -1, 22, 4, 0, 53, 0, 75, 0, 0, 181 }, { 0, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0 }, { -1, 0, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleBusiness CasualThe Alligator", { 0, 0, -1, 4, 23, 0, 20, 12, 10, 0, 0, 23 }, { 0, 0, 0, 0, 8, 0, 8, 2, 2, 0, 0, 0 }, { 5, 7, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleBusiness CasualThe Bulldog", { 0, 0, -1, 11, 10, 0, 11, 10, 15, 0, 0, 26 }, { 0, 0, 0, 0, 0, 0, 12, 2, 0, 0, 0, 9 }, { 12, 17, -1, -1, -1 }, { 0, 6, 0, 0, 0 }),
			ClothX("MaleBusiness CasualThe Collector", { 0, 0, -1, 11, 23, 0, 21, 25, 6, 0, 0, 25 }, { 0, 0, 0, 0, 8, 0, 6, 13, 12, 0, 0, 9 }, { 12, 18, -1, -1, -1 }, { 4, 7, 0, 0, 0 }),
			ClothX("MaleBusiness CasualThe Devil Chic", { 0, 0, -1, 4, 4, 0, 20, 25, 4, 0, 0, 24 }, { 0, 0, 0, 0, 0, 0, 7, 2, 0, 0, 0, 3 }, { 12, 18, -1, -1, -1 }, { 0, 2, 0, 0, 0 }),
			ClothX("MaleBusiness CasualThe Eccentric", { 0, 0, -1, 11, 23, 0, 1, 22, 6, 0, 0, 25 }, { 0, 0, 0, 0, 2, 0, 11, 8, 7, 0, 0, 5 }, { 27, -1, -1, -1, -1 }, { 5, 0, 0, 0, 0 }),
			ClothX("MaleBusiness CasualThe European", { 0, 0, -1, 11, 23, 0, 10, 12, 15, 0, 0, 26 }, { 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 5 }, { 26, 18, -1, -1, -1 }, { 0, 3, 0, 0, 0 }),
			ClothX("MaleBusiness CasualThe Mogul", { 0, 0, -1, 11, 22, 0, 20, 25, 6, 0, 0, 25 }, { 0, 0, 0, 0, 7, 0, 2, 10, 11, 0, 0, 0 }, { -1, 18, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleBusiness CasualThe Mr. Frost", { 0, 0, -1, 4, 4, 0, 20, 25, 4, 0, 0, 24 }, { 0, 0, 0, 0, 2, 0, 5, 2, 0, 0, 0, 4 }, { 7, 18, -1, -1, -1 }, { 1, 2, 0, 0, 0 }),
			ClothX("MaleBusiness CasualThe Tally-ho", { 0, 0, -1, 4, 22, 0, 20, 25, 4, 0, 0, 24 }, { 0, 0, 0, 0, 11, 0, 6, 4, 1, 0, 0, 0 }, { 7, -1, -1, -1, -1 }, { 3, 0, 0, 0, 0 }),
			ClothX("MaleBusiness CasualThe Victorious", { 0, 0, -1, 14, 22, 0, 20, 21, 28, 0, 0, 27 }, { 0, 0, 0, 0, 1, 0, 11, 1, 8, 0, 0, 1 }, { 26, -1, -1, -1, -1 }, { 4, 0, 0, 0, 0 }),
			ClothX("MaleBusiness SmartThe Collegiate", { 0, 0, -1, 4, 23, 0, 20, 25, 26, 0, 0, 23 }, { 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 2 }, { 26, 17, -1, -1, -1 }, { 2, 4, 0, 0, 0 }),
			ClothX("MaleBusiness SmartThe Country Club", { 0, 0, -1, 4, 23, 0, 21, 0, 25, 0, 0, 24 }, { 0, 0, 0, 0, 10, 0, 11, 0, 11, 0, 0, 10 }, { 26, -1, -1, -1, -1 }, { 12, 0, 0, 0, 0 }),
			ClothX("MaleBusiness SmartThe Discoteque", { 0, 0, -1, 4, 22, 0, 10, 25, 26, 0, 0, 23 }, { 0, 0, 0, 0, 12, 0, 0, 0, 12, 0, 0, 3 }, { 12, 18, -1, -1, -1 }, { 1, 3, 0, 0, 0 }),
			ClothX("MaleBusiness SmartThe Egotist", { 0, 0, -1, 4, 23, 0, 20, 25, 26, 0, 0, 24 }, { 0, 0, 0, 0, 3, 0, 3, 15, 3, 0, 0, 3 }, { -1, 13, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleBusiness SmartThe Gentry", { 0, 0, -1, 4, 22, 0, 20, 22, 26, 0, 0, 24 }, { 0, 0, 0, 0, 6, 0, 0, 10, 5, 0, 0, 6 }, { 27, -1, -1, -1, -1 }, { 8, 0, 0, 0, 0 }),
			ClothX("MaleBusiness SmartThe High Roller", { 0, 0, -1, 14, 10, 0, 20, 22, 26, 0, 0, 27 }, { 0, 0, 0, 0, 0, 0, 7, 4, 3, 0, 0, 1 }, { 27, -1, -1, -1, -1 }, { 1, 0, 0, 0, 0 }),
			ClothX("MaleBusiness SmartThe Panther", { 0, 0, -1, 4, 10, 0, 10, 25, 4, 0, 0, 28 }, { 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0 }, { -1, 17, -1, -1, -1 }, { 0, 5, 0, 0, 0 }),
			ClothX("MaleBusiness SmartThe Sophisticate", { 0, 0, -1, 4, 13, 0, 20, 21, 10, 0, 0, 24 }, { 0, 0, 0, 0, 0, 0, 8, 11, 10, 0, 0, 1 }, { 27, 18, -1, -1, -1 }, { 4, 5, 0, 0, 0 }),
			ClothX("MaleBusiness SmartThe Tycoon", { 0, 0, -1, 4, 10, 0, 10, 25, 4, 0, 0, 28 }, { 0, 0, 0, 0, 2, 0, 0, 14, 2, 0, 0, 2 }, { 26, 17, -1, -1, -1 }, { 2, 6, 0, 0, 0 }),
			ClothX("MaleBusiness SmartThe Yeehaw", { 0, 0, -1, 1, 22, 0, 20, 0, 12, 0, 0, 24 }, { 0, 0, 0, 0, 6, 0, 2, 0, 5, 0, 0, 7 }, { 13, 18, -1, -1, -1 }, { 2, 0, 0, 0, 0 }),
			ClothX("MaleCasualThe Denims", { 0, 0, -1, 8, 4, 0, 4, 0, 15, 0, 0, 38 }, { 0, 0, 0, 0, 4, 0, 1, 0, 0, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleCasualThe Hangout", { 0, 0, -1, 0, 0, 0, 1, 17, 15, 0, 0, 33 }, { 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleCasualThe Mellow", { 0, 0, -1, 12, 1, 0, 1, 0, 15, 0, 0, 41 }, { 0, 0, 0, 0, 14, 0, 4, 0, 0, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleCasualThe Plain White", { 0, 0, -1, 0, 0, 0, 0, 0, 15, 0, 0, 1 }, { 0, 0, 0, 0, 2, 0, 10, 0, 0, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleCasualThe Simple", { 0, 0, -1, 0, 1, 0, 1, 0, 15, 0, 0, 22 }, { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleCEOAlpha", { 0, 0, -1, 4, 82, 0, 12, 15, 0, 0, 0, 192 }, { 0, 0, 0, 0, 6, 0, 3, 0, 1, 0, 0, 1 }, { -1, 8, -1, -1, -1 }, { 0, 2, 0, 0, 0 }),
			ClothX("MaleCEOAnonymous", { 0, 106, -1, 131, 87, 0, 62, 0, 15, 0, 0, 205 }, { 0, 25, 0, 0, 9, 0, 3, 0, 0, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleCEOBroker", { 0, 0, -1, 4, 37, 0, 20, 38, 10, 0, 0, 142 }, { 0, 0, 0, 0, 2, 0, 5, 6, 2, 0, 0, 2 }, { 29, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleCEOChief", { 0, 0, -1, 12, 37, 0, 20, 0, 73, 0, 0, 137 }, { 0, 0, 0, 0, 2, 0, 5, 0, 1, 0, 0, 0 }, { -1, 17, -1, -1, -1 }, { 0, 9, 0, 0, 0 }),
			ClothX("MaleCEOClown", { 0, 95, -1, 44, 39, 0, 27, 0, 15, 0, 0, 66 }, { 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleCEOCommander", { 0, 0, -1, 22, 92, 0, 24, 0, 15, 0, 0, 228 }, { 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 14 }, { 114, 5, -1, -1, -1 }, { 10, 1, 0, 0, 0 }),
			ClothX("MaleCEOCreator", { 0, 0, -1, 12, 13, 0, 7, 0, 11, 0, 0, 35 }, { 0, 0, 0, 0, 0, 0, 15, 0, 2, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleCEODemon", { 0, 94, -1, 33, 79, 0, 52, 0, 15, 0, 0, 153 }, { 0, 4, 0, 0, 0, 0, 1, 0, 0, 0, 0, 7 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleCEODirector", { 0, 0, -1, 4, 37, 0, 15, 28, 31, 0, 0, 140 }, { 0, 0, 0, 0, 0, 0, 10, 1, 0, 0, 0, 2 }, { -1, 17, -1, -1, -1 }, { 0, 5, 0, 0, 0 }),
			ClothX("MaleCEOFat Cat", { 0, 0, -1, 4, 20, 0, 40, 11, 35, 0, 0, 27 }, { 0, 0, 0, 0, 2, 0, 9, 2, 0, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleCEOFounder", { 0, 0, -1, 0, 0, 0, 2, 0, 15, 0, 0, 0 }, { 0, 0, 0, 0, 1, 0, 6, 0, 0, 0, 0, 1 }, { -1, -1, 0, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleCEOGeneral", { 0, 0, -1, 4, 9, 0, 27, 0, 15, 0, 0, 138 }, { 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 2 }, { 6, 16, -1, -1, -1 }, { 5, 2, 0, 0, 0 }),
			ClothX("MaleCEOGuerilla", { 0, 0, -1, 141, 87, 0, 60, 0, 15, 0, 0, 221 }, { 0, 0, 0, 16, 16, 0, 3, 0, 0, 0, 0, 9 }, { 108, 5, -1, -1, -1 }, { 16, 0, 0, 0, 0 }),
			ClothX("MaleCEOGunrunner", { 0, 0, -1, 145, 87, 0, 62, 0, 15, 0, 0, 222 }, { 0, 0, 0, 19, 17, 0, 7, 0, 0, 0, 0, 23 }, { 107, 5, -1, -1, -1 }, { 17, 5, 0, 0, 0 }),
			ClothX("MaleCEOHedonist", { 0, 0, -1, 14, 56, 0, 37, 0, 15, 0, 0, 114 }, { 0, 0, 0, 0, 5, 0, 3, 0, 0, 0, 0, 5 }, { 13, 8, -1, -1, -1 }, { 2, 5, 0, 0, 0 }),
			ClothX("MaleCEOIcon", { 0, 0, -1, 6, 83, 0, 29, 89, 15, 0, 0, 190 }, { 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0 }, { 96, -1, -1, -1, -1 }, { 6, 0, 0, 0, 0 }),
			ClothX("MaleCEOInstigator", { 0, 107, -1, 41, 89, 0, 61, 0, 15, 0, 0, 208 }, { 0, 18, 0, 0, 20, 0, 0, 0, 0, 0, 0, 11 }, { 96, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleCEOMastermind", { 0, 0, -1, 35, 79, 0, 43, 0, 15, 0, 0, 204 }, { 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0 }, { 96, 7, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleCEOMoon Curser", { 0, 104, -1, 141, 87, 0, 60, 0, 15, 0, 0, 220 }, { 0, 25, 0, 3, 3, 0, 0, 0, 0, 0, 0, 20 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleCEOPioneer", { 0, 0, -1, 4, 63, 0, 2, 0, 15, 0, 0, 139 }, { 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 0, 3 }, { -1, 3, -1, -1, -1 }, { 0, 9, 0, 0, 0 }),
			ClothX("MaleCEOPlayboy", { 0, 0, -1, 4, 60, 0, 36, 0, 72, 0, 0, 108 }, { 0, 0, 0, 0, 2, 0, 3, 0, 3, 0, 0, 4 }, { -1, 13, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleCEOPremier", { 0, 0, -1, 23, 43, 0, 57, 0, 15, 4, 0, 5 }, { 0, 0, 0, 0, 0, 0, 6, 0, 0, 1, 0, 2 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleCEOPresident", { 0, 0, -1, 6, 7, 0, 28, 0, 15, 0, 0, 75 }, { 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 10 }, { 66, 18, -1, -1, -1 }, { 0, 1, 0, 0, 0 }),
			ClothX("MaleCEORecon Leader", { 0, 0, -1, 37, 33, 0, 27, 0, 15, 10, 0, 222 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 20 }, { 112, -1, -1, -1, -1 }, { 4, 0, 0, 0, 0 }),
			ClothX("MaleCEORingleader", { 0, 0, -1, 4, 78, 0, 57, 0, 0, 0, 0, 191 }, { 0, 0, 0, 0, 7, 0, 4, 0, 5, 0, 0, 0 }, { 55, 13, -1, -1, -1 }, { 19, 2, 0, 0, 0 }),
			ClothX("MaleCEORoller", { 0, 0, -1, 0, 43, 0, 57, 51, 81, 0, 0, 170 }, { 0, 0, 0, 0, 1, 0, 6, 0, 2, 0, 0, 3 }, { -1, 18, -1, -1, -1 }, { 0, 3, 0, 0, 0 }),
			ClothX("MaleCEOStanduot", { 0, 0, -1, 0, 82, 0, 57, 0, 15, 0, 0, 193 }, { 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 5 }, { 96, 7, -1, -1, -1 }, { 1, 2, 0, 0, 0 }),
			ClothX("MaleCEOSurvivalist", { 0, 0, -1, 141, 87, 0, 60, 0, 101, 0, 0, 212 }, { 0, 0, 0, 19, 6, 0, 7, 0, 6, 0, 0, 18 }, { 105, 23, -1, -1, -1 }, { 23, 9, 0, 0, 0 }),
			ClothX("MaleCEOTop Dog", { 0, 0, -1, 0, 43, 0, 57, 15, 15, 0, 0, 193 }, { 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 22 }, { 55, 2, 32, -1, -1 }, { 1, 0, 0, 0, 0 }),
			ClothX("MaleCEOTrailblazer", { 0, 0, -1, 4, 78, 0, 57, 123, 23, 0, 0, 191 }, { 0, 0, 0, 0, 6, 0, 9, 0, 0, 0, 0, 23 }, { 96, 7, -1, -1, -1 }, { 0, 3, 0, 0, 0 }),
			ClothX("MaleCEOVillain", { 0, 101, -1, 33, 78, 0, 57, 0, 71, 0, 0, 203 }, { 0, 13, 0, 0, 2, 0, 10, 0, 3, 0, 0, 25 }, { 102, 13, -1, -1, -1 }, { 7, 5, 0, 0, 0 }),
			ClothX("MaleCEOWarlord", { 0, 0, -1, 6, 59, 0, 27, 85, 5, 0, 0, 35 }, { 0, 0, 0, 0, 7, 0, 0, 0, 2, 0, 0, 1 }, { 55, 23, -1, -1, -1 }, { 1, 9, 0, 0, 0 }),
			ClothX("MaleDesignerThe Deluxe", { 0, 0, -1, 14, 4, 0, 29, 49, 65, 0, 0, 74 }, { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 }, { 29, 18, 20, -1, -1 }, { 1, 2, 0, 0, 0 }),
			ClothX("MaleDesignerThe Exclusive", { 0, 0, -1, 4, 4, 0, 28, 0, 15, 0, 0, 75 }, { 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 10 }, { 6, 13, 17, -1, -1 }, { 1, 0, 2, 0, 0 }),
			ClothX("MaleDesignerThe Flash", { 0, 0, -1, 14, 26, 0, 28, 0, 23, 0, 0, 74 }, { 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 6 }, { 7, 8, 5, -1, -1 }, { 0, 0, 2, 0, 0 }),
			ClothX("MaleDesignerThe Grand", { 0, 0, -1, 6, 26, 0, 28, 52, 5, 0, 0, 70 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4 }, { -1, 13, 14, -1, -1 }, { 0, 2, 2, 0, 0 }),
			ClothX("MaleDesignerThe Luxor", { 0, 0, -1, 14, 7, 0, 22, 50, 65, 0, 0, 74 }, { 0, 0, 0, 0, 4, 0, 11, 0, 3, 0, 0, 5 }, { -1, 18, 17, -1, -1 }, { 0, 2, 0, 0, 0 }),
			ClothX("MaleDesignerThe Midas", { 0, 0, -1, 0, 37, 0, 29, 50, 15, 0, 0, 71 }, { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }, { 4, 18, 8, -1, -1 }, { 1, 5, 0, 0, 0 }),
			ClothX("MaleDesignerThe Perseus", { 0, 0, -1, 0, 7, 0, 29, 0, 15, 0, 0, 73 }, { 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 5 }, { -1, 13, 14, -1, -1 }, { 0, 2, 0, 0, 0 }),
			ClothX("MaleDesignerThe Pimp", { 0, 0, -1, 4, 26, 0, 28, 54, 10, 0, 0, 70 }, { 0, 0, 0, 0, 9, 0, 4, 0, 2, 0, 0, 1 }, { 12, 10, 14, -1, -1 }, { 4, 4, 0, 0, 0 }),
			ClothX("MaleDesignerThe Refined", { 0, 0, -1, 50, 37, 0, 12, 0, 23, 0, 0, 72 }, { 0, 0, 0, 0, 2, 0, 3, 0, 1, 0, 0, 1 }, { 7, -1, 20, -1, -1 }, { 2, 0, 4, 0, 0 }),
			ClothX("MaleDesignerThe Sessanta Nove", { 0, 0, -1, 6, 4, 0, 29, 49, 5, 0, 0, 74 }, { 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 3 }, { 7, 18, 11, -1, -1 }, { 2, 0, 0, 0, 0 }),
			ClothX("MaleDesignerThe Talent", { 0, 0, -1, 1, 26, 0, 28, 53, 65, 0, 0, 62 }, { 0, 0, 0, 0, 11, 0, 3, 1, 10, 0, 0, 0 }, { 4, 7, 20, -1, -1 }, { 0, 0, 3, 0, 0 }),
			ClothX("MaleDesignerThe Vogue", { 0, 0, -1, 6, 26, 0, 28, 55, 5, 0, 0, 72 }, { 0, 0, 0, 0, 6, 0, 5, 1, 0, 0, 0, 0 }, { -1, 10, 8, -1, -1 }, { 0, 1, 1, 0, 0 }),
			ClothX("MaleFlashyThe Blues", { 0, 0, -1, 12, 25, 0, 10, 29, 31, 0, 0, 31 }, { 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 2 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleFlashyThe Musician", { 0, 0, -1, 6, 4, 0, 10, 0, 5, 0, 0, 4 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleFlashyThe Royal", { 0, 0, -1, 6, 26, 0, 20, 0, 5, 0, 0, 4 }, { 0, 0, 0, 0, 0, 0, 3, 0, 2, 0, 0, 14 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleFlashyThe V.I.P.", { 0, 0, -1, 12, 24, 0, 10, 27, 35, 0, 0, 30 }, { 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("MaleHipsterActivist", { 0, 0, -1, 8, 26, 0, 23, 0, 15, 0, 0, 38 }, { 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1 }, { 28, 20, -1, -1, -1 }, { 5, 1, 0, 0, 0 }),
			ClothX("MaleHipsterAficionado", { 0, 0, -1, 1, 26, 0, 22, 0, 38, 0, 0, 35 }, { 0, 0, 0, 0, 11, 0, 8, 0, 0, 0, 0, 0 }, { 28, 20, -1, -1, -1 }, { 2, 3, 0, 0, 0 }),
			ClothX("MaleHipsterArtist", { 0, 0, -1, 11, 28, 0, 23, 0, 15, 0, 0, 43 }, { 0, 0, 0, 0, 12, 0, 4, 0, 0, 0, 0, 0 }, { 29, 20, -1, -1, -1 }, { 6, 0, 0, 0, 0 }),
			ClothX("MaleHipsterEco", { 0, 0, -1, 11, 26, 0, 22, 11, 15, 0, 0, 42 }, { 0, 0, 0, 0, 3, 0, 7, 2, 0, 0, 0, 0 }, { 28, 20, -1, -1, -1 }, { 3, 2, 0, 0, 0 }),
			ClothX("MaleHipsterElitist", { 0, 0, -1, 1, 26, 0, 23, 0, 43, 0, 0, 35 }, { 0, 0, 0, 0, 7, 0, 6, 0, 3, 0, 0, 3 }, { -1, 19, -1, -1, -1 }, { 0, 6, 0, 0, 0 }),
			ClothX("MaleHipsterFitness", { 0, 0, -1, 0, 27, 0, 22, 0, 15, 0, 0, 39 }, { 0, 0, 0, 0, 5, 0, 10, 0, 0, 0, 0, 0 }, { 29, 19, -1, -1, -1 }, { 7, 2, 0, 0, 0 }),
			ClothX("MaleHipsterIronic", { 0, 0, -1, 11, 26, 0, 23, 11, 6, 0, 0, 40 }, { 0, 0, 0, 0, 9, 0, 3, 2, 12, 0, 0, 0 }, { 29, 20, -1, -1, -1 }, { 5, 6, 0, 0, 0 }),
			ClothX("MaleHipsterNatural", { 0, 0, -1, 1, 26, 0, 22, 0, 38, 0, 0, 35 }, { 0, 0, 0, 0, 6, 0, 0, 0, 1, 0, 0, 2 }, { 29, 19, -1, -1, -1 }, { 2, 4, 0, 0, 0 }),
			ClothX("MaleHipsterPop Up", { 0, 0, -1, 0, 28, 0, 23, 30, 15, 0, 0, 33 }, { 0, 0, 0, 0, 2, 0, 14, 4, 0, 0, 0, 0 }, { 28, 20, -1, -1, -1 }, { 1, 5, 0, 0, 0 }),
			ClothX("MaleHipsterUrban Hippy", { 0, 0, -1, 8, 26, 0, 22, 30, 15, 0, 0, 38 }, { 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 4 }, { 28, 20, -1, -1, -1 }, { 0, 7, 0, 0, 0 })
		};
		std::vector<ClothX> FemaleOut = {
			ClothX("FemaleBeachThe Bather", { 0, 0, -1, 11, 17, 0, 16, 11, 3, 0, 0, 36 }, { 0, 0, 0, 0, 7, 0, 8, 2, 0, 0, 0, 3 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleBeachThe Beach Babe", { 0, 0, -1, 15, 12, 0, 3, 11, 3, 0, 0, 18 }, { 0, 0, 0, 0, 14, 0, 13, 1, 0, 0, 0, 9 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleBeachThe Day Tripper", { 0, 0, -1, 5, 16, 0, 16, 10, 16, 0, 0, 31 }, { 0, 0, 0, 0, 10, 0, 7, 0, 4, 0, 0, 5 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleBeachThe Lifeguard", { 0, 0, -1, 11, 17, 0, 16, 3, 3, 0, 0, 11 }, { 0, 0, 0, 0, 4, 0, 7, 1, 0, 0, 0, 10 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleBeachThe Siesta", { 0, 0, -1, 15, 25, 0, 16, 1, 3, 0, 0, 18 }, { 0, 0, 0, 0, 2, 0, 9, 2, 0, 0, 0, 10 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleBeachThe Sun Kissed", { 0, 0, -1, 15, 16, 0, 16, 9, 3, 0, 0, 18 }, { 0, 0, 0, 0, 8, 0, 1, 0, 0, 0, 0, 6 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleBeachThe Tropical", { 0, 0, -1, 9, 16, 0, 15, 14, 3, 0, 0, 17 }, { 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleBikerThe Avarus", { 0, 0, -1, 31, 78, 0, 54, 0, 86, 0, 0, 181 }, { 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0 }, { 82, 26, -1, -1, -1 }, { 0, 1, 0, 0, 0 }),
			ClothX("FemaleBikerThe Blazer", { 0, 0, -1, 3, 73, 0, 11, 0, 72, 0, 0, 164 }, { 0, 0, 0, 0, 3, 0, 3, 0, 2, 0, 0, 3 }, { 75, -1, -1, -1, -1 }, { 4, 0, 0, 0, 0 }),
			ClothX("FemaleBikerThe Chimera", { 0, 0, -1, 147, 76, 0, 53, 0, 15, 0, 0, 159 }, { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3 }, { 82, 11, -1, -1, -1 }, { 0, 3, 0, 0, 0 }),
			ClothX("FemaleBikerThe Faggio", { 0, 0, -1, 3, 0, 0, 13, 0, 3, 0, 15, 186 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleBikerThe Hakuchou", { 0, 0, -1, 3, 11, 0, 51, 0, 3, 0, 0, 162 }, { 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 1 }, { 82, 27, -1, -1, -1 }, { 5, 7, 0, 0, 0 }),
			ClothX("FemaleBikerThe Manchez", { 0, 0, -1, 4, 71, 0, 11, 0, 3, 0, 0, 171 }, { 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 1 }, { 76, 27, -1, -1, -1 }, { 6, 6, 0, 0, 0 }),
			ClothX("FemaleBikerThe Nightblade", { 0, 0, -1, 133, 77, 0, 53, 0, 17, 0, 0, 175 }, { 0, 0, 0, 1, 2, 0, 0, 0, 3, 0, 0, 2 }, { -1, 11, -1, -1, -1 }, { 0, 4, 0, 0, 0 }),
			ClothX("FemaleBikerThe Rat Bike", { 0, 0, -1, 24, 76, 0, 51, 0, 3, 0, 0, 173 }, { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }, { -1, 26, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleBikerThe Wolfsbane", { 0, 0, -1, 27, 74, 0, 51, 0, 3, 0, 0, 176 }, { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 }, { -1, 10, -1, -1, -1 }, { 0, 1, 0, 0, 0 }),
			ClothX("FemaleBikerThe Zombie", { 0, 0, -1, 23, 0, 0, 56, 0, 77, 0, 0, 163 }, { 0, 0, 0, 0, 8, 0, 0, 0, 3, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleBusiness PantsThe Art Dealer", { 0, 0, -1, 5, 23, 0, 0, 2, 23, 0, 0, 24 }, { 0, 0, 0, 0, 1, 0, 2, 5, 3, 0, 0, 7 }, { 14, -1, -1, -1, -1 }, { 5, 0, 0, 0, 0 }),
			ClothX("FemaleBusiness PantsThe Aviator", { 0, 0, -1, 0, 11, 0, 8, 13, 24, 0, 0, 28 }, { 0, 0, 0, 0, 2, 0, 2, 0, 2, 0, 0, 4 }, { 27, 2, -1, -1, -1 }, { 8, 0, 0, 0, 0 }),
			ClothX("FemaleBusiness PantsThe Ball-buster", { 0, 0, -1, 5, 23, 0, 0, 0, 13, 0, 0, 24 }, { 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 2 }, { 26, 2, -1, -1, -1 }, { 3, 5, 0, 0, 0 }),
			ClothX("FemaleBusiness PantsThe Boardroom", { 0, 0, -1, 5, 6, 0, 20, 6, 23, 0, 0, 6 }, { 0, 0, 0, 0, 2, 0, 7, 4, 6, 0, 0, 2 }, { 26, 18, -1, -1, -1 }, { 2, 7, 0, 0, 0 }),
			ClothX("FemaleBusiness PantsThe Designer", { 0, 0, -1, 6, 23, 0, 19, 11, 23, 0, 0, 25 }, { 0, 0, 0, 0, 10, 0, 5, 1, 1, 0, 0, 10 }, { 27, 16, -1, -1, -1 }, { 9, 0, 0, 0, 0 }),
			ClothX("FemaleBusiness PantsThe Director", { 0, 0, -1, 6, 6, 0, 19, 0, 0, 0, 0, 25 }, { 0, 0, 0, 0, 0, 0, 4, 0, 12, 0, 0, 1 }, { -1, 19, -1, -1, -1 }, { 0, 2, 0, 0, 0 }),
			ClothX("FemaleBusiness PantsThe Fashionista", { 0, 0, -1, 6, 23, 0, 6, 1, 23, 0, 0, 25 }, { 0, 0, 0, 0, 11, 0, 2, 2, 3, 0, 0, 7 }, { 26, 16, -1, -1, -1 }, { 9, 6, 0, 0, 0 }),
			ClothX("FemaleBusiness PantsThe Liberty", { 0, 0, -1, 0, 23, 0, 19, 13, 2, 0, 0, 27 }, { 0, 0, 0, 0, 4, 0, 8, 2, 0, 0, 0, 4 }, { 8, -1, -1, -1, -1 }, { 4, 0, 0, 0, 0 }),
			ClothX("FemaleBusiness PantsThe Networker", { 0, 0, -1, 5, 23, 0, 19, 1, 23, 0, 0, 24 }, { 0, 0, 0, 0, 0, 0, 3, 1, 2, 0, 0, 0 }, { -1, 18, -1, -1, -1 }, { 0, 3, 0, 0, 0 }),
			ClothX("FemaleBusiness PantsThe Parisienne", { 0, 0, -1, 5, 23, 0, 20, 6, 0, 0, 0, 24 }, { 0, 0, 0, 0, 12, 0, 11, 0, 13, 0, 0, 9 }, { 13, -1, -1, -1, -1 }, { 1, 0, 0, 0, 0 }),
			ClothX("FemaleBusiness SkirtsThe 9-5", { 0, 0, -1, 0, 24, 0, 19, 13, 2, 0, 0, 27 }, { 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0 }, { -1, 18, -1, -1, -1 }, { 0, 3, 0, 0, 0 }),
			ClothX("FemaleBusiness SkirtsThe Architect", { 0, 0, -1, 5, 24, 0, 19, 1, 13, 0, 0, 24 }, { 0, 0, 0, 0, 7, 0, 9, 1, 8, 0, 0, 3 }, { 13, 1, -1, -1, -1 }, { 1, 2, 0, 0, 0 }),
			ClothX("FemaleBusiness SkirtsThe Creative", { 0, 0, -1, 5, 24, 0, 20, 6, 23, 0, 0, 24 }, { 0, 0, 0, 0, 0, 0, 9, 1, 3, 0, 0, 10 }, { -1, 10, -1, -1, -1 }, { 0, 5, 0, 0, 0 }),
			ClothX("FemaleBusiness SkirtsThe Extrovert", { 0, 0, -1, 5, 24, 0, 20, 10, 23, 0, 0, 24 }, { 0, 0, 0, 0, 10, 0, 8, 0, 12, 0, 0, 1 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleBusiness SkirtsThe Firm", { 0, 0, -1, 6, 8, 0, 0, 11, 23, 0, 0, 7 }, { 0, 0, 0, 0, 4, 0, 0, 3, 1, 0, 0, 1 }, { 26, -1, -1, -1, -1 }, { 13, 0, 0, 0, 0 }),
			ClothX("FemaleBusiness SkirtsThe Fresco", { 0, 0, -1, 5, 24, 0, 20, 11, 0, 0, 0, 24 }, { 0, 0, 0, 0, 5, 0, 7, 3, 15, 0, 0, 8 }, { -1, 18, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleBusiness SkirtsThe Heritage", { 0, 0, -1, 0, 12, 0, 8, 13, 24, 0, 0, 28 }, { 0, 0, 0, 0, 2, 0, 0, 5, 2, 0, 0, 3 }, { 27, 4, -1, -1, -1 }, { 10, 4, 0, 0, 0 }),
			ClothX("FemaleBusiness SkirtsThe Leopard", { 0, 0, -1, 6, 24, 0, 0, 1, 13, 0, 0, 25 }, { 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 9 }, { -1, 19, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleBusiness SkirtsThe Purrfectionist", { 0, 0, -1, 4, 24, 0, 0, 2, 2, 0, 0, 13 }, { 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 15 }, { 26, -1, -1, -1, -1 }, { 8, 0, 0, 0, 0 }),
			ClothX("FemaleBusiness SkirtsThe Vanquish", { 0, 0, -1, 5, 8, 0, 19, 6, 23, 0, 0, 24 }, { 0, 0, 0, 0, 0, 0, 4, 1, 1, 0, 0, 6 }, { 26, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCasualThe Casual", { 0, 0, -1, 0, 16, 0, 2, 2, 3, 0, 0, 0 }, { 0, 0, 0, 0, 4, 0, 5, 1, 0, 0, 0, 11 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCasualThe Comfort", { 0, 0, -1, 2, 2, 0, 2, 5, 3, 0, 0, 2 }, { 0, 0, 0, 0, 2, 0, 0, 4, 0, 0, 0, 6 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCasualThe Daily", { 0, 0, -1, 9, 4, 0, 13, 1, 3, 0, 0, 9 }, { 0, 0, 0, 0, 9, 0, 12, 2, 0, 0, 0, 9 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCasualThe Easy", { 0, 0, -1, 3, 2, 0, 16, 2, 3, 0, 0, 3 }, { 0, 0, 0, 0, 0, 0, 6, 1, 0, 0, 0, 11 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCasualThe Homebody", { 0, 0, -1, 2, 3, 0, 16, 1, 3, 0, 0, 2 }, { 0, 0, 0, 0, 7, 0, 11, 0, 0, 0, 0, 15 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCasualThe Laid Back", { 0, 0, -1, 3, 3, 0, 16, 1, 3, 0, 0, 3 }, { 0, 0, 0, 0, 11, 0, 1, 3, 0, 0, 0, 10 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCatsuitsBlack & Red", { 21, 0, 0, 7, 102, 0, 77, 0, 14, 0, 0, 262 }, { 0, 0, 0, 0, 13, 0, 0, 0, 0, 0, 0, 13 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCatsuitsBlack", { 21, 0, 0, 7, 102, 0, 77, 0, 14, 0, 0, 262 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCatsuitsBold Abstract", { 21, 0, 0, 7, 102, 0, 77, 0, 14, 0, 0, 262 }, { 0, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 15 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCatsuitsBrown", { 21, 0, 0, 7, 102, 0, 77, 0, 14, 0, 0, 262 }, { 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 0, 2 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCatsuitsCobble", { 21, 0, 0, 7, 102, 0, 77, 0, 14, 0, 0, 262 }, { 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 11 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCatsuitsCrosshatch", { 21, 0, 0, 7, 102, 0, 77, 0, 14, 0, 0, 262 }, { 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 12 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCatsuitsGray Camo", { 21, 0, 0, 7, 102, 0, 77, 0, 14, 0, 0, 262 }, { 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 8 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCatsuitsGray", { 21, 0, 0, 7, 102, 0, 77, 0, 14, 0, 0, 262 }, { 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCatsuitsLight Woodland", { 21, 0, 0, 7, 102, 0, 77, 0, 14, 0, 0, 262 }, { 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 9 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCatsuitsOx Blood", { 21, 0, 0, 7, 102, 0, 77, 0, 14, 0, 0, 262 }, { 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 4 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCatsuitsPatterned", { 21, 0, 0, 7, 102, 0, 77, 0, 14, 0, 0, 262 }, { 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 14 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCatsuitsSplinter", { 21, 0, 0, 7, 102, 0, 77, 0, 14, 0, 0, 262 }, { 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 10 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCatsuitsTan", { 21, 0, 0, 7, 102, 0, 77, 0, 14, 0, 0, 262 }, { 0, 0, 0, 0, 3, 0, 3, 0, 0, 0, 0, 3 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCatsuitsTiger", { 21, 0, 0, 7, 102, 0, 77, 0, 14, 0, 0, 262 }, { 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 16 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCatsuitsWorn Brown", { 21, 0, 0, 7, 102, 0, 77, 0, 14, 0, 0, 262 }, { 0, 0, 0, 0, 6, 0, 2, 0, 0, 0, 0, 6 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCatsuitsWorn Gray", { 21, 0, 0, 7, 102, 0, 77, 0, 14, 0, 0, 262 }, { 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 7 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCatsuitsYellow", { 21, 0, 0, 7, 102, 0, 77, 0, 14, 0, 0, 262 }, { 0, 0, 0, 0, 5, 0, 4, 0, 0, 0, 0, 5 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOAlpha", { 0, 0, -1, 3, 84, 0, 30, 0, 51, 0, 0, 194 }, { 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 1 }, { -1, 11, -1, -1, -1 }, { 0, 3, 0, 0, 0 }),
			ClothX("FemaleCEOAnonymous", { 0, 106, -1, 147, 90, 0, 65, 0, 15, 0, 0, 207 }, { 0, 25, 0, 0, 9, 0, 3, 0, 0, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOArms Dealer", { 0, 0, -1, 3, 41, 0, 29, 20, 39, 0, 0, 97 }, { 0, 0, 0, 0, 2, 0, 2, 5, 4, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOBroker", { 0, 0, -1, 7, 54, 0, 0, 22, 38, 0, 0, 139 }, { 0, 0, 0, 0, 2, 0, 1, 6, 2, 0, 0, 2 }, { 28, -1, -1, -1, -1 }, { 4, 0, 0, 0, 0 }),
			ClothX("FemaleCEOBusiness Oligarch", { 0, 0, -1, 36, 41, 0, 29, 0, 67, 0, 0, 107 }, { 0, 0, 0, 0, 2, 0, 0, 0, 4, 0, 0, 0 }, { -1, 10, -1, -1, -1 }, { 0, 1, 0, 0, 0 }),
			ClothX("FemaleCEOChief", { 0, 0, -1, 3, 24, 0, 19, 0, 75, 0, 0, 134 }, { 0, 0, 0, 0, 3, 0, 9, 0, 1, 0, 0, 0 }, { -1, 14, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOClown", { 0, 95, -1, 49, 39, 0, 26, 0, 3, 0, 0, 60 }, { 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOCommander", { 0, 0, -1, 23, 95, 0, 24, 0, 14, 0, 0, 238 }, { 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 14 }, { 113, 11, -1, -1, -1 }, { 10, 1, 0, 0, 0 }),
			ClothX("FemaleCEOCreator", { 0, 0, -1, 7, 27, 0, 11, 0, 39, 0, 0, 66 }, { 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEODemon", { 0, 94, -1, 36, 81, 0, 53, 0, 3, 0, 0, 150 }, { 0, 4, 0, 0, 0, 0, 1, 0, 0, 0, 0, 7 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEODirector", { 0, 0, -1, 7, 64, 0, 0, 0, 13, 0, 0, 137 }, { 0, 0, 0, 0, 3, 0, 0, 0, 6, 0, 0, 2 }, { -1, 18, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEODrug Baron", { 0, 0, -1, 9, 0, 0, 38, 0, 2, 0, 0, 96 }, { 0, 0, 0, 0, 10, 0, 2, 0, 0, 0, 0, 0 }, { -1, 11, -1, -1, -1 }, { 0, 2, 0, 0, 0 }),
			ClothX("FemaleCEOEl Jefe", { 0, 0, -1, 3, 0, 0, 30, 0, 2, 18, 0, 103 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1 }, { 6, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOEntrepreneur", { 0, 0, -1, 3, 43, 0, 4, 84, 65, 0, 0, 100 }, { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 }, { 55, -1, -1, -1, -1 }, { 24, 0, 0, 0, 0 }),
			ClothX("FemaleCEOFat Cat", { 0, 0, -1, 3, 64, 0, 6, 23, 41, 0, 0, 58 }, { 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOFounder", { 0, 0, -1, 2, 0, 0, 10, 0, 2, 0, 0, 2 }, { 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1 }, { -1, -1, 0, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOGeneral", { 0, 0, -1, 3, 11, 0, 30, 0, 3, 0, 0, 135 }, { 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 2 }, { 14, 0, -1, -1, -1 }, { 0, 4, 0, 0, 0 }),
			ClothX("FemaleCEOGuerrilla", { 0, 0, -1, 174, 90, 0, 63, 0, 3, 0, 0, 231 }, { 0, 0, 0, 16, 16, 0, 3, 0, 0, 0, 0, 9 }, { 107, 11, -1, -1, -1 }, { 16, 1, 0, 0, 0 }),
			ClothX("FemaleCEOGunrunner", { 0, 0, -1, 180, 90, 0, 65, 0, 14, 0, 0, 232 }, { 0, 0, 0, 19, 17, 0, 7, 0, 0, 0, 0, 23 }, { 106, 11, -1, -1, -1 }, { 17, 6, 0, 0, 0 }),
			ClothX("FemaleCEOHedonist", { 0, 0, -1, 0, 57, 0, 38, 0, 3, 0, 0, 105 }, { 0, 0, 0, 0, 5, 0, 3, 0, 0, 0, 0, 5 }, { 2, 11, -1, -1, -1 }, { 1, 3, 0, 0, 0 }),
			ClothX("FemaleCEOIcon", { 0, 0, -1, 0, 85, 0, 31, 67, 3, 0, 0, 192 }, { 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0 }, { 95, -1, 13, -1, -1 }, { 6, 0, 0, 0, 0 }),
			ClothX("FemaleCEOInstigator", { 0, 107, -1, 55, 92, 0, 62, 0, 2, 0, 0, 226 }, { 0, 18, 0, 0, 20, 0, 20, 0, 0, 0, 0, 11 }, { 95, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOKingpin", { 0, 0, -1, 3, 50, 0, 37, 0, 66, 0, 0, 104 }, { 0, 0, 0, 0, 0, 0, 3, 0, 5, 0, 0, 0 }, { -1, 2, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOMastermind", { 0, 0, -1, 40, 85, 0, 7, 0, 3, 0, 0, 206 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 95, 24, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOMedia Mogul", { 0, 0, -1, 7, 0, 0, 3, 85, 55, 0, 0, 66 }, { 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0 }, { 58, -1, -1, -1, -1 }, { 2, 0, 0, 0, 0 }),
			ClothX("FemaleCEOMob Boss", { 0, 0, -1, 3, 51, 0, 37, 0, 3, 0, 0, 102 }, { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 }, { -1, 16, -1, -1, -1 }, { 0, 6, 0, 0, 0 }),
			ClothX("FemaleCEOMoon Curser", { 0, 104, -1, 174, 90, 0, 63, 0, 14, 0, 0, 230 }, { 0, 25, 0, 3, 3, 0, 0, 0, 0, 0, 0, 20 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOOil Tycoon", { 0, 0, -1, 36, 37, 0, 29, 0, 39, 0, 0, 65 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOOverseas Investor", { 0, 0, -1, 3, 37, 0, 29, 22, 38, 0, 0, 7 }, { 0, 0, 0, 0, 0, 0, 0, 6, 4, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOPioneer", { 0, 0, -1, 3, 1, 0, 10, 0, 2, 0, 0, 136 }, { 0, 0, 0, 0, 8, 0, 2, 0, 0, 0, 0, 3 }, { -1, 19, -1, -1, -1 }, { 0, 9, 0, 0, 0 }),
			ClothX("FemaleCEOPlayboy", { 0, 0, -1, 3, 63, 0, 41, 0, 76, 0, 0, 99 }, { 0, 0, 0, 0, 2, 0, 2, 0, 3, 0, 0, 2 }, { -1, 2, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOPremier", { 0, 0, -1, 31, 0, 0, 60, 0, 3, 1, 0, 73 }, { 0, 0, 0, 0, 0, 0, 6, 0, 0, 1, 0, 1 }, { -1, -1, 14, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOPresident", { 0, 0, -1, 7, 43, 0, 20, 0, 13, 0, 0, 69 }, { 0, 0, 0, 0, 4, 0, 0, 0, 6, 0, 0, 0 }, { -1, 11, -1, -1, -1 }, { 0, 2, 0, 0, 0 }),
			ClothX("FemaleCEORecon Leader", { 0, 0, -1, 41, 32, 0, 26, 0, 14, 12, 0, 232 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 20 }, { 111, -1, -1, -1, -1 }, { 4, 0, 0, 0, 0 }),
			ClothX("FemaleCEORingleader", { 0, 0, -1, 3, 80, 0, 60, 0, 69, 0, 0, 193 }, { 0, 0, 0, 0, 7, 0, 4, 0, 2, 0, 0, 0 }, { 55, 11, -1, -1, -1 }, { 19, 2, 0, 0, 0 }),
			ClothX("FemaleCEORoller", { 0, 0, -1, 139, 73, 0, 60, 38, 52, 0, 0, 167 }, { 0, 0, 0, 0, 1, 0, 10, 0, 1, 0, 0, 3 }, { -1, 11, 13, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOShipping Tycoon", { 0, 0, -1, 3, 54, 0, 30, 0, 3, 0, 0, 98 }, { 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2 }, { -1, 24, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOStandout", { 0, 0, -1, 4, 84, 0, 60, 0, 3, 0, 0, 195 }, { 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 5 }, { 95, -1, 16, -1, -1 }, { 1, 0, 0, 0, 0 }),
			ClothX("FemaleCEOSurvivalist", { 0, 0, -1, 174, 90, 0, 63, 0, 130, 0, 0, 216 }, { 0, 0, 0, 19, 6, 0, 7, 0, 6, 0, 0, 18 }, { 104, 25, -1, -1, -1 }, { 23, 6, 0, 0, 0 }),
			ClothX("FemaleCEOSyndicate Leader", { 0, 0, -1, 3, 55, 0, 29, 0, 38, 0, 0, 95 }, { 0, 0, 0, 0, 0, 0, 2, 0, 10, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleCEOTop Dog", { 0, 0, -1, 4, 83, 0, 60, 0, 3, 0, 0, 195 }, { 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 2 }, { 55, -1, 14, -1, -1 }, { 1, 0, 0, 0, 0 }),
			ClothX("FemaleCEOTrailblazer", { 0, 0, -1, 3, 80, 0, 60, 93, 59, 0, 0, 193 }, { 0, 0, 0, 0, 6, 0, 9, 0, 0, 0, 0, 23 }, { 95, 2, -1, -1, -1 }, { 0, 5, 0, 0, 0 }),
			ClothX("FemaleCEOVillain", { 0, 101, -1, 23, 80, 0, 60, 0, 3, 0, 0, 205 }, { 0, 13, 0, 0, 2, 0, 10, 0, 0, 0, 0, 25 }, { 101, 3, -1, -1, -1 }, { 7, 3, 0, 0, 0 }),
			ClothX("FemaleCEOWarlord", { 0, 0, -1, 7, 61, 0, 26, 53, 13, 0, 0, 34 }, { 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0 }, { 55, 25, -1, -1, -1 }, { 1, 9, 0, 0, 0 }),
			ClothX("FemaleDesignerThe Accessory", { 0, 0, -1, 14, 41, 0, 3, 37, 3, 0, 0, 68 }, { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 3 }, { 4, 3, 12, -1, -1 }, { 0, 7, 0, 0, 0 }),
			ClothX("FemaleDesignerThe Chic", { 0, 0, -1, 39, 43, 0, 30, 39, 45, 0, 0, 64 }, { 0, 0, 0, 0, 0, 0, 0, 1, 10, 0, 0, 3 }, { -1, 2, 3, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleDesignerThe Class", { 0, 0, -1, 6, 43, 0, 8, 0, 45, 0, 0, 65 }, { 0, 0, 0, 0, 0, 0, 6, 0, 11, 0, 0, 5 }, { -1, 16, 4, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleDesignerThe Elite", { 0, 0, -1, 7, 9, 0, 22, 0, 23, 0, 0, 65 }, { 0, 0, 0, 0, 7, 0, 0, 0, 1, 0, 0, 8 }, { -1, 16, 9, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleDesignerThe Golden Girl", { 0, 0, -1, 2, 16, 0, 20, 42, 3, 0, 0, 67 }, { 0, 0, 0, 0, 8, 0, 11, 0, 0, 0, 0, 0 }, { -1, -1, 7, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleDesignerThe Lavish", { 0, 0, -1, 6, 27, 0, 0, 0, 46, 0, 0, 66 }, { 0, 0, 0, 0, 9, 0, 2, 0, 2, 0, 0, 3 }, { -1, 4, 11, -1, -1 }, { 0, 1, 0, 0, 0 }),
			ClothX("FemaleDesignerThe Perseus", { 0, 0, -1, 14, 43, 0, 31, 41, 3, 0, 0, 68 }, { 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 5 }, { -1, 11, -1, -1, -1 }, { 0, 2, 0, 0, 0 }),
			ClothX("FemaleDesignerThe Puma", { 0, 0, -1, 6, 43, 0, 30, 29, 15, 0, 0, 66 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, 7, 4, -1, -1 }, { 0, 1, 0, 0, 0 }),
			ClothX("FemaleDesignerThe Shine", { 0, 0, -1, 6, 9, 0, 31, 0, 49, 0, 0, 64 }, { 0, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0 }, { 13, 11, 5, -1, -1 }, { 0, 2, 0, 0, 0 }),
			ClothX("FemaleDesignerThe Socialite", { 0, 0, -1, 3, 43, 0, 19, 0, 50, 0, 0, 65 }, { 0, 0, 0, 0, 2, 0, 2, 0, 2, 0, 0, 0 }, { -1, 7, 6, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleDesignerThe Status", { 0, 0, -1, 7, 26, 0, 19, 33, 13, 0, 0, 66 }, { 0, 0, 0, 0, 0, 0, 2, 0, 15, 0, 0, 2 }, { 22, 16, 11, -1, -1 }, { 1, 0, 0, 0, 0 }),
			ClothX("FemaleDesignerThe Wealth", { 0, 0, -1, 3, 43, 0, 30, 41, 45, 0, 0, 64 }, { 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0 }, { -1, 2, 8, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleHipsterThe 90s", { 0, 0, -1, 5, 27, 0, 22, 14, 28, 0, 0, 35 }, { 0, 0, 0, 0, 13, 0, 3, 2, 8, 0, 0, 1 }, { -1, 20, -1, -1, -1 }, { 0, 7, 0, 0, 0 }),
			ClothX("FemaleHipsterThe Bubblegum", { 0, 0, -1, 5, 27, 0, 22, 14, 28, 0, 0, 35 }, { 0, 0, 0, 0, 11, 0, 4, 3, 6, 0, 0, 3 }, { -1, 21, -1, -1, -1 }, { 0, 6, 0, 0, 0 }),
			ClothX("FemaleHipsterThe Craze", { 0, 0, -1, 5, 27, 0, 22, 14, 28, 0, 0, 31 }, { 0, 0, 0, 0, 12, 0, 5, 2, 7, 0, 0, 2 }, { 29, 20, -1, -1, -1 }, { 0, 3, 0, 0, 0 }),
			ClothX("FemaleHipsterThe Jungle", { 0, 0, -1, 6, 27, 0, 3, 0, 28, 0, 0, 34 }, { 0, 0, 0, 0, 2, 0, 10, 0, 3, 0, 0, 0 }, { -1, 21, -1, -1, -1 }, { 0, 3, 0, 0, 0 }),
			ClothX("FemaleHipsterThe Palms", { 0, 0, -1, 5, 27, 0, 22, 14, 26, 0, 0, 31 }, { 0, 0, 0, 0, 10, 0, 10, 2, 1, 0, 0, 3 }, { 29, 21, -1, -1, -1 }, { 2, 7, 0, 0, 0 }),
			ClothX("FemaleHipsterThe Roar", { 0, 0, -1, 4, 27, 0, 22, 11, 2, 0, 0, 33 }, { 0, 0, 0, 0, 8, 0, 7, 0, 0, 0, 0, 2 }, { -1, 21, -1, -1, -1 }, { 0, 0, 0, 0, 0 }),
			ClothX("FemaleHipsterThe Safari", { 0, 0, -1, 5, 25, 0, 21, 14, 4, 0, 0, 35 }, { 0, 0, 0, 0, 4, 0, 9, 0, 14, 0, 0, 9 }, { 28, 20, -1, -1, -1 }, { 1, 0, 0, 0, 0 }),
			ClothX("FemaleHipsterThe Tropics", { 0, 0, -1, 4, 25, 0, 3, 15, 2, 0, 0, 33 }, { 0, 0, 0, 0, 12, 0, 13, 0, 0, 0, 0, 4 }, { -1, 20, -1, -1, -1 }, { 0, 4, 0, 0, 0 }),
			ClothX("FemaleHipsterThe Vintage", { 0, 0, -1, 15, 21, 0, 22, 14, 2, 0, 0, 37 }, { 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 4 }, { -1, 20, -1, -1, -1 }, { 0, 1, 0, 0, 0 }),
			ClothX("FemaleHipsterThe Youth", { 0, 0, -1, 4, 27, 0, 11, 14, 2, 0, 0, 33 }, { 0, 0, 0, 0, 7, 0, 3, 1, 0, 0, 0, 5 }, { 29, 21, -1, -1, -1 }, { 3, 1, 0, 0, 0 }),
			ClothX("FemaleHipsterThe Yuppie", { 0, 0, -1, 5, 27, 0, 22, 14, 28, 0, 0, 31 }, { 0, 0, 0, 0, 6, 0, 5, 0, 0, 0, 0, 6 }, { 28, 20, -1, -1, -1 }, { 4, 1, 0, 0, 0 })
		};

		std::vector<std::string> MaFiles = ReadDirectory(DirectOutfitMale);
		std::vector<std::string> FeFiles = ReadDirectory(DirectOutfitFemale);

		if (MaFiles.size() == 0)
		{
			WriteFile(HasOutfits, ReadMe);
			for (int i = 0; i < (int)MaleOut.size(); i++)
				SaveOutfits(MaleOut[i], DirectOutfitMale);
		}

		if (FeFiles.size() == 0)
		{
			WriteFile(HasOutfits, ReadMe);
			for (int i = 0; i < (int)MaleOut.size(); i++)
				SaveOutfits(MaleOut[i], DirectOutfitFemale);
		}
	}
	
	int PlayerZinSesh()
	{
		return (int)PedList.size() + (int)AFKList.size();
	}
	bool WHileKeyDown(int key)
	{
		bool Output = false;
		while (IsKeyDown(KeyFind[key]))
		{
			if (!Output)
				Output = true;
			WAIT(1);
		}
		return Output;
	}
	void DisplayPlayers()
	{
		CAM::DO_SCREEN_FADE_IN(1);

		std::vector<std::string> PlayerZList1 = {};
		std::vector<std::string> PlayerZList2 = {};

		std::vector<std::string> PlayerZLevel1 = {};
		std::vector<std::string> PlayerZLevel2 = {};

		int Playerz = PlayerZinSesh();
		std::string Public = PZTranslate[4];
		if (PZData::MySettings.InviteOnly)
			Public = PZTranslate[5];
		std::string Caption = "GTA Online (" + Public + ", " + std::to_string(Playerz) + ")";

		for (int i = 0; i < PedList.size(); i++)
		{
			if (PlayerZList1.size() > 16)
			{
				PlayerZList2.push_back(PedList[i].MyName);
				PlayerZLevel2.push_back(std::to_string(PedList[i].Level));
			}
			else
			{
				PlayerZList1.push_back(PedList[i].MyName);
				PlayerZLevel1.push_back(std::to_string(PedList[i].Level));
			}
		}

		for (int i = 0; i < AFKList.size(); i++)
		{
			if (PlayerZList1.size() > 16)
			{
				PlayerZList2.push_back(AFKList[i].MyName);
				PlayerZLevel2.push_back(std::to_string(AFKList[i].Level));
			}
			else
			{
				PlayerZList1.push_back(AFKList[i].MyName);
				PlayerZLevel1.push_back(std::to_string(AFKList[i].Level));
			}
		}

		int WaitFor = InGameTime() + 7000;

		std::vector<std::string> sInstBut = { PZTranslate[4] };
		std::vector<int> iInstBut = { PZData::MySettings.Control_Keys_Players_List };

		float lineWidthA = 252.0;
		float lineHeightA = 5.0;
		float lineTopA = 11.5;
		float lineLeftA = 25.5;
		float textLeftA = 5.0;
		float textLeftA2 = 5.0;

		float lineWidthB = 252.0;
		float lineHeightB = -0.5;
		float lineTopB = 22.5;
		float lineLeftB = 25.5;
		float textLeftB = 9.0;
		float textLeftB2 = 219.5;

		bool bSwitch = true;
		DWORD waitTime = 0;

		while (true)
		{
			DWORD maxTickCount = GetTickCount() + waitTime;
			do
			{
				DrawSessionList(Caption, "", lineWidthA, lineHeightA, lineTopA, lineLeftA, textLeftA, textLeftA2, false, true, 0);
				if (bSwitch || Playerz < 16)
				{
					for (int i = 0; i < PlayerZList1.size(); i++)
					{
						DrawSessionList(PlayerZList1[i], PlayerZLevel1[i], lineWidthB, lineHeightB, (float)45.0 + (float)i * lineTopB, lineLeftB, textLeftB, textLeftB2, true, false, i);
					}
				}
				else
				{
					for (int i = 0; i < PlayerZList2.size(); i++)
					{
						DrawSessionList(PlayerZList2[i], PlayerZLevel2[i], lineWidthB, lineHeightB, (float)45.0 + (float)i * lineTopB, lineLeftB, textLeftB, textLeftB2, true, false, i);
					}
				}
				WAIT(0);
			} while (GetTickCount() < maxTickCount);
			waitTime = 500;

			if (GVM::ButtonDown(PZData::MySettings.Control_Keys_Players_List))
				bSwitch = !bSwitch;

			if (WaitFor < InGameTime())
				break;
		}
	}

	void TimeOutThisPed(Ped peddy)
	{
		int PedInList = ReteaveBrain(peddy);
		if (PedInList < (int)PedList.size() && PedInList > -1)
				PedList[PedInList].TimeOn = 0;
	}
	bool PlayerOnline(const std::string& id)
	{
		bool b = false;
		for (int i = 0; i < (int)PedList.size(); i++)
		{
			if (id == PedList[i].MyIdentity)
			{
				b = true;
				break;
			}
		}
		return b;
	}

	void FindAddCars()
	{
		const std::string sVehList01 = GetDir() + "/PlayerZero/Vehicles/StandardRoadVehicles.ini";
		PreVeh_01 = ReadFile(sVehList01);
		if (PreVeh_01.size() == 0)
		{
			const std::vector<std::string> PreVehSet_01 = {
				"PFISTER811", //><!-- 811 -->
				"ADDER", //>
				"AUTARCH", //>
				"BANSHEE2", //><!-- Banshee 900R -->
				"BULLET", //>
				"CHAMPION", //>
				"CHEETAH", //>
				"CYCLONE", //>
				"DEVESTE", //>
				"EMERUS", //>
				"ENTITYXF", //>
				"ENTITY2", //><!-- Entity XXR -->
				"SHEAVA", //><!-- ETR1 -->
				"FMJ", //>
				"FURIA", //>
				"GP1", //>
				"IGNUS", //>
				"INFERNUS", //>
				"ITALIGTB", //>
				"ITALIGTB2", //><!-- Itali GTB Custom -->
				"KRIEGER", //>
				"LM87", //>
				"OSIRIS", //>
				"NERO", //>
				"NERO2", //><!-- Nero Custom -->
				"PENETRATOR", //>
				"LE7B", //><!-- RE-7B -->
				"REAPER", //>
				"S80", //>
				"SC1", //>
				"SULTANRS", //>
				"T20", //>
				"TAIPAN", //>
				"TEMPESTA", //>
				"TEZERACT", //>
				"THRAX", //>
				"TIGON", //>
				"TORERO2", //><!-- Torero XO -->
				"TURISMOR", //>
				"TYRANT", //>
				"TYRUS", //>
				"VACCA", //>
				"VAGNER", //>
				"VISIONE", //>
				"VOLTIC", //>
				"PROTOTIPO", //><!-- X80 Proto -->
				"XA21", //>
				"ZENO", //>
				"ZENTORNO", //>
				"ZORRUSSO", //>
				"COGCABRIO", //>
				"EXEMPLAR", //>
				"F620", //>
				"FELON", //>
				"FELON2", //><!-- Felon GT -->
				"JACKAL", //>
				"KANJOSJ", //>
				"ORACLE", //>
				"ORACLE2", //><!-- Oracle XS -->
				"POSTLUDE", //>
				"PREVION", //>
				"SENTINEL2", //><!-- Sentinel -->
				"SENTINEL", //><!-- Sentinel XS -->
				"WINDSOR", //>
				"WINDSOR2", //><!-- Windsor Drop -->
				"ZION", //>
				"ZION2", //><!-- Zion Cabrio -->
				"DRAFTER", //><!-- 8F Drafter -->
				"NINEF", //>
				"NINEF2", //><!-- 9F Cabrio -->
				"TENF", //>
				"TENF2", //><!-- 10F Widebody -->
				"ALPHA", //>
				"ZR380", //><!-- Apocalypse ZR380 -->
				"BANSHEE", //>
				"BESTIAGTS", //>
				"BLISTA2", //><!-- Blista Compact -->
				"BUFFALO", //>
				"BUFFALO2", //><!-- Buffalo S -->
				"CALICO", //><!-- Calico GTF -->
				"CARBONIZZARE", //>
				"COMET2", //><!-- Comet -->
				"COMET3", //><!-- Comet Retro Custom -->
				"COMET6", //><!-- Comet S2 -->
				"COMET7", //><!-- Comet S2 Cabrio -->
				"COMET4", //><!-- Comet Safari -->
				"COMET5", //><!-- Comet SR -->
				"COQUETTE", //>
				"COQUETTE4", //><!-- Coquette D10 -->
				"CORSITA", //>
				"CYPHER", //>
				"TAMPA2", //><!-- Drift Tampa -->
				"ELEGY", //><!-- Elegy Retro Custom -->
				"ELEGY2", //><!-- Elegy RH8 -->
				"EUROS", //>
				"FELTZER2", //><!-- Feltzer -->
				"FLASHGT", //>
				"FUROREGT", //>
				"FUSILADE", //>
				"FUTO", //>
				"FUTO2", //><!-- Futo GTX -->
				"ZR3802", //><!-- Future Shock ZR380 -->
				"GB200", //>
				"GROWLER", //>
				"HOTRING", //>
				"IMORGON", //>
				"ISSI7", //><!-- Issi Sport -->
				"ITALIGTO", //>
				"ITALIRSX", //>
				"JESTER", //>
				"JESTER2", //><!-- Jester (Racecar) -->
				"JESTER3", //><!-- Jester Classic -->
				"JESTER4", //><!-- Jester RR -->
				"JUGULAR", //>
				"KHAMELION", //>
				"KOMODA", //>
				"KURUMA", //>
				"KURUMA2", //><!-- Kuruma (Armored) -->
				"LOCUST", //>
				"LYNX", //>
				"MASSACRO", //>
				"MASSACRO2", //><!-- Massacro (Racecar) -->
				"NEO", //>
				"NEON", //>
				"ZR3803", //><!-- Nightmare ZR380 -->
				"OMNIS", //>
				"OMNISEGT", //>
				"PARAGON", //>
				"PARAGON2", //><!-- Paragon R (Armored) -->
				"PARIAH", //>
				"PENUMBRA", //>
				"PENUMBRA2", //><!-- Penumbra FF -->
				"RAIDEN", //>
				"RAPIDGT", //>
				"RAPIDGT2", //><!-- Rapid GT Cabrio -->
				"RAPTOR", //>
				"REMUS", //>
				"REVOLTER", //>
				"RT3000", //>
				"RUSTON", //>
				"SCHAFTER4", //><!-- Schafter LWB -->
				"SCHAFTER3", //><!-- Schafter V12 -->
				"SCHLAGEN", //>
				"SCHWARZER", //>
				"SENTINEL3", //><!-- Sentinel Classic -->
				"SENTINEL4", //><!-- Sentinel Classic Widebody -->
				"SEVEN70", //>
				"SM722", //>
				"SPECTER", //>
				"SPECTER2", //><!-- Specter Custom -->
				"BUFFALO3", //><!-- Sprunk Buffalo -->
				"STREITER", //>
				"SUGOI", //>
				"SULTAN", //>
				"SULTAN2", //><!-- Sultan Classic -->
				"SULTAN3", //><!-- Sultan RS Classic -->
				"SURANO", //>
				"TROPOS", //>
				"VSTR", //><!-- V-STR -->
				"VECTRE", //>
				"VERLIERER2", //>
				"VETO", //><!-- Veto Classic -->
				"VETO2", //><!-- Veto Modern -->
				"ZR350", //>
				"DUKES3", //><!-- Beater Dukes -->
				"BLADE", //>
				"BUCCANEER", //>
				"BUCCANEER2", //><!-- Buccaneer Custom -->
				"BUFFALO4", //><!-- Buffalo STX -->
				"STALION2", //><!-- Burger Shot Stallion -->
				"CHINO", //>
				"CHINO2", //><!-- Chino Custom -->
				"CLIQUE", //>
				"COQUETTE3", //><!-- Coquette BlackFin -->
				"DEVIANT", //>
				"DOMINATOR", //>
				"DOMINATOR7", //><!-- Dominator ASP -->
				"DOMINATOR8", //><!-- Dominator GTT -->
				"DOMINATOR3", //><!-- Dominator GTX -->
				"YOSEMITE2", //><!-- Drift Yosemite -->
				"DUKES2", //><!-- Duke O'Death -->
				"DUKES", //>
				"ELLIE", //>
				"FACTION", //>
				"FACTION2", //><!-- Faction Custom -->
				"FACTION3", //><!-- Faction Custom Donk -->
				"DOMINATOR5", //><!-- Future Shock Dominator -->
				"IMPALER3", //><!-- Future Shock Impaler -->
				"IMPERATOR2", //><!-- Future Shock Imperator -->
				"SLAMVAN5", //><!-- Future Shock Slamvan -->
				"GAUNTLET", //>
				"GAUNTLET3", //><!-- Gauntlet Classic -->
				"GAUNTLET5", //><!-- Gauntlet Classic Custom -->
				"GAUNTLET4", //><!-- Gauntlet Hellfire -->
				"GREENWOOD", //>
				"HERMES", //>
				"HOTKNIFE", //>
				"HUSTLER", //>
				"IMPALER", //>
				"SLAMVAN2", //><!-- Lost Slamvan -->
				"LURCHER", //>
				"MANANA2", //><!-- Manana Custom -->
				"MOONBEAM", //>
				"MOONBEAM2", //><!-- Moonbeam Custom -->
				"DOMINATOR6", //><!-- Nightmare Dominator -->
				"IMPALER4", //><!-- Nightmare Impaler -->
				"IMPERATOR3", //><!-- Nightmare Imperator -->
				"SLAMVAN6", //><!-- Nightmare Slamvan -->
				"NIGHTSHADE", //>
				"PEYOTE2", //><!-- Peyote Gasser -->
				"PHOENIX", //>
				"PICADOR", //>
				"DOMINATOR2", //><!-- Pisswasser Dominator -->
				"RATLOADER", //>
				"RATLOADER2", //><!-- Rat-Truck -->
				"GAUNTLET2", //><!-- Redwood Gauntlet -->
				"RUINER", //>
				"RUINER3", //><!-- Ruiner 2000 wreck -->
				"RUINER4", //><!-- Ruiner ZZ-8 -->
				"SABREGT", //>
				"SABREGT2", //><!-- Sabre Turbo Custom -->
				"SLAMVAN", //>
				"SLAMVAN3", //><!-- Slamvan Custom -->
				"STALION", //>
				"TAMPA", //>
				"TULIP", //>
				"VAMOS", //>
				"VIGERO", //>
				"VIGERO2", //><!-- Vigero ZX -->
				"VIRGO", //>
				"VIRGO3", //><!-- Virgo Classic -->
				"VIRGO2", //><!-- Virgo Classic Custom -->
				"VOODOO", //>
				"VOODOO2", //><!-- Voodoo Custom -->
				"WEEVIL2", //><!-- Weevil Custom -->
				"YOSEMITE", //>
				"Z190", //><!-- 190z -->
				"CASCO", //>
				"CHEBUREK", //>
				"CHEETAH2", //><!-- Cheetah Classic -->
				"COQUETTE2", //><!-- Coquette Classic -->
				"DYNASTY", //>
				"FAGALOA", //>
				"BTYPE2", //><!-- Fr�nken Stange -->
				"GT500", //>
				"INFERNUS2", //><!-- Infernus Classic -->
				"JB700", //>
				"MAMBA", //>
				"MANANA", //>
				"MICHELLI", //>
				"MONROE", //>
				"NEBULA", //>
				"PEYOTE", //>
				"PEYOTE3", //><!-- Peyote Custom -->
				"PIGALLE", //>
				"RAPIDGT3", //><!-- Rapid GT Classic -->
				"RETINUE", //>
				"RETINUE2", //><!-- Retinue MkII -->
				"BTYPE", //><!-- Roosevelt -->
				"BTYPE3", //><!-- Roosevelt Valor -->
				"SAVESTRA", //>
				"STINGER", //>
				"STINGERGT", //>
				"FELTZER3", //><!-- Stirling GT -->
				"SWINGER", //>
				"TORERO", //>
				"TORNADO", //>
				"TORNADO2", //><!-- Tornado Cabrio -->
				"TORNADO3", //><!-- Rusty Tornado -->
				"TORNADO4", //><!-- Mariachi Tornado -->
				"TORNADO5", //><!-- Tornado Custom -->
				"TORNADO6", //><!-- Tornado Rat Rod -->
				"TURISMO2", //><!-- Turismo Classic -->
				"VISERIS", //>
				"ZTYPE", //>
				"ZION3", //><!-- Zion Classic -->
				"CERBERUS", //><!-- Apocalypse Cerberus -->
				"CERBERUS2", //><!-- Future Shock Cerberus -->
				"GUARDIAN", //>
				"CERBERUS3", //><!-- Nightmare Cerberus -->
				"ASEA", //>
				"ASTEROPE", //>
				"CINQUEMILA", //>
				"COGNOSCENTI", //>
				"COGNOSCENTI2", //><!-- Cognoscenti (Armored) -->
				"COG55", //><!-- Cognoscenti 55 -->
				"COG552", //><!-- Cognoscenti 55 (Armored) -->
				"DEITY", //>
				"EMPEROR", //>
				"EMPEROR2", //><!-- Emperor beater variant -->
				"FUGITIVE", //>
				"GLENDALE", //>
				"GLENDALE2", //><!-- Glendale Custom -->
				"INGOT", //>
				"INTRUDER", //>
				"PREMIER", //>
				"PRIMO", //>
				"PRIMO2", //><!-- Primo Custom -->
				"REGINA", //>
				"RHINEHART", //>
				"ROMERO", //>
				"SCHAFTER2", //>
				"SCHAFTER6", //><!-- Schafter LWB (Armored) -->
				"SCHAFTER5", //><!-- Schafter V12 (Armored) -->
				"STAFFORD", //>
				"STANIER", //>
				"STRATUM", //>
				"STRETCH", //>
				"SUPERD", //>
				"SURGE", //>
				"TAILGATER", //>
				"TAILGATER2", //><!-- Tailgater S -->
				"WARRENER", //>
				"WARRENER2", //><!-- Warrener HKR -->
				"WASHINGTON", //>
				"BRUISER", //><!-- Apocalypse Bruiser -->
				"BRUTUS", //><!-- Apocalypse Brutus -->
				"MONSTER3", //><!-- Apocalypse Sasquatch -->
				"BIFTA", //>
				"BLAZER", //>
				"BLAZER5", //><!-- Blazer Aqua -->
				"BLAZER2", //><!-- Blazer Lifeguard -->
				"BODHI2", //>
				"BRAWLER", //>
				"CARACARA2", //><!-- Caracara 4x4 -->
				"TROPHYTRUCK2", //><!-- Desert Raid -->
				"DRAUGUR", //>
				"DUBSTA3", //><!-- Dubsta 6x6 -->
				"DUNE", //>
				"DUNE3", //><!-- Dune FAV -->
				"DLOADER", //>
				"EVERON", //>
				"FREECRAWLER", //>
				"BRUISER2", //><!-- Future Shock Bruiser -->
				"BRUTUS2", //><!-- Future Shock Brutus -->
				"MONSTER4", //><!-- Future Shock Sasquatch -->
				"HELLION", //>
				"BLAZER3", //><!-- Hot Rod Blazer -->
				"BFINJECTION", //>
				"KALAHARI", //>
				"KAMACHO", //>
				"MONSTER", //><!-- Liberator -->
				"MARSHALL", //>
				"MESA3", //><!-- Merryweather Mesa -->
				"BRUISER3", //><!-- Nightmare Bruiser -->
				"BRUTUS3", //><!-- Nightmare Brutus -->
				"MONSTER5", //><!-- Nightmare Sasquatch -->
				"OUTLAW", //>
				"PATRIOT3", //><!-- Patriot Mil-Spec -->
				"DUNE4", //><!-- Ramp Buggy mission variant -->
				"DUNE5", //><!-- Ramp Buggy -->
				"RANCHERXL", //>
				"REBEL2", //>
				"RIATA", //>
				"REBEL", //><!-- Rusty Rebel -->
				"SANDKING2", //><!-- Sandking SWB -->
				"SANDKING", //><!-- Sandking XL -->
				"DUNE2", //><!-- Space Docker -->
				"BLAZER4", //><!-- Street Blazer -->
				"TROPHYTRUCK", //>
				"VAGRANT", //>
				"VERUS", //>
				"WINKY", //>
				"YOSEMITE3", //><!-- Yosemite Rancher -->
				"ZHABA", //>
				"ASTRON", //>
				"BALLER", //>
				"BALLER2", //><!-- Baller 2nd gen variant -->
				"BALLER3", //><!-- Baller LE -->
				"BALLER5", //><!-- Baller LE (Armored) -->
				"BALLER4", //><!-- Baller LE LWB -->
				"BALLER6", //><!-- Baller LE LWB (Armored) -->
				"BALLER7", //><!-- Baller ST -->
				"BJXL", //>
				"CAVALCADE", //>
				"CAVALCADE2", //><!-- Cavalcade 2nd gen variant -->
				"CONTENDER", //>
				"DUBSTA", //>
				"DUBSTA2", //><!-- Dubsta black variant -->
				"FQ2", //>
				"GRANGER", //>
				"GRANGER2", //><!-- Granger 3600LX -->
				"GRESLEY", //>
				"HABANERO", //>
				"HUNTLEY", //>
				"IWAGEN", //>
				"JUBILEE", //>
				"LANDSTALKER", //>
				"LANDSTALKER2", //><!-- Landstalker XL -->
				"MESA", //>
				"NOVAK", //>
				"PATRIOT", //>
				"PATRIOT2", //><!-- Patriot Stretch -->
				"RADI", //>
				"REBLA", //>
				"ROCOTO", //>
				"SEMINOLE", //>
				"SEMINOLE2", //><!-- Seminole Frontier -->
				"SERRANO", //>
				"SQUADDIE", //>
				"TOROS", //>
				"XLS", //>
				"XLS2", //><!-- XLS (Armored) -->
				"ISSI4", //><!-- Apocalypse Issi -->
				"ASBO", //>
				"BLISTA", //>
				"KANJO", //><!-- Blista Kanjo -->
				"BRIOSO", //>
				"BRIOSO2", //><!-- Brioso 300 -->
				"BRIOSO3", //><!-- Brioso 300 Widebody -->
				"CLUB", //>
				"DILETTANTE", //>
				"ISSI5", //><!-- Future Shock Issi -->
				"ISSI2", //>
				"ISSI3", //><!-- Issi Classic -->
				"ISSI6", //><!-- Nightmare Issi -->
				"PANTO", //>
				"PRAIRIE", //>
				"RHAPSODY", //>
				"WEEVIL", //>
				"CONTENDER", //>
				"DUBSTA3", //><!-- Dubsta 6x6 -->
				"GUARDIAN", //>
				"PICADOR", //>
				"SADLER", //>
				"SLAMVAN", //>
				"SLAMVAN3", //><!-- Slamvan Custom -->
				"YOSEMITE", //>
				"BOXVILLE5", //><!-- Armored Boxville -->
				"BISON", //>
				"BOBCATXL", //>
				"BURRITO", //>
				"CAMPER", //>
				"GBURRITO", //><!-- Gang Burrito Lost MC variant -->
				"GBURRITO2", //><!-- Gang Burrito heist variant -->
				"JOURNEY", //>
				"MINIVAN", //>
				"MINIVAN2", //><!-- Minivan Custom -->
				"PARADISE", //>
				"PONY", //>
				"PONY2", //><!-- Pony Smoke on the Water variant -->
				"RUMPO", //>
				"RUMPO2", //><!-- Rumpo Deludamol variant -->
				"RUMPO3", //><!-- Rumpo Custom -->
				"SPEEDO", //>
				"SPEEDO4", //><!-- Speedo Custom -->
				"SURFER", //>
				"SURFER2", //><!-- Surfer beater variant -->
				"TACO", //>
				"YOUGA", //>
				"YOUGA2", //><!-- Youga Classic -->
				"YOUGA3", //><!-- Youga Classic 4x4 -->
				"YOUGA4", //><!-- Youga Custom -->
				"BRICKADE", //>
				"RALLYTRUCK", //><!-- Dune -->
				"SLAMTRUCK", //>
				"WASTLNDR",
				"AKUMA", //>
				"DEATHBIKE", //><!-- Apocalypse Deathbike -->
				"AVARUS", //>
				"BAGGER", //>
				"BATI", //>
				"BATI2", //><!-- Bati 801RR -->
				"BF400", //>
				"CARBONRS", //>
				"CHIMERA", //>
				"CLIFFHANGER", //>
				"DAEMON", //><!-- Daemon Lost MC variant -->
				"DAEMON2", //><!-- Daemon Bikers DLC variant -->
				"DEFILER", //>
				"DIABLOUS", //>
				"DIABLOUS2", //><!-- Diabolus Custom -->
				"DOUBLE", //>
				"ENDURO", //>
				"ESSKEY", //>
				"FAGGIO2", //>
				"FAGGIO3", //><!-- Faggio Mod -->
				"FAGGIO", //><!-- Faggio Sport -->
				"FCR", //>
				"FCR2", //><!-- FCR 1000 Custom -->
				"DEATHBIKE2", //><!-- Future Shock Deathbike -->
				"GARGOYLE", //>
				"HAKUCHOU", //>
				"HAKUCHOU2", //><!-- Hakuchou Drag -->
				"HEXER", //>
				"INNOVATION", //>
				"LECTRO", //>
				"MANCHEZ", //>
				"MANCHEZ2", //><!-- Manchez Scout -->
				"NEMESIS", //>
				"NIGHTBLADE", //>
				"DEATHBIKE3", //><!-- Nightmare Deathbike -->
				"PCJ", //>
				"RROCKET", //><!-- Rampant Rocket -->
				"RATBIKE", //>
				"REEVER", //>
				"RUFFIAN", //>
				"SANCHEZ", //><!-- Sanchez livery variant -->
				"SANCHEZ2", //>
				"SANCTUS", //>
				"SHINOBI", //>
				"SHOTARO", //>
				"SOVEREIGN", //>
				"STRYDER", //>
				"THRUST", //>
				"VADER", //>
				"VINDICATOR", //>
				"VORTEX", //>
				"WOLFSBANE", //>
				"ZOMBIEA", //><!-- Zombie Bobber -->
				"ZOMBIEB", //><!-- Zombie Chopper -->  
				"aleutian", // SUVs
				"baller8", // SUVs
				"cavalcade3", // SUVs
				"dorado", // SUVs
				"vivanite", // SUVs
				"asterope2", // Sedans
				"impaler5", // Sedans
				"dominator9", // Muscle
				"driftyosemite", // Muscle
				"impaler6", // Muscle
				"Vigero3", // Muscle
				"turismo3", // Super
				"drifteuros", // Sports
				"driftfuto", // Sports
				"driftjester", // Sports
				"driftremus", // Sports
				"drifttampa", // Sports
				"driftzr350", // Sports
				"driftfr36", // Coupes
				"fr36", // Coupes
				"terminus", // Off-Road
				"polgauntlet", //  Emergency
			};

			PreVeh_01 = PreVehSet_01;
			WriteFile(sVehList01, PreVeh_01);
		}

		const std::string sVehList02 = GetDir() + "/PlayerZero/Vehicles/HeliNoWeapons.ini";
		PreVeh_02 = ReadFile(sVehList02);
		if (PreVeh_02.size() == 0)
		{
			const std::vector<std::string> PreVehSet_02 = {
				"BUZZARD2", //><!-- Buzzard -->
				"CARGOBOB", //><!-- Military Cargobob -->
				"CARGOBOB2", //><!-- Jetsam Cargobob -->
				"CONADA", //>
				"FROGGER", //>
				"FROGGER2", //><!-- Frogger Trevor Philips Industries variant -->
				"MAVERICK", //>
				"POLMAV", //>
				"SUPERVOLITO", //>
				"SUPERVOLITO2", //><!-- SuperVolito Carbon -->
				"SWIFT", //>
				"SWIFT2", //><!-- Swift Deluxe -->
				"VOLATUS", //>
			};

			PreVeh_02 = PreVehSet_02;
			WriteFile(sVehList02, PreVeh_02);
		}

		const std::string sVehList03 = GetDir() + "/PlayerZero/Vehicles/PlaneNoWeapons.ini";
		PreVeh_03 = ReadFile(sVehList03);
		if (PreVeh_03.size() == 0)
		{
			const std::vector<std::string> PreVehSet_03 = {
				"CUBAN800", //>
				"DODO", //>
				"DUSTER", //>
				"LUXOR", //>
				"LUXOR2", //><!-- Luxor Deluxe -->
				"MAMMATUS", //>
				"MILJET", //>
				"NIMBUS", //>
				"BOMBUSHKA", //><!-- RM-10 Bombushka -->
				"ALKONOST", //><!-- RO-86 Alkonost -->
				"SHAMAL", //>
				"TITAN", //>
				"VELUM", //>
				"VELUM2", //><!-- Velum 5-Seater -->
				"VESTRA", //>
				"VOLATOL", //>
				"ALPHAZ1", //>
				"BESRA", //>
				"HOWARD", //><!-- Howard NX-25 -->
				"STUNT", //><!-- Mallard -->
				"STREAMER216"
			};

			PreVeh_03 = PreVehSet_03;
			WriteFile(sVehList03, PreVeh_03);
		}

		const std::string sVehList04 = GetDir() + "/PlayerZero/Vehicles/HeliWithWeapons.ini";
		PreVeh_04 = ReadFile(sVehList04);
		if (PreVeh_04.size() == 0)
		{
			const std::vector<std::string> PreVehSet_04 = {
				"AKULA", //>
				"ANNIHILATOR", //>
				"ANNIHILATOR2", //><!-- Annihilator Stealth -->
				"BUZZARD", //><!-- Buzzard Attack Chopper -->
				"HUNTER", //><!-- FH-1 Hunter -->
				"SAVAGE", //>
				"SEASPARROW", //>
				"SEASPARROW2", //><!-- Sparrow -->
				"VALKYRIE", //>
				"VALKYRIE2", //><!-- Valkyrie MOD.0 -->
				"HAVOK", //>
				"Conada2"
			};

			PreVeh_04 = PreVehSet_04;
			WriteFile(sVehList04, PreVeh_04);
		}

		const std::string sVehList05 = GetDir() + "/PlayerZero/Vehicles/PlaneWithWeapons.ini";
		PreVeh_05 = ReadFile(sVehList05);
		if (PreVeh_05.size() == 0)
		{
			const std::vector<std::string> PreVehSet_05 = {
				"MOGUL", //>
				"PYRO", //>
				"SEABREEZE", //>
				"TULA", //>
				"STRIKEFORCE", //><!-- B-11 Strikeforce -->
				"HYDRA", //>
				"STARLING", //><!-- LF-22 Starling -->
				"NOKOTA", //><!-- P-45 Nokota -->
				"LAZER", //><!-- P-996 LAZER -->
				"ROGUE", //>
				"MICROLIGHT", //><!-- Ultralight -->
				"MOLOTOK", //><!-- V-65 Molotok -->
				"RAIJU"
			};
			
			PreVeh_05 = PreVehSet_05;
			WriteFile(sVehList05, PreVeh_05);
		}

		const std::string sVehList06 = GetDir() + "/PlayerZero/Vehicles/WeaponisedRoadVehicles.ini";
		PreVeh_06 = ReadFile(sVehList06);		
		if (PreVeh_06.size() == 0)
		{
			const std::vector<std::string> PreVehSet_06 = {
				"LIMO2", //><!-- Turreted Limo -->
				"INSURGENT", //>
				"INSURGENT2", //><!-- Insurgent Pick-Up -->
				"INSURGENT3", //><!-- Insurgent Pick-Up Custom -->
				"NIGHTSHARK", //>
				"CARACARA", //>
				"MENACER", //>
				"TECHNICAL", //>
				"TECHNICAL2", //><!-- Technical Aqua -->
				"TECHNICAL3", //><!-- Technical Custom -->
				"SCARAB", //><!-- Apocalypse Scarab -->
				"APC", //>
				"HALFTRACK", //>
				"SCARAB2", //><!-- Future Shock Scarab -->
				"SCARAB3", //><!-- Nightmare Scarab -->
				"RIOT2", //><!-- RCV -->
				"RHINO", //>
				"KHANJALI" //><!-- TM-02 Khanjali -->
			};
			
			PreVeh_06 = PreVehSet_06;
			WriteFile(sVehList06, PreVeh_06);
		}
	}

	void ReBuildIni()
	{
		const std::vector<std::string> AddIni = {
			"[Settings]",
			"Pz_Lang=" + std::to_string(MySettings.Pz_Lang),
			"Aggression=" + std::to_string(MySettings.Aggression),
			"MaxPlayers=" + std::to_string(MySettings.MaxPlayers),
			"MinWait=" + std::to_string(MySettings.MinWait),
			"MaxWait=" + std::to_string(MySettings.MaxWait),
			"MinSession=" + std::to_string(MySettings.MinSession),
			"MaxSession=" + std::to_string(MySettings.MaxSession),
			"MinAccuracy=" + std::to_string(MySettings.AccMin),
			"MaxAccuracy=" + std::to_string(MySettings.AccMax),
			"AirVeh=" + std::to_string(MySettings.AirVeh),
			"TFHTA=" + std::to_string(MySettings.TFHTA),
			"FriendlyFire=" + BoolToString(MySettings.FriendlyFire),
			"SpaceWeaps=" + BoolToString(MySettings.SpaceWeaps),
			"NameTags=" + BoolToString(MySettings.NameTags),
			"PassiveMode=" + BoolToString(MySettings.PassiveMode),
			"Debug=" + BoolToString(MySettings.Debugger),
			"PlayerzBlips=" + BoolToString(MySettings.PlayerzBlips),
			"NoNotify=" + BoolToString(MySettings.NoNotify),
			"MobileData=" + BoolToString(MySettings.MobileData),
			"InviteOnly=" + BoolToString(MySettings.InviteOnly),
			"BackChat=" + BoolToString(MySettings.BackChat),
			"MenuLeftSide=" + BoolToString(MySettings.MenuLeftSide),
			"[Controls]",
			"Control_A_Open_Menu=" + std::to_string(MySettings.Control_A_Open_Menu),
			"Control_B_Open_Menu=" + std::to_string(MySettings.Control_B_Open_Menu),
			"Control_A_Clear_Session=" + std::to_string(MySettings.Control_A_Clear_Session),
			"Control_B_Clear_Session=" + std::to_string(MySettings.Control_B_Clear_Session),
			"Control_A_Invite_Only=" + std::to_string(MySettings.Control_A_Invite_Only),
			"Control_B_Invite_Only=" + std::to_string(MySettings.Control_B_Invite_Only),
			"Keys_Open_Menu=" + std::to_string(MySettings.Keys_Open_Menu),
			"Keys_Clear_Session=" + std::to_string(MySettings.Keys_Clear_Session),
			"Keys_Invite_Only=" + std::to_string(MySettings.Keys_Invite_Only),
			"Control_Keys_Players_List=" + std::to_string(MySettings.Control_Keys_Players_List),
			"Control_Keys_AddPed=" + std::to_string(MySettings.Control_Keys_AddPed),
			"Control_Keys_EnterVeh=" + std::to_string(MySettings.Control_Keys_EnterVeh),
			"Control_Keys_DissmisPed=" + std::to_string(MySettings.Control_Keys_DissmisPed)
		};

		WriteFile(PzSettingsDir, AddIni);
	}
	void FindSettings()
	{
		LoggerLight("FindSettings");

		std::vector<std::string> setString = ReadFile(PzSettingsDir);

		if ((int)setString.size() == 0)
			FirstRun = true;
		else
		{
			for (int i = 0; i < (int)setString.size(); i++)
			{
				std::string line = setString[i];
				if (StringContains("Pz_Lang", line))
					MySettings.Pz_Lang = StingNumbersInt(line);
				else if (StringContains("Aggression", line))
					MySettings.Aggression = StingNumbersInt(line);
				else if (StringContains("MaxPlayers", line))
					MySettings.MaxPlayers = StingNumbersInt(line);
				else if (StringContains("MinWait", line))
					MySettings.MinWait = StingNumbersInt(line);
				else if (StringContains("MaxWait", line))
					MySettings.MaxWait = StingNumbersInt(line);
				else if (StringContains("MinSession", line))
					MySettings.MinSession = StingNumbersInt(line);
				else if (StringContains("MaxSession", line))
					MySettings.MaxSession = StingNumbersInt(line);
				else if (StringContains("MinAccuracy", line))
					MySettings.AccMin = StingNumbersInt(line);
				else if (StringContains("MaxAccuracy", line))
					MySettings.AccMax = StingNumbersInt(line);
				else if (StringContains("AirVeh", line))
					MySettings.AirVeh = StingNumbersInt(line);
				else if (StringContains("TFHTA", line))
					MySettings.TFHTA = StingNumbersInt(line);
				else if (StringContains("FriendlyFire", line))
					MySettings.FriendlyFire = StringBool(line);
				else if (StringContains("SpaceWeaps", line))
					MySettings.SpaceWeaps = StringBool(line);
				else if (StringContains("NameTags", line))
					MySettings.NameTags = StringBool(line);
				else if (StringContains("PassiveMode", line))
					MySettings.PassiveMode = StringBool(line);
				else if (StringContains("Debug", line))
					MySettings.Debugger = StringBool(line);
				else if (StringContains("PlayerzBlips", line))
					MySettings.PlayerzBlips = StringBool(line);
				else if (StringContains("NoNotify", line))
					MySettings.NoNotify = StringBool(line);
				else if (StringContains("MobileData", line))
					MySettings.MobileData = StringBool(line);
				else if (StringContains("InviteOnly", line))
					MySettings.InviteOnly = StringBool(line);
				else if (StringContains("BackChat", line))
					MySettings.BackChat = StringBool(line);
				else if (StringContains("MenuLeftSide", line))
					MySettings.MenuLeftSide = StringBool(line);
				else if (StringContains("Control_A_Open_Menu=", line))
					MySettings.Control_A_Open_Menu = StingNumbersInt(line);
				else if (StringContains("Control_B_Open_Menu=", line))
					MySettings.Control_B_Open_Menu = StingNumbersInt(line);
				else if (StringContains("Control_A_Clear_Session=", line))
					MySettings.Control_A_Clear_Session = StingNumbersInt(line);
				else if (StringContains("Control_B_Clear_Session=", line))
					MySettings.Control_B_Clear_Session = StingNumbersInt(line);
				else if (StringContains("Control_A_Invite_Only=", line))
					MySettings.Control_A_Invite_Only = StingNumbersInt(line);
				else if (StringContains("Control_B_Invite_Only=", line))
					MySettings.Control_B_Invite_Only = StingNumbersInt(line);
				else if (StringContains("Keys_Open_Menu=", line))
					MySettings.Keys_Open_Menu = StingNumbersInt(line);
				else if (StringContains("Keys_Clear_Session=", line))
					MySettings.Keys_Clear_Session = StingNumbersInt(line);
				else if (StringContains("Keys_Invite_Only=", line))
					MySettings.Keys_Invite_Only = StingNumbersInt(line);
				else if (StringContains("Control_Keys_Players_List=", line))
					MySettings.Control_Keys_Players_List = StingNumbersInt(line);
				else if (StringContains("Control_Keys_AddPed=", line))
					MySettings.Control_Keys_AddPed = StingNumbersInt(line);
				else if (StringContains("Control_Keys_EnterVeh=", line))
					MySettings.Control_Keys_EnterVeh = StingNumbersInt(line);
				else if (StringContains("Control_Keys_DissmisPed=", line))
					MySettings.Control_Keys_DissmisPed = StingNumbersInt(line);
			}
		}

		if (MySettings.Control_A_Open_Menu >= (int)ControlSym.size() || MySettings.Control_A_Open_Menu < 0)
			MySettings.Control_A_Open_Menu = 46;

		if (MySettings.Control_B_Open_Menu >= (int)ControlSym.size() || MySettings.Control_B_Open_Menu < 0)
			MySettings.Control_B_Open_Menu = 105;

		if (MySettings.Control_A_Clear_Session >= (int)ControlSym.size() || MySettings.Control_A_Clear_Session < 0)
			MySettings.Control_A_Clear_Session = 46;

		if (MySettings.Control_B_Clear_Session >= (int)ControlSym.size() || MySettings.Control_B_Clear_Session < 0)
			MySettings.Control_B_Clear_Session = 99;

		if (MySettings.Control_A_Invite_Only >= (int)ControlSym.size() || MySettings.Control_A_Invite_Only < 0)
			MySettings.Control_A_Invite_Only = 46;

		if (MySettings.Control_B_Invite_Only >= (int)ControlSym.size() || MySettings.Control_B_Invite_Only < 0)
			MySettings.Control_B_Invite_Only = 45;

		if (MySettings.Keys_Open_Menu >= (int)KeyFind.size() || MySettings.Keys_Open_Menu < 0)
			MySettings.Keys_Open_Menu = 90;

		if (MySettings.Keys_Clear_Session >= (int)KeyFind.size() || MySettings.Keys_Clear_Session < 0)
			MySettings.Keys_Clear_Session = 88;

		if (MySettings.Keys_Invite_Only >= (int)KeyFind.size() || MySettings.Keys_Invite_Only < 0)
			MySettings.Keys_Invite_Only = 67;

		if (MySettings.Control_Keys_Players_List >= (int)ControlSym.size() || MySettings.Control_Keys_Players_List < 0)
			MySettings.Control_Keys_Players_List = 236;

		if (MySettings.Control_Keys_AddPed >= (int)ControlSym.size() || MySettings.Control_Keys_AddPed < 0)
			MySettings.Control_Keys_AddPed = 47;

		if (MySettings.Control_Keys_EnterVeh >= (int)ControlSym.size() || MySettings.Control_Keys_EnterVeh < 0)
			MySettings.Control_Keys_EnterVeh = 47;

		if (MySettings.Control_Keys_DissmisPed >= (int)ControlSym.size() || MySettings.Control_Keys_DissmisPed < 0)
			MySettings.Control_Keys_DissmisPed = 46;

		if (MySettings.Aggression > 11)
			MySettings.Aggression = 11;
		if (MySettings.MinWait > MySettings.MaxWait)
			MySettings.MaxWait = MySettings.MinWait + 1;
		if (MySettings.MinSession > MySettings.MaxSession)
			MySettings.MaxSession = MySettings.MinSession + 1;
		if (MySettings.AccMin > MySettings.AccMax)
			MySettings.AccMax = MySettings.AccMin + 1;

		ReBuildIni();
	}

	const std::vector<std::string> Lanfiles = {
		"/PlayerZero/Translate/English.txt",
		"/PlayerZero/Translate/French.txt",
		"/PlayerZero/Translate/German.txt",
		"/PlayerZero/Translate/Italian.txt",
		"/PlayerZero/Translate/Spanish.txt",
		"/PlayerZero/Translate/Portuguese.txt",
		"/PlayerZero/Translate/Polish.txt",
		"/PlayerZero/Translate/Russian.txt",
		"/PlayerZero/Translate/Korean.txt",
		"/PlayerZero/Translate/Chinese.txt",
		"/PlayerZero/Translate/Japanese.txt",
		"/PlayerZero/Translate/Spanish.txt",
		"/PlayerZero/Translate/ChineseSimplify.txt"
	};
	void LoadLang(int lang)
	{
		LoggerLight("LoadLang lang == " + std::to_string(lang));

		if (lang == -1)
			lang = UNK::_GET_UI_LANGUAGE_ID(); // 2BDD44CC428A7EAE

		PZTranslate = ReadFile(GetDir() + Lanfiles[lang]);

		if ((int)PZTranslate.size() < 165)
			PZTranslate = PZLangEng;

		MySettings.Pz_Lang = lang;
	}

	void StartScript(const std::string& scriptName, int buffer)
	{
		SCRIPT::REQUEST_SCRIPT((LPSTR)scriptName.c_str());
		while (!SCRIPT::HAS_SCRIPT_LOADED((LPSTR)scriptName.c_str()))
		{
			SCRIPT::REQUEST_SCRIPT((LPSTR)scriptName.c_str());
			WAIT(1);
		}

		SYSTEM::START_NEW_SCRIPT((LPSTR)scriptName.c_str(), buffer);
		SCRIPT::SET_SCRIPT_AS_NO_LONGER_NEEDED((LPSTR)scriptName.c_str());
	}
}