/*
	 Player Zero MainScript
	--By Adopocalipt 2023--
*/

#include "script.h"
#include "keyboard.h"
#include "PZSys.h"
#include "LSRData.h"

using namespace PZSys;
using namespace PZClass;
using namespace PZData;

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <Windows.h>

bool PrivateJet = false;
bool BusDriver = false;
bool RentoCop = false;
bool HackAttacks = false;
bool AddHackAttacks = false;

bool PlayDead = false;
bool ClosedSession = false;
bool MissingFunk = false;
bool InPasiveMode = false;
bool StartTheMod = true;

int AirVehCount = 0;
int FollowMe;

Hash GP_Player;
Hash Gp_Friend;
Hash GP_Attack;
Hash Gp_Follow;
Hash GP_Mental;
std::unordered_set<Hash> ActiveGangGroups;

// Forward declarations for LSR gang group helpers
void ApplyGangRelationships(Hash gangGroup);
Hash GetGangGroupForZone(Ped peddy);

std::vector<GVM::GVMSystem> Pz_MenuList = {};

JoinMe YouFriend = JoinMe(nullptr);

typedef bool(*SnowFallType)();
typedef void(*LetItSnowType)(bool On);

bool AccessSnowFallType()
{
	bool b = false;
	if (FileExists(VFunk))
	{
		// Load the first .asi file (DLL)
		HMODULE hMod = LoadLibrary(L"V_Functions.asi");
		if (hMod != nullptr)
		{
			// Get the function pointer
			SnowFallType MyExportedFunction = (SnowFallType)GetProcAddress(hMod, "SnowFall");

			if (MyExportedFunction != nullptr)
				b = MyExportedFunction();

			FreeLibrary(hMod);
		}
	}
	else
		MissingFunk = true;

	return b;
}
void AccessLetItSnowType(bool On)
{
	if (FileExists(VFunk))
	{
		// Load the first .asi file (DLL)
		HMODULE hMod = LoadLibrary(L"V_Functions.asi");
		if (hMod != nullptr)
		{
			// Get the function pointer
			LetItSnowType MyExportedFunction = (LetItSnowType)GetProcAddress(hMod, "LetItSnow");

			if (MyExportedFunction != nullptr)
				MyExportedFunction(On);

			FreeLibrary(hMod);
		}
	}
	else
		MissingFunk = true;
}

int AddRelationship(const std::string& name)
{
	Hash hash = -1;

	PED::ADD_RELATIONSHIP_GROUP((LPSTR)name.c_str(), &hash);

	return hash;
}
Hash GetRelationship()
{
	return PED::GET_PED_RELATIONSHIP_GROUP_HASH(PLAYER::PLAYER_PED_ID());
}
void RelGroupMember(Ped peddy, Hash group)
{
	LoggerLight("-GroupMember-");

	PED::SET_PED_RELATIONSHIP_GROUP_HASH(peddy, group);
}

void AddGraphics(const std::string& graphics)
{
	GRAPHICS::_SET_PTFX_ASSET_NEXT_CALL("scr_xm_orbital");
}

void MoveEntity(Entity ent, Vector3 pos)
{
	ENTITY::SET_ENTITY_COORDS(ent, pos.x, pos.y, pos.z, 1, 0, 0, 1);
}

ClothX GetCloths(bool male)
{
	LoggerLight("-GetCloths-");

	ClothX cothing = FemaleDefault;
	std::string OutputFolder = DirectOutfitFemale;

	if (male)
	{
		cothing = MaleDefault;
		OutputFolder = DirectOutfitMale;
	}


	std::vector<std::string> Files = ReadDirectory(OutputFolder);

	int Rando = 0;
	if (male)
		Rando = LessRandomInt("Outfits_01", 0, (int)Files.size() - 1);
	else
		Rando = LessRandomInt("Outfits_02", 0, (int)Files.size() - 1);

	std::string Cloths = ""; std::vector<int> ClothA = {};	std::vector<int> ClothB = {}; std::vector<int> ExtraA = {}; std::vector<int> ExtraB = {};
	int intList = 0;

	std::vector<std::string> MyColect = ReadFile(Files[Rando]);

	for (int i = 0; i < MyColect.size(); i++)
	{
		std::string line = MyColect[i];
		if (StringContains("Title", line))
		{
			Cloths = StingNumbersInt(line);
		}
		else if (StringContains("[ClothA]", line))
		{
			intList = 1;
		}
		else if (StringContains("[ClothB]", line))
		{
			intList = 2;
		}
		else if (StringContains("[ExtraA]", line))
		{
			intList = 3;
		}
		else if (StringContains("[ExtraB]", line))
		{
			intList = 4;
		}
		else if (StringContains("[FreeOverLay]", line))
		{
			intList = 5;
		}
		else if (StringContains("[Tattoo]", line))
		{
			intList = 6;
		}
		else if (intList == 1)
		{
			ClothA.push_back(StingNumbersInt(line));
		}
		else if (intList == 2)
		{
			int iSpot = StingNumbersInt(line);
			if (iSpot < 0)
				iSpot = 0;
			ClothB.push_back(iSpot);
		}
		else if (intList == 3)
		{
			ExtraA.push_back(StingNumbersInt(line));
		}
		else if (intList == 4)
		{
			int iSpot = StingNumbersInt(line);
			if (iSpot < 0)
				iSpot = 0;
			ExtraB.push_back(iSpot);
		}
	}

	cothing = ClothX(Cloths, ClothA, ClothB, ExtraA, ExtraB);

	return cothing;
}

void PassiveDontShoot()
{
	LoggerLight("-PassiveDontShoot-");

	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, GP_Player, Gp_Follow);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, Gp_Follow, GP_Player);

	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, GP_Player, Gp_Friend);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, Gp_Friend, GP_Player);

	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, GP_Player, GP_Attack);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, GP_Attack, GP_Player);

	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, GP_Player, GP_Mental);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, GP_Mental, GP_Player);

	if (PED::GET_RELATIONSHIP_BETWEEN_GROUPS(GP_Player, GP_Mental) != 0)
	{
		WAIT(1000);
		PassiveDontShoot();
	}
}
void SetRelationType(bool friendly)
{
	LoggerLight("-SetRelationType-");
	if (friendly)
	{
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, GP_Player, Gp_Follow);
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, Gp_Follow, GP_Player);

		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(1, Gp_Follow, Gp_Friend);
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(1, Gp_Friend, Gp_Follow);

		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, GP_Attack, Gp_Follow);
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, Gp_Follow, GP_Attack);

		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, GP_Mental, Gp_Follow);
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, Gp_Follow, GP_Mental);

		PED::SET_RELATIONSHIP_BETWEEN_GROUPS((MySettings.Aggression <= 3) ? 3 : 5, GP_Attack, Gp_Friend);
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS((MySettings.Aggression <= 3) ? 3 : 5, Gp_Friend, GP_Attack);

		if (MySettings.Aggression > 5)
		{		
			PED::SET_RELATIONSHIP_BETWEEN_GROUPS(2, GP_Player, Gp_Friend);
			PED::SET_RELATIONSHIP_BETWEEN_GROUPS(2, Gp_Friend, GP_Player);

			PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, GP_Player, GP_Attack);
			PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, GP_Attack, GP_Player);
		}
		else
		{
			PED::SET_RELATIONSHIP_BETWEEN_GROUPS(3, GP_Player, GP_Attack);
			PED::SET_RELATIONSHIP_BETWEEN_GROUPS(3, GP_Attack, GP_Player);
		}

		PED::SET_RELATIONSHIP_BETWEEN_GROUPS((MySettings.Aggression <= 3) ? 3 : 5, GP_Mental, Gp_Friend);
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS((MySettings.Aggression <= 3) ? 3 : 5, Gp_Friend, GP_Mental);

		PED::SET_RELATIONSHIP_BETWEEN_GROUPS((MySettings.Aggression <= 3) ? 3 : 5, GP_Attack, GP_Mental);
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS((MySettings.Aggression <= 3) ? 3 : 5, GP_Mental, GP_Attack);

		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, GP_Player, GP_Mental);
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, GP_Mental, GP_Player);

		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, GP_Mental, GP_Mental);
	}
	else
	{
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(2, GP_Player, Gp_Follow);
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(2, Gp_Follow, GP_Player);

		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(2, Gp_Follow, Gp_Friend);
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(2, Gp_Friend, Gp_Follow);

		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, GP_Attack, Gp_Follow);
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, Gp_Follow, GP_Attack);

		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, GP_Mental, Gp_Follow);
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, Gp_Follow, GP_Mental);

		PED::SET_RELATIONSHIP_BETWEEN_GROUPS((MySettings.Aggression <= 3) ? 3 : 5, GP_Attack, Gp_Friend);
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS((MySettings.Aggression <= 3) ? 3 : 5, Gp_Friend, GP_Attack);

		if (MySettings.Aggression > 5)
		{
			PED::SET_RELATIONSHIP_BETWEEN_GROUPS(2, GP_Player, Gp_Friend);
			PED::SET_RELATIONSHIP_BETWEEN_GROUPS(2, Gp_Friend, GP_Player);

			PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, GP_Player, GP_Attack);
			PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, GP_Attack, GP_Player);
		}
		else
		{
			PED::SET_RELATIONSHIP_BETWEEN_GROUPS(3, GP_Player, GP_Attack);
			PED::SET_RELATIONSHIP_BETWEEN_GROUPS(3, GP_Attack, GP_Player);
		}

		PED::SET_RELATIONSHIP_BETWEEN_GROUPS((MySettings.Aggression <= 3) ? 3 : 5, GP_Mental, Gp_Friend);
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS((MySettings.Aggression <= 3) ? 3 : 5, Gp_Friend, GP_Mental);

		PED::SET_RELATIONSHIP_BETWEEN_GROUPS((MySettings.Aggression <= 3) ? 3 : 5, GP_Attack, GP_Mental);
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS((MySettings.Aggression <= 3) ? 3 : 5, GP_Mental, GP_Attack);

		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, GP_Player, GP_Mental);
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, GP_Mental, GP_Player);

		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, GP_Mental, GP_Mental);
	}

	// Keep any active LSR gang groups in sync when aggression changes
	for (Hash gangGroup : ActiveGangGroups)
		ApplyGangRelationships(gangGroup);

	if (PED::GET_RELATIONSHIP_BETWEEN_GROUPS(GP_Player, GP_Mental) != 5)
	{
		WAIT(1000);
		SetRelationType(friendly);
	}
}

bool ItsChristmas = false;
bool ItHalloween = false;
void LoadinData()
{
	LoggerLight("-LoadinData-");

	FindSettings();

	GP_Player = GetRelationship();
	FollowMe = PED::GET_PED_GROUP_INDEX(PLAYER::PLAYER_PED_ID());
	Gp_Friend = AddRelationship("Stranger");
	GP_Attack = AddRelationship("Hostile");
	Gp_Follow = AddRelationship("Friend");
	GP_Mental = AddRelationship("Bystander");
	ActiveGangGroups.clear();
	LoadLang(MySettings.Pz_Lang);

	std::string Today = TimeDate();

	if (StringContains("Dec", Today))
	{
		if (StringContains(" 24 ", Today) || StringContains(" 25 ", Today) || StringContains(" 26 ", Today))
			ItsChristmas = true;
	}
	else if (StringContains("Oct", Today))
	{
		if (StringContains(" 31 ", Today))
			ItHalloween = true;
	}

	FindAddCars();

	SetRelationType(MySettings.FriendlyFire);

	PED::SET_PED_AS_GROUP_LEADER(PLAYER::PLAYER_PED_ID(), FollowMe);
	PED::SET_GROUP_FORMATION(FollowMe, 3);
	
	MySettings.TFHTA--;
	if (MySettings.TFHTA < 0 && !MySettings.InviteOnly)
	{
		if (LessRandomInt("TFHTAV2", 0, 90) == 13)
		{
			AddHackAttacks = true;
			MySettings.TFHTA = 10;
		}
	}
	
	OutfitFolderTest();

	if (MySettings.PassiveMode)
	{	
		PassiveDontShoot();
		PLAYER::DISABLE_PLAYER_FIRING(PLAYER::PLAYER_ID(), true);
		InPasiveMode = true;
	}

	if (!MySettings.MenuLeftSide)
		GVM::DefaultRatio.MenuRightSide();

	GVM::DefaultRatio.BlueAndYellow();

	CleanOutOldCalls();
	LoadContacts();

	// Detect and load LSR data files for enhanced NPC simulation.
	// If LSR is not installed the mod runs in standalone mode — nothing breaks.
	// NOTE: notification is deliberately deferred — it fires in the main loop once
	// the player is in control, not here during the loading screen where it would
	// be invisible.
	LSRData::Init("plugins/LosSantosRED");
	if (LSRData::IsAvailable)
		LoggerLight("LSRData: loaded — enhanced NPC simulation active");
	else
		LoggerLight("LSRData: LSR not found — running in standalone mode");
}

bool GotThatName(const std::vector<std::string>& listing, const std::string& name)
{
	LoggerLight("GotThatName");
	bool b = false;
	for (int i = 0; i < (int)listing.size(); i++)
	{
		if (listing[i] == name)
		{
			b = true;
			break;
		}
	}

	return b;
}
int GotYourNumber(const std::vector<int>& listing, int num)
{
	LoggerLight("GotThatName");
	bool b = false;
	for (int i = 0; i < (int)listing.size(); i++)
	{
		if (listing[i] == num)
		{
			b = true;
			break;
		}
	}

	return b;
}
std::string FindNewName()
{
	LoggerLight("FindNewName");

	// prefix (empty ~4/21 of the time so many names have no prefix)
	std::string sName = sListOpeniLet[LessRandomInt("NameFind01", 0, (int)sListOpeniLet.size() - 1)];

	// core gamertag word
	sName += sListVowls[LessRandomInt("NameFind02", 0, (int)sListVowls.size() - 1)];

	// suffix style
	int iStyle = LessRandomInt("NameFind03", 0, 10);
	if (iStyle < 3)
		sName += sListPadding[RandomInt(0, (int)sListPadding.size() - 1)];      // e.g. 420, 69, 1337
	else if (iStyle < 6)
		sName += sListPostfix[RandomInt(0, (int)sListPostfix.size() - 1)];      // e.g. Xx, _Pro, YT
	else if (iStyle < 8)
		sName += sListNumbers[RandomInt(0, (int)sListNumbers.size() - 1)]
		       + sListNumbers[RandomInt(0, (int)sListNumbers.size() - 1)];      // e.g. 47, 99
	// else no suffix — name stands alone

	return sName;
}
std::string SillyNameList()
{
	LoggerLight("SillyNameList");

	std::string name = FindNewName();

	while (GotThatName(PlayerNames, name))
	{
		name = FindNewName();
		WAIT(1);
	}
	PlayerNames.push_back(name);

	return name;
}
int UniqueLevels()
{
	LoggerLight("UniqueLevels");

	int iNumber1 = LessRandomInt("Levels", 0, 10);

	int iNumber2 = LessRandomInt("Levels", 0, 10);

	int iNumber3 = LessRandomInt("Levels", 0, 10);

	return iNumber1 + (iNumber2 * 10) + (iNumber3 * 100);
}
void EraseBlip(Blip* blip)
{
	if (*blip != NULL)
	{
		if ((bool)UI::DOES_BLIP_EXIST(*blip))
			UI::REMOVE_BLIP(blip);
	}
}
FaceBank AddFace(bool male)
{
	int ShapeFirstID;
	int ShapeSecondID;
	if (male)
	{
		ShapeFirstID = LessRandomInt("AddFaceM01", 0, 20);
		ShapeSecondID = LessRandomInt("AddFaceM01", 0, 20);
	}
	else
	{
		ShapeFirstID = LessRandomInt("AddFaceF01", 21, 41);
		ShapeSecondID = LessRandomInt("AddFaceF01", 21, 41);
	}

	float ShapeMix = RandomFloat(-0.9, 0.9);
	float SkinMix = RandomFloat(0.9, 0.99);
	float ThirdMix = RandomFloat(-0.9, 0.9);

	return FaceBank(ShapeFirstID, ShapeSecondID, ShapeMix, SkinMix, ThirdMix);
}
std::vector<FreeOverLay> AddOverLay(bool male)
{
	LoggerLight("AddOverLay");
	std::vector<FreeOverLay> YourOver = {};

	for (int i = 0; i < 12; i++)
	{
		int iColour = 0;
		int iChange = RandomInt(0, PED::_GET_NUM_HEAD_OVERLAY_VALUES(i));
		float fVar = RandomFloat(0.45, 0.99);

		if (i == 0)
		{
			iChange = RandomInt(0, iChange);
		}//Blemishes
		else if (i == 1)
		{
			if (male)
				iChange = RandomInt(0, iChange);
			else
				iChange = 255;
			iColour = 1;
		}//Facial Hair
		else if (i == 2)
		{
			iChange = RandomInt(0, iChange);
			iColour = 1;
		}//Eyebrows
		else if (i == 3)
		{
			iChange = 255;
		}//Ageing
		else if (i == 4)
		{
			int iFace = RandomInt(0, 50);
			if (iFace < 30)
			{
				iChange = RandomInt(0, 15);
			}
			else if (iFace < 45)
			{
				iChange = RandomInt(0, iChange);
				fVar = RandomFloat(0.85f, 0.99f);
			}
			else
				iChange = 255;
		}//Makeup
		else if (i == 5)
		{
			if (!male)
			{
				iChange = RandomInt(0, iChange);
				fVar = RandomFloat(0.15f, 0.39f);
			}
			else
				iChange = 255;
			iColour = 2;
		}//Blush
		else if (i == 6)
		{
			iChange = RandomInt(0, iChange);
		}//Complexion
		else if (i == 7)
		{
			iChange = 255;
		}//Sun Damage
		else if (i == 8)
		{
			if (!male)
				iChange = RandomInt(0, iChange);
			else
				iChange = 255;
			iColour = 2;
		}//Lipstick
		else if (i == 9)
		{
			iChange = RandomInt(0, iChange);
		}//Moles/Freckles
		else if (i == 10)
		{
			if (male)
				iChange = RandomInt(0, iChange);
			else
				iChange = 255;
			iColour = 1;
		}//Chest Hair
		else if (i == 11)
		{
			iChange = RandomInt(0, iChange);
		}//Body Blemishes

		int AddColour = 0;

		if (iColour > 0)
			AddColour = RandomInt(0, 64);

		YourOver.push_back(FreeOverLay(iChange, AddColour, fVar));
	}

	return YourOver;
}
std::vector<Tattoo> AddRandTats(bool male)
{
	LoggerLight("AddRandTats");
	std::vector<Tattoo> Tatlist = {};

	if (male)
	{
		if (RandomInt(0, 10) < 5)
		{
			Tattoo T = maleTats01[RandomInt(0, (int)maleTats01.size() - 1)];
			Tatlist.push_back(T);
		}
		if (RandomInt(0, 10) < 5)
		{
			Tattoo T = maleTats02[RandomInt(0, (int)maleTats02.size() - 1)];
			Tatlist.push_back(T);
		}
		if (RandomInt(0, 10) < 5)
		{
			Tattoo T = maleTats03[RandomInt(0, (int)maleTats03.size() - 1)];
			Tatlist.push_back(T);
		}
		if (RandomInt(0, 10) < 5)
		{
			Tattoo T = maleTats04[RandomInt(0, (int)maleTats04.size() - 1)];
			Tatlist.push_back(T);
		}
		if (RandomInt(0, 10) < 5)
		{
			Tattoo T = maleTats05[RandomInt(0, (int)maleTats05.size() - 1)];
			Tatlist.push_back(T);
		}
		if (RandomInt(0, 10) < 5)
		{
			Tattoo T = maleTats06[RandomInt(0, (int)maleTats06.size() - 1)];
			Tatlist.push_back(T);
		}
		if (RandomInt(0, 10) < 5)
		{
			Tattoo T = maleTats07[RandomInt(0, (int)maleTats07.size() - 1)];
			Tatlist.push_back(T);
		}
		if (RandomInt(0, 10) < 5)
		{
			Tattoo T = maleTats08[RandomInt(0, (int)maleTats08.size() - 1)];
			Tatlist.push_back(T);
		}
	}
	else
	{
		if (RandomInt(0, 10) < 5)
		{
			Tattoo T = femaleTats01[RandomInt(0, (int)femaleTats01.size() - 1)];
			Tatlist.push_back(T);
		}
		if (RandomInt(0, 10) < 5)
		{
			Tattoo T = femaleTats02[RandomInt(0, (int)femaleTats02.size() - 1)];
			Tatlist.push_back(T);
		}
		if (RandomInt(0, 10) < 5)
		{
			Tattoo T = femaleTats03[RandomInt(0, (int)femaleTats03.size() - 1)];
			Tatlist.push_back(T);
		}
		if (RandomInt(0, 10) < 5)
		{
			Tattoo T = femaleTats04[RandomInt(0, (int)femaleTats04.size() - 1)];
			Tatlist.push_back(T);
		}
		if (RandomInt(0, 10) < 5)
		{
			Tattoo T = femaleTats05[RandomInt(0, (int)femaleTats05.size() - 1)];
			Tatlist.push_back(T);
		}
		if (RandomInt(0, 10) < 5)
		{
			Tattoo T = femaleTats06[RandomInt(0, (int)femaleTats06.size() - 1)];
			Tatlist.push_back(T);
		}
		if (RandomInt(0, 10) < 5)
		{
			Tattoo T = femaleTats07[RandomInt(0, (int)femaleTats07.size() - 1)];
			Tatlist.push_back(T);
		}
		if (RandomInt(0, 10) < 5)
		{
			Tattoo T = femaleTats08[RandomInt(0, (int)femaleTats08.size() - 1)];
			Tatlist.push_back(T);
		}
	}

	return Tatlist;
}
ClothBank NewClothBank()
{
	if (LessRandomInt("GotMale", 0, 10) < 5)
	{
		bool Male = true;
		std::string Model = "mp_m_freemode_01";
		ClothX Cothing = GetCloths(true);
		HairSets MyHair = MHairsets[LessRandomInt("MCBank02", 0, (int)MHairsets.size() - 1)];
		return ClothBank(SillyNameList(), Model, Cothing, AddFace(Male), Male, MyHair, RandomInt(0, 13), RandomInt(0, 13), RandomInt(0, 5), AddOverLay(Male), AddRandTats(Male));
	}
	else
	{
		bool Male = false;
		std::string Model = "mp_f_freemode_01";
		ClothX Cothing = GetCloths(false);
		HairSets MyHair = FHairsets[LessRandomInt("FCBank02", 0, (int)FHairsets.size() - 1)];
		return ClothBank(SillyNameList(), Model, Cothing, AddFace(Male), Male, MyHair, RandomInt(0, 13), RandomInt(0, 13), RandomInt(0, 5), AddOverLay(Male), AddRandTats(Male));
	}
}
int OhMyBlip(Vehicle vic)
{
	LoggerLight("OhMyBlip");

	int iBeLip = 0;
	if (vic != NULL)
	{
		int iVehClass = VEHICLE::GET_VEHICLE_CLASS(vic);

		if (iVehClass == 14)
			iBeLip = 427;
		else if (iVehClass == 15)
			iBeLip = 64;
		else if (iVehClass == 8)
			iBeLip = 226;
		else if (iVehClass == 16)
			iBeLip = 424;
		else if (iVehClass == 12)
			iBeLip = 616;
		else if (iVehClass == 20)//mule
			iBeLip = 477;
		else if (iVehClass == 10)//trucks
			iBeLip = 477;
		else if (iVehClass == 17)//buss
			iBeLip = 513;
		else if (iVehClass == 7)
			iBeLip = 595;
		else if (iVehClass == 6)
			iBeLip = 523;
		else if (iVehClass == 13)
			iBeLip = 376;
		else
			iBeLip = 225;


		for (int i = 0; i < vehBlips.size(); i++)
		{
			if ((bool)VEHICLE::IS_VEHICLE_MODEL(vic, MyHashKey(vehBlips[i].VehicleKey)))
				iBeLip = vehBlips[i].BlipNo;
		}
	}

	return iBeLip;
}
void ClearYourFriend(bool Retry)
{
	if (YouFriend.MyBrain != nullptr)
	{
		YouFriend.MyBrain->WanBeFriends = false;

		if (Retry)
			YouFriend.MyBrain->ApprochPlayer = true;
	}

	YouFriend = JoinMe(nullptr);
}
void WarptoAnyVeh(Vehicle vic, Ped peddy, int seat)
{
	LoggerLight("WarptoAnyVeh");

	PED::SET_PED_INTO_VEHICLE(peddy, vic, seat);
}
void GetOutVehicle(Ped peddy)
{
	LoggerLight("-GetOutVehicle-");

	if ((bool)PED::IS_PED_IN_ANY_VEHICLE(peddy, 0))
		AI::TASK_LEAVE_VEHICLE(peddy, PED::GET_VEHICLE_PED_IS_IN(peddy, true), 4160);
}
void EmptyVeh(Vehicle vic)
{
	LoggerLight("PedActions.EmptyVeh");

	if ((bool)ENTITY::DOES_ENTITY_EXIST(vic))
	{
		int iSeats = 0;
		while (iSeats < VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(vic))
		{
			if (!(bool)VEHICLE::IS_VEHICLE_SEAT_FREE(vic, iSeats))
				GetOutVehicle(VEHICLE::GET_PED_IN_VEHICLE_SEAT(vic, iSeats));
			iSeats += 1;
		}
	}
}

int FindUSeat(Vehicle vic, bool kickOut)
{
	int iSeats = 0;
	bool bPass = true;
	Ped Barry = NULL;
	for (int i = 0; i < (int)GunnerSeat.size(); i++)
	{
		if (MyHashKey(GunnerSeat[i]) == ENTITY::GET_ENTITY_MODEL(vic))
		{
			bPass = false;
			break;
		}
	}

	if (bPass)
	{
		if (!kickOut)
		{
			while (iSeats < VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(vic))
			{
				if ((bool)VEHICLE::IS_VEHICLE_SEAT_FREE(vic, iSeats))
					break;
				else
					iSeats++;
			}
			if (iSeats == VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(vic))
				iSeats = -10;
		}
	}
	else
	{
		iSeats = VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(vic) -1;
		if (!kickOut)
		{
			while (iSeats > -1)
			{
				if ((bool)VEHICLE::IS_VEHICLE_SEAT_FREE(vic, iSeats))
					break;
				else
					iSeats--;
			}
			if (iSeats == -1)
				iSeats = -10;
		}
	}

	return iSeats;
}
void ResetPlayer(PlayerBrain* brain, bool del)
{
	if (brain->IsPlane || brain->IsHeli)
		AirVehCount--;

	if (brain->ThisVeh != NULL && brain->Driver)
	{
		EmptyVeh(brain->ThisVeh);
		ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&brain->ThisVeh);
		brain->ThisVeh = NULL;
	}

	if (brain->Oppressor != NULL)
	{
		EmptyVeh(brain->Oppressor);
		ENTITY::DELETE_ENTITY(&brain->Oppressor);
		brain->Oppressor = NULL;
	}

	if (brain->XmasTree != NULL)
	{
		ENTITY::DELETE_ENTITY(&brain->XmasTree);
		brain->XmasTree = NULL;
	}

	if (brain->PiggyBackin)
	{
		brain->PiggyBackin = false;
		ENTITY::DETACH_ENTITY(PLAYER::PLAYER_PED_ID(), 0, 0);
		AI::CLEAR_PED_TASKS_IMMEDIATELY(PLAYER::PLAYER_PED_ID());
	}

	if (brain->AirTranspport)
	{
		PrivateJet = false;
		brain->AirTranspport = false;
		brain->TimeOn = 0;
	}

	if (brain->BusDriver)
	{
		BusDriver = false;
		brain->BusDriver = false;
		brain->TimeOn = 0;
	}
	
	if (brain->RentaCop)
	{
		RentoCop = false;
		brain->RentaCop = false;
		brain->TimeOn = 0;
	}

	EraseBlip(&brain->ThisBlip);

	UI::_0x31698AA80E0223F8(brain->HeadTag);

	if (del)
	{
		if (brain->ThisPed != NULL)
		{
			if ((bool)ENTITY::DOES_ENTITY_EXIST(brain->ThisPed))
			{
				ENTITY::DETACH_ENTITY(brain->ThisPed, 0, 0);
				PED::REMOVE_PED_FROM_GROUP(brain->ThisPed);
				AI::CLEAR_PED_TASKS_IMMEDIATELY(brain->ThisPed);
				AI::TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(brain->ThisPed, true);
				ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&brain->ThisPed);
			}
		}
	}
}

void PedCleaning(PlayerBrain* brain, std::string leaving, bool del)
{
	LoggerLight("ClearUp.PedCleaning, MyName == " + brain->MyName);
	GVM::BottomLeft("~h~" + brain->MyName + "~s~ " + leaving);
	ResetPlayer(brain, del);

	if (brain->IsInContacts)
	{
		int iCon = RetreaveCont(brain->MyIdentity);
		if (iCon > -1 && iCon < PhoneContacts.size())
			AddContacttoList(PhoneContacts[iCon]);
	}

	brain->TimeToGo = true;
}

void BlipFiler(Blip* blip, int blippy, const std::string& name, int colour)
{
	UI::SET_BLIP_SPRITE(*blip, blippy);
	UI::SET_BLIP_AS_SHORT_RANGE(*blip, true);
	UI::SET_BLIP_CATEGORY(*blip, 2);
	UI::SET_BLIP_COLOUR(*blip, colour);

	std::string Name = " Player: " + name;

	UI::BEGIN_TEXT_COMMAND_SET_BLIP_NAME("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)Name.c_str());
	UI::END_TEXT_COMMAND_SET_BLIP_NAME(*blip);
}
void PedBlimp(Blip* blip, Ped peddy, int blippy, const std::string& name, int colour, bool heading, bool passenger)
{
	LoggerLight("PedBlimp, blippy == " + std::to_string(blippy) + ", name == " + name + ", colour" + std::to_string(colour));

	if (*blip != NULL)
		EraseBlip(blip);

	*blip = UI::ADD_BLIP_FOR_ENTITY(peddy);;

	BlipFiler(blip, blippy, name, colour);
	if (passenger)
		UI::SET_BLIP_SCALE(*blip, 0.01f);
	else if (heading)
		UI::_SET_BLIP_SHOW_HEADING_INDICATOR(*blip, true);
}
void PedBlimp(Blip* blip, Ped peddy, int blippy, const std::string& name, int colour, bool heading)
{
	PedBlimp(blip, peddy, blippy, name, colour, heading, false);
}
void LocalBlip(Blip* blip, Vector4 local, int blippy, const std::string& name, int colour)
{
	LoggerLight("BuildObjects, blippy == " + std::to_string(blippy) + ", name == " + name);

	EraseBlip(blip);
	*blip = UI::ADD_BLIP_FOR_COORD(local.X, local.Y, local.Z);
	BlipFiler(blip, blippy, name, colour);
}
void BlipingBlip(PlayerBrain* brain)
{
	if (MySettings.PlayerzBlips && !brain->OffRadarBool)
	{
		if ((bool)PED::IS_PED_IN_ANY_VEHICLE(brain->ThisPed, 0))
		{
			if (brain->DirBlip)
			{
				brain->DirBlip = false;
				if (brain->Driver && brain->ThisVeh != NULL)
					PedBlimp(&brain->ThisBlip, brain->ThisPed, OhMyBlip(brain->ThisVeh), brain->MyName, brain->BlipColour, brain->DirBlip);
				else
					PedBlimp(&brain->ThisBlip, brain->ThisPed, 1, brain->MyName, brain->BlipColour, brain->DirBlip, true);
			}
		}
		else
		{
			if (!brain->DirBlip)
			{
				brain->DirBlip = true;
				PedBlimp(&brain->ThisBlip, brain->ThisPed, 1, brain->MyName, brain->BlipColour, brain->DirBlip);
				UI::_SET_BLIP_SHOW_HEADING_INDICATOR(brain->ThisBlip, 1);
			}
			else if (brain->ThisBlip == NULL)
			{
				PedBlimp(&brain->ThisBlip, brain->ThisPed, 1, brain->MyName, brain->BlipColour, brain->DirBlip);
				UI::_SET_BLIP_SHOW_HEADING_INDICATOR(brain->ThisBlip, 1);
			}
		}
	}
	else
	{
		if (brain->ThisBlip != NULL)
		{
			EraseBlip(&brain->ThisBlip);
			brain->ThisBlip = NULL;
		}
	}
}

void AddMonies(int amount)
{
	Hash H1 = ENTITY::GET_ENTITY_MODEL(PLAYER::PLAYER_PED_ID());
	int iPedCred = -1;
	if (H1 == -1686040670)
		iPedCred = MyHashKey("SP2_TOTAL_CASH");
	else if (H1 == -1692214353)
		iPedCred = MyHashKey("SP1_TOTAL_CASH");
	else if (H1 == 225514697)
		iPedCred = MyHashKey("SP0_TOTAL_CASH");
	int Credit = 0;
	STATS::STAT_GET_INT(iPedCred, &Credit, -1);
	Credit += amount;
	STATS::STAT_SET_INT(iPedCred, Credit, 1);
}
void PlayerDump(bool keepFriend)
{
	LoggerLight("-PlayerDump-");
		
	for (int i = 0; i < (int)PedList.size(); i++)
	{
		if (keepFriend)
		{
			if (!PedList[i].Follower)
				PedList[i].TimeOn = 0;
		}
		else
			PedList[i].TimeOn = 0;
	}

	for (int i = 0; i < (int)AFKList.size(); i++)
		AFKList[i].TimeOn = 0;

}

bool SessionCleaning = false;
void LaggOut(bool keepFriend)
{
	PrivateJet = false;
	BusDriver = false;
	RentoCop = false;

	if (!keepFriend)
		SessionCleaning = true;

	PlayerDump(keepFriend);

	ClearYourFriend(false);

	AirVehCount = 0;
}
void RandomChat(const std::string& ped, int type, int nat)
{
	std::string PreAll = ped + " [All] ";

	if (nat == 1)
	{
		if (type == 1)
			PreAll += ShitGreet01[LessRandomInt("ShitGreet01", 0, (int)ShitGreet01.size() - 1)];
		else if (type == 2)
			PreAll += ShitGreet02[LessRandomInt("ShitGreet02", 0, (int)ShitGreet02.size() - 1)];
		else if (type == 3)
			PreAll += ShitGreet03[LessRandomInt("ShitGreet03", 0, (int)ShitGreet03.size() - 1)];
		else if (type == 4)
			PreAll += ShitGreet04[LessRandomInt("ShitGreet04", 0, (int)ShitGreet04.size() - 1)];
		else if (type == 5)
			PreAll += ShitGreet05[LessRandomInt("ShitGreet05", 0, (int)ShitGreet05.size() - 1)];
	}
	else if (nat == 2)
	{
		if (type == 1)
			PreAll += ShitTalk01[LessRandomInt("ShitTalk01", 0, (int)ShitTalk01.size() - 1)];
		else if (type == 2)
			PreAll += ShitTalk02[LessRandomInt("ShitTalk02", 0, (int)ShitTalk02.size() - 1)];
		else if (type == 3)
			PreAll += ShitTalk03[LessRandomInt("ShitTalk03", 0, (int)ShitTalk03.size() - 1)];
		else if (type == 4)
			PreAll += ShitTalk04[LessRandomInt("ShitTalk04", 0, (int)ShitTalk04.size() - 1)];
		else if (type == 5)
			PreAll += ShitTalk05[LessRandomInt("ShitTalk05", 0, (int)ShitTalk05.size() - 1)];
	}
	else if (nat == 3)
	{
		PreAll += NoobTalk01[LessRandomInt("NoobTalk01", 0, (int)NoobTalk01.size() - 1)];
	}
	else if (nat == 4)
	{
		PreAll += AppTalk01[LessRandomInt("AppTalk01", 0, (int)AppTalk01.size() - 1)];
	}
	else if (nat == 5)
	{
		PreAll += CrazyTalk01[LessRandomInt("CrazyTalk01", 0, (int)CrazyTalk01.size() - 1)];
	}

	int iTime = InGameTime() + 5000;

	ShitTalk.push_back(ShitTalking(PreAll, iTime));

	if ((int)PreAll.length() > 43)
		ShitTalk.push_back(ShitTalking("", iTime));
}
float GroundHight(Vector3 pos)
{
	float GroundHight = pos.z;
	GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(pos.x, pos.y, pos.z, &GroundHight, 1);
	return GroundHight;
}
void StayOnGround(Vehicle vic)
{
	LoggerLight("StayOnGround");
	while (!VEHICLE::IS_VEHICLE_ON_ALL_WHEELS(vic))
	{
		VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(vic);
		WAIT(10);
	}
}
std::vector<Vector3> BuildFlightPath(Vector3 start)
{
	const std::vector<Vector3> landSand = {
		NewVector3(2308.72f, 3342.185f, 122.7201f),
		NewVector3(1714.221f, 3258.899f, 77.91228f),
		NewVector3(1149.67f, 3101.854f, 40.30316f),
		NewVector3(1091.371f, 3024.233f, 40.65675f),
		NewVector3(1135.063f, 3029.95f, 40.4108f),
		NewVector3(1562.661f, 3141.121f, 85.64391f),
		NewVector3(798.5253, 2388.362, 282.331),
		NewVector3(413.3483, 2034.074, 425.4946),
		NewVector3(-175.5016, 1448.899, 598.845),
		NewVector3(-349.6658, -187.2563, 398.4032)
	};
	const std::vector<Vector3> landLS = {
		 NewVector3(-1002.727, -1650.774, 134.2087),
		 NewVector3(-1193.304, -1941.04, 59.51603),
		 NewVector3(-1571.467, -2617.15, 14.57554),
		 NewVector3(-1612.011, -2789.524, 14.62421),
		 NewVector3(-1532.074, -2835.987, 14.58676),
		 NewVector3(-991.79, -3147.605, 90.8317),
		 NewVector3(-440.5424, -3123.56, 232.0305),
		 NewVector3(-88.80471, -2403.97, 234.262),
		 NewVector3(-18.7144, -1591.593, 351.0859),
		 NewVector3(29.53166, 219.7558, 581.6113),
		 NewVector3(-169.9742, 1746.14, 484.2034)
	};

	float f1 = DistanceTo(start, landSand[0]);
	float f2 = DistanceTo(start, landLS[0]);

	if (f1 < f2)
		return landSand;
	else
		return landLS;
}
std::vector<int> RandVehModsist()
{
	std::vector<int> RandMods = {};

	for (int i = 0; i < 67; i++)
		RandMods.push_back(-1);

	LoggerLight("RandVehModsist Count == " + std::to_string((int)RandMods.size()));

	return RandMods;
}
Prop BuildProps(const std::string& object, Vector3 pos, Vector3 rot, bool push)
{
	LoggerLight("BuildProps,  object == " + object);
	Prop Plop = OBJECT::CREATE_OBJECT(MyHashKey(object), pos.x, pos.y, pos.z, 1, 1, 1);
	MoveEntity(Plop, pos);
	ENTITY::SET_ENTITY_ROTATION(Plop, rot.x, rot.y, rot.z, 2, 1);
	ENTITY::SET_ENTITY_AS_MISSION_ENTITY(Plop, 1, 1);
	if (push)
		ENTITY::APPLY_FORCE_TO_ENTITY(Plop, 1, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1, 1, 1, 1, 1, 1);

	return Plop;
}
void OrbExp(Ped ped, Vector3 pos1, Vector3 pos2, Vector3 pos3, Vector3 pos4, Vector3 pos5)
{
	LoggerLight("OrbExp");

	FIRE::ADD_OWNED_EXPLOSION(ped, pos2.x, pos2.y, pos2.z, 49, 1, true, false, 1);
	FIRE::ADD_OWNED_EXPLOSION(ped, pos3.x, pos3.y, pos3.z, 49, 1, true, false, 1);
	FIRE::ADD_OWNED_EXPLOSION(ped, pos4.x, pos4.y, pos4.z, 49, 1, true, false, 1);
	FIRE::ADD_OWNED_EXPLOSION(ped, pos5.x, pos5.y, pos5.z, 49, 1, true, false, 1);
	FIRE::ADD_OWNED_EXPLOSION(ped, pos1.x, pos1.y, pos1.z, 54, 1, true, false, 1);

	AUDIO::PLAY_SOUND_FROM_COORD(-1, "DLC_XM_Explosions_Orbital_Cannon", pos1.x, pos1.y, pos1.z, 0, 0, 1, 0);
	AddGraphics("scr_xm_orbital");
}
void OrbLoad(const std::string& whoDidit, const std::string& whoDied, bool playerStrike)
{
	LoggerLight("OrbLoad, whoDidit == " + whoDidit + ", whoDied == " + whoDied);

	if (playerStrike)
	{
		GVM::BottomLeft(PZTranslate[6] + whoDied + PZTranslate[7]);
		GVM::BigMessage(PZTranslate[9], "", 3);
	}
	else
		GVM::BottomLeft(whoDidit + PZTranslate[9] + " " + whoDied + PZTranslate[7]);

}
void FireOrb(PlayerBrain* brainShooter, PlayerBrain* brainTarg)
{
	LoggerLight("FireOrb");


	if (brainTarg != nullptr)
	{
		if (brainTarg->ThisPed != NULL)
		{
			Vector3 TargetPos = ENTITY::GET_ENTITY_COORDS(brainTarg->ThisPed, true);
			float GHight = GroundHight(TargetPos);
			if (GHight < TargetPos.z)
			{
				Vector3 TargF = FowardOf(brainTarg->ThisPed, 5, true);
				Vector3 TargB = FowardOf(brainTarg->ThisPed, 5, false);
				Vector3 TargR = RightOf(brainTarg->ThisPed, 5, true);
				Vector3 TargL = RightOf(brainTarg->ThisPed, 5, false);

				Ped PedFire = PLAYER::PLAYER_PED_ID();
				std::string Blame = "You";
				std::string Victim = "You";
				bool PlayerStrike = true;
				if (brainShooter != nullptr)
				{
					if (brainShooter->ThisPed != NULL)
						PedFire = brainShooter->ThisPed;
					PlayerStrike = false;
					Blame = brainShooter->MyName;
					Victim = brainTarg->MyName;
				}
				OrbExp(PedFire, TargetPos, TargF, TargB, TargR, TargL);
				OrbLoad(Blame, Victim, PlayerStrike);
				WAIT(4000);
			}
		}
	}
	else
	{
		Vector3 TargetPos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
		float GHight = GroundHight(TargetPos);
		if (GHight < TargetPos.z)
		{
			Vector3 TargF = FowardOf(PLAYER::PLAYER_PED_ID(), 5, true);
			Vector3 TargB = FowardOf(PLAYER::PLAYER_PED_ID(), 5, false);
			Vector3 TargR = RightOf(PLAYER::PLAYER_PED_ID(), 5, true);
			Vector3 TargL = RightOf(PLAYER::PLAYER_PED_ID(), 5, false);

			Ped PedFire = PLAYER::PLAYER_PED_ID();
			std::string Blame = "You";
			std::string Victim = "You";
			bool PlayerStrike = true;

			if (brainShooter != nullptr)
			{
				if (brainShooter->ThisPed != NULL)
					PedFire = brainShooter->ThisPed;
				PlayerStrike = false;
				Blame = brainShooter->MyName;
			}
			OrbExp(PedFire, TargetPos, TargF, TargB, TargR, TargL);
			OrbLoad(Blame, Victim, PlayerStrike);
			WAIT(4000);
		}
	}
}

bool HasASeat(Vehicle vic)
{
	bool bIn = false;
	if (vic != NULL)
	{
		if (VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(vic) > 0)
			bIn = true;
	}
	return bIn;
}
Vector4 FindingShops(Ped peddy)
{
	Vector3 PedPos = ENTITY::GET_ENTITY_COORDS(peddy, true);
	float fDis = 5000.0;
	int This = 0;
	for (int i = 0; i < (int)ShopsNTings.size(); i++)
	{
		float fit = DistanceTo(ShopsNTings[i], PedPos);
		if (fit < fDis)
		{
			fDis = fit;
			This = i;
		}
	}
	return ShopsNTings[This];
}
int YourGunNum()
{
	int iGun = 0;

	if (MySettings.Aggression <= 3)
	{
		// Low aggression: mostly unarmed or melee, rarely a light gun
		int iRoll = RandomInt(1, 10);
		if (iRoll < 6)
			iGun = 0;      // 50%: unarmed
		else if (iRoll < 9)
			iGun = 1;      // 30%: melee only
		else
			iGun = 2;      // 20%: light pistol/PDW/sawnoff only
	}
	else if (MySettings.Aggression > 3)
	{
		if (MySettings.SpaceWeaps)
			iGun = RandomInt(2, 10);
		else
			iGun = RandomInt(2, 9);
	}

	return iGun;
}
void GunningIt(Ped peddy, int gun)
{
	LoggerLight("GunningIt");
	WEAPON::REMOVE_ALL_PED_WEAPONS(peddy, 0);

	std::vector<std::string> sWeapList = {};
	if (!MySettings.SpaceWeaps && gun == 10)
		gun = 9;

	if (gun == 1)
	{
		sWeapList.push_back("WEAPON_dagger");  //0x92A27487",
		sWeapList.push_back("WEAPON_hammer");  //0x4E875F73",
		sWeapList.push_back("WEAPON_battleaxe");  //0xCD274149",
		sWeapList.push_back("WEAPON_golfclub");  //0x440E4788",
		sWeapList.push_back("WEAPON_machete");  //0xDD5DF8D9",
	}
	else if (gun == 2)
	{
		sWeapList.push_back("WEAPON_dagger");  //0x92A27487",
		sWeapList.push_back("WEAPON_pipebomb");  //0xBA45E8B8",
		sWeapList.push_back("WEAPON_navyrevolver");  //0x917F6C8C"
		sWeapList.push_back("WEAPON_combatpdw");  //0xA3D4D34",
		sWeapList.push_back("WEAPON_sawnoffshotgun");  //0x7846A318",
		sWeapList.push_back("WEAPON_sniperrifle");  //0x5FC3C11",
	}
	else if (gun == 3)
	{
		sWeapList.push_back("WEAPON_hammer");  //0x4E875F73",
		sWeapList.push_back("WEAPON_revolver");  //0xC1B3C3D1",
		sWeapList.push_back("WEAPON_smg");  //0x2BE6766B",
		sWeapList.push_back("WEAPON_pumpshotgun");  //0x1D073A89",
		sWeapList.push_back("WEAPON_advancedrifle");  //0xAF113F99",
	}
	else if (gun == 4)
	{
		sWeapList.push_back("WEAPON_battleaxe");  //0xCD274149",
		sWeapList.push_back("WEAPON_molotov");  //0x24B17070",
		sWeapList.push_back("WEAPON_stungun");  //0x3656C8C1",
		sWeapList.push_back("WEAPON_microsmg");  //0x13532244",
		sWeapList.push_back("WEAPON_musket");  //0xA89CB99E",
		sWeapList.push_back("WEAPON_gusenberg");  //0x61012683"--69
	}
	else if (gun == 5)
	{
		sWeapList.push_back("WEAPON_golfclub");  //0x440E4788",
		sWeapList.push_back("WEAPON_grenade");  //0x93E220BD",
		sWeapList.push_back("WEAPON_appistol");  //0x22D8FE39",
		sWeapList.push_back("WEAPON_assaultshotgun");  //0xE284C527",
		sWeapList.push_back("WEAPON_mg");  //0x9D07F764",
	}
	else if (gun == 6)
	{
		sWeapList.push_back("WEAPON_machete");  //0xDD5DF8D9",
		sWeapList.push_back("WEAPON_heavypistol");  //0xD205520E",
		sWeapList.push_back("WEAPON_microsmg");  //0x13532244",
		sWeapList.push_back("WEAPON_specialcarbine");  //0xC0A3098D",

	}
	else if (gun == 7)
	{
		sWeapList.push_back("WEAPON_flashlight");  //0x8BB05FD7",
		sWeapList.push_back("WEAPON_GADGETPISTOL");  //0xAF3696A1",--new to cayo ppero
		sWeapList.push_back("WEAPON_MILITARYRIFLE");  //0x624FE830"--65
		sWeapList.push_back("WEAPON_COMBATSHOTGUN");  //0x5A96BA4--54
	}
	else if (gun == 8)
	{
		sWeapList.push_back("WEAPON_marksmanrifle");  //0xC734385A",
		sWeapList.push_back("WEAPON_marksmanrifle_mk2");  //0x6A6C02E0"--74
	}
	else if (gun == 9)
	{
		sWeapList.push_back("WEAPON_pistol_mk2");  //0xBFE256D4",---------19
		sWeapList.push_back("WEAPON_snspistol_mk2");  //0x88374054",---24
		sWeapList.push_back("WEAPON_revolver_mk2");  //0xCB96392F",----29
		sWeapList.push_back("WEAPON_pumpshotgun_mk2");  //0x555AF99A",-----------46
		sWeapList.push_back("WEAPON_assaultrifle_mk2");  //0x394F415C",-------56
		sWeapList.push_back("WEAPON_carbinerifle_mk2");  //0xFAD1F1C9",------58
		sWeapList.push_back("WEAPON_specialcarbine_mk2");  //0x969C3D67",------61
		sWeapList.push_back("WEAPON_bullpuprifle_mk2");  //0x84D6FAFD",----63
		sWeapList.push_back("WEAPON_combatmg_mk2");  //0xDBBD7280",------68
		sWeapList.push_back("WEAPON_heavysniper_mk2");  //0xA914799",---72
		sWeapList.push_back("WEAPON_marksmanrifle_mk2");  //0x6A6C02E0"--74
	}
	else if (gun == 10)
	{
		sWeapList.push_back("WEAPON_raypistol");  //0xAF3696A1",--36
		sWeapList.push_back("WEAPON_raycarbine");  //0x476BF155"--44
		sWeapList.push_back("weapon_rayminigun");
	}
	else if (gun == 15)
	{
		sWeapList.push_back("WEAPON_nightstick");  //0xAF3696A1",--36
		sWeapList.push_back("WEAPON_pistol");  //0x476BF155"--44
		sWeapList.push_back("WEAPON_pumpshotgun");
	}

	for (int i = 0; i < sWeapList.size(); i++)
		WEAPON::GIVE_WEAPON_TO_PED(peddy, MyHashKey(sWeapList[i]), 9999, false, false); // holstered — weapon is owned but never drawn on spawn
	// Force unarmed stance so the ped does not visually walk around with gun out
	WEAPON::SET_CURRENT_PED_WEAPON(peddy, GAMEPLAY::GET_HASH_KEY("WEAPON_UNARMED"), true);
}
void GetInVehicle(Ped peddy, Vehicle vic, int seat)
{
	LoggerLight("GetInVehicle, seat == " + std::to_string(seat));

	Vector3 NotMoving = EntityPosition(peddy);

	int ThreePass = 3;

	while (!VEHICLE::IS_VEHICLE_SEAT_FREE(vic, seat))
	{
		GetOutVehicle(VEHICLE::GET_PED_IN_VEHICLE_SEAT(vic, seat));
		WAIT(1000);
	}

	AI::TASK_ENTER_VEHICLE(peddy, vic, -1, seat, 1.0f, 1, 0);
	WAIT(1000);

	if (DistanceTo(peddy, NotMoving) < 1.0f)
		WarptoAnyVeh(vic, peddy, seat);
	else
	{
		WAIT(5000);
		if (vic != PED::GET_VEHICLE_PED_IS_IN(peddy, false))
			WarptoAnyVeh(vic, peddy, seat);
	}
}
void PlayerEnterVeh(Vehicle vic)
{
	int Seats = FindUSeat(vic, true);
	if (Seats != -10)
		GetInVehicle(PLAYER::PLAYER_PED_ID(), vic, Seats);
}
void ForceAnim(Ped peddy, const std::string& animDict, const std::string& animName, Vector3 pos, Vector3 rot)
{
	LoggerLight("ForceAnim, animName == " + animName);

	AI::CLEAR_PED_TASKS(peddy);
	STREAMING::REQUEST_ANIM_DICT((LPSTR)animDict.c_str());
	while (!STREAMING::HAS_ANIM_DICT_LOADED((LPSTR)animDict.c_str()))
		WAIT(1);
	AI::TASK_PLAY_ANIM_ADVANCED(peddy, (LPSTR)animDict.c_str(), (LPSTR)animName.c_str(), pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, 8.0f, 0.00f, -1, 1, 0.01f, 0, 0);
	STREAMING::REMOVE_ANIM_DICT((LPSTR)animDict.c_str());
}
void ForceAnim(Ped peddy, const std::string& animDict, const std::string& animName, Vector4 pos)
{
	LoggerLight("ForceAnimv4, animName == " + animName);
	ForceAnim(peddy, animDict, animName, NewVector3(pos.X, pos.Y, pos.Z), NewVector3(0.0f, 0.0f, pos.R));
}
void AddSenario(Ped peddy, const std::string& senareo, Vector4 pos, bool sitDown)
{
	AI::TASK_START_SCENARIO_AT_POSITION(peddy, (LPSTR)senareo.c_str(), pos.X, pos.Y, pos.Z, pos.R, -1, sitDown, true);
}
// Ambient idle scenarios: make NPCs feel like real online players
// Returns the LSR-compatible gang relationship group hash for the zone the ped is in.
// Returns 0 if the zone has no gang territory mapping.
Hash GetGangGroupForZone(Ped peddy)
{
	Vector3 pos  = ENTITY::GET_ENTITY_COORDS(peddy, true);
	std::string zone = std::string(ZONE::GET_NAME_OF_ZONE(pos.x, pos.y, pos.z));

	std::string gangID;

	// Prefer live data read from LSR's GangTerritories.xml at startup.
	if (LSRData::IsAvailable)
		gangID = LSRData::GetGangForZone(zone);

	// Fallback hardcoded map used when LSR is not installed.
	if (gangID.empty()) {
		static const std::unordered_map<std::string, std::string> FallbackMap = {
			{ "DAVIS",   "AMBIENT_GANG_BALLAS"    },
			{ "SKID",    "AMBIENT_GANG_BALLAS"    },
			{ "TEXTI",   "AMBIENT_GANG_BALLAS"    },
			{ "EAST_V",  "AMBIENT_GANG_LOST"      },
			{ "MIRR",    "AMBIENT_GANG_LOST"      },
			{ "PALFOR",  "AMBIENT_GANG_LOST"      },
			{ "SLAB",    "AMBIENT_GANG_LOST"      },
			{ "EBURO",   "AMBIENT_GANG_MARABUNTE" },
			{ "MURRI",   "AMBIENT_GANG_MARABUNTE" },
			{ "CHAMH",   "AMBIENT_GANG_FAMILY"    },
			{ "STRAW",   "AMBIENT_GANG_FAMILY"    },
			{ "RANCHO",  "AMBIENT_GANG_MEXICAN"   },
			{ "SANAND",  "AMBIENT_GANG_MEXICAN"   },
			{ "CYPRE",   "AMBIENT_GANG_MEXICAN"   },
			{ "STAD",    "AMBIENT_GANG_SALVA"     },
			{ "DTVINE",  "AMBIENT_GANG_GAMBETTI"  },
			{ "WVINE",   "AMBIENT_GANG_GAMBETTI"  },
			{ "LMESA",   "AMBIENT_GANG_MADRAZO"   },
			{ "CHIL",    "AMBIENT_GANG_MADRAZO"   },
			{ "KOREAT",  "AMBIENT_GANG_KKANGPAE"  },
			{ "VCANA",   "AMBIENT_GANG_KKANGPAE"  },
			{ "LEGSQU",  "AMBIENT_GANG_WEICHENG"  },
			{ "PBOX",    "AMBIENT_GANG_WEICHENG"  },
			{ "LOSPUER", "AMBIENT_GANG_ARMENIAN"  },
			{ "BEACH",   "AMBIENT_GANG_YARDIES"   },
			{ "DELSOL",  "AMBIENT_GANG_YARDIES"   },
			{ "ELYSIAN", "AMBIENT_GANG_DIABLOS"   },
			{ "GRAPES",  "AMBIENT_GANG_PAVANO"    },
			{ "RICHM",   "AMBIENT_GANG_MESSINA"   },
			{ "CHU",     "AMBIENT_GANG_ANCELOTTI" },
			{ "ALAMO",   "AMBIENT_GANG_HILLBILLY" },
			{ "SANDY",   "AMBIENT_GANG_HILLBILLY" },
			{ "DESRT",   "AMBIENT_GANG_ANGELS"    },
			{ "HARMO",   "AMBIENT_GANG_ANGELS"    },
			{ "PALETO",  "AMBIENT_GANG_LUPISELLA" },
		};
		auto it = FallbackMap.find(zone);
		if (it != FallbackMap.end()) gangID = it->second;
	}

	if (gangID.empty()) return 0;

	Hash gangHash = -1;
	PED::ADD_RELATIONSHIP_GROUP((LPSTR)gangID.c_str(), &gangHash);
	return gangHash;
}

// Mirrors the GP_Attack relationship values onto a gang group so that
// LSR-recognised gang NPCs still behave identically to hostile PZ NPCs.
void ApplyGangRelationships(Hash gangGroup)
{
	int hostileVal = (MySettings.Aggression <= 3) ? 3 : 5;
	int playerVal  = (MySettings.Aggression > 5)  ? 5 : 3;

	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(playerVal,  GP_Player,  gangGroup);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(playerVal,  gangGroup,  GP_Player);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(hostileVal, gangGroup,  Gp_Friend);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(hostileVal, Gp_Friend,  gangGroup);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5,          gangGroup,  Gp_Follow);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5,          Gp_Follow,  gangGroup);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(hostileVal, gangGroup,  GP_Attack);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(hostileVal, GP_Attack,  gangGroup);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(hostileVal, gangGroup,  GP_Mental);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(hostileVal, GP_Mental,  gangGroup);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0,          gangGroup,  gangGroup); // same gang: neutral
}

// Returns 1 (low), 2 (medium), or 3 (high) crime aggression for the given GTA zone name.
// High = Davis, Strawberry, Rancho, Skid Row, etc.
// Low = Vinewood Hills, Rockford Hills, Golf club, etc.
// Everything else = 2 (medium baseline).
int GetZoneAggressionTier(const std::string& zone)
{
	static const char* highCrime[] = {
		"DAVIS", "STRAW", "RANCHO", "TEXTI", "SKID",
		"BANNING", "ELYSIAN", "EBURO", "MURIE", "CYPRE"
	};
	static const char* lowCrime[] = {
		"VINE", "ROCKF", "BEVER", "RTRAK", "GOLF",
		"RICHM", "DELPE", "LMESA", "AIRP"
	};
	for (auto s : highCrime) if (zone == s) return 3;
	for (auto s : lowCrime)  if (zone == s) return 1;
	return 2;
}

// Gives the criminal ped a weapon appropriate to their zone's gang profile.
// equipNow=false keeps the weapon holstered so no draw animation fires
// and no wanted level is generated on spawn.
void ArmCriminalPed(PlayerBrain* brain, const LSRGangProfile* gp)
{
	Ped peddy = brain->ThisPed;

	// Default percentages when no gang profile is available
	int pctLong   = gp ? gp->pctLongGuns  : 10;
	int pctSide   = gp ? gp->pctSidearms  : 30;
	int pctMelee  = gp ? gp->pctMelee     : 20;

	int roll = RandomInt(1, 100);
	Hash weapon = 0;
	int  ammo   = 0;

	static const Hash longGuns[] = {
		GAMEPLAY::GET_HASH_KEY("WEAPON_ASSAULTRIFLE"),
		GAMEPLAY::GET_HASH_KEY("WEAPON_CARBINERIFLE"),
		GAMEPLAY::GET_HASH_KEY("WEAPON_SMG"),
		GAMEPLAY::GET_HASH_KEY("WEAPON_PUMPSHOTGUN")
	};
	static const Hash sidearms[] = {
		GAMEPLAY::GET_HASH_KEY("WEAPON_PISTOL"),
		GAMEPLAY::GET_HASH_KEY("WEAPON_COMBATPISTOL"),
		GAMEPLAY::GET_HASH_KEY("WEAPON_APPISTOL")
	};
	static const Hash melees[] = {
		GAMEPLAY::GET_HASH_KEY("WEAPON_KNIFE"),
		GAMEPLAY::GET_HASH_KEY("WEAPON_BAT"),
		GAMEPLAY::GET_HASH_KEY("WEAPON_CROWBAR")
	};

	if (roll <= pctLong) {
		weapon = longGuns[RandomInt(0, 3)];
		ammo   = RandomInt(60, 120);
	}
	else if (roll <= pctLong + pctSide) {
		weapon = sidearms[RandomInt(0, 2)];
		ammo   = RandomInt(20, 45);
	}
	else if (roll <= pctLong + pctSide + pctMelee) {
		weapon = melees[RandomInt(0, 2)];
		ammo   = 1;
	}

	if (weapon != 0) {
		// equipNow=false — weapon is owned but holstered; no draw, no wanted level.
		WEAPON::GIVE_WEAPON_TO_PED(peddy, weapon, ammo, false, false);
		brain->ArmedWeaponHash = weapon;
	}
}

void DoAmbientScenario(PlayerBrain* brain)
{
	Ped peddy = brain->ThisPed;

	// Street / gang-territory scenarios
	static const std::string StreetScen[] = {
		"WORLD_HUMAN_HANG_OUT_STREET",
		"WORLD_HUMAN_SMOKING",
		"WORLD_HUMAN_DRINKING",
		"WORLD_HUMAN_STAND_IMPATIENT",
		"WORLD_HUMAN_AA_SMOKE",
		"WORLD_HUMAN_LEANING",
	};
	// Rural / open-area scenarios
	static const std::string RuralScen[] = {
		"WORLD_HUMAN_SMOKING",
		"WORLD_HUMAN_AA_SMOKE",
		"WORLD_HUMAN_LOOK_AT_SCENERY",
		"WORLD_HUMAN_GUARD_STAND",
		"WORLD_HUMAN_BINOCULARS",
		"WORLD_HUMAN_LEANING",
	};
	// Urban / commercial scenarios
	static const std::string UrbanScen[] = {
		"WORLD_HUMAN_STAND_MOBILE",
		"WORLD_HUMAN_CLIPBOARD",
		"WORLD_HUMAN_STRETCHING",
		"WORLD_HUMAN_LEANING",
		"WORLD_HUMAN_DRINKING",
		"WORLD_HUMAN_LOOK_AT_SCENERY",
	};

	Vector3 pos = ENTITY::GET_ENTITY_COORDS(peddy, true);
	std::string zone = std::string(ZONE::GET_NAME_OF_ZONE(pos.x, pos.y, pos.z));

	static const std::unordered_set<std::string> StreetZones = {
		"DAVIS","SKID","TEXTI","RANCHO","CHAMH","STRAW","EAST_V","MIRR",
		"EBURO","MURRI","STAD","CYPRE","LOSPUER","ELYSIAN","SANAND"
	};
	static const std::unordered_set<std::string> RuralZones = {
		"ALAMO","DESRT","SANDY","HARMO","GRAPES","BLMCC","MTCHIL",
		"MTGORDO","CANNY","ELGORL","NCHU","PALETO","PALFOR","SLAB","SANSZ"
	};

	const std::string* pool;
	int poolSize = 6;

	if (StreetZones.count(zone))
		pool = StreetScen;
	else if (RuralZones.count(zone))
		pool = RuralScen;
	else
		pool = UrbanScen;

	int idx = RandomInt(0, poolSize - 1);
	AI::CLEAR_PED_TASKS(peddy);
	AI::TASK_START_SCENARIO_IN_PLACE(peddy, (LPSTR)pool[idx].c_str(), 0, true);
	PED::SET_PED_KEEP_TASK(peddy, true);
	PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy, false); // allow LSR interactions

	// NPC will abandon this scenario after 10-20 seconds and pick a new action
	brain->ScenarioTimer = InGameTime() + RandomInt(10000, 20000);
	brain->ShopTimer = 0;
}

// Forward declarations needed by PickNextAction (defined later in file)
void WalkHere(Ped peddy, Vector3 pos);
void WalkHere(Ped peddy, Vector4 pos);
extern const std::vector<Vector3> PlayerHotspots;

// Criminal ped draws their weapon and attacks the nearest non-PZ ambient pedestrian.
// LSR's passive crime detection will naturally pick this up as a GTA assault event.
void DoFight(PlayerBrain* brain)
{
	Ped peddy = brain->ThisPed;
	Vector3 myPos = ENTITY::GET_ENTITY_COORDS(peddy, true);

	// Scan nearby world peds (ScriptHookV extension).
	const int MAX_SCAN = 32;
	int nearPeds[MAX_SCAN] = {};
	int count = worldGetAllPeds(nearPeds, MAX_SCAN);

	Ped target = NULL;
	float nearest = 40.0f;
	for (int i = 0; i < count; i++)
	{
		Ped p = nearPeds[i];
		if (!ENTITY::DOES_ENTITY_EXIST(p) || p == peddy) continue;
		if (ENTITY::IS_ENTITY_DEAD(p))                    continue;
		if (PED::IS_PED_A_PLAYER(p))                      continue;

		// Skip PZ-managed peds
		bool isPZ = false;
		for (auto& b : PedList)
			if (b.ThisPed == p) { isPZ = true; break; }
		if (isPZ) continue;

		float dist = SYSTEM::VDIST(myPos.x, myPos.y, myPos.z,
			ENTITY::GET_ENTITY_COORDS(p, true).x,
			ENTITY::GET_ENTITY_COORDS(p, true).y,
			ENTITY::GET_ENTITY_COORDS(p, true).z);
		if (dist < nearest)
		{
			nearest = dist;
			target  = p;
		}
	}

	if (target == NULL) return;

	// Equip the armed weapon now that a crime is actually happening.
	if (brain->ArmedWeaponHash != 0)
		WEAPON::SET_CURRENT_PED_WEAPON(peddy, brain->ArmedWeaponHash, true);

	AI::CLEAR_PED_TASKS(peddy);
	AI::TASK_COMBAT_PED(peddy, target, 0, 16);
	brain->IsWanted    = true;
	brain->WantedTimer = InGameTime() + 45000; // flee after 45s
	brain->ScenarioTimer = InGameTime() + RandomInt(12000, 20000);
}

// Carjack the nearest ambient vehicle that isn't a PZ-managed vehicle or the player's car.
// Uses flag 9 (force carjack) which triggers GTA V's native carjack animation and ejects the driver.
void DoCarjack(PlayerBrain* brain)
{
	Ped peddy = brain->ThisPed;
	Vector3 pos = ENTITY::GET_ENTITY_COORDS(peddy, true);

	// Find closest driveable vehicle within 50m (flags 70 = cars/bikes, excludes boats/planes)
	Vehicle targetVeh = VEHICLE::GET_CLOSEST_VEHICLE(pos.x, pos.y, pos.z, 50.0f, 0, 70);
	if (!ENTITY::DOES_ENTITY_EXIST(targetVeh))
		return;

	// Don't carjack any PZ-managed vehicle
	for (auto& b : PedList)
		if (b.ThisVeh == targetVeh)
			return;

	// Don't carjack the player's current vehicle
	if (targetVeh == PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false))
		return;

	// Flag 9 = carjack force: GTA V ejects the current driver and plays the carjack animation
	AI::CLEAR_PED_TASKS(peddy);
	AI::TASK_ENTER_VEHICLE(peddy, targetVeh, -1, -2, 15.0f, 9, 0);

	brain->IsWanted    = true;
	brain->WantedTimer = InGameTime() + 60000; // drive away for 60s then clear
	// Keep AI tick dormant while the ped drives the stolen vehicle
	brain->ShopTimer = InGameTime() + RandomInt(90000, 180000);
}

// Two PZ peds meet and face each other briefly — simulates a drug exchange visually.
// LSR will not flag this as a tracked crime, but it looks authentic in the game world.
void DoDrugDeal(PlayerBrain* brain)
{
	Ped peddy = brain->ThisPed;
	Vector3 myPos = ENTITY::GET_ENTITY_COORDS(peddy, true);

	// Find the nearest on-foot PZ ped within 20m to deal with
	Ped target = NULL;
	float nearest = 20.0f;
	for (auto& b : PedList)
	{
		if (b.ThisPed == peddy || b.Driver || b.Passenger || b.YoDeeeed)
			continue;
		float dist = SYSTEM::VDIST(myPos.x, myPos.y, myPos.z,
			ENTITY::GET_ENTITY_COORDS(b.ThisPed, true).x,
			ENTITY::GET_ENTITY_COORDS(b.ThisPed, true).y,
			ENTITY::GET_ENTITY_COORDS(b.ThisPed, true).z);
		if (dist < nearest)
		{
			nearest = dist;
			target = b.ThisPed;
		}
	}
	if (target == NULL)
		return;

	// Walk to target and face them — the brief proximity exchange sells the deal visually
	AI::CLEAR_PED_TASKS(peddy);
	AI::TASK_GO_TO_ENTITY(peddy, target, -1, 2.0f, 1.5f, 1073741824, 0);
	brain->ScenarioTimer = InGameTime() + RandomInt(8000, 15000);
}

// ---------------------------------------------------------------------------
// DoBuyDrugs — civilian or criminal buyer approaches a nearby dealer.
// Priority order:
//   1. PZ dealer ped (IsDealer == true) within 30 m
//   2. Ambient ped in an LSR gang relationship group within 30 m (LSR gang member / dealer)
// The buyer walks to the dealer for 12-22 s (ShopTimer), then the ProcessPZ
// ShopTimer handler clears DrugBuyTarget and returns the ped to normal AI.
// ---------------------------------------------------------------------------
void DoBuyDrugs(PlayerBrain* brain)
{
	Ped peddy = brain->ThisPed;
	Vector3 myPos = ENTITY::GET_ENTITY_COORDS(peddy, true);

	Ped dealerPed = NULL;
	float nearestDist = 30.0f;

	// 1) Prefer a PZ dealer ped
	for (int bi = 0; bi < (int)PedList.size(); bi++)
	{
		PlayerBrain& b = PedList[bi];
		if (b.ThisPed == peddy || b.Driver || b.Passenger || b.YoDeeeed || !b.IsDealer)
			continue;
		float d = DistanceTo(b.ThisPed, myPos);
		if (d < nearestDist) { nearestDist = d; dealerPed = b.ThisPed; }
	}

	// 2) Fall back to an ambient LSR gang-member ped
	if (dealerPed == NULL && !ActiveGangGroups.empty())
	{
		int nearBuf[256] = {};
		PED::GET_PED_NEARBY_PEDS(peddy, nearBuf, -1);
		int nearN = nearBuf[0];
		if (nearN > 255) nearN = 255;
		for (int ki = 1; ki <= nearN; ki++)
		{
			Ped cand = (Ped)nearBuf[ki];
			if (!cand || !(bool)ENTITY::DOES_ENTITY_EXIST(cand)) continue;
			if ((bool)ENTITY::IS_ENTITY_DEAD(cand))              continue;
			if ((bool)PED::IS_PED_A_PLAYER(cand))               continue;
			// Skip PZ peds
			bool isPZ = false;
			for (int pi = 0; pi < (int)PedList.size(); pi++)
				if (PedList[pi].ThisPed == cand) { isPZ = true; break; }
			if (isPZ) continue;
			// Must be in a known LSR gang relationship group
			Hash relGroup = PED::GET_PED_RELATIONSHIP_GROUP_HASH(cand);
			if (ActiveGangGroups.find(relGroup) == ActiveGangGroups.end()) continue;
			float d = DistanceTo(cand, myPos);
			if (d < nearestDist) { nearestDist = d; dealerPed = cand; break; }
		}
	}

	if (dealerPed == NULL) return;

	// Walk toward the dealer — proximity + movement sells the exchange visually
	AI::CLEAR_PED_TASKS(peddy);
	AI::TASK_GO_TO_ENTITY(peddy, dealerPed, 30000, 1.5f, 1.0f, 0, 0);
	PED::SET_PED_KEEP_TASK(peddy, true);
	PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy, true);

	brain->DrugBuyTarget = dealerPed;
	brain->ShopTimer     = InGameTime() + RandomInt(12000, 22000);
}

// After a scenario or shop visit expires, pick the next thing the NPC should do.
// Criminal peds (IsCriminal) get a chance to carjack or deal drugs when their CrimeTimer has elapsed.
// Weighted for civilians: 50% ambient wander, 20% walk to shop, 15% brief scenario, 15% walk to hotspot.
// Phase 1: Walk the criminal ped to the nearest robbable store within 200m.
// Phase 2 (triggered by ShopTimer expiry in ProcessPZ): draw weapon and hold.
// Phase 3 (triggered by ScenarioTimer expiry): holster, sprint flee, IsWanted.
// If no store is within range the function does nothing and returns false.
bool DoStoreRobbery(PlayerBrain* brain)
{
	if (!LSRData::IsAvailable) return false;

	Ped peddy = brain->ThisPed;
	Vector3 pos = ENTITY::GET_ENTITY_COORDS(peddy, true);

	// Try ConvenienceStore first, fall back to LiquorStore
	const LSRLocation* store =
		LSRData::GetNearestLocationWithin(pos.x, pos.y, pos.z, "ConvenienceStore", 200.0f);
	if (!store)
		store = LSRData::GetNearestLocationWithin(pos.x, pos.y, pos.z, "LiquorStore", 200.0f);
	if (!store) return false;

	// Phase 1: walk to the store entrance
	Vector3 v; v.x = store->x; v.y = store->y; v.z = store->z;
	WalkHere(peddy, v);
	brain->RobPhase  = 1;
	brain->ShopTimer = InGameTime() + RandomInt(18000, 28000); // time to walk there
	return true;
}

void PickNextAction(PlayerBrain* brain)
{
	Ped peddy = brain->ThisPed;
	brain->ScenarioTimer = 0;
	brain->ShopTimer = 0;

	// Crime opportunity: only for criminal peds whose cooldown has expired
	if (brain->IsCriminal && brain->CrimeTimer < InGameTime())
	{
		// Crime timer scale by zone aggression tier.
		int ctMin, ctMax;
		if      (brain->AggressionTier == 3) { ctMin = 30000;  ctMax = 60000;  }
		else if (brain->AggressionTier == 2) { ctMin = 60000;  ctMax = 120000; }
		else                                  { ctMin = 120000; ctMax = 240000; }

		int crimeRoll = RandomInt(1, 100);

		// Tier 3 (high-crime) rolls fight/carjack more aggressively.
		// Tier 1 (low-crime) almost never carjacks; fights are rarer still.
		int carjackThresh = (brain->AggressionTier == 3) ? 30 :
		                    (brain->AggressionTier == 2) ? 20 : 10;
		int fightThresh   = carjackThresh +
		                    ((brain->AggressionTier == 3) ? 40 :
		                     (brain->AggressionTier == 2) ? 20 : 5);

		if (crimeRoll <= carjackThresh)
		{
			DoCarjack(brain);
			brain->CrimeTimer = InGameTime() + RandomInt(ctMin * 3, ctMax * 3);
			return;
		}
		else if (crimeRoll <= fightThresh)
		{
			// Pick a fight with the nearest ambient pedestrian.
			DoFight(brain);
			brain->CrimeTimer = InGameTime() + RandomInt(ctMin, ctMax);
			return;
		}
		else if (brain->IsDealer && crimeRoll <= fightThresh + 15)
		{
			DoDrugDeal(brain);
			brain->CrimeTimer = InGameTime() + RandomInt(ctMin, ctMax);
			return;
		}
		else if (crimeRoll <= fightThresh + 25 && brain->RobPhase == 0)
		{
			// Attempt a store robbery — walks to nearest ConvenienceStore/LiquorStore
			// within 200m. Phase 2 (draw weapon) and phase 3 (flee) are handled by
			// the ShopTimer and ScenarioTimer checks in ProcessPZ.
			if (DoStoreRobbery(brain))
			{
				brain->CrimeTimer = InGameTime() + RandomInt(ctMin * 4, ctMax * 4);
				return;
			}
		}
		// Fall through: no action this tick, reset timer.
		brain->CrimeTimer = InGameTime() + RandomInt(ctMin, ctMax);
	}

	// Drug purchase opportunity: non-dealer peds in poor/gang zones occasionally
	// approach a nearby dealer (PZ or LSR gang member) to buy drugs.
	// 12% in poor zones, 10% in tier-3 zones, 4% in mixed zones.
	if (!brain->IsDealer && brain->DrugBuyTarget == NULL && brain->DrugBuyTimer < InGameTime())
	{
		int buyChance = 0;
		if      (brain->ZoneEconomy == 1)        buyChance = 12;
		else if (brain->AggressionTier >= 3)     buyChance = 10;
		else if (brain->ZoneEconomy == 2)        buyChance =  4;
		if (buyChance > 0 && RandomInt(1, 100) <= buyChance)
		{
			DoBuyDrugs(brain);
			if (brain->DrugBuyTarget != NULL)
			{
				brain->DrugBuyTimer = InGameTime() + RandomInt(180000, 420000); // 3-7 min cooldown
				return;
			}
		}
	}

	int roll = RandomInt(1, 100);

	if (roll <= 50)
	{
		// Use GTA's native ambient ped AI - ped walks around naturally,
		// uses crossings, stops to look around, reacts to environment.
		AI::CLEAR_PED_TASKS(peddy);
		AI::TASK_WANDER_STANDARD(peddy, 10.0f, 10);
		PED::SET_PED_KEEP_TASK(peddy, true);
		PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy, false);
		brain->ScenarioTimer = InGameTime() + RandomInt(20000, 45000);
	}
	else if (roll <= 70)
	{
		// Walk to a shop / amenity point
		WalkHere(peddy, FindingShops(peddy));
		brain->ShopTimer = InGameTime() + RandomInt(15000, 30000);
	}
	else if (roll <= 85)
	{
		// Brief standing animation (smoking, leaning, phone, etc.)
		DoAmbientScenario(brain);
	}
	else if (LSRData::IsAvailable)
	{
		// Route to a time-of-day appropriate LSR location that is within 250m.
		// Locations further away are skipped — peds would despawn before arriving.
		int hour = TIME::GET_CLOCK_HOURS();
		Vector3 pedPos = ENTITY::GET_ENTITY_COORDS(peddy, true);
		const float MAX_ROUTE_DIST = 250.0f;

		const LSRLocation* dest = nullptr;

		if (brain->SchedulePhase == 3 || brain->SchedulePhase == 2)
		{
			// Night/evening: nearest open bar within 250m, else nearest restaurant.
			dest = LSRData::GetNearestLocationWithin(pedPos.x, pedPos.y, pedPos.z, "Bar", MAX_ROUTE_DIST);
			if (!dest)
				dest = LSRData::GetNearestLocationWithin(pedPos.x, pedPos.y, pedPos.z, "Restaurant", MAX_ROUTE_DIST);
		}
		else if (brain->IsDealer)
		{
			dest = LSRData::GetNearestLocationWithin(pedPos.x, pedPos.y, pedPos.z, "IllicitMarketplace", MAX_ROUTE_DIST);
		}
		else
		{
			const char* type = (hour < 12) ? "GasStation" : "Restaurant";
			dest = LSRData::GetNearestLocationWithin(pedPos.x, pedPos.y, pedPos.z, type, MAX_ROUTE_DIST);
		}

		if (dest != nullptr)
		{
			Vector3 v; v.x = dest->x; v.y = dest->y; v.z = dest->z;
			WalkHere(peddy, v);
			brain->ShopTimer      = InGameTime() + RandomInt(20000, 40000);
			brain->InteriorEntryID = dest->interiorID; // -1 if no interior
		}
		else
		{
			// No LSR location within range — fall back to static hotspot or wander.
			if (!PlayerHotspots.empty())
			{
				int iSpot = RandomInt(0, (int)PlayerHotspots.size() - 1);
				WalkHere(peddy, PlayerHotspots[iSpot]);
				brain->ShopTimer = InGameTime() + RandomInt(20000, 40000);
			}
			else
			{
				AI::CLEAR_PED_TASKS(peddy);
				AI::TASK_WANDER_STANDARD(peddy, 10.0f, 10);
				PED::SET_PED_KEEP_TASK(peddy, true);
				PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy, false);
				brain->ScenarioTimer = InGameTime() + RandomInt(20000, 45000);
			}
		}
	}
	else if (!PlayerHotspots.empty())
	{
		// Standalone mode: use static hotspot list
		int iSpot = RandomInt(0, (int)PlayerHotspots.size() - 1);
		WalkHere(peddy, PlayerHotspots[iSpot]);
		brain->ShopTimer = InGameTime() + RandomInt(20000, 40000);
	}
	else
	{
		// Last resort: ambient wander
		AI::CLEAR_PED_TASKS(peddy);
		AI::TASK_WANDER_STANDARD(peddy, 10.0f, 10);
		PED::SET_PED_KEEP_TASK(peddy, true);
		PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy, false);
		brain->ScenarioTimer = InGameTime() + RandomInt(20000, 45000);
	}
}
void FollowPed(Ped target, Ped follower)
{
	AI::TASK_FOLLOW_TO_OFFSET_OF_ENTITY(follower, target, 0, 0, 0, 45, -1, 10, 1);
}
int LandingGear(Vehicle vic)
{
	//	0: Deployed
	//	1 : Closing
	//	2 : Opening
	//	3 : Retracted
	return VEHICLE::_GET_VEHICLE_LANDING_GEAR(vic);
}
void LandNearHeli(Ped peddy, Vehicle vic, Vector3 targetPos)
{
	AI::CLEAR_PED_TASKS(peddy);
	float HeliDesX = targetPos.x;
	float HeliDesY = targetPos.y;
	float HeliDesZ = targetPos.z;
	float HeliSpeed = 35.0f;
	float HeliLandArea = 10.0f;
	Vector3 VHickPos = ENTITY::GET_ENTITY_COORDS(vic, true);
	float dx = VHickPos.x - HeliDesX;
	float dy = VHickPos.y - HeliDesY;

	float HeliDirect = GAMEPLAY::GET_HEADING_FROM_VECTOR_2D(dx, dy) - 180.0f;

	AI::TASK_HELI_MISSION(peddy, vic, 0, 0, HeliDesX, HeliDesY, HeliDesZ, 20, HeliSpeed, HeliLandArea, HeliDirect, -1, -1, -1, 32);

	PED::SET_PED_FIRING_PATTERN(peddy, MyHashKey("FIRING_PATTERN_BURST_FIRE_HELI"));

	PED::SET_PED_KEEP_TASK(peddy, true);
	PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy, true);
}
void LandNearPlane(Ped peddy, Vehicle vic, Vector3 startPos, Vector3 finishPos)
{
	AI::CLEAR_PED_TASKS(peddy);
	AI::TASK_PLANE_LAND(peddy, vic, startPos.x, startPos.y, startPos.z, finishPos.x, finishPos.y, finishPos.z);
	PED::SET_PED_KEEP_TASK(peddy, true);
	PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy, true);
}
void WalkHere(Ped peddy, Vector3 pos)
{
	AI::CLEAR_PED_TASKS(peddy);
	AI::TASK_FOLLOW_NAV_MESH_TO_COORD(peddy, pos.x, pos.y, pos.z, 1.0, -1, 0.0, false, 0.0);
	PED::SET_PED_KEEP_TASK(peddy, true);
	PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy, false); // allow LSR interactions
}
void WalkHere(Ped peddy, Vector4 pos)
{
	WalkHere(peddy, NewVector3(pos.X, pos.Y, pos.Z));
}
void RunHere(Ped peddy, Vector3 pos)
{
	AI::CLEAR_PED_TASKS(peddy);
	AI::TASK_FOLLOW_NAV_MESH_TO_COORD(peddy, pos.x, pos.y, pos.z, 2.0, -1, 0.0, false, 0.0);
	PED::SET_PED_KEEP_TASK(peddy, true);
}
void RunHere(Ped Peddy, Vector4 pos)
{
	RunHere(Peddy, NewVector3(pos.X, pos.Y, pos.Z));
}
// Real GTA V hotspot locations for FiveM-like driving behavior
const std::vector<Vector3> PlayerHotspots = {
	NewVector3(-1391.76f, -590.92f,  30.00f),  // LSIA Airport
	NewVector3( 414.15f, -1020.33f,  29.35f),  // Legion Square
	NewVector3(-710.40f,  -939.45f,  19.22f),  // Del Perro Pier
	NewVector3(1174.60f,  -324.48f,  69.33f),  // Vinewood Hills
	NewVector3(1109.48f,  -982.25f,  46.42f),  // Rockford Hills
	NewVector3( -43.85f, -1103.23f,  26.42f),  // Downtown LS
	NewVector3( 350.19f, -1960.80f,  23.26f),  // Elysian Island
	NewVector3(-2009.36f,  -50.88f,  18.61f),  // Chumash
	NewVector3(1702.48f,  4934.50f,  41.80f),  // Paleto Bay
	NewVector3(-1093.24f, 2705.14f,  18.89f),  // Sandy Shores
	NewVector3( 177.41f,  3648.50f,  36.00f),  // Grapeseed
	NewVector3(-269.72f,  -957.50f,  31.22f),  // Little Seoul
	NewVector3(-2165.62f, 4285.45f,  47.31f),  // Paleto Forest
	NewVector3(2681.57f,  3476.43f,  55.24f),  // Grand Senora Desert
	NewVector3(-1216.89f,-1578.56f,   4.61f),  // Vespucci Canals
	NewVector3(1136.41f,  2126.74f,  42.32f),  // Alamo Sea
	NewVector3(-3175.97f, 1086.28f,  20.84f),  // Pacific Bluffs
	NewVector3( -552.49f,  -192.32f, 37.63f),  // Maze Bank Arena
	NewVector3(   26.07f,  -951.75f, 28.57f),  // Maze Bank Tower
	NewVector3(-1299.30f, -1128.90f,  6.99f),  // Chumash Beach
};

// lawAbiding: true = friendly civilian (obeys traffic signals, modest speed)
//             false = hostile/gang driver (reckless, ignores lights)
// Driving style constants (GTA5 native bitmask):
//   1074528293 = careful + follow traffic (law-abiding)
//   786603     = aggressive / ignore lights (reckless)
//   262956     = standard wander
void DriveToHotspot(Ped peddy, Vehicle vic, bool lawAbiding = false)
{
	LoggerLight("DriveToHotspot");
	if (!(bool)PED::IS_PED_IN_ANY_VEHICLE(peddy, 0)) return;
	if (peddy != VEHICLE::GET_PED_IN_VEHICLE_SEAT(PED::GET_VEHICLE_PED_IS_USING(peddy), -1)) return;
	int iSpot = LessRandomInt("Hotspot", 0, (int)PlayerHotspots.size() - 1);
	Vector3 dest = PlayerHotspots[iSpot];
	int iDriveStyle;
	float fSpeed;
	if (lawAbiding)
	{
		// Civilian: obey lights, reasonable city speed (10-18 m/s ≈ 36-65 km/h)
		iDriveStyle = 1074528293;
		fSpeed      = RandomFloat(10.0f, 18.0f);
	}
	else
	{
		// Gang/hostile: reckless — occasionally very aggressive
		fSpeed      = RandomFloat(22.0f, 40.0f);
		iDriveStyle = 1074528293;
		int iRand   = LessRandomInt("HotspotStyle", 1, 10);
		if      (iRand > 8) iDriveStyle = 786603;
		else if (iRand > 6) fSpeed = RandomFloat(15.0f, 25.0f);
	}
	AI::CLEAR_PED_TASKS(peddy);
	AI::TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(peddy, vic, dest.x, dest.y, dest.z, fSpeed, iDriveStyle, 15.0f);
	PED::SET_PED_KEEP_TASK(peddy, true);
	PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy, false);
}

void DriveAround(Ped peddy, bool lawAbiding = false)
{
	AI::CLEAR_PED_TASKS(peddy);
	if ((bool)PED::IS_PED_IN_ANY_VEHICLE(peddy, 0))
	{
		if (peddy == VEHICLE::GET_PED_IN_VEHICLE_SEAT(PED::GET_VEHICLE_PED_IS_USING(peddy), -1))
		{
			Vehicle Vic = PED::GET_VEHICLE_PED_IS_IN(peddy, false);
			int iStyle;
			float fSpeed;
			if (lawAbiding)
			{
				// Civilian: obey traffic lights, moderate speed
				iStyle = 1074528293;
				fSpeed = RandomFloat(10.0f, 18.0f);
			}
			else
			{
				iStyle = 262956;
				fSpeed = 25.0f;
				int iRand = LessRandomInt("DriveWanderStyle", 1, 10);
				if      (iRand > 8) { iStyle = 786603; fSpeed = RandomFloat(35.0f, 55.0f); }
				else if (iRand > 5) { fSpeed = RandomFloat(22.0f, 32.0f); }
				else                { fSpeed = RandomFloat(12.0f, 22.0f); }
			}
			AI::TASK_VEHICLE_DRIVE_WANDER(peddy, Vic, fSpeed, iStyle);
			PED::SET_PED_KEEP_TASK(peddy, true);
			PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy, true);
		}
	}

}
void DriveToooDest(Ped peddy, Vehicle vic, Vector3 pos, float speed, bool blocking, bool runOver, bool planeTaxi)
{
	AI::CLEAR_PED_TASKS(peddy);
	if ((bool)PED::IS_PED_IN_ANY_VEHICLE(peddy, 0))
	{
		if (runOver)
			AI::TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(peddy, vic, pos.x, pos.y, pos.z, speed, 262956, 0.1f);
		else if (planeTaxi)
			AI::TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(peddy, vic, pos.x, pos.y, pos.z, speed, 16777216, 1.0f);
		else
			AI::TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(peddy, vic, pos.x, pos.y, pos.z, speed, 1074528293, 15.0f);//536871355

		PED::SET_PED_KEEP_TASK(peddy, true);
		if (blocking)
			PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy, true);
	}
}
void FollowThatCart(Ped peddy, Vehicle vic, Ped target, bool attack)
{
	AI::CLEAR_PED_TASKS(peddy);
	if ((bool)PED::IS_PED_IN_ANY_VEHICLE(peddy, 0))
	{
		if (attack)
		{
			AI::TASK_VEHICLE_CHASE(peddy, target);
			AI::SET_TASK_VEHICLE_CHASE_IDEAL_PURSUIT_DISTANCE(peddy, 15.0f);
		}
		else
		{
			if ((bool)PED::IS_PED_IN_ANY_VEHICLE(target, 0))
			{
				Vehicle TargVeh = PED::GET_VEHICLE_PED_IS_IN(target, false);
				float fSpeeds = VEHICLE::_GET_VEHICLE_SPEED(TargVeh);
				AI::TASK_VEHICLE_ESCORT(peddy, vic, TargVeh, -1, fSpeeds, 262956, 30.0f, -1, 0.1f);
			}
			else
			{
				WEAPON::REMOVE_ALL_PED_WEAPONS(peddy, true);
				AI::TASK_VEHICLE_CHASE(peddy, target);
				AI::SET_TASK_VEHICLE_CHASE_IDEAL_PURSUIT_DISTANCE(peddy, 35.0f);
			}
		}
	}
}
void FollowThatRaming(Ped peddy, Vehicle vic, Ped target)
{
	AI::CLEAR_PED_TASKS(peddy);
	if ((bool)PED::IS_PED_IN_ANY_VEHICLE(peddy, 0))
	{
		WEAPON::REMOVE_ALL_PED_WEAPONS(peddy, true);
		AI::TASK_VEHICLE_CHASE(peddy, target);
		AI::SET_TASK_VEHICLE_CHASE_IDEAL_PURSUIT_DISTANCE(peddy, 5.0f);
	}
}
void DriveBye(Ped peddy, Vehicle vic, Ped target, bool driver)
{
	AI::CLEAR_PED_TASKS(peddy);
	if ((bool)PED::IS_PED_IN_ANY_VEHICLE(peddy, 0))
	{
		AI::TASK_VEHICLE_SHOOT_AT_PED(peddy, target, 5);
		if (driver)
		{
			if ((bool)PED::IS_PED_IN_ANY_VEHICLE(target, 0))
				AI::TASK_VEHICLE_CHASE(peddy, target);
			else
				DriveToooDest(peddy, vic, ENTITY::GET_ENTITY_COORDS(target, true), 45.0f, true, true, false);
		}
	}
}
void FlyHeli(Ped peddy, Vehicle vic, Vector3 destPos, float speed, float landing)
{
	float ActualGround = 0.0f;
	GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(destPos.x, destPos.y, destPos.z, &ActualGround, 0);
	if (ActualGround > destPos.z)
		destPos.z = ActualGround;
	ENTITY::FREEZE_ENTITY_POSITION(vic, false);
	AI::CLEAR_PED_TASKS(peddy);
	float HeliDesX = destPos.x;
	float HeliDesY = destPos.y;
	float HeliDesZ = destPos.z;
	float HeliSpeed = speed;
	float HeliLandArea = landing;

	Vector3 VHickPos = ENTITY::GET_ENTITY_COORDS(peddy, true);

	float Dx = VHickPos.x - HeliDesX;
	float Dy = VHickPos.y - HeliDesY;
	float HeliDirect = GAMEPLAY::GET_HEADING_FROM_VECTOR_2D(Dx, Dy) - 180.00f;

	AI::TASK_HELI_MISSION(peddy, vic, 0, 0, HeliDesX, HeliDesY, HeliDesZ, 9, HeliSpeed, HeliLandArea, HeliDirect, -1, -1, -1, 0);
	PED::SET_PED_FIRING_PATTERN(peddy, MyHashKey("FIRING_PATTERN_BURST_FIRE_HELI"));
	PED::SET_PED_KEEP_TASK(peddy, true);
	PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy, true);
}
void ChaseHeli(Ped peddy, Vehicle vic, Ped victim)
{
	Vector3 vHeliDest = EntityPosition(victim);
	ENTITY::FREEZE_ENTITY_POSITION(vic, false);
	AI::CLEAR_PED_TASKS(peddy);

	float HeliDesX = vHeliDest.x;
	float HeliDesY = vHeliDest.y;
	float HeliDesZ = vHeliDest.z + 25.0f;
	float HeliSpeed = 5.0f;

	if (DistanceTo(peddy, victim) > 65.0f)
		HeliSpeed = 45.0f;

	float HeliLandArea = 45.00f;

	Vector3 VHickPos = ENTITY::GET_ENTITY_COORDS(vic, true);

	float dx = VHickPos.x - HeliDesX;
	float dy = VHickPos.y - HeliDesY;
	float HeliDirect = GAMEPLAY::GET_HEADING_FROM_VECTOR_2D(dx, dy) - 180.00f;

	AI::TASK_HELI_CHASE(peddy, victim, 0.0f, 0.0f, 35.0f);

	PED::SET_PED_FIRING_PATTERN(peddy, MyHashKey("FIRING_PATTERN_BURST_FIRE_HELI"));
	PED::SET_PED_KEEP_TASK(peddy, true);
	PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy, true);
}
void FlyPlane(Ped peddy, Vehicle vic, Vector3 pos, Ped target, bool fightPed, int vehType)
{
	float Angle = GetAngle(ENTITY::GET_ENTITY_COORDS(vic, true), pos);
	AI::CLEAR_PED_TASKS(peddy);

	if (target != NULL)
	{
		if (fightPed)
		{
			if (vehType == 3)
				AI::TASK_PLANE_MISSION(peddy, vic, 0, 0, pos.x, pos.y, pos.z, 4, 20.0f, 50.0f, Angle, pos.z + 10.0f, pos.z - 10.0f);
			else
				AI::TASK_PLANE_MISSION(peddy, vic, 0, target, 0, 0, 0, 6, 0.0f, 0.0f, Angle, 5000.0f, -10.0f);
			PED::SET_PED_FLEE_ATTRIBUTES(peddy, 0, true);
			PED::SET_PED_KEEP_TASK(peddy, true);
			PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy, true);
		}
		else
		{
			Vehicle Pv = GetPedsVehicle(target);
			Vector3 Pos = EntityPosition(target);
			if (Pv != NULL)
				AI::TASK_PLANE_CHASE(peddy, target, 0, -10.0f, 10.0f);
			else
				AI::TASK_PLANE_MISSION(peddy, vic, 0, 0, Pos.x, Pos.y, Pos.z + 255.0f, 4, 20.0f, 50.0f, Angle, 5000.0f, -100.0f);

			PED::SET_PED_FLEE_ATTRIBUTES(peddy, 0, true);
			PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy, false);
		}
	}
	else
	{
		AI::TASK_PLANE_MISSION(peddy, vic, 0, 0, pos.x, pos.y, pos.z, 4, 20.0f, 50.0f, Angle, pos.z + 100.0f, pos.z - 100.0f);

		PED::SET_PED_KEEP_TASK(peddy, true);
		PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy, true);
	}
}
void PickFight(Ped peddy, Vehicle vic, Ped target, int prefVeh)
{
	if (target != NULL)
	{
		Vector3 FighterPos = EntityPosition(target);

		if (prefVeh == 3 || prefVeh == 5)
			FlyPlane(peddy, vic, FighterPos, target, true, prefVeh);
		else if (prefVeh == 2 || prefVeh == 4)
			ChaseHeli(peddy, vic, target);
		else
			DriveBye(peddy, vic, target, true);
	}
}
bool PedDoGetIn(Vehicle vic, PlayerBrain* brain)
{
	bool b = false;
	if (brain->ThisPed != NULL)
	{
		int Seats = FindUSeat(vic, false);
		Vector3 PedPos = ENTITY::GET_ENTITY_COORDS(brain->ThisPed, true);
		Vector3 VehPos = ENTITY::GET_ENTITY_COORDS(vic, true);

		if (Seats > -1)
		{
			if (DistanceTo(PedPos, VehPos) < 65.00)
				GetInVehicle(brain->ThisPed, vic, Seats);
			else
				WarptoAnyVeh(vic, brain->ThisPed, Seats);
			b = true;
			brain->Passenger = true;
		}
	}
	return b;
}		// Add a ped summon own veh...
void GreefWar(Ped peddy, Ped victim)
{
	if (victim != NULL)
	{
		AI::CLEAR_PED_TASKS(peddy);
		PED::SET_PED_FLEE_ATTRIBUTES(peddy, 0, true);
		PED::SET_PED_COMBAT_ATTRIBUTES(peddy, 46, true);

		if (MySettings.Aggression <= 3)
		{
			// Low aggression: NPCs disengage rather than fight to the death
			PED::SET_PED_COMBAT_ATTRIBUTES(peddy, 52, true);  // flee if not in line of sight
			PED::SET_PED_COMBAT_ATTRIBUTES(peddy, 5, true);   // can flee
			PED::SET_PED_FLEE_ATTRIBUTES(peddy, 2, true);     // flee when injured
		}

		AI::TASK_COMBAT_PED(peddy, victim, 0, 16);
	}
}
void FightPlayer(PlayerBrain* brain)
{
	brain->BlipColour = 1;
	brain->Friendly = false;
	brain->WanBeFriends = false;
	brain->ApprochPlayer = false;

	UI::SET_BLIP_COLOUR(brain->ThisBlip, 1);
	PED::REMOVE_PED_FROM_GROUP(brain->ThisPed);

	if (MySettings.Aggression > 8)
	{
		RelGroupMember(brain->ThisPed, GP_Mental);
	}
	else
	{
		// Assign LSR-compatible gang group for the NPC's spawn zone when available.
		// This lets LSR recognise them as gang members with full crime tracking,
		// territory awareness, and correct police dispatch — without any LSR changes.
		Hash gangGroup = GetGangGroupForZone(brain->ThisPed);
		if (gangGroup != 0)
		{
			if (ActiveGangGroups.find(gangGroup) == ActiveGangGroups.end())
			{
				ActiveGangGroups.insert(gangGroup);
				ApplyGangRelationships(gangGroup);
			}
			RelGroupMember(brain->ThisPed, gangGroup);
		}
		else
		{
			RelGroupMember(brain->ThisPed, GP_Attack);
		}
	}
}

void FightTogether(Vehicle vic, Ped peddy)
{
	for (int i = 0; i < (int)PedList.size(); i++)
	{
		if (PedList[i].Passenger)
		{
			if (PedList[i].ThisVeh == vic)
				DriveBye(PedList[i].ThisPed, vic, peddy, false);
		}
	}
}
Ped FindAFight(PlayerBrain* brain)
{
	Ped FightMe = NULL;
	if (brain != nullptr)
	{
		float Dist = 4000.0f;
		int You = -1;
		bool Friend = brain->Friendly;
		if (brain->IsPlane)
		{
			for (int i = 0; i < (int)PedList.size(); i++)
			{
				if (MySettings.Aggression > 9 || brain->TheHacker)
				{
					if (PedList[i].ThisPed != brain->ThisPed && PedList[i].ThisPed != NULL && !PedList[i].YoDeeeed)
					{
						if (PedList[i].IsPlane || PedList[i].IsHeli)
							You = i;
					}
				}
				else
				{
					if (PedList[i].ThisPed != brain->ThisPed && PedList[i].Friendly != Friend && PedList[i].ThisPed != NULL && !PedList[i].YoDeeeed && PedList[i].IsPlane)
					{
						if (PedList[i].IsPlane || PedList[i].IsHeli)
							You = i;
					}
				}
			}

			if (You == -1)
			{
				for (int i = 0; i < (int)PedList.size(); i++)
				{
					if (MySettings.Aggression > 9 || brain->TheHacker)
					{
						if (PedList[i].ThisPed != brain->ThisPed && PedList[i].ThisPed != NULL && !PedList[i].YoDeeeed)
						{
							if (DistanceTo(PedList[i].ThisPed, brain->ThisPed) < Dist)
							{
								Dist = DistanceTo(PedList[i].ThisPed, brain->ThisPed);
								You = i;
							}
						}
					}
					else
					{
						if (PedList[i].ThisPed != brain->ThisPed && PedList[i].Friendly != Friend && PedList[i].ThisPed != NULL && !PedList[i].YoDeeeed)
						{
							if (DistanceTo(PedList[i].ThisPed, brain->ThisPed) < Dist)
							{
								Dist = DistanceTo(PedList[i].ThisPed, brain->ThisPed);
								You = i;
							}
						}
					}
				}
			}
		}
		else if (brain->IsHeli)
		{
			for (int i = 0; i < (int)PedList.size(); i++)
			{
				if (MySettings.Aggression > 9 || brain->TheHacker)
				{
					if (PedList[i].ThisPed != brain->ThisPed && PedList[i].ThisPed != NULL && !PedList[i].YoDeeeed && PedList[i].IsHeli)
						You = i;
				}
				else
				{
					if (PedList[i].ThisPed != brain->ThisPed && PedList[i].Friendly != Friend && PedList[i].ThisPed != NULL && !PedList[i].YoDeeeed && PedList[i].IsHeli)
						You = i;
				}
			}

			if (You == -1)
			{
				for (int i = 0; i < (int)PedList.size(); i++)
				{
					if (MySettings.Aggression > 9)
					{
						if (PedList[i].ThisPed != brain->ThisPed && PedList[i].ThisPed != NULL && !PedList[i].YoDeeeed && !PedList[i].IsPlane)
						{
							if (DistanceTo(PedList[i].ThisPed, brain->ThisPed) < Dist)
							{
								Dist = DistanceTo(PedList[i].ThisPed, brain->ThisPed);
								You = i;
							}
						}
					}
					else
					{
						if (PedList[i].ThisPed != brain->ThisPed && PedList[i].Friendly != Friend && PedList[i].ThisPed != NULL && !PedList[i].YoDeeeed && !PedList[i].IsPlane)
						{
							if (DistanceTo(PedList[i].ThisPed, brain->ThisPed) < Dist)
							{
								Dist = DistanceTo(PedList[i].ThisPed, brain->ThisPed);
								You = i;
							}
						}
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < (int)PedList.size(); i++)
			{
				if (MySettings.Aggression > 9 || brain->TheHacker)
				{
					if (PedList[i].ThisPed != brain->ThisPed && PedList[i].ThisPed != NULL && !PedList[i].YoDeeeed && !PedList[i].IsPlane && !PedList[i].IsHeli)
					{
						if (DistanceTo(PedList[i].ThisPed, brain->ThisPed) < Dist)
						{
							Dist = DistanceTo(PedList[i].ThisPed, brain->ThisPed);
							You = i;
						}
					}
				}
				else
				{
					if (PedList[i].ThisPed != brain->ThisPed && PedList[i].Friendly != Friend && PedList[i].ThisPed != NULL && !PedList[i].YoDeeeed && !PedList[i].IsPlane && !PedList[i].IsHeli)
					{
						if (DistanceTo(PedList[i].ThisPed, brain->ThisPed) < Dist)
						{
							Dist = DistanceTo(PedList[i].ThisPed, brain->ThisPed);
							You = i;
						}
					}
				}
			}
		}

		if (!Friend && MySettings.Aggression > 5 || brain->TheHacker)
		{
			if (You != -1)
			{
				if (DistanceTo(PLAYER::PLAYER_PED_ID(), brain->ThisPed) < Dist)
					You = -2;
			}
			else
				You = -2;
		}

		if (You == -2)
			FightMe = PLAYER::PLAYER_PED_ID();
		else if (You > -1 && You < (int)PedList.size())
			FightMe = PedList[You].ThisPed;
	}
	return FightMe;
}
bool EnterFriendsVeh(PlayerBrain* brain)
{
	bool Found = false;
	int i = 0;
	for (; i < (int)PedList.size(); i++)
	{ 
		if (PedList[i].Driver && PedList[i].Follower && !PedList[i].IsAnimal)
		{
			if (PedList[i].ThisVeh != NULL)
			{
				int iSeats = FindUSeat(PedList[i].ThisVeh, false);
				if (iSeats != -10)
				{
					if (brain != nullptr)
					{
						PedDoGetIn(PedList[i].ThisVeh, brain);
						brain->Passenger = true;
						Found = true;
					}
					break;
				}
			}
		}
	}

	return Found;
}
PlayerBrain* FindaPassenger(bool friends)
{
	int i = 0;
	for (; i < (int)PedList.size(); i++)
	{
		if (friends)
		{
			if (!PedList[i].Passenger && !PedList[i].Driver && PedList[i].Friendly && !PedList[i].Follower && !PedList[i].IsAnimal)
				break;
		}
		else
		{
			if (!PedList[i].Passenger && !PedList[i].Driver && !PedList[i].Friendly && !PedList[i].IsAnimal)
				break;
		}
	}

	if (i < (int)PedList.size())
		return &PedList[i];
	else
		return nullptr;
}

int iPasiveMode = -1;
Ped YourKilller(Ped peddy)
{
	Entity ThisEnt = PED::_GET_PED_KILLER(peddy);
	if (ThisEnt != NULL)
	{
		if ((bool)ENTITY::IS_ENTITY_A_PED(ThisEnt))
		{

		}
		else if ((bool)ENTITY::IS_ENTITY_A_VEHICLE(ThisEnt))
			ThisEnt = VEHICLE::GET_PED_IN_VEHICLE_SEAT(ThisEnt, -1);
		else
			ThisEnt = NULL;
	}
	return ThisEnt;
}
int WhoShotMe(Ped peddy)
{
	int iShoot = -1;
	Entity Killer = YourKilller(peddy);

	if (Killer == PLAYER::PLAYER_PED_ID())
		iShoot = -10;
	else
	{
		for (int i = 0; i < (int)PedList.size(); i++)
		{
			if (Killer != NULL)
			{
				if (Killer == PedList[i].ThisPed)
				{
					iShoot = i;
					break;
				}
			}
		}
	}

	return iShoot;
}
int BackOffPig()
{
	LoggerLight("BackOffPig");

	int Output = -1;
	for (int i = 0; i < (int)PedList.size(); i++)
	{
		if (PedList[i].PiggyBackin)
		{
			Output = i;
			break;
		}
	}
	return Output;
}
int iFolPos;
void OhDoKeepUp(Ped peddy)
{
	LoggerLight("OhDoKeepUp");
	AI::CLEAR_PED_TASKS(peddy);

	float Xpos = -2.50;
	float Ypos = 1.50;

	if (iFolPos == 1)
	{
		Xpos = -2.50;
		Ypos = 0.00;
	}
	else if (iFolPos == 2)
	{
		Xpos = -2.50;
		Ypos = -2.50;
	}
	else if (iFolPos == 3)
	{
		Xpos = 2.50;
		Ypos = 0.00;
	}
	else if (iFolPos == 4)
	{
		Xpos = 1.50;
		Ypos = 0.00;
	}
	else if (iFolPos == 5)
	{
		Xpos = -1.50;
		Ypos = 0.00;
	}
	else if (iFolPos == 6)
	{
		Xpos = 2.50;
		Ypos = -2.50;
	}
	else if (iFolPos == 7)
	{
		Xpos = -1.50;
		Ypos = -2.50;
		iFolPos = 0;
	}

	iFolPos++;
	AI::TASK_FOLLOW_TO_OFFSET_OF_ENTITY(peddy, PLAYER::PLAYER_PED_ID(), Xpos, Ypos, 0.0f, 1.0f, -1, 0.0f, 1);
	PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy, 0);
}
void FolllowTheLeader(Ped peddy)
{
	LoggerLight("FolllowTheLeader");

	PED::REMOVE_PED_FROM_GROUP(peddy);

	RelGroupMember(peddy, Gp_Follow);
	PED::SET_PED_AS_GROUP_MEMBER(peddy, FollowMe);//was as FollowMe me now as Gp_

	if ((bool)PED::IS_PED_GROUP_MEMBER(peddy, FollowMe))
		LoggerLight("IS_PED_GROUP_MEMBER == true");

	OhDoKeepUp(peddy);
}
int NearHiest()
{
	int Near = -1;
	Vector3 PlayPos = PlayerPosi();

	for (int i = 0; i < (int)HeistDrop.size(); i++)
	{
		if (DistanceTo(HeistDrop[i], PlayPos) < 55.0)
		{
			Near = i;
			break;
		}
	}
	return Near;
}

void OnlineDress(Ped peddy, ClothX* clothClass)
{
	LoggerLight("OnlineDress");

	PED::CLEAR_ALL_PED_PROPS(peddy);

	if (StringContains("Bodysuit", clothClass->Title) && clothClass->ClothA.size() > 5)
		clothClass->ClothA[1] = -10;

	for (int i = 0; i < clothClass->ClothA.size(); i++)
	{
		if (clothClass->ClothA[i] != -10)
			PED::SET_PED_COMPONENT_VARIATION(peddy, i, clothClass->ClothA[i], clothClass->ClothB[i], 2);

	}

	for (int i = 0; i < clothClass->ExtraA.size(); i++)
	{
		if (clothClass->ExtraA[i] != -10)
			PED::SET_PED_PROP_INDEX(peddy, i, clothClass->ExtraA[i], clothClass->ExtraB[i], false);
	}
}
void OnlineFaces(Ped peddy, ClothBank* clothBankClass)
{
	LoggerLight("OnlineFaces");
	//****************FaceShape/Colour****************
	PED::SET_PED_HEAD_BLEND_DATA(peddy, clothBankClass->MyFaces.ShapeFirstID, clothBankClass->MyFaces.ShapeSecondID, clothBankClass->MyFaces.ShapeThirdID, clothBankClass->MyFaces.SkinFirstID, clothBankClass->MyFaces.SkinSecondID, clothBankClass->MyFaces.SkinThirdID, clothBankClass->MyFaces.ShapeMix, clothBankClass->MyFaces.SkinMix, clothBankClass->MyFaces.ThirdMix, 0);

	if (ItsChristmas)
	{
		if (clothBankClass->Male)
		{
			ClothX Outfit = XmasOut_M[LessRandomInt("Crimb01", 0, (int)XmasOut_M.size() - 1)];
			OnlineDress(peddy, &Outfit);
		}
		else
		{
			ClothX Outfit = XmasOut_F[LessRandomInt("Crimb01", 0, (int)XmasOut_F.size() - 1)];
			OnlineDress(peddy, &Outfit);
		}
	}
	else
		OnlineDress(peddy, &clothBankClass->Cothing);

	PED::SET_PED_COMPONENT_VARIATION(peddy, 2, clothBankClass->MyHair.Comp, clothBankClass->MyHair.Text, 2);

	if (clothBankClass->MyHair.Over != -1)
		PED::_APPLY_PED_OVERLAY(peddy, clothBankClass->MyHair.OverLib, clothBankClass->MyHair.Over);
	PED::_SET_PED_HAIR_COLOR(peddy, clothBankClass->HairColour, clothBankClass->HairStreaks);

	//****************Face****************
	for (int i = 0; i < clothBankClass->MyOverlay.size(); i++)
	{
		int Colour = 0;

		if (i == 1)
			Colour = 1; //Facial Hair
		else if (i == 2)
			Colour = 1; //Eyebrows
		else if (i == 5)
			Colour = 2; //Blush
		else if (i == 8)
			Colour = 2; //Lipstick
		else if (i == 10)
			Colour = 1; //Chest Hair

		int Change = clothBankClass->MyOverlay[i].Overlay;
		int AddColour = clothBankClass->MyOverlay[i].OverCol;
		float Opc = clothBankClass->MyOverlay[i].OverOpc;

		PED::SET_PED_HEAD_OVERLAY(peddy, i, Change, Opc);

		if (Colour > 0)
			PED::_SET_PED_HEAD_OVERLAY_COLOR(peddy, i, Colour, AddColour, 0);
	}
	//****************Tattoos****************
	for (int i = 0; i < clothBankClass->MyTattoo.size(); i++)
		PED::_APPLY_PED_OVERLAY(peddy, MyHashKey(clothBankClass->MyTattoo[i].BaseName), MyHashKey(clothBankClass->MyTattoo[i].TatName));
}

void MaxOutAllModsNoWheels(Vehicle vic, int modItem)
{
	LoggerLight("MaxOutAllModsNoWheels");

	VEHICLE::SET_VEHICLE_MOD_KIT(vic, 0);
	for (int i = 0; i < 50; i++)
	{
		int Spoilher = VEHICLE::GET_NUM_VEHICLE_MODS(vic, i);

		if (i == 18 || i == 22 || i == 23 || i == 24)
		{

		}
		else
		{
			Spoilher -= 1;
			VEHICLE::SET_VEHICLE_MOD(vic, i, Spoilher, true);
		}
	}

	if (modItem != 13 && modItem != 14 && modItem != 15 && modItem != 16)
	{
		VEHICLE::TOGGLE_VEHICLE_MOD(vic, 18, true);
		VEHICLE::TOGGLE_VEHICLE_MOD(vic, 22, true);
	}
	else if (modItem == 15 || modItem == 16)
		VEHICLE::_SET_VEHICLE_LANDING_GEAR(vic, 3);
}
void MakeModsNotWar(Vehicle vic, std::vector<int>& modItems)
{
	LoggerLight("MakeModsNotWar");

	VEHICLE::SET_VEHICLE_MOD_KIT(vic, 0);

	for (int i = 0; i < modItems.size(); i++)
	{
		if (modItems[i] == -10)
		{

		}
		else if (i == 48)
		{
			int Spoilher = -1;
			if (modItems[i] == -1)
			{
				Spoilher = VEHICLE::GET_NUM_VEHICLE_MODS(vic, i) - 1;
				if (Spoilher < 1)
					Spoilher = VEHICLE::GET_VEHICLE_LIVERY_COUNT(vic) - 1;

				if (Spoilher > 0)
					modItems[i] = RandomInt(0, Spoilher);
			}

			if (Spoilher > -1 || modItems[i] > -1)
			{
				VEHICLE::SET_VEHICLE_LIVERY(vic, modItems[i]);
				VEHICLE::SET_VEHICLE_MOD(vic, i, modItems[i], true);
			}
		}
		else if (i == 66)
		{
			if (modItems[i] == -1)
				modItems[i] = RandomInt(0, 159);
		}
		else if (i == 67)
		{
			if (modItems[i] == -1)
				VEHICLE::SET_VEHICLE_COLOURS(vic, modItems[i - 1], RandomInt(0, 159));
			else
				VEHICLE::SET_VEHICLE_COLOURS(vic, modItems[i - 1], modItems[i]);
		}
		else if (modItems[i] == -1)
		{
			int Spoilher = VEHICLE::GET_NUM_VEHICLE_MODS(vic, i) - 1;
			if (Spoilher > 0)
			{
				int CheckHere = RandomInt(0, Spoilher);
				VEHICLE::SET_VEHICLE_MOD(vic, i, CheckHere, true);
			}
		}
		else
			VEHICLE::SET_VEHICLE_MOD(vic, i, modItems[i], true);
	}
}
Vehicle OppresiveDump(Vector4 pos)
{
	Vehicle BuildVehicle = NULL;
	int iVehHash = MyHashKey("oppressor2");

	if ((bool)STREAMING::IS_MODEL_IN_CDIMAGE(iVehHash) && (bool)STREAMING::IS_MODEL_A_VEHICLE(iVehHash))
	{
		STREAMING::REQUEST_MODEL(iVehHash);
		while (!(bool)STREAMING::HAS_MODEL_LOADED(iVehHash))
			WAIT(1);

		BuildVehicle = VEHICLE::CREATE_VEHICLE(iVehHash, pos.X, pos.Y, pos.Z, pos.R, true, true);
		MaxOutAllModsNoWheels(BuildVehicle, 15);

		ENTITY::SET_ENTITY_AS_MISSION_ENTITY(BuildVehicle, 1, 1);
	}

	return BuildVehicle;
}

inline const std::vector<std::string> AnimalFarm = {
	"a_c_boar",//"a_c_boar",		0
	"a_c_husky",//					2
	"a_c_poodle",//					3
	"a_c_pug",//					4
	"a_c_retriever",//				5
	"a_c_rottweiler",//				6
	"a_c_shepherd",//				7
	"a_c_westy",//					8
	"a_c_rat",//"a_c_rat" />		10
	"a_c_cow",//"a_c_cow" />		11
	"a_c_coyote",//"a_c_coyote" />	12
	"a_c_crow",//"a_c_crow" />		13
	"a_c_rabbit_01",//"a_c_rabbit_01" />14
	"a_c_deer",//"a_c_deer" />			15
	"a_c_hen",//"a_c_hen" />		16
	"a_c_mtlion",//"mountain lion" />17
	"a_c_pig",//"a_c_pig" />		18
};
Ped PlayerPedGen(Vector4 pos, PlayerBrain* brain, bool partyPed)
{
	LoggerLight("PlayerPedGen = " + brain->MyName);

	Ped ThisPed = NULL;

	WAIT(100);

	Hash MyModel = MyHashKey(brain->PFMySetting.Model);

	if (HackAttacks && !brain->TheHacker && !brain->Driver && !brain->Passenger && LessRandomInt("AnimalChance", 1, 10) < 5)
	{
		brain->IsAnimal = true;
		MyModel = MyHashKey(AnimalFarm[RandomInt(0, (int)AnimalFarm.size() - 1)]);
	}
	else if (brain->TheHacker)
		brain->IsAnimal = true;

	STREAMING::REQUEST_MODEL(MyModel);// Check if the model is valid

	if ((bool)STREAMING::IS_MODEL_IN_CDIMAGE(MyModel) && (bool)STREAMING::IS_MODEL_VALID(MyModel))
	{
		while (!STREAMING::HAS_MODEL_LOADED(MyModel))
			WAIT(10);

		ThisPed = PED::CREATE_PED(4, MyModel, pos.X, pos.Y, pos.Z, pos.R, true, false);
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(MyModel);

		if ((bool)ENTITY::DOES_ENTITY_EXIST(ThisPed))
		{
			ENTITY::SET_ENTITY_AS_MISSION_ENTITY(ThisPed, 1, 1);

			if (MySettings.Aggression > 5)
				PED::SET_PED_ACCURACY(ThisPed, MySettings.AccMax);
			else if (MySettings.Aggression <= 3)
				PED::SET_PED_ACCURACY(ThisPed, max(5, MySettings.AccMin - 10));
			else
				PED::SET_PED_ACCURACY(ThisPed, MySettings.AccMin);

			int iHeal = RandomInt(200, 300);
			PED::SET_PED_MAX_HEALTH(ThisPed, iHeal);
			ENTITY::SET_ENTITY_HEALTH(ThisPed, iHeal);
			brain->HeadTag = UI::_CREATE_HEAD_DISPLAY(ThisPed, (LPSTR)brain->MyName.c_str(), false, false, "", 1);//
			UI::_SET_HEAD_DISPLAY_FLAG(brain->HeadTag, 0, true);
			//UI::_0xEE76FF7E6A0166B0(brain->HeadTag, true);
			//UI::_0xA67F9C46D612B6F1(brain->HeadTag, true);

			if (MySettings.Aggression > 5)
				PED::SET_PED_COMBAT_ABILITY(ThisPed, 150);
			else if (MySettings.Aggression > 3)
				PED::SET_PED_COMBAT_ABILITY(ThisPed, 100);
			else if (MySettings.Aggression > 1)
				PED::SET_PED_COMBAT_ABILITY(ThisPed, 50);
			else
				PED::SET_PED_COMBAT_ABILITY(ThisPed, 10);

			if (!brain->Driver)
			{
				PED::SET_PED_COMBAT_MOVEMENT(ThisPed, 2);
				AI::SET_PED_PATH_CAN_USE_CLIMBOVERS(ThisPed, true);
				AI::SET_PED_PATH_CAN_USE_LADDERS(ThisPed, true);
				AI::SET_PED_PATH_CAN_DROP_FROM_HEIGHT(ThisPed, true);
				AI::SET_PED_PATH_PREFER_TO_AVOID_WATER(ThisPed, false);
				PED::SET_PED_COMBAT_ATTRIBUTES(ThisPed, 0, true);
				PED::SET_PED_COMBAT_ATTRIBUTES(ThisPed, 1, true);
				PED::SET_PED_COMBAT_ATTRIBUTES(ThisPed, 3, true);
				PED::SET_PED_COMBAT_ATTRIBUTES(ThisPed, 5, true);

				if (MySettings.Aggression > 1)
				{
					PED::SET_PED_COMBAT_ATTRIBUTES(ThisPed, 46, true);
					PED::SET_PED_COMBAT_ATTRIBUTES(ThisPed, 2, true);
					if (MySettings.Aggression > 2)
						PED::SET_PED_CAN_SWITCH_WEAPON(ThisPed, true);
				}
			}

			brain->ThisPed = ThisPed;

			if (partyPed)
				brain->AtTheParty = true;

			if (!brain->IsAnimal)
			{
				OnlineFaces(ThisPed, &brain->PFMySetting);
				// Friendly / civilian peds are unarmed civilians — never give them a weapon loadout.
				// Hostile / criminal peds get a loadout via GunningIt (weapons holstered until combat).
				if (!brain->Friendly && !brain->Follower)
					GunningIt(ThisPed, brain->GunSelect);
			}

			if (brain->MyIdentity == "")
			{
				int PZSetMinSession = (MySettings.MinSession * 60) * 1000;
				int PZSetMaxSession = (MySettings.MaxSession * 60) * 1000;

				brain->TimeOn = InGameTime() + RandomInt(PZSetMinSession, PZSetMaxSession);
				brain->Level = UniqueLevels();
				brain->MyIdentity = brain->MyName + std::to_string(brain->Level);
			}

			if (!brain->AtTheParty)
			{
				if (brain->Follower)
				{
					brain->Friendly = true;
					if (!brain->Driver && !brain->Passenger)
						FolllowTheLeader(ThisPed);
					else
						RelGroupMember(ThisPed, Gp_Follow);
				}
				else if (brain->Friendly)
				{
					RelGroupMember(ThisPed, Gp_Friend);
					//if (!brain->Driver || !brain->Passenger)
					//	WalkHere(ThisPed, FindingShops(ThisPed));
				}
				else if (brain->TheHacker)
					RelGroupMember(brain->ThisPed, GP_Mental);
				else
					FightPlayer(brain);

				// Force initial movement for on-foot NPCs to prevent AFK spawning.
				// Push FindPlayer into the future so the AI tick does not
				// immediately override the just-assigned task (FindPlayer defaults to 0).
				if (!brain->Driver && !brain->Passenger && !brain->Follower)
				{
					// Determine crime / dealer rates and zone aggression tier.
					// With LSR loaded: use gang profile + economy.
					// Without LSR: use tier derived purely from hardcoded zone table.
					int crimChance   = 20;
					int dealerChance = 25;
					int aggrTier     = 2; // default medium

					if (LSRData::IsAvailable)
					{
						Vector3 spawnPos = ENTITY::GET_ENTITY_COORDS(brain->ThisPed, true);
						std::string zoneName = std::string(ZONE::GET_NAME_OF_ZONE(
							spawnPos.x, spawnPos.y, spawnPos.z));

						brain->GangID      = LSRData::GetGangForZone(zoneName);
						brain->ZoneEconomy = LSRData::GetEconomyCode(
							LSRData::GetEconomyForZone(zoneName));

						aggrTier = GetZoneAggressionTier(zoneName);

						// -------------------------------------------------------
						// NEIGHBORHOOD BALANCE: re-roll Friendly based on zone economy.
						//   Wealthy  (3) = upscale areas — 85 % friendly civilians
						//   Middle   (2) = mixed areas   — keep Aggression-based roll
						//   Poor     (1) = high-crime    — 25 % friendly, rest hostile
						// This overrides the global Aggression-based roll so that
						// Vinewood feels calm and Davis/Strawberry feel dangerous.
						// -------------------------------------------------------
						bool wasHostile = !brain->Friendly;
						if (brain->ZoneEconomy == 3)
							brain->Friendly = (RandomInt(1, 100) <= 85);
						else if (brain->ZoneEconomy == 1)
							brain->Friendly = (RandomInt(1, 100) <= 25);
						// ZoneEconomy == 2: leave Friendly unchanged (Aggression roll stands)

						// If the economy re-roll changed the alignment, fix relationship group.
						if (wasHostile && brain->Friendly)
						{
							// Was hostile → now civilian: strip any weapon loadout given by GunningIt.
							WEAPON::REMOVE_ALL_PED_WEAPONS(ThisPed, true);
							brain->ArmedWeaponHash = 0;
							brain->IsCriminal      = false;
							brain->IsDealer        = false;
							brain->BlipColour = 0;
							UI::SET_BLIP_COLOUR(brain->ThisBlip, 0);
							RelGroupMember(ThisPed, Gp_Friend);
						}
						else if (!wasHostile && !brain->Friendly)
						{
							// Was friendly → now hostile: give them a weapon loadout (holstered).
							GunningIt(ThisPed, brain->GunSelect);
							brain->BlipColour = 1;
							UI::SET_BLIP_COLOUR(brain->ThisBlip, 1);
							FightPlayer(brain);
						}

						const LSRGangProfile* gp = nullptr;
						if (!brain->GangID.empty())
						{
							gp = LSRData::GetGangProfile(brain->GangID);
							if (gp)
							{
								crimChance   = min(gp->fightPercentage, 75);
								dealerChance = gp->drugDealerPercentage;
							}
						}
						else
						{
							// Non-gang zone: scale by economy.
							if      (brain->ZoneEconomy == 1) { crimChance = 35; dealerChance = 40; }
							else if (brain->ZoneEconomy == 3) { crimChance =  8; dealerChance = 10; }
						}

						// Set schedule phase from current in-game hour.
						int h = TIME::GET_CLOCK_HOURS();
						if      (h >= 6  && h < 12) brain->SchedulePhase = 0;
						else if (h >= 12 && h < 18) brain->SchedulePhase = 1;
						else if (h >= 18 && h < 22) brain->SchedulePhase = 2;
						else                         brain->SchedulePhase = 3;

						// Friendly / civilian peds never commit crimes and are never armed.
						// Hostile peds roll for criminal status and get a holstered weapon.
						if (!brain->Friendly)
						{
							if (RandomInt(1, 100) <= crimChance)
							{
								brain->IsCriminal = true;
								if (RandomInt(1, 100) <= dealerChance)
									brain->IsDealer = true;

								// Arm the criminal — weapon given holstered (no draw, no wanted level).
								ArmCriminalPed(brain, gp);
							}
						}
						else
						{
							// Civilian: clear any crime flags, configure flee-first behaviour.
							brain->IsCriminal = false;
							brain->IsDealer   = false;
							PED::SET_PED_FLEE_ATTRIBUTES(ThisPed, 1, true); // flee from nearby weapons
							PED::SET_PED_FLEE_ATTRIBUTES(ThisPed, 2, true); // flee when injured
							PED::SET_PED_COMBAT_ATTRIBUTES(ThisPed, 5,  true); // can flee
							PED::SET_PED_COMBAT_ATTRIBUTES(ThisPed, 17, true); // flee from fire
							PED::SET_PED_COMBAT_ATTRIBUTES(ThisPed, 52, true); // disengage if out of LoS
						}
					}
					else
					{
						// Standalone mode: use zone table for aggression; flat rates for crime.
						Vector3 spawnPos = ENTITY::GET_ENTITY_COORDS(brain->ThisPed, true);
						std::string zoneName = std::string(ZONE::GET_NAME_OF_ZONE(
							spawnPos.x, spawnPos.y, spawnPos.z));
						aggrTier = GetZoneAggressionTier(zoneName);

						// In standalone mode use aggression tier to bias the friendly roll:
						// High-crime zone (tier 3) = at most 35 % friendly.
						// Low-crime zone  (tier 1) = at least 75 % friendly.
						if      (aggrTier == 1) brain->Friendly = (RandomInt(1, 100) <= 75);
						else if (aggrTier == 3) brain->Friendly = (RandomInt(1, 100) <= 35);

						if (!brain->Friendly)
						{
							if (RandomInt(1, 100) <= crimChance)
							{
								brain->IsCriminal = true;
								if (RandomInt(1, 100) <= dealerChance)
									brain->IsDealer = true;
								ArmCriminalPed(brain, nullptr);
							}
						}
						else
						{
							brain->IsCriminal = false;
							brain->IsDealer   = false;
							PED::SET_PED_FLEE_ATTRIBUTES(ThisPed, 1, true);
							PED::SET_PED_FLEE_ATTRIBUTES(ThisPed, 2, true);
							PED::SET_PED_COMBAT_ATTRIBUTES(ThisPed, 5,  true);
							PED::SET_PED_COMBAT_ATTRIBUTES(ThisPed, 17, true);
							PED::SET_PED_COMBAT_ATTRIBUTES(ThisPed, 52, true);
						}
					}

					brain->AggressionTier = aggrTier;

					// Crime cooldown timer scaled by zone aggression:
					// Tier 3 (high-crime) = 30-60s  — things happen often in Davis/Strawberry
					// Tier 2 (medium)     = 60-120s
					// Tier 1 (low-crime)  = 120-240s — rare events in wealthy areas
					int ctMin, ctMax;
					if      (aggrTier == 3) { ctMin = 30000;  ctMax = 60000;  }
					else if (aggrTier == 2) { ctMin = 60000;  ctMax = 120000; }
					else                    { ctMin = 120000; ctMax = 240000; }
					brain->CrimeTimer = InGameTime() + RandomInt(ctMin, ctMax);

					PickNextAction(brain);
					brain->FindPlayer = InGameTime() + RandomInt(45000, 90000);
				}

				if (brain->Oppressor != NULL)
				{
					PED::SET_PED_ACCURACY(ThisPed, 100);
					brain->DirBlip = false;
					PedBlimp(&brain->ThisBlip, ThisPed, 639, brain->MyName, brain->BlipColour, false);
				}
				else if (brain->Driver)
				{
					float fAggi = (float)MySettings.Aggression / 100;
					PED::SET_DRIVER_ABILITY(ThisPed, 1.00f);
					PED::SET_DRIVER_AGGRESSIVENESS(ThisPed, fAggi);
					PED::SET_PED_STEERS_AROUND_VEHICLES(ThisPed, true);
					brain->DirBlip = true;
				}
				else if (brain->Passenger)
					brain->DirBlip = true;
				else
					brain->DirBlip = false;
			}

			if (brain->TheHacker)
			{
				ENTITY::SET_ENTITY_INVINCIBLE(brain->ThisPed, true);
				PED::SET_PED_COMPONENT_VARIATION(brain->ThisPed, 0, 0, 1, 2);

			}
			else if (MySettings.PassiveMode)
				ENTITY::SET_ENTITY_ALPHA(brain->ThisPed, 120, false);
		}
		else
			ThisPed = NULL;
	}
	else
		ThisPed = NULL;

	return ThisPed;
}

std::string RandVeh(int vicList)
{
	LoggerLight("RandVeh, vicList == " + std::to_string(vicList));

	std::string sVeh = "";

	if (vicList == 1)
		sVeh = PreVeh_01[LessRandomInt("RandVeh01", 0, (int)PreVeh_01.size() - 1)];		// Carz
	else if (vicList == 2)
		sVeh = PreVeh_02[LessRandomInt("RandVeh02", 0, (int)PreVeh_02.size() - 1)];		// CiviHeli
	else if (vicList == 3)
		sVeh = PreVeh_03[LessRandomInt("RandVeh03", 0, (int)PreVeh_03.size() - 1)];		// CivPlane
	else if (vicList == 4)
		sVeh = PreVeh_04[LessRandomInt("RandVeh04", 0, (int)PreVeh_04.size() - 1)];		// MilHeli
	else if (vicList == 5)
		sVeh = PreVeh_05[LessRandomInt("RandVeh05", 0, (int)PreVeh_05.size() - 1)];		// MilPlane
	else if (vicList == 6)
		sVeh = PreVeh_06[LessRandomInt("RandVeh06", 0, (int)PreVeh_06.size() - 1)];		// MilCar
	else if (vicList == 7)
		sVeh = PreVeh_07[LessRandomInt("RandVeh07", 0, (int)PreVeh_07.size() - 1)];
	else if (vicList == 8 || vicList == 999)
		sVeh = "HYDRA";
	else if (vicList == 9)
		sVeh = "oppressor2";
	else
		sVeh = "ZENTORNO";

	return sVeh;
}
bool IsItARealVehicle(Hash vehHash)
{
	LoggerLight("IsItARealVehicle");

	bool bIsReal = false;
	if ((bool)STREAMING::IS_MODEL_A_VEHICLE(vehHash))
		bIsReal = true;

	return bIsReal;
}

Vehicle SpawnVehicleProp(Vector3 pos)
{
	Vehicle BuildVehicle = NULL;

	LoggerLight("SpawnVehicle");

	Hash VehModel = MyHashKey(RandVeh(7));

	if (!IsItARealVehicle(VehModel))
		VehModel = MyHashKey("MAMBA");

	if ((bool)STREAMING::IS_MODEL_IN_CDIMAGE(VehModel) && (bool)STREAMING::IS_MODEL_A_VEHICLE(VehModel))
	{
		STREAMING::REQUEST_MODEL(VehModel);
		while (!(bool)STREAMING::HAS_MODEL_LOADED(VehModel))
			WAIT(1);

		BuildVehicle = VEHICLE::CREATE_VEHICLE(VehModel, pos.x, pos.y, pos.z, 0.0, true, true);

		ENTITY::APPLY_FORCE_TO_ENTITY(BuildVehicle, 1, 0, 0, 1, 0, 0, 1, 1, false, true, true, false, true);

		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(VehModel);
	}
	return BuildVehicle;
}
Vehicle SpawnVehicle(PlayerBrain* brain, bool newPlayer, bool canFill)
{
	LoggerLight("SpawnVehicle");

	Vehicle BuildVehicle = NULL;
	if (brain != nullptr)
	{
		Hash VehModel;
		WAIT(100);
		if (brain->PrefredVehicle < 7)
		{
			if (brain->FaveVehicle == "")
				VehModel = MyHashKey(RandVeh(brain->PrefredVehicle));
			else
				VehModel = MyHashKey(brain->FaveVehicle);
		}
		else
			VehModel = MyHashKey(RandVeh(brain->PrefredVehicle));

		if (!IsItARealVehicle(VehModel))
			VehModel = MyHashKey("MAMBA");

		Vector4 VecLocal = FindVehSpPoint(brain->PrefredVehicle);

		if ((bool)STREAMING::IS_MODEL_IN_CDIMAGE(VehModel) && (bool)STREAMING::IS_MODEL_A_VEHICLE(VehModel))
		{
			STREAMING::REQUEST_MODEL(VehModel);
			while (!(bool)STREAMING::HAS_MODEL_LOADED(VehModel))
				WAIT(1);

			BuildVehicle = VEHICLE::CREATE_VEHICLE(VehModel, VecLocal.X, VecLocal.Y, VecLocal.Z, VecLocal.R, true, true);

			ENTITY::FREEZE_ENTITY_POSITION(BuildVehicle, true);
			ENTITY::SET_ENTITY_AS_MISSION_ENTITY(BuildVehicle, 1, 1);

			int iClass = VEHICLE::GET_VEHICLE_CLASS(BuildVehicle);
			if (iClass == 15 || iClass == 16)
				MaxOutAllModsNoWheels(BuildVehicle, iClass);
			else
				MakeModsNotWar(BuildVehicle, RandVehModsist());

			brain->ThisVeh = BuildVehicle;
			brain->Driver = true;
			brain->ApprochPlayer = false; // drivers never stop to approach the player
			brain->FindPlayer = 0;

			if (brain->PrefredVehicle == 8)
			{
				brain->IsPlane = true;
				Vector3 MyPos = ENTITY::GET_ENTITY_COORDS(BuildVehicle, true);
				MyPos.z = 550.00;
				brain->Oppressor = OppresiveDump(Vector4(MyPos.x, MyPos.y, MyPos.z, 0.0));
				ENTITY::ATTACH_ENTITY_TO_ENTITY(brain->Oppressor, BuildVehicle, PED::GET_PED_BONE_INDEX(BuildVehicle, 0), 0.00, 3.32999945, -0.10, 0.00, 0.00, 0.00, false, false, false, false, 2, true);
				ENTITY::SET_ENTITY_ALPHA(BuildVehicle, 0, 0);
				MoveEntity(BuildVehicle, MyPos);
			}
			else if (brain->PrefredVehicle == 2 || brain->PrefredVehicle == 4)
				brain->IsHeli = true;
			else if (brain->PrefredVehicle == 5 || brain->PrefredVehicle == 3)
			{
				brain->IsPlane = true;
				if (!PrivateJet && HasASeat(BuildVehicle) && brain->Friendly)
				{
					brain->PlaneLand = 10;
					brain->AirTranspport = true;
					brain->IsSpecialPed = true;
					brain->ApprochPlayer = false;
					PrivateJet = true;
					canFill = false;
				}
			}

			if (newPlayer)
			{
				brain->ThisPed = PlayerPedGen(VecLocal, brain, false);
				WarptoAnyVeh(BuildVehicle, brain->ThisPed, -1);
				GunningIt(brain->ThisPed, 6);
				PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(brain->ThisPed, 1);
				int Seating = VEHICLE::GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(BuildVehicle);
				if (canFill && PlayerZinSesh() + Seating < MySettings.MaxPlayers)
				{
					for (int i = 0; i < Seating; i++)
					{
						LoggerLight("Fill Vehicle with peds");
						int Level = UniqueLevels();

						ClothBank MB = NewClothBank();
						int GetGunnin = YourGunNum();

						int PZSetMinSession = (MySettings.MinSession * 60) * 1000;
						int PZSetMaxSession = (MySettings.MaxSession * 60) * 1000;

						PlayerBrain NewBrain = PlayerBrain(MB.Name, MB.Name + std::to_string(Level), MB, InGameTime() + RandomInt(PZSetMinSession, PZSetMaxSession), Level, false, false, brain->Nationality, GetGunnin, 0, "", -1);
						NewBrain.Driver = false;
						NewBrain.Passenger = true;
						NewBrain.ThisVeh = BuildVehicle;
						NewBrain.Friendly = brain->Friendly;
						PedList.push_back(NewBrain);
						Ped CarPed = PlayerPedGen(VecLocal, &PedList[PedList.size() - 1], false);
						WarptoAnyVeh(BuildVehicle, CarPed, i);
						GunningIt(CarPed, 6);
					}
				}
			}
			else
				WarptoAnyVeh(BuildVehicle, brain->ThisPed, -1);

			if (MySettings.PassiveMode)
				ENTITY::SET_ENTITY_ALPHA(BuildVehicle, 120, false);

			VEHICLE::SET_VEHICLE_ENGINE_ON(BuildVehicle, 1, 1, 0);
			ENTITY::FREEZE_ENTITY_POSITION(BuildVehicle, false);

			if (brain->IsPlane)
			{
				FlyPlane(brain->ThisPed, BuildVehicle, PlayerPosi(500.0f), NULL, false, brain->PrefredVehicle);
				ENTITY::APPLY_FORCE_TO_ENTITY(BuildVehicle, 1, 0, 0, -1, 0, 0, 1, 1, false, false, true, false, true);
			}

			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(VehModel);
		}
		else
			BuildVehicle = NULL;
	}

	return BuildVehicle;
}

bool HeistPopUp = true;
void HeistDrips(int myArea)
{
	LoggerLight("HeistDrips, iMyArea == " + std::to_string(myArea));
	std::vector<Vector4> VectorList = {};

	if (myArea == 0)
	{
		VectorList.push_back(Vector4(-1105.577, -1692.11, 4.345489, 0.0));
		VectorList.push_back(Vector4(-1107.141, -1690.495, 4.353377, 0.0));     //1
		VectorList.push_back(Vector4(-1105.481, -1690.545, 4.325913, 0.0));     //2--Vesp Beach
		VectorList.push_back(Vector4(716.5444, -979.9716, 24.11811, 0.0));      //3+Darnel
	}
	else if (myArea == 1)
	{

		VectorList.push_back(Vector4(60.53763, 8.939384, 69.14648, 0.0));      //4
		VectorList.push_back(Vector4(58.98491, 8.12832, 69.18693, 0.0));        //5
		VectorList.push_back(Vector4(60.66815, 6.700741, 69.12641, 0.0));     //6
		VectorList.push_back(Vector4(61.93483, 7.88855, 69.09691, 0.0));       //7--Vinwood

	}
	else if (myArea == 2)
	{

		VectorList.push_back(Vector4(718.9022, -980.336, 24.12285, 0.0));      //8
		VectorList.push_back(Vector4(717.793, -982.8883, 24.13336, 0.0));     //9
		VectorList.push_back(Vector4(715.0596, -982.1675, 24.1188, 0.0));      //10
		VectorList.push_back(Vector4(716.5444, -979.9716, 24.11811, 0.0));    //11--Darnels LaMessa

	}
	else if (myArea == 3)
	{
		VectorList.push_back(Vector4(1681.823, 4817.896, 42.01214, 0.0));    //12
		VectorList.push_back(Vector4(1681.932, 4819.233, 42.03329, 0.0));     //13
		VectorList.push_back(Vector4(1681.289, 4820.775, 42.05544, 0.0));    //14
		VectorList.push_back(Vector4(1681.09, 4822.904, 42.05639, 0.0));       //15--Grapesead

	}
	else
	{

		VectorList.push_back(Vector4(-1038.972, -2736.403, 20.16928, 0.0));   //16
		VectorList.push_back(Vector4(-1037.887, -2738.665, 20.16928, 0.0));    //17
		VectorList.push_back(Vector4(-1035.784, -2738.059, 20.16928, 0.0));    //18
		VectorList.push_back(Vector4(-1036.624, -2735.942, 20.16928, 0.0));   //19--Airport

	}

	int Level;
	int Nat = RandomInt(1, 5);

	int PZSetMinSession = (MySettings.MinSession * 60) * 1000;
	int PZSetMaxSession = (MySettings.MaxSession * 60) * 1000;

	for (int i = 0; i < (int)VectorList.size() - 1; i++)
	{
		Level = UniqueLevels();
		ClothBank MB = NewClothBank();
		PlayerBrain newBrain = PlayerBrain(MB.Name, MB.Name + std::to_string(Level), MB, InGameTime() + RandomInt(PZSetMinSession, PZSetMaxSession), Level, false, false, Nat, YourGunNum(), 0, "", - 1);
		PedList.push_back(newBrain);
		PlayerPedGen(VectorList[i], &PedList[PedList.size() - 1], false);
	}
	Level = UniqueLevels();
	ClothBank MB = NewClothBank();
	PlayerBrain newBrain = PlayerBrain(MB.Name, MB.Name + std::to_string(Level), MB, InGameTime() + RandomInt(PZSetMinSession, PZSetMaxSession), Level, false, false, Nat, YourGunNum(), 0, "", - 1);
	PedList.push_back(newBrain);
	Ped Bob = PlayerPedGen(VectorList[(int)VectorList.size() - 1], &PedList[PedList.size() - 1], false);
	WAIT(1200);

	FIRE::ADD_OWNED_EXPLOSION(Bob, VectorList[0].X, VectorList[0].Y, VectorList[0].Z, 0, 1, true, false, 1);

	HeistPopUp = false;
}

int RadioSwitching = 0;

bool FindNSPMTargets(std::vector<Ped>* playerHaters, Vector3* pos, Ped* peddy, Ped* target)
{
	bool OutPut = false;
	if (ClosedSession)
	{
		LoggerLight("FindNSPMTargets");
		std::vector<std::string> Findings = ReadFile(NspmTargs);
		int ListSize = (int)Findings.size();
		for (int i = 0; i < ListSize; i++)
		{
			std::string line = Findings[i];
			if (StringContains("MultiPed=", line))
			{
				Ped YoPed = (Ped)StingNumbersInt(line);
				if (ENTITY::DOES_ENTITY_EXIST(YoPed))
				{
					playerHaters->push_back(YoPed);
					OutPut = true;
				}
			}
			else if (StringContains("FollowPed=", line) || StringContains("Ped=", line))
			{
				line = line.substr(4);
				Ped YoPed = (Ped)StingNumbersInt(line);
				if (ENTITY::DOES_ENTITY_EXIST(YoPed))
				{
					*peddy = YoPed;
					OutPut = true;
				}
				break;
			}
			else if (StringContains("AttackPed=", line) || StringContains("RamYou=", line))
			{
				line = line.substr(4);
				Ped YoPed = (Ped)StingNumbersInt(line);
				if (ENTITY::DOES_ENTITY_EXIST(YoPed))
				{
					*target = YoPed;
					OutPut = true;
				}
				break;
			}
			else if (StringContains("[Vectors]", line))
			{
				if (ListSize > 2)
				{
					float LineX = StingNumbersFloat(Findings[i + 1].substr(5));
					float LineY = StingNumbersFloat(Findings[i + 2].substr(5));
					float LineZ = StingNumbersFloat(Findings[i + 3].substr(5));
					*pos = NewVector3(LineX, LineY, LineZ);
					OutPut = true;
				}
				break;
			}
		}
	}
	return OutPut;
}
void ThisWayDriver(PlayerBrain* brain)
{
	brain->FindPlayer = InGameTime() + 15000;
	std::vector<Ped> AltoPed = {};
	Vector3 NSPMVec = RandomLocation();
	Entity NSPMPed = NULL;
	Entity TargetPed = NULL;
	bool WayPointAct = false;

	if (brain->ThisEnemy != NULL)
	{
		if (!(bool)ENTITY::DOES_ENTITY_EXIST(brain->ThisEnemy))
			brain->ThisEnemy = NULL;
		else if ((bool)ENTITY::IS_ENTITY_DEAD(brain->ThisEnemy))
			brain->ThisEnemy = NULL;
	}

	if (brain->FaveRadio > 0)
	{
		if (brain->FaveRadio == 99)
		{
			if (RadioSwitching < InGameTime())
			{
				RadioSwitching = InGameTime() + LessRandomInt("RadioSwitching01", 20000, 60000);
				AUDIO::SET_VEH_RADIO_STATION(brain->ThisVeh, (LPSTR)RadioGaGa[LessRandomInt("RadioSwitching02", 0, (int)RadioGaGa.size() - 1)].c_str());
			}
		}
		else if (brain->RadioHeads)
		{
			brain->RadioHeads = false;
			AUDIO::SET_VEH_RADIO_STATION(brain->ThisVeh, (LPSTR)RadioGaGa[brain->FaveRadio].c_str());
		}
	}
	else if (brain->RadioHeads)
	{
		brain->RadioHeads = false;
		AUDIO::SET_VEHICLE_RADIO_ENABLED(brain->ThisVeh, false);
	}

	if ((bool)UI::IS_WAYPOINT_ACTIVE())
	{
		WayPointAct = true;
		NSPMVec = MyWayPoint();
		brain->ThisEnemy = NULL;
	}
	else if (FindNSPMTargets(&AltoPed, &NSPMVec, &NSPMPed, &TargetPed))
	{
		if (AltoPed.size() > 0 && brain->ThisEnemy == NULL)
		{
			float fDis = 5000.0f;
			for (int i = 0; i < (int)AltoPed.size(); i++)
			{
				if (ENTITY::DOES_ENTITY_EXIST(AltoPed[i]))
				{
					if (!ENTITY::IS_ENTITY_DEAD(AltoPed[i]))
					{
						float fpDis = DistanceTo(brain->ThisPed, AltoPed[i]);
						if (fpDis < fDis)
						{
							fDis = fpDis;
							brain->ThisEnemy = AltoPed[i];
						}
					}
				}
			}
		}
		else if (NSPMPed == NULL && TargetPed == NULL)
			WayPointAct = true;
	}
	else
	{
		if (brain->ThisEnemy == NULL)
			brain->ThisEnemy = FindAFight(brain);
	}

	if (brain->IsPlane)
	{
		if (NSPMPed != NULL)
			FlyPlane(brain->ThisPed, brain->ThisVeh, EntityPosition(NSPMPed), NSPMPed, false, brain->PrefredVehicle);
		else if (TargetPed != NULL)
			FlyPlane(brain->ThisPed, brain->ThisVeh, EntityPosition(TargetPed), TargetPed, true, brain->PrefredVehicle);
		else if (brain->ThisEnemy != NULL)
			FlyPlane(brain->ThisPed, brain->ThisVeh, EntityPosition(brain->ThisEnemy), brain->ThisEnemy, true, brain->PrefredVehicle);
		else
			FlyPlane(brain->ThisPed, brain->ThisVeh, NSPMVec, NULL, false, brain->PrefredVehicle);
	}
	else if (brain->IsHeli)
	{
		if (WayPointAct)
		{
			if (DistanceTo(brain->ThisPed, NSPMVec) < 150.0f)
				LandNearHeli(brain->ThisPed, brain->ThisVeh, NSPMVec);
			else
				FlyHeli(brain->ThisPed, brain->ThisVeh, NewVector3(NSPMVec.x, NSPMVec.y, NSPMVec.z + 115.0f), 45.0f, 25.0f);
		}
		else if (NSPMPed != NULL)
			ChaseHeli(brain->ThisPed, brain->ThisVeh, NSPMPed);
		else if (TargetPed != NULL)
			ChaseHeli(brain->ThisPed, brain->ThisVeh, TargetPed);
		else if (brain->ThisEnemy != NULL)
			ChaseHeli(brain->ThisPed, brain->ThisVeh, brain->ThisEnemy);
		else
		{
			if (DistanceTo(brain->ThisPed, NSPMVec) < 150.0f)
				LandNearHeli(brain->ThisPed, brain->ThisVeh, NSPMVec);
			else
				FlyHeli(brain->ThisPed, brain->ThisVeh, NewVector3(NSPMVec.x, NSPMVec.y, NSPMVec.z + 115.0f), 45.0f, 25.0f);
		}
	}
	else
	{
		if (WayPointAct)
			DriveToooDest(brain->ThisPed, brain->ThisVeh, NSPMVec, 35.0f, true, false, false);
		else if (NSPMPed != NULL)
			FollowThatCart(brain->ThisPed, brain->ThisVeh, NSPMPed, false);
		else if (TargetPed != NULL)
			FollowThatRaming(brain->ThisPed, brain->ThisVeh, TargetPed);
		else if (brain->ThisEnemy != NULL)
			FollowThatCart(brain->ThisPed, brain->ThisVeh, brain->ThisEnemy, true);
		else
			DriveToooDest(brain->ThisPed, brain->ThisVeh, NSPMVec, 35.0f, true, false, false);
	}
}
void ThisWayFollower(PlayerBrain* brain, bool playVeh)
{
	brain->FindPlayer = InGameTime() + 15000;
	Vector3 NSPMVec = PlayerPosi();

	if (brain->ThisEnemy != NULL)
	{
		if (!(bool)ENTITY::DOES_ENTITY_EXIST(brain->ThisEnemy))
			brain->ThisEnemy = NULL;
		else if ((bool)ENTITY::IS_ENTITY_DEAD(brain->ThisEnemy))
			brain->ThisEnemy = NULL;
	}

	if (brain->IsPlane)
	{
		std::vector<Ped> AltoPed = {};
		Entity NSPMPed = NULL;
		Entity TargetPed = NULL;
		if (FindNSPMTargets(&AltoPed, &NSPMVec, &NSPMPed, &TargetPed))
		{
			if (AltoPed.size() > 0 && brain->ThisEnemy == NULL)
			{
				float fDis = 5000.0f;
				for (int i = 0; i < (int)AltoPed.size(); i++)
				{
					if (ENTITY::DOES_ENTITY_EXIST(AltoPed[i]))
					{
						if (!ENTITY::IS_ENTITY_DEAD(AltoPed[i]))
						{
							float fpDis = DistanceTo(brain->ThisPed, AltoPed[i]);
							if (fpDis < fDis)
							{
								fDis = fpDis;
								brain->ThisEnemy = AltoPed[i];
							}
						}
					}
				}
			}
		}
		else
		{
			if (brain->ThisEnemy == NULL)
				brain->ThisEnemy = FindAFight(brain);
		}

		if (brain->ThisEnemy != NULL)
			FlyPlane(brain->ThisPed, brain->ThisVeh, EntityPosition(brain->ThisEnemy), brain->ThisEnemy, true, brain->PrefredVehicle);
		else
			FlyPlane(brain->ThisPed, brain->ThisVeh, NSPMVec, PLAYER::PLAYER_PED_ID(), false, brain->PrefredVehicle);
	}
	else if (brain->IsHeli)
	{
		if (playVeh)
			ChaseHeli(brain->ThisPed, brain->ThisVeh, PLAYER::PLAYER_PED_ID());
		else
		{
			if (DistanceTo(brain->ThisPed, NSPMVec) < 150.0f)
				LandNearHeli(brain->ThisPed, brain->ThisVeh, NSPMVec);
			else
				FlyHeli(brain->ThisPed, brain->ThisVeh, NewVector3(NSPMVec.x, NSPMVec.y, NSPMVec.z + 115.0f), 45.0f, 25.0f);
		}
	}
	else
	{
		if (playVeh)
			FollowThatCart(brain->ThisPed, brain->ThisVeh, PLAYER::PLAYER_PED_ID(), false);
		else
			DriveToooDest(brain->ThisPed, brain->ThisVeh, NSPMVec, 35.0f, true, false, false);
	}
}
void CreatePlayer()
{
	LoggerLight("CreatePlayer");

	if (AirVehCount < 0)
		AirVehCount = 0;

	int PZSetMinSession = (MySettings.MinSession * 60) * 1000;
	int PZSetMaxSession = (MySettings.MaxSession * 60) * 1000;

	int Level = UniqueLevels();
	int GetGunnin = YourGunNum();
	ClothBank MB = NewClothBank();
	int Rad = LessRandomInt("RadioSet", -1, (int)RadioGaGa.size() - 1);
	if (LessRandomInt("RadioFlik", 0, 20) < 3)
		Rad = 99;
	int Nat = RandomInt(1, 5);
	PlayerBrain newBrain = PlayerBrain(MB.Name, MB.Name + std::to_string(Level), MB, InGameTime() + RandomInt(PZSetMinSession, PZSetMaxSession), Level, false, false, Nat, GetGunnin, 0, "", Rad);

	bool CanFill = false;
	int BrainType = LessRandomInt("CreatePlayerz01", 1, 10);

	if (MySettings.Aggression == 0)
	{
		newBrain.IsSpecialPed = true;
		newBrain.SessionJumper = true;
	}
	else if (MySettings.Aggression == 1)
	{
		if (BrainType < 5)
		{
			newBrain.IsSpecialPed = true;
			newBrain.SessionJumper = true;
		}
	}
	else if (MySettings.Aggression == 2)
	{
		if (BrainType < 2)
			newBrain.Friendly = false;
		else if (BrainType > 8)
		{
			newBrain.IsSpecialPed = true;
			newBrain.SessionJumper = true;
		}
	}
	else if (MySettings.Aggression == 3)
	{
		if (BrainType < 3)
			newBrain.Friendly = false;
		else if (BrainType > 8)
		{
			newBrain.IsSpecialPed = true;
			newBrain.SessionJumper = true;
		}
	}
	else if (MySettings.Aggression == 4)
	{
		if (BrainType < 4)
			newBrain.Friendly = false;
		else if (BrainType > 9)
		{
			newBrain.IsSpecialPed = true;
			newBrain.SessionJumper = true;
		}
	}
	else if (MySettings.Aggression == 5)
	{
		if (BrainType < 5)
			newBrain.Friendly = false;
		else if (BrainType > 9)
		{
			newBrain.IsSpecialPed = true;
			newBrain.SessionJumper = true;
		}
	}
	else if (MySettings.Aggression == 6)
	{
		if (BrainType < 6)
			newBrain.Friendly = false;
	}
	else if (MySettings.Aggression == 7)
	{
		if (BrainType < 7)
			newBrain.Friendly = false;
	}
	else if (MySettings.Aggression == 8)
	{
		if (BrainType < 8)
			newBrain.Friendly = false;
	}
	else if (MySettings.Aggression == 9)
	{
		if (BrainType < 9)
			newBrain.Friendly = false;
	}
	else
		newBrain.Friendly = false;

	if (!newBrain.Friendly)
		newBrain.BlipColour = 1;

	if (!MySettings.PlayerzBlips)
	{
		newBrain.OffRadarBool = true;
		newBrain.OffRadar = -1;
	}

	if (LessRandomInt("iBrain02", 1, 10) < 4 || MySettings.Aggression < 2 || newBrain.SessionJumper)
	{
		LoggerLight("CreatePlayer on foot");

		if (!newBrain.SessionJumper && newBrain.Friendly)
			newBrain.ApprochPlayer = true;
		PedList.push_back(newBrain);
		PlayerPedGen(FindPedSpPoint(), &PedList[(int)PedList.size() - 1], false);
	}
	else
	{
		LoggerLight("CreatePlayer Vehicle");

		int iTypeO = LessRandomInt("iBrain03", 1, 60);

		if (MySettings.Aggression < 4)
		{
			if (AirVehCount < MySettings.AirVeh)
			{
				if (iTypeO < 5 && newBrain.Friendly)
				{
					AirVehCount++;
					newBrain.PrefredVehicle = 3;//Plane
					newBrain.IsPlane = true;
				}
				else if (iTypeO < 20 && newBrain.Friendly)
				{
					AirVehCount++;
					newBrain.PrefredVehicle = 2;//Heli 
					newBrain.IsHeli = true;
					CanFill = true;
				}
				else
				{
					newBrain.PrefredVehicle = 1;//Veh				
					CanFill = true;
				}
			}
			else
			{
				newBrain.PrefredVehicle = 1;//Veh				
				CanFill = true;
			}
		}
		else
		{
			if (AirVehCount < MySettings.AirVeh)
			{
				if (iTypeO < 5 && MySettings.Aggression > 6)
				{
					AirVehCount++;
					newBrain.IsPlane = true;
					newBrain.PrefredVehicle = 8;//Oppressor
				}
				else if (iTypeO < 15)
				{
					newBrain.PrefredVehicle = 6;//ArmoredVeh
					CanFill = true;
				}
				else if (iTypeO < 30)
				{
					AirVehCount++;
					newBrain.IsPlane = true;
					newBrain.PrefredVehicle = 5;//AttPlane
				}
				else if (iTypeO < 40)
				{
					AirVehCount++;
					newBrain.IsHeli = true;
					newBrain.PrefredVehicle = 4;
					CanFill = true;
				}
				else if (iTypeO < 50 && newBrain.Friendly)
				{
					AirVehCount++;
					newBrain.IsPlane = true;
					newBrain.PrefredVehicle = 3;//Plane
				}
				else if (iTypeO < 55 && newBrain.Friendly)
				{
					AirVehCount++;
					newBrain.IsHeli = true;
					newBrain.PrefredVehicle = 2;//Heli 
					CanFill = true;
				}
				else
				{
					CanFill = true;
					newBrain.PrefredVehicle = 1;//Veh
				}
			}
			else
			{
				CanFill = true;
				if (iTypeO < 25)
					newBrain.PrefredVehicle = 6;//ArmoredVeh
				else
					newBrain.PrefredVehicle = 1;//Veh
			}

			if (!RentoCop && LessRandomInt("RentoCop", 1, 20) < 2)
			{
				RentoCop = true;
				newBrain.Friendly = true;
				newBrain.IsSpecialPed = true;
				newBrain.IsPlane = false;
				newBrain.IsHeli = false;
				newBrain.BlipColour = 0;
				if (newBrain.PFMySetting.Male)
					newBrain.PFMySetting.Cothing = ClothX("Cop", { 0, 0, -1, 0, 35, 0, 25, 0, 58, 0, 0, 55 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1 });
				else
					newBrain.PFMySetting.Cothing = ClothX("Cop", { 0, 0, -1, 14, 34, 0, 25, 0, 35, 0, 0, 48 }, { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 }, { -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1 });
				newBrain.PrefredVehicle = 1;
				newBrain.FaveVehicle = "police5";
				newBrain.GunSelect = 15;
				newBrain.RentaCop = true;
				CanFill = false;
			}
		}
		PedList.push_back(newBrain);
		newBrain.ThisVeh = SpawnVehicle(&PedList[(int)PedList.size() - 1], true, true);
		
	}
}
void InABuilding()
{
	LoggerLight("InABuilding");

	int PZSetMinSession = (MySettings.MinSession * 60) * 1000;
	int PZSetMaxSession = (MySettings.MaxSession * 60) * 1000;

	int iMit = LessRandomInt("InABuilding", 0, (int)AFKPlayers.size() - 1);
	std::string sName = SillyNameList();
	if (AddHackAttacks)
	{
		sName = "DoomSlayer";
		iMit = 3;
	}
	Blip FakeB = NULL;
	LocalBlip(&FakeB, AFKPlayers[iMit], 417, sName, 0);
	int iNat = RandomInt(1, 5);
	AfkPlayer MyAfk = AfkPlayer(FakeB, InGameTime() + RandomInt(PZSetMinSession, PZSetMaxSession), iNat, iMit, sName, sName, UniqueLevels());
	if (AddHackAttacks)
	{
		MyAfk.TheHacker = true;
		AddHackAttacks = false;
		HackAttacks = true;
	}

	AFKList.push_back(MyAfk);
}

int NewPedTime = 0;	
void NewPlayer()
{
	LoggerLight("NewPlayer");
	int PZSetMinWait = MySettings.MinWait * 1000;
	int PZSetMaxWait = MySettings.MaxWait * 1000;

	NewPedTime = InGameTime() + RandomInt(PZSetMinWait, PZSetMaxWait);

	int iHeister = NearHiest();
	if (PlayerZinSesh() + 5 < MySettings.MaxPlayers && iHeister != -1 && HeistPopUp)
		HeistDrips(iHeister);
	else
	{
		if (LessRandomInt("NewPlayer", 1, 10) < 8)
			CreatePlayer();
		else
			InABuilding();
	}
}

Prop WindMill;
bool GotWindMill = false;
void EclipsWindMill()
{
	LoggerLight("AddEclipsWindMill");
	if (WindMill == NULL)
		WindMill = BuildProps("prop_windmill_01", NewVector3(-832.50, 290.95, 83.00), NewVector3(-90.00, 94.72, 0.00), false);
	else
	{
		ENTITY::DELETE_ENTITY(&WindMill);
		WindMill = NULL;
	}
}
void DropObjects(Vector3 target)
{
	LoggerLight("DropObjects");

	std::string sObject;
	Entity Plop;
	if (LessRandomInt("DropObjects", 1, 10) < 5)
	{
		sObject = DropProplist[LessRandomInt("GetObject", 0, (int)DropProplist.size() - 1)];
		Plop = BuildProps(sObject, target, NewVector3(0.0, 0.0, 0.0), true);
		ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&Plop);

	}
	else
	{
		Plop = SpawnVehicleProp(target);
		ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&Plop);
	}
}
const std::vector<std::string> fluids = {
	"scr_solomon3",
	"scr_trev4_747_blood_splash",
	"cut_trevor1",
	"cs_trev1_blood_pool",
};
void TakeAwayYourMoney(MoneyBags* moneybag)
{
	for (int i = 0; i < (int)moneybag->TheseBags.size(); i++)
	{
		if ((bool)ENTITY::DOES_ENTITY_EXIST(moneybag->TheseBags[i]))
			ENTITY::DELETE_ENTITY(&moneybag->TheseBags[i]);
	}
}

void EasyWayOut(Ped peddy)
{
	WEAPON::GIVE_DELAYED_WEAPON_TO_PED(peddy, MyHashKey("WEAPON_pistol"), 1, true);
	WEAPON::SET_PED_AMMO(peddy, MyHashKey("WEAPON_pistol"), 100);
	WEAPON::SET_CURRENT_PED_WEAPON(peddy, MyHashKey("WEAPON_pistol"), true);
	WAIT(450);
	ForceAnim(peddy, "mp_suicide", "pistol", EntityPositionV4(peddy));
	AUDIO::PLAY_SOUND_FROM_ENTITY(-1, "GENERIC_CURSE_HIGH", peddy, 0, 0, 0);
	WAIT(700);
	int WaitHere = InGameTime() + 200;
	while (true)
	{
		if (WaitHere < InGameTime())
			break;
		if ((bool)STREAMING::HAS_NAMED_PTFX_ASSET_LOADED((LPSTR)fluids[0].c_str()))
		{
			GRAPHICS::_SET_PTFX_ASSET_NEXT_CALL((LPSTR)fluids[0].c_str());
			GRAPHICS::START_PARTICLE_FX_NON_LOOPED_ON_ENTITY((LPSTR)fluids[1].c_str(), peddy, 0.0f, 0.15f, 0.75f, 0.0f, 0.0f, 90.0f, 1.0f, false, false, false);
		}
		else
			STREAMING::REQUEST_NAMED_PTFX_ASSET((LPSTR)fluids[0].c_str());

		if ((bool)STREAMING::HAS_NAMED_PTFX_ASSET_LOADED((LPSTR)fluids[2].c_str()))
		{
			GRAPHICS::_SET_PTFX_ASSET_NEXT_CALL((LPSTR)fluids[2].c_str());
			GRAPHICS::START_PARTICLE_FX_NON_LOOPED_ON_ENTITY((LPSTR)fluids[3].c_str(), peddy, 0.0f, 0.15f, 0.75f, 0.0f, 0.0f, 90.0f, 0.5f, false, false, false);
		}
		else
			STREAMING::REQUEST_NAMED_PTFX_ASSET((LPSTR)fluids[2].c_str());

		WAIT(1);
	}
	ENTITY::SET_ENTITY_HEALTH(peddy, 1);
}
void PlayHorn(Vehicle vic, int duration)
{
	VEHICLE::START_VEHICLE_HORN(vic, duration, MyHashKey("HELDDOWN"), 0);
}
void FollowOn(PlayerBrain* brain, bool inVeh, bool playVeh)
{
	if (brain != NULL)
	{
		brain->WanBeFriends = false;
		brain->ApprochPlayer = false;
		brain->TimeOn = InGameTime() + ((MySettings.MaxSession * 60) * 1000);
		brain->Follower = true;
		brain->BlipColour = 38;
		if (brain->ThisBlip != NULL)
			UI::SET_BLIP_COLOUR(brain->ThisBlip, 38);

		if (inVeh)
		{
			brain->FindPlayer = InGameTime() + 5000;
			RelGroupMember(brain->ThisPed, Gp_Follow);
			PlayerEnterVeh(brain->ThisVeh);
		}
		else
		{
			if (playVeh)
			{
				PedDoGetIn(GetPlayersVehicle(), brain);
				RelGroupMember(brain->ThisPed, Gp_Follow);
			}
			else
				FolllowTheLeader(brain->ThisPed);
		}
	}
	YouFriend = JoinMe(brain);
}
void WillYouBeMyFriend()
{
	if (YouFriend.MyBrain != nullptr)
	{
		if (!(bool)ENTITY::DOES_ENTITY_EXIST(YouFriend.MyBrain->ThisPed))
			ClearYourFriend(true);
		else if ((bool)ENTITY::IS_ENTITY_DEAD(YouFriend.MyBrain->ThisPed))
			ClearYourFriend(true);
		else if (YouFriend.MyBrain->Driver)
		{
			if (!(bool)ENTITY::DOES_ENTITY_EXIST(YouFriend.MyBrain->ThisVeh))
				ClearYourFriend(true);
			else if ((bool)ENTITY::IS_ENTITY_DEAD(YouFriend.MyBrain->ThisVeh))
				ClearYourFriend(true);
			else if (!(bool)PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 0) && (bool)PED::IS_PED_IN_ANY_VEHICLE(YouFriend.MyBrain->ThisPed, 0) && DistanceTo(PlayerPosi(), ENTITY::GET_ENTITY_COORDS(YouFriend.MyBrain->ThisPed, true)) < 35.0f)
			{
				if (YouFriend.Horny)
				{
					YouFriend.Horny = false;
					PlayHorn(YouFriend.MyBrain->ThisVeh, RandomInt(3000, 8000));
				}
				GVM::TopLeft(YouFriend.MyBrain->MyName + " " + PZTranslate[12] + ControlSym[MySettings.Control_Keys_EnterVeh] + PZTranslate[14] + ". " + PZTranslate[12] + ControlSym[MySettings.Control_Keys_DissmisPed] + PZTranslate[15]);
				AddMarker(YouFriend.MyBrain->ThisVeh);
				if (GVM::ButtonDown(MySettings.Control_Keys_EnterVeh))
				{
					if (YouFriend.Planes)
					{
						PlayerEnterVeh(YouFriend.MyBrain->ThisVeh);
						YouFriend.MyBrain->PlaneLand = 5;
						ClearYourFriend(false);
					}
					else if (MySettings.Aggression < 9 || YouFriend.MyBrain->IsInContacts)
						FollowOn(YouFriend.MyBrain, true, false);
					else
					{
						FightPlayer(YouFriend.MyBrain);
						ClearYourFriend(false);
					}
				}
				else if (GVM::ButtonDown(MySettings.Control_Keys_DissmisPed))
				{
					PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(YouFriend.MyBrain->ThisPed, false);

					if (YouFriend.MyBrain->Follower)
						GetOutVehicle(YouFriend.MyBrain->ThisPed);
						
					ClearYourFriend(false);
				}
			}
			else
			{
				if (YouFriend.MyBrain->PlaneLand == 5)
					YouFriend.MyBrain->PlaneLand = 7;
				ClearYourFriend(true);
			}
		}
		else
		{
			if ((bool)PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 0))
			{
				if (DistanceTo(PlayerPosi(), ENTITY::GET_ENTITY_COORDS(YouFriend.MyBrain->ThisPed, true)) < 15.0f)
				{
					GVM::TopLeft(YouFriend.MyBrain->MyName + " " + PZTranslate[12] + ControlSym[86] + PZTranslate[16]);
					AddMarker(YouFriend.MyBrain->ThisPed);
					if (GVM::ButtonDown(86))
					{
						if (MySettings.Aggression < 9)
							FollowOn(YouFriend.MyBrain, false, true);
						else
						{
							FightPlayer(YouFriend.MyBrain);
							ClearYourFriend(false);
						}
					}
				}
			}
			else
			{
				if (DistanceTo(PlayerPosi(), ENTITY::GET_ENTITY_COORDS(YouFriend.MyBrain->ThisPed, true)) < 10.0f)
				{
					GVM::TopLeft(YouFriend.MyBrain->MyName + " " + PZTranslate[12] + ControlSym[MySettings.Control_Keys_AddPed] + PZTranslate[17] + ". " + PZTranslate[12] + ControlSym[MySettings.Control_Keys_DissmisPed] + PZTranslate[15]);
					AddMarker(YouFriend.MyBrain->ThisPed);
					if (GVM::ButtonDown(MySettings.Control_Keys_AddPed))
					{
						if (MySettings.Aggression < 9)
							FollowOn(YouFriend.MyBrain, false, false);
						else
						{
							FightPlayer(YouFriend.MyBrain);
							ClearYourFriend(false);
						}
					}
					else if (GVM::ButtonDown(MySettings.Control_Keys_DissmisPed))
						ClearYourFriend(false);
				}
			}
		}
	}
}

void PlayersKiller()
{
	int iKiller = -1;
	Ped WhoShot = PED::_GET_PED_KILLER(PLAYER::PLAYER_PED_ID());
	for (int i = 0; i < (int)PedList.size(); i++)
	{
		if (PedList[i].ThisPed == WhoShot)
		{
			PedList[i].Kills += 1;
			iKiller = i;
			break;
		}
	}

	while ((bool)ENTITY::IS_ENTITY_DEAD(PLAYER::PLAYER_PED_ID()))
		WAIT(100);

	if (iKiller != -1)
	{
		GVM::BottomLeft(PZTranslate[18] + std::to_string(PedList[iKiller].Killed) + " - " + std::to_string(PedList[iKiller].Kills) + " " + PedList[iKiller].MyName);

		if (MySettings.Aggression < 2)
		{
			PED::REMOVE_PED_FROM_GROUP(PedList[iKiller].ThisPed);
			GetOutVehicle(PedList[iKiller].ThisPed);
			PedList[iKiller].TimeOn = 0;
		}
	}
	PlayDead = false;
}

bool HackerIsInSession = false;
bool KeepFrieands = true;
std::vector<Vector3> PlaneFlightPath;
int ChatSHat = 0;

void ProcessAfk(AfkPlayer* brain)
{
	if (brain->TheHacker)
	{
		if (brain->StartTheHack == 0)
		{
			if (PlayerZinSesh() > MySettings.MaxPlayers - 4)
			{
				brain->StartTheHack--;
				brain->HackingTime = InGameTime() + 2500;
			}
		}
		else if (brain->StartTheHack == 1)
		{
			if (brain->HackingTime < InGameTime())
			{
				brain->StartTheHack = 2;
				for (int i = 0; i < (int)PedList.size(); i++)
				{
					if (PedList[i].ThisPed != NULL)
						DropObjects(EntityPosition(PedList[i].ThisPed));
				}
			}
		}
		else if (brain->StartTheHack == 2)
		{
			brain->StartTheHack = 3;

			int iPedLocate = ReteaveAfk(brain->MyIdentity);

			ClothBank MB = NewClothBank();
			int iNat = RandomInt(1, 5);
			int iGetGunnin = YourGunNum();
			Vector3 LandHere = FowardOf(PLAYER::PLAYER_PED_ID(), 5, true);
			PlayerBrain newBrain = PlayerBrain(brain->MyName, brain->MyIdentity, MB, InGameTime() + 10000000, brain->Level, false, false, iNat, iGetGunnin, 0, "", -1);
			newBrain.TheHacker = true;
			newBrain.IsSpecialPed = true;
			newBrain.PFMySetting.Model = "a_c_cat_01";
			PedList.push_back(newBrain);
			PlayerPedGen(Vector4(LandHere.x, LandHere.y, LandHere.z, 0.0), &PedList[PedList.size() - 1], false);
			AFKList[iPedLocate].MoveToOpen = true;
			AFKList[iPedLocate].TimeOn = 0;

		}
		else if (brain->StartTheHack < -50)
		{
			if (!(bool)ENTITY::IS_ENTITY_DEAD((Entity)PLAYER::PLAYER_PED_ID()))
			{
				HackerIsInSession = true;
				WAIT(4000);
				brain->StartTheHack = 1;
				Vector3 WindyMiller = NewVector3(-797.73, 295.47, 190.00);
				if (WindMill == NULL)
					EclipsWindMill();

				WAIT(1000);
				for (int i = 0; i < (int)PedList.size(); i++)
				{
					if (PedList[i].ThisPed != NULL)
						MoveEntity(PedList[i].ThisPed, InAreaOfV3(WindyMiller, 2.0f, 7.0f));
					WAIT(500);
				}
				WAIT(1000);
				MoveEntity(PLAYER::PLAYER_PED_ID(), InAreaOfV3(WindyMiller, 2.0f, 7.0f));
				brain->HackingTime = InGameTime() + 2000;
			}
		}
		else if (brain->StartTheHack < 0)
		{
			if (brain->HackingTime < InGameTime())
			{
				brain->StartTheHack--;
				brain->HackingTime = InGameTime() + 500;
				if (brain->HackSwitch)
					LocalBlip(&brain->ThisBlip, AFKPlayers[brain->App], 303, brain->MyName, 1);
				else
					LocalBlip(&brain->ThisBlip, AFKPlayers[brain->App], 417, brain->MyName, 0);

				brain->HackSwitch = !brain->HackSwitch;
			}
		}
	}
}
void ProcessPZ(PlayerBrain* brain)
{
	if (brain->ThisPed == NULL)
	{
		if (brain->TimeOn < InGameTime())
			PedCleaning(brain, PZTranslate[29], false);
	}
	else
	{
		if (brain->YoDeeeed)
		{
			if (brain->DeathSequence == 99)
			{
				int iDie = WhoShotMe(brain->ThisPed);

				if (iDie == -10)
				{
					if (brain->Bounty)
						AddMonies(7000);

					brain->Friendly = false;
					brain->BlipColour = 1;
					brain->ApprochPlayer = false;
					brain->Follower = false;
					brain->Killed += 1;

					GVM::BottomLeft(PZTranslate[18] + std::to_string(brain->Killed) + " - " + std::to_string(brain->Kills) + " " + brain->MyName);

				}
				else if (iDie != -1)
					GVM::BottomLeft(PedList[iDie].MyName + PZTranslate[19] + brain->MyName);
				else
				{
					if (brain->EWO)
					{
						brain->EWO = false;
						GVM::BottomLeft(brain->MyName + " " + PZTranslate[25]);
					}
					else
						GVM::BottomLeft(brain->MyName + " " + PZTranslate[28]);
				}

				brain->DeathTime = InGameTime() + 10000;
				brain->ThisEnemy = NULL;
				brain->Bounty = false;
				if (brain->MoneyDrops.TheseBags.size() > 0)
					TakeAwayYourMoney(&brain->MoneyDrops);
				brain->MoneyDrops.TheseBags.clear();
				brain->MoneyDrops.BagsDroped = 0;
				brain->MoneyDrops.BagTimer = 0;
				brain->DropMoneyBags = false;
				brain->InCombat = false;
				brain->SessionJumper = false;
				brain->Driver = false;
				brain->Passenger = false;
				brain->OffRadarBool = false;
				brain->IsPlane = false;
				brain->IsHeli = false;
				brain->GrabVeh = false;
				brain->IsAnimal = false;

				if (brain->Friendly || brain->Follower && iDie != -10)
				{
					brain->ApprochPlayer = true;
					brain->WanBeFriends = false;
				}
				PED::REMOVE_PED_FROM_GROUP(brain->ThisPed);
				brain->DeathSequence = 1;
			}
			else if (brain->DeathSequence == 1 || brain->DeathSequence == 3 || brain->DeathSequence == 5 || brain->DeathSequence == 7)
			{
				if (brain->DeathTime < InGameTime())
				{
					ENTITY::SET_ENTITY_ALPHA(brain->ThisPed, 80, false);
					brain->DeathSequence++;
					brain->DeathTime = InGameTime() + 500;
				}
			}
			else if (brain->DeathSequence == 2 || brain->DeathSequence == 4 || brain->DeathSequence == 6)
			{
				if (brain->DeathTime < InGameTime())
				{
					ENTITY::SET_ENTITY_ALPHA(brain->ThisPed, 255, false);
					brain->DeathSequence++;
					brain->DeathTime = InGameTime() + 500;
				}
			}
			else if (brain->DeathSequence == 8)
			{
				if (brain->DeathTime < InGameTime())
				{
					LoggerLight("-BringoutDead-");

					brain->DeathSequence = 0;
					brain->FindPlayer = 0;
					brain->DeathTime = 0;
					ENTITY::DELETE_ENTITY(&brain->ThisPed);
					brain->ThisPed = NULL;
					brain->YoDeeeed = false;

					if (brain->Killed > RandomInt(30, 50) || MySettings.Aggression < 2)
						brain->TimeOn = 0;

					PlayerPedGen(FindPedSpPoint(), brain, false);
				}
			}
		}
		else
		{
			int GameTime = InGameTime();
			Ped ThePlayer = PLAYER::PLAYER_PED_ID();
			Ped PlayZero = brain->ThisPed;
			Vector3 PlayerPos = PlayerPosi();
			Vector3 PedPos = EntityPosition(PlayZero);

			if ((bool)ENTITY::IS_ENTITY_DEAD(PlayZero))
			{
				ResetPlayer(brain, false);
				brain->DeathSequence = 99;
				brain->YoDeeeed = true;
			}
			else
			{
				BlipingBlip(brain);

				// Wanted level awareness: hostile NPCs respond faster when player has stars
				if (!brain->Friendly && !brain->Follower && !brain->Driver && !brain->Passenger && !brain->TheHacker)
				{
					int iWanted = PLAYER::GET_PLAYER_WANTED_LEVEL(ThePlayer);
					if (iWanted >= 2 && brain->FindPlayer > GameTime + 30000)
						brain->FindPlayer = GameTime + RandomInt(5000, 15000);
				}

				if (InPasiveMode)
				{
					if (ENTITY::GET_ENTITY_ALPHA(brain->ThisPed) != 120)
						ENTITY::SET_ENTITY_ALPHA(brain->ThisPed, 120, false);

					if (brain->ThisVeh != NULL)
					{
						if (ENTITY::GET_ENTITY_ALPHA(brain->ThisVeh) != 120)
							ENTITY::SET_ENTITY_ALPHA(brain->ThisVeh, 120, false);
					}
				}
				else
				{
					if (ENTITY::GET_ENTITY_ALPHA(brain->ThisPed) != 255)
						ENTITY::SET_ENTITY_ALPHA(brain->ThisPed, 255, false);

					if (brain->ThisVeh != NULL)
					{
						if (ENTITY::GET_ENTITY_ALPHA(brain->ThisVeh) != 255)
							ENTITY::SET_ENTITY_ALPHA(brain->ThisVeh, 255, false);
					}
				}

				if (brain->TimeOn < GameTime)
				{
					GetOutVehicle(PlayZero);
					PedCleaning(brain, PZTranslate[29], true);
				}
				else if (brain->IsSpecialPed)
				{
					if (brain->TheHacker)
					{
						if (brain->ThisEnemy != NULL)
						{
							if (!(bool)ENTITY::DOES_ENTITY_EXIST(brain->ThisEnemy))
								brain->ThisEnemy = NULL;
							else if ((bool)ENTITY::IS_ENTITY_DEAD(brain->ThisEnemy))
								brain->ThisEnemy = NULL;
							else if (brain->FindPlayer < GameTime)
							{
								int iVic = ReteaveBrain(brain->ThisEnemy);
								if (iVic > -1 && iVic < (int)PedList.size())
									FireOrb(brain, &PedList[iVic]);
								else if (iVic == -1)
									FireOrb(brain, nullptr);
								brain->ThisEnemy = NULL;
							}
						}
						else
						{
							brain->ThisEnemy = FindAFight(brain);
							if (brain->ThisEnemy == NULL)
							{
								WalkHere(PlayZero, FindingShops(PlayZero));
								brain->ShopTimer = GameTime + RandomInt(20000, 45000);
							}
							else
							{
								brain->FindPlayer = GameTime + RandomInt(25000, 30000);
								MoveEntity(PlayZero, EntityPosition(brain->ThisEnemy));
								GreefWar(PlayZero, brain->ThisEnemy);
							}
						}
					}
					else if (brain->SessionJumper)
					{
						if (DistanceTo(PedPos, PlayerPos) < 10.00f)
						{
							PED::REMOVE_PED_FROM_GROUP(PlayZero);
							PedCleaning(brain, PZTranslate[30], true);
							brain->TimeToGo = true;
						}
					}
					else if (brain->AirTranspport)
					{
						if (brain->PlaneLand == 10)
						{
							if ((bool)PED::IS_PED_IN_VEHICLE(PlayZero, brain->ThisVeh, false))
							{
								if ((int)PlaneFlightPath.size() < 1)
									PlaneFlightPath = BuildFlightPath(PlayerPos);
								ENTITY::FREEZE_ENTITY_POSITION(brain->ThisVeh, false);
								PED::SET_PED_CAN_BE_DRAGGED_OUT(PlayZero, false);
								AI::SET_DRIVE_TASK_DRIVING_STYLE(PlayZero, 262144);//16777216
								brain->FlightPath = 0;
								FlyPlane(PlayZero, brain->ThisVeh, PlaneFlightPath[brain->FlightPath], NULL, false, brain->PrefredVehicle);
								brain->PlaneLand--;
							}
						}
						else if (!(bool)PED::IS_PED_IN_ANY_VEHICLE(PlayZero, 0) || brain->ThisVeh == NULL)
						{
							brain->Driver = false;
							brain->TimeOn = 0;
						}
						else if (brain->PlaneLand == 9)
						{
							if (DistanceTo(PlaneFlightPath[brain->FlightPath], PedPos) < 150.0f)
							{
								LoggerLight("TakeOff...9");
								LandNearPlane(PlayZero, brain->ThisVeh, PlaneFlightPath[1], PlaneFlightPath[2]);
								brain->FlightPath++;
								brain->FlightPath++;
								brain->PlaneLand--;
							}
							else
								FlyPlane(PlayZero, brain->ThisVeh, PlaneFlightPath[brain->FlightPath], NULL, false, brain->PrefredVehicle);
						}
						else if (brain->PlaneLand == 8)
						{
							if (DistanceTo(PlaneFlightPath[2], PedPos) < 30.0f)
							{
								LoggerLight("TakeOff...8");
								ENTITY::FREEZE_ENTITY_POSITION(brain->ThisVeh, true);
								brain->PlaneLand--;
							}
						}
						else if (brain->PlaneLand == 7)
						{
							if (DistanceTo(PlayerPos, PedPos) < 30.0f)
							{
								LoggerLight("TakeOff...7");
								StayOnGround(brain->ThisVeh);
								ClearYourFriend(true);
								YouFriend.MyBrain = brain;

								brain->TimeOn = InGameTime() + ((MySettings.MaxSession * 60) * 1000);
								brain->PlaneLand--;
							}
						}
						else if (brain->PlaneLand == 6)
						{

						}
						else if (brain->PlaneLand == 5)
						{
							if ((bool)PED::IS_PED_IN_VEHICLE(ThePlayer, brain->ThisVeh, false))
							{
								LoggerLight("TakeOff...5");
								ENTITY::FREEZE_ENTITY_POSITION(brain->ThisVeh, false);
								brain->FlightPath++;
								DriveToooDest(PlayZero, brain->ThisVeh, PlaneFlightPath[brain->FlightPath], 12.0f, false, true, true);
								brain->PlaneLand--;
							}
						}
						else if (brain->PlaneLand == 4)
						{
							if (DistanceTo(PlaneFlightPath[brain->FlightPath], PedPos) < 5.0f)
							{
								LoggerLight("TakeOff...4");
								brain->FlightPath++;
								DriveToooDest(PlayZero, brain->ThisVeh, PlaneFlightPath[brain->FlightPath], 12.0f, true, true, true);
								brain->PlaneLand--;
							}
						}
						else if (brain->PlaneLand == 3)
						{
							if (DistanceTo(PlaneFlightPath[brain->FlightPath], PedPos) < 5.0f)
							{
								LoggerLight("TakeOff...3");
								brain->FlightPath++;
								FlyPlane(PlayZero, brain->ThisVeh, PlaneFlightPath[brain->FlightPath], NULL, false, brain->PrefredVehicle);
								brain->PlaneLand--;
							}
						}
						else if (brain->PlaneLand == 2)
						{
							if (DistanceTo(PlaneFlightPath[brain->FlightPath], PedPos) < 150)
							{
								LoggerLight("TakeOff...2");
								if (LandingGear(brain->ThisVeh) == 0)
									VEHICLE::_SET_VEHICLE_LANDING_GEAR(brain->ThisVeh, 1);
								brain->FlightPath++;
								if (brain->FlightPath < (int)PlaneFlightPath.size())
									FlyPlane(PlayZero, brain->ThisVeh, PlaneFlightPath[brain->FlightPath], NULL, false, brain->PrefredVehicle);
								else
								{
									brain->FlightPath = 0;
									PlaneFlightPath = BuildFlightPath(PlayerPos);
									FlyPlane(PlayZero, brain->ThisVeh, PlaneFlightPath[0], NULL, false, brain->PrefredVehicle);
									brain->PlaneLand = 9;
								}
							}
							else
								FlyPlane(PlayZero, brain->ThisVeh, PlaneFlightPath[brain->FlightPath], NULL, false, brain->PrefredVehicle);
						}
					}
					else if (brain->RentaCop)
					{
						if (brain->ThisVeh == NULL || ENTITY::IS_ENTITY_DEAD(brain->ThisVeh))
						{
							EasyWayOut(PlayZero);
							brain->Driver = false;
							brain->TimeOn = 0;
						}
						else
						{

							if (brain->ThisEnemy == ThePlayer)
							{
								Vehicle PlayVeh = GetPlayersVehicle();
								if (PlayVeh != NULL)
								{
									if (VEHICLE::IS_VEHICLE_STOPPED(PlayVeh))
									{
										if (DistanceTo(PlayZero, ThePlayer) < 30.0f)
										{
											AI::CLEAR_PED_TASKS(PlayZero);
											GetOutVehicle(PlayZero);
											AI::TASK_ARREST_PED(PlayZero, ThePlayer);
										}
									}
								}
								else
								{
									brain->ThisEnemy = NULL;
									AI::CLEAR_PED_TASKS(PlayZero);
								}
							}
							else if (brain->ThisEnemy != NULL)
							{
								if (!(bool)ENTITY::DOES_ENTITY_EXIST(brain->ThisEnemy))
									brain->ThisEnemy = NULL;
								else if ((bool)ENTITY::IS_ENTITY_DEAD(brain->ThisEnemy))
									brain->ThisEnemy = NULL;
							}
							else
							{
								if (!(bool)PED::IS_PED_IN_ANY_VEHICLE(PlayZero, 0))
								{
									if (brain->FindPlayer < GameTime)
									{
										brain->FindPlayer = GameTime + 5000;
										GetInVehicle(PlayZero, brain->ThisVeh, -1);
									}
								}
								else if (brain->FindPlayer < GameTime)
								{
									brain->FindPlayer = GameTime + 5000;
									if ((bool)PED::IS_PED_IN_ANY_VEHICLE(ThePlayer, 0))
									{
										brain->ThisEnemy = ThePlayer;
										AI::TASK_VEHICLE_CHASE(PlayZero, ThePlayer);
										VEHICLE::SET_VEHICLE_SIREN(brain->ThisVeh, true);
									}
									else
									{
										brain->ThisEnemy = FindAFight(brain);
										if (brain->ThisEnemy == NULL)
											VEHICLE::SET_VEHICLE_SIREN(brain->ThisVeh, false);
										else
										{
											VEHICLE::SET_VEHICLE_SIREN(brain->ThisVeh, true);
											PickFight(PlayZero, brain->ThisVeh, brain->ThisEnemy, brain->PrefredVehicle);
										}
									}
								}
							}
						}
					}
				}
				else if (brain->PlayerInVeh)
				{
					if (!(bool)PED::IS_PED_IN_ANY_VEHICLE(ThePlayer, 0))
						brain->PlayerInVeh = false;
					else if (brain->FindPlayer < GameTime)
						ThisWayDriver(brain);
				}
				else if (brain->DropMoneyBags)
				{
					if (brain->MoneyDrops.BagsDroped < 10)
					{
						if (brain->MoneyDrops.BagTimer < InGameTime())
						{
							brain->MoneyDrops.BagsDroped++;
							brain->MoneyDrops.BagTimer = InGameTime() + 500;
							Vector4 AboutHere = InAreaOf(ENTITY::GET_ENTITY_COORDS(PlayZero, true), 0.01f, 4.0f);
							Prop MoreBag = BuildProps(brain->MoneyDrops.Bags, NewVector3(AboutHere.X, AboutHere.Y, AboutHere.Z + 5.0f), NewVector3(0.0, 0.0, 0.0), true);
							brain->MoneyDrops.TheseBags.push_back(MoreBag);

							PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(PlayZero, true);
							RunHere(PlayZero, AboutHere);
						}
						for (int i = 0; i < (int)brain->MoneyDrops.TheseBags.size(); i++)
						{
							if (DistanceTo(PlayZero, brain->MoneyDrops.TheseBags[i]) < 1.25f)
							{
								if ((bool)ENTITY::DOES_ENTITY_EXIST(brain->MoneyDrops.TheseBags[i]))
									ENTITY::DELETE_ENTITY(&brain->MoneyDrops.TheseBags[i]);
							}
						}
					}
					else
					{
						TakeAwayYourMoney(&brain->MoneyDrops);

						brain->ApprochPlayer = true;
						brain->Friendly = true;
						brain->BlipColour = 0;

						PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(PlayZero, false);
						UI::SET_BLIP_COLOUR(brain->ThisBlip, 0);
						PED::REMOVE_PED_FROM_GROUP(PlayZero);
						PED::SET_PED_RELATIONSHIP_GROUP_HASH(PlayZero, Gp_Friend);
						AI::CLEAR_PED_TASKS_IMMEDIATELY(PlayZero);

						brain->MoneyDrops.TheseBags.clear();
						brain->MoneyDrops.BagsDroped = 0;
						brain->MoneyDrops.BagTimer = 0;
						brain->DropMoneyBags = false;
						brain->HackReaction = false;
					}
				}
				else if (brain->HackReaction)
				{
					if (LessRandomInt("HackReackt", 1, 10) < 3)
						brain->TimeOn = 0;
					brain->HackReaction = false;
				}
				else if (brain->WanBeFriends)
				{
					if (YouFriend.MyBrain != nullptr)
					{
						if (YouFriend.MyBrain->MyIdentity == brain->MyIdentity)
						{
							if ((bool)ENTITY::HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(PlayZero, ThePlayer, 1) || (bool)PLAYER::IS_PLAYER_FREE_AIMING_AT_ENTITY(ThePlayer, PlayZero))
							{
								ClearYourFriend(false);
								brain->WanBeFriends = false;

								if (MySettings.Aggression > 1)
									FightPlayer(brain);
								else
									PED::SET_PED_FLEE_ATTRIBUTES(PlayZero, 32, 1);
							}
							else if (brain->ThisVeh != NULL)
							{
								if ((bool)PED::IS_PED_IN_ANY_VEHICLE(ThePlayer, 0))
								{
									if (brain->ThisVeh == GetPlayersVehicle())
										brain->PlayerInVeh = true;
									else
									{
										ClearYourFriend(true);
										brain->WanBeFriends = false;
									}
								}
								else if (DistanceTo(PedPos, PlayerPos) > 60.0f)
								{
									ClearYourFriend(true);
									brain->WanBeFriends = false;
								}
							}
							else
							{
								if (brain->FindPlayer < GameTime)
								{
									WalkHere(PlayZero, PlayerPos);
									brain->FindPlayer = GameTime + 5000;
								}
								else if (DistanceTo(PedPos, PlayerPos) > 15.0f)
								{
									ClearYourFriend(true);
									brain->WanBeFriends = false;
								}
							}
						}
						else
						{
							ClearYourFriend(true);
							brain->WanBeFriends = false;
						}
					}
					else
						brain->WanBeFriends = false;
				}
				else if (brain->Follower)
				{
					if (brain->SessionGreating)
						brain->SessionGreating = false;

					if (brain->AtTheParty)
					{

					}
					else 
					{
						if (brain->Driver)
						{
							if (brain->ThisVeh == NULL)
								brain->Driver = false;
							else if (!(bool)PED::IS_PED_IN_ANY_VEHICLE(PlayZero, 0))
							{
								if ((bool)ENTITY::DOES_ENTITY_EXIST(brain->ThisVeh))
								{
									ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&brain->ThisVeh);
									brain->ThisVeh = NULL;
								}
							}
							else if ((bool)PED::IS_PED_IN_ANY_VEHICLE(ThePlayer, 0))
							{
								if (brain->ThisVeh == GetPlayersVehicle())
									brain->PlayerInVeh = true;
								else if (brain->FindPlayer < GameTime)
									ThisWayFollower(brain, false);
							}
							else
							{
								if (DistanceTo(PedPos, PlayerPos) < 30.0f && !brain->IsPlane)
								{
									ClearYourFriend(true);
									YouFriend.MyBrain = brain;
									brain->WanBeFriends = true;
								}
								else if (brain->FindPlayer < GameTime)
									ThisWayFollower(brain, false);
							}
						}
						else
						{
							if (brain->Passenger)
							{
								if (!(bool)PED::IS_PED_IN_ANY_VEHICLE(ThePlayer, 0))
								{
									GetOutVehicle(PlayZero);

									PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(PlayZero, false);
									brain->Passenger = false;
									FolllowTheLeader(PlayZero);
								}
							}
							else
							{
								if ((bool)PED::IS_PED_IN_ANY_VEHICLE(ThePlayer, 0))
								{
									Vehicle Vick = GetPlayersVehicle();
									if (Vick != NULL)
									{
										if (!PedDoGetIn(Vick, brain))
										{
											if (!EnterFriendsVeh(brain))
											{
												if (brain->FaveVehicle == "")
												{
													int iClass = VEHICLE::GET_VEHICLE_CLASS(Vick);
													if (iClass == 15 || iClass == 16)
														brain->PrefredVehicle = 4;
													else if (MySettings.Aggression > 5)
														brain->PrefredVehicle = 6;
													else
														brain->PrefredVehicle = 1;
												}
												brain->ThisVeh = SpawnVehicle(brain, false, false);
											}
										}
									}
								}
								else if (DistanceTo(PlayerPos, PedPos) > 150.00 && !PED::IS_PED_FALLING(ThePlayer) && !PED::IS_PED_IN_PARACHUTE_FREE_FALL(ThePlayer) && PED::GET_PED_PARACHUTE_STATE(ThePlayer) < 1)
								{
									MoveEntity(PlayZero, PlayerPos);
									FolllowTheLeader(PlayZero);
								}
							}
						}
					}
				}
				else if (brain->Friendly)
				{

					if ((bool)ENTITY::HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(PlayZero, ThePlayer, 1) || (bool)PED::IS_PED_IN_COMBAT(PlayZero, ThePlayer) || (bool)PLAYER::IS_PLAYER_FREE_AIMING_AT_ENTITY(ThePlayer, PlayZero))
					{
						brain->ApprochPlayer = false;
						PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(PlayZero, false);

						if (MySettings.Aggression > 1)
							FightPlayer(brain);
						else
							PED::SET_PED_FLEE_ATTRIBUTES(PlayZero, 32, 1);
					}
					else
					{
						if (brain->Passenger)
						{
							if (!(bool)PED::IS_PED_IN_ANY_VEHICLE(PlayZero, 0))
							{
								brain->ThisVeh = NULL;
								brain->Passenger = false;
							}
						}
						else if (brain->Driver)
						{
							if (brain->ThisVeh == NULL)
								brain->Driver = false;
							else if (!(bool)PED::IS_PED_IN_ANY_VEHICLE(PlayZero, 0))
							{
								ENTITY::SET_ENTITY_ALPHA(brain->ThisVeh, 255, false);
								brain->Driver = false;
							}
							else if (!(bool)PED::IS_PED_IN_ANY_VEHICLE(ThePlayer, 0))
							{
								if (!brain->IsPlane && brain->ApprochPlayer && YouFriend.MyBrain == nullptr && DistanceTo(PlayerPos, PedPos) < 50.0f)
								{
									YouFriend.MyBrain = brain;

									if (brain->IsHeli)
									{
										YouFriend.Horny = false;
										LandNearHeli(PlayZero, brain->ThisVeh, PlayerPos);
									}
									else if (brain->Driver)
										FollowThatCart(PlayZero, brain->ThisVeh, ThePlayer, false);

									brain->WanBeFriends = true;
									brain->ApprochPlayer = false;
								}
								else
								{
									if (MySettings.Aggression < 2)
									{
										if (!brain->IsHeli && !brain->IsPlane && FindUSeat(brain->ThisVeh, false) != -10)
										{
											PlayerBrain* ThisBrian = FindaPassenger(true);
											if (ThisBrian != nullptr)
												PedDoGetIn(brain->ThisVeh, ThisBrian);
										}
										else
										{
											if (brain->FindPlayer < GameTime)
											{
												DriveAround(PlayZero, brain->Friendly);
												brain->FindPlayer = GameTime + 25000;
											}
										}
									}
									else
									{
										if (brain->FindPlayer < GameTime)
										{
											brain->FindPlayer = GameTime + 5000;
											if (brain->ThisEnemy != NULL)
											{
												if (!(bool)ENTITY::DOES_ENTITY_EXIST(brain->ThisEnemy))
													brain->ThisEnemy = NULL;
												else if ((bool)ENTITY::IS_ENTITY_DEAD(brain->ThisEnemy))
													brain->ThisEnemy = NULL;
											}
											else if (!brain->IsHeli && !brain->IsPlane && FindUSeat(brain->ThisVeh, false) != -10)
											{
												PlayerBrain* ThisBrian = FindaPassenger(true);
												if (ThisBrian != nullptr)
													PedDoGetIn(brain->ThisVeh, ThisBrian);
											}
											else
											{
												// Low aggression: cruise to hotspot instead of fighting from vehicle
												if (MySettings.Aggression <= 3 && LessRandomInt("FriHotDrive", 1, 10) < 9)
												{
													DriveToHotspot(PlayZero, brain->ThisVeh, brain->Friendly);
													brain->FindPlayer = GameTime + 60000;
												}
												else
												{
													brain->ThisEnemy = FindAFight(brain);
													if (brain->ThisEnemy != NULL)
													{
														brain->EnemyPos = DistanceTo((Entity)brain->ThisEnemy, PedPos) - 1.0f;
														PickFight(PlayZero, brain->ThisVeh, brain->ThisEnemy, brain->PrefredVehicle);
														FightTogether(brain->ThisVeh, brain->ThisEnemy);
													}
												}
											}
										}
									}
								}
							}
							else
							{
								if (MySettings.Aggression < 2)
								{
									if (!brain->IsHeli && !brain->IsPlane && FindUSeat(brain->ThisVeh, false) != -10)
									{
										PlayerBrain* ThisBrian = FindaPassenger(true);
										if (ThisBrian != nullptr)
											PedDoGetIn(brain->ThisVeh, ThisBrian);
									}
									else
									{
										if (brain->FindPlayer < GameTime)
										{
											DriveAround(PlayZero, brain->Friendly);
											brain->FindPlayer = GameTime + 25000;
										}
									}
								}
								else
								{
									if (brain->FindPlayer < GameTime)
									{
										brain->FindPlayer = GameTime + 5000;
										if (brain->ThisEnemy != NULL)
										{
											if (!(bool)ENTITY::DOES_ENTITY_EXIST(brain->ThisEnemy))
												brain->ThisEnemy = NULL;
											else if ((bool)ENTITY::IS_ENTITY_DEAD(brain->ThisEnemy))
												brain->ThisEnemy = NULL;
										}
										else if (!brain->IsHeli && !brain->IsPlane && FindUSeat(brain->ThisVeh, false) != -10)
										{
											PlayerBrain* ThisBrian = FindaPassenger(true);
											if (ThisBrian != nullptr)
												PedDoGetIn(brain->ThisVeh, ThisBrian);
										}
										else if (MySettings.Aggression <= 3 && LessRandomInt("FriHotDrive", 1, 10) < 9)
										{
											// Aggression 2-3: cruise to a hotspot like a real player
											DriveToHotspot(PlayZero, brain->ThisVeh, brain->Friendly);
											brain->FindPlayer = GameTime + 60000;
										}
										else
										{
											// Low aggression: cruise to hotspot instead of fighting from vehicle
											if (MySettings.Aggression <= 3 && LessRandomInt("FriHotDrive", 1, 10) < 9)
											{
												DriveToHotspot(PlayZero, brain->ThisVeh, brain->Friendly);
												brain->FindPlayer = GameTime + 60000;
											}
											else
											{
												brain->ThisEnemy = FindAFight(brain);
												if (brain->ThisEnemy != NULL)
												{
													brain->EnemyPos = DistanceTo((Entity)brain->ThisEnemy, PedPos) - 1.0f;
													PickFight(PlayZero, brain->ThisVeh, brain->ThisEnemy, brain->PrefredVehicle);
													FightTogether(brain->ThisVeh, brain->ThisEnemy);
												}
											}
										}
									}
								}
							}
						}
						else
						{
							if (DistanceTo(PedPos, PlayerPos) < 7.0f)
							{
								if (brain->ApprochPlayer && YouFriend.MyBrain == nullptr)
								{
									if (!brain->WanBeFriends)
									{
										YouFriend.MyBrain = brain;

										brain->WanBeFriends = true;
										brain->ApprochPlayer = false;
									}
								}
							}
							else if (brain->IsWanted && brain->WantedTimer < GameTime)
							{
								// Wanted status expired — holster weapon and return to normal.
								brain->IsWanted = false;
								if (brain->ArmedWeaponHash != 0)
									WEAPON::SET_CURRENT_PED_WEAPON(brain->ThisPed, GAMEPLAY::GET_HASH_KEY("WEAPON_UNARMED"), true);
								brain->FindPlayer = GameTime + RandomInt(15000, 40000);
								PickNextAction(brain);
							}
							else if (brain->IsWanted && !ENTITY::IS_ENTITY_DEAD(brain->ThisPed))
							{
								// While wanted: flee from the player on foot (looks like evading cops).
								if (!brain->Driver && GameTime % 8000 < 100) // nudge flee task every ~8s
								{
									Vector3 pp = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
									AI::TASK_SMART_FLEE_COORD(brain->ThisPed, pp.x, pp.y, pp.z, 100.0f, 30000, false, false);
								}
							}
							else if (brain->ScenarioTimer > 0 && GameTime > brain->ScenarioTimer)
							{
								// Robbery phase 3: ScenarioTimer expired while robbing -- holster, flee.
								if (brain->RobPhase == 2)
								{
									brain->RobPhase = 3;
									if (brain->ArmedWeaponHash != 0)
									    WEAPON::SET_CURRENT_PED_WEAPON(brain->ThisPed,
									        GAMEPLAY::GET_HASH_KEY("WEAPON_UNARMED"), true);
									Vector3 pp = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);
									AI::TASK_SMART_FLEE_COORD(brain->ThisPed,
									    pp.x, pp.y, pp.z, 150.0f, 30000, false, false);
									brain->IsWanted      = true;
									brain->WantedTimer   = GameTime + 45000;
									brain->ScenarioTimer = 0;
								}
								else
								{
									// Normal scenario timeout -- pick a new action
									brain->ScenarioTimer = 0;
									brain->FindPlayer = GameTime + RandomInt(15000, 40000);
									PickNextAction(brain);
								}
							}
							else if (brain->ShopTimer > 0 && GameTime > brain->ShopTimer)
							{
								// Robbery phase 2: ped arrived at the store -- draw weapon, hold briefly.
								if (brain->RobPhase == 1)
								{
									brain->RobPhase = 2;
									Ped peddy = brain->ThisPed;
									if (brain->ArmedWeaponHash != 0)
									    WEAPON::SET_CURRENT_PED_WEAPON(peddy, brain->ArmedWeaponHash, true);
									Vector3 pos = ENTITY::GET_ENTITY_COORDS(peddy, true);
									float hdg  = ENTITY::GET_ENTITY_HEADING(peddy) * 0.0174533f;
									float aimX = pos.x + cosf(hdg) * 3.0f;
									float aimY = pos.y + sinf(hdg) * 3.0f;
									AI::TASK_AIM_GUN_AT_COORD(peddy, aimX, aimY, pos.z, 10000, false, false);
									brain->ShopTimer     = 0;
									brain->ScenarioTimer = GameTime + RandomInt(8000, 14000);
								}
										else if (brain->InteriorEntryID != -1 && !brain->InsideInterior && !brain->ShopBrowsing)
										{
											// Interior phase 1: ped arrived at the entrance ΓÇö holster if restricted,
											// then walk 5 m inward to simulate entering the store.
											Ped peddy2 = brain->ThisPed;
											const LSRInterior* intr = LSRData::GetInterior(brain->InteriorEntryID);
											if (intr && intr->isWeaponRestricted && brain->ArmedWeaponHash != 0)
											    WEAPON::SET_CURRENT_PED_WEAPON(peddy2,
											        GAMEPLAY::GET_HASH_KEY("WEAPON_UNARMED"), true);
											// Walk a few metres inside from the entrance
											float inHdg = ENTITY::GET_ENTITY_HEADING(peddy2) * 0.0174533f;
											Vector3 inPos = ENTITY::GET_ENTITY_COORDS(peddy2, true);
											inPos.x += cosf(inHdg) * 5.0f;
											inPos.y += sinf(inHdg) * 5.0f;
											WalkHere(peddy2, inPos);
											PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy2, true);
											brain->InsideInterior = true;
											brain->ShopTimer      = GameTime + RandomInt(8000, 14000);  // walk-in time
										}
										else if (brain->InsideInterior && !brain->ShopBrowsing)
										{
											// Interior phase 2: look for a nearby ambient clerk ped (non-PZ)
											// to walk up to, simulating a counter purchase. If none exists,
											// play a browsing scenario in place.
											Ped peddy2 = brain->ThisPed;
											Vector3 myPos2 = ENTITY::GET_ENTITY_COORDS(peddy2, true);
											int nearBuf[256] = {};
											PED::GET_PED_NEARBY_PEDS(peddy2, nearBuf, -1);
											int nearN = nearBuf[0];
											if (nearN > 255) nearN = 255;
											Ped clerkPed = 0;
											for (int ki = 1; ki <= nearN; ki++)
											{
												Ped cand = (Ped)nearBuf[ki];
												if (!cand || !(bool)ENTITY::DOES_ENTITY_EXIST(cand)) continue;
												if ((bool)ENTITY::IS_ENTITY_DEAD(cand))              continue;
												if ((bool)PED::IS_PED_A_PLAYER(cand))               continue;
												bool isPZ = false;
												for (int pi = 0; pi < (int)PedList.size(); pi++)
													if (PedList[pi].ThisPed == cand) { isPZ = true; break; }
												if (isPZ) continue;
												if (DistanceTo(cand, myPos2) < 9.0f) { clerkPed = cand; break; }
											}
											AI::CLEAR_PED_TASKS(peddy2);
											int browseMs = RandomInt(25000, 60000);
											if (clerkPed != 0)
											{
												// Walk up to the clerk and wait nearby — looks like a purchase
												AI::TASK_GO_TO_ENTITY(peddy2, clerkPed, browseMs, 1.2f, 1.0f, 0, 0);
											}
											else
											{
												static const char* shopAnims[] = {
												    "WORLD_HUMAN_BROWSING",
												    "WORLD_HUMAN_STAND_MOBILE",
												    "WORLD_HUMAN_CLIPBOARD"
												};
												AI::TASK_START_SCENARIO_IN_PLACE(
												    peddy2, (LPSTR)shopAnims[RandomInt(0, 2)], 0, true);
											}
											PED::SET_PED_KEEP_TASK(peddy2, true);
											PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy2, true);
											brain->ShopBrowsing = true;
											brain->ShopTimer    = GameTime + browseMs;
										}
										else if (brain->ShopBrowsing)
										{
											// Interior phase 3: done browsing ΓÇö exit the shop.
											Ped peddy2 = brain->ThisPed;
											AI::CLEAR_PED_TASKS(peddy2);
											brain->InsideInterior  = false;
											brain->ShopBrowsing    = false;
											brain->InteriorEntryID = -1;
											brain->ShopTimer       = 0;
											PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy2, false);
											Vector3 ePos = ENTITY::GET_ENTITY_COORDS(peddy2, true);
											float eAngle = (float)(rand() % 360) * 0.0174533f;
											Vector3 eExit; eExit.x = ePos.x + cosf(eAngle) * 15.0f;
											               eExit.y = ePos.y + sinf(eAngle) * 15.0f; eExit.z = ePos.z;
											WalkHere(peddy2, eExit);
											brain->FindPlayer = GameTime + RandomInt(20000, 40000);
										}
								else if (brain->DrugBuyTarget != NULL)
								{
									// Drug purchase complete: buyer has reached the dealer and waited.
									// Clear task, allow events, walk off naturally.
									Ped peddy2 = brain->ThisPed;
									AI::CLEAR_PED_TASKS(peddy2);
									PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(peddy2, false);
									brain->DrugBuyTarget = NULL;
									brain->ShopTimer     = 0;
									brain->FindPlayer    = GameTime + RandomInt(20000, 40000);
									PickNextAction(brain);
								}
								else
								{
									// Normal shop visit complete -- pick a new action
									brain->ShopTimer  = 0;
									brain->RobPhase   = 0;
									brain->FindPlayer = GameTime + RandomInt(30000, 60000);
									PickNextAction(brain);
								}
							}
							else if (brain->FindPlayer < GameTime)
							{
								brain->FindPlayer = GameTime + 1000;

								if (MySettings.Aggression > 2)
								{
									if (brain->ThisEnemy != NULL)
									{
										if (!(bool)ENTITY::DOES_ENTITY_EXIST(brain->ThisEnemy))
											brain->ThisEnemy = NULL;
										else if ((bool)ENTITY::IS_ENTITY_DEAD(brain->ThisEnemy))
											brain->ThisEnemy = NULL;
										else if (DistanceTo((Entity)brain->ThisEnemy, PedPos) > brain->EnemyPos)
										{
											if (MySettings.Aggression > 5)
											{
												brain->EWO = true;
												EasyWayOut(PlayZero);
											}
											else
											{
												brain->ThisEnemy = NULL;
												brain->FindPlayer = GameTime + RandomInt(15000, 20000);
											}
										}
									}
									else
									{
										// Low aggression: friendly NPCs rarely pick fights on foot
										if (MySettings.Aggression <= 3 && RandomInt(1, 10) < 9)
										{
											brain->FindPlayer = GameTime + RandomInt(60000, 120000);
											PickNextAction(brain);
										}
										else
										{
											brain->FindPlayer = GameTime + RandomInt(60000, 120000);
											brain->ThisEnemy = FindAFight(brain);
											if (brain->ThisEnemy != NULL)
											{
												brain->EnemyPos = DistanceTo((Entity)brain->ThisEnemy, PedPos) - 1.0f;
												GreefWar(PlayZero, brain->ThisEnemy);
											}
											else
											{
												if (RandomInt(1, 10) < 4)
													PickNextAction(brain);
												else
													WalkHere(PlayZero, FindingShops(PlayZero));
													brain->ShopTimer = GameTime + RandomInt(20000, 45000);
											}
										}
									}
								}
								else
								{
									brain->FindPlayer = GameTime + RandomInt(50000, 100000);
									WalkHere(PlayZero, FindingShops(PlayZero));
									brain->ShopTimer = GameTime + RandomInt(20000, 45000);
								}
							}
						}
					}
				}
				else
				{


					if (brain->Driver)
					{
						if (brain->ThisVeh == NULL)
							brain->Driver = false;
						else if (brain->ThisVeh == GetPlayersVehicle())
						{
							ENTITY::SET_ENTITY_ALPHA(brain->ThisVeh, 255, false);
							brain->Driver = false;
							GetOutVehicle(PlayZero);
						}
						else if (!(bool)PED::IS_PED_IN_ANY_VEHICLE(PlayZero, 0))
						{
							ENTITY::SET_ENTITY_ALPHA(brain->ThisVeh, 255, false);
							brain->Driver = false;
						}
						// --- STUCK VEHICLE RECOVERY (checked every 30 s) ---
						// 30 s window avoids false-triggering on red-light stops (8-15 s).
						// Only re-route if the vehicle hasn't moved 3 m over the full window,
						// which means the ped is genuinely wedged against a wall, post, etc.
						if (ENTITY::DOES_ENTITY_EXIST(brain->ThisVeh) && brain->StuckCheckTimer < GameTime)
						{
							brain->StuckCheckTimer = GameTime + 30000;
							float curX = ENTITY::GET_ENTITY_COORDS(brain->ThisVeh, true).x;
							float curY = ENTITY::GET_ENTITY_COORDS(brain->ThisVeh, true).y;
							float moved = sqrtf((curX - brain->LastStuckX) * (curX - brain->LastStuckX) +
							                    (curY - brain->LastStuckY) * (curY - brain->LastStuckY));
							if (moved < 3.0f && ENTITY::GET_ENTITY_SPEED(brain->ThisVeh) < 0.5f &&
							    !brain->IsPulledOver && brain->ThisEnemy == NULL)
							{
								// Unstick: clear tasks and re-route to a hotspot
								AI::CLEAR_PED_TASKS(PlayZero);
								DriveToHotspot(PlayZero, brain->ThisVeh, brain->Friendly);
								brain->FindPlayer = GameTime + RandomInt(20000, 40000);
							}
							brain->LastStuckX = curX;
							brain->LastStuckY = curY;
						}
						else if (brain->FindPlayer < GameTime)
						{
							// Traffic stop: while pulled over, wait for timer, then resume.
							if (brain->IsPulledOver)
							{
								if (brain->PulloverTimer < GameTime)
								{
									brain->IsPulledOver = false;
									brain->FindPlayer   = GameTime + RandomInt(30000, 60000);
								}
							}
							else
							{
								// Roll for a traffic violation every 10 game-seconds when not in combat.
								if (brain->TrafficViolTimer < GameTime &&
								    brain->ThisEnemy == NULL &&
								    ENTITY::DOES_ENTITY_EXIST(brain->ThisVeh))
								{
									brain->TrafficViolTimer = GameTime + 10000;
									float spd = ENTITY::GET_ENTITY_SPEED(brain->ThisVeh);
									// >28 m/s (~100 km/h) = speeding; 3% chance = other violation
									if (spd > 28.0f || RandomInt(1, 100) <= 3)
									{
										Vector3 vPos = ENTITY::GET_ENTITY_COORDS(brain->ThisVeh, true);
										float   hdg  = ENTITY::GET_ENTITY_HEADING(brain->ThisVeh);
										AI::TASK_VEHICLE_PARK(PlayZero, brain->ThisVeh,
										    vPos.x, vPos.y, vPos.z, hdg, 1, 20.0f, false);
										brain->IsPulledOver  = true;
										brain->PulloverTimer = GameTime + RandomInt(40000, 75000);
									}
								}

								if (!brain->IsPulledOver)
								{
									if (brain->ThisEnemy != NULL)
									{
										if (!(bool)ENTITY::DOES_ENTITY_EXIST(brain->ThisEnemy))
										    brain->ThisEnemy = NULL;
										else if ((bool)ENTITY::IS_ENTITY_DEAD(brain->ThisEnemy))
										    brain->ThisEnemy = NULL;
									}
									else if (FindUSeat(brain->ThisVeh, false) != -10)
									{
										PlayerBrain* ThisBrian = FindaPassenger(false);
										if (ThisBrian != nullptr)
											PedDoGetIn(brain->ThisVeh, ThisBrian);
									}
									else if (MySettings.Aggression <= 3 && LessRandomInt("HostHotDrive", 1, 10) < 8)
									{
										// Aggression 2-3: cruise to a hotspot before engaging
										DriveToHotspot(PlayZero, brain->ThisVeh, brain->Friendly);
										brain->FindPlayer = GameTime + 50000;
									}
									else
									{
										if (brain->FindPlayer < GameTime)
										{
											// Low aggression: usually cruise to hotspot instead of fighting
											if (MySettings.Aggression <= 3 && RandomInt(1, 10) < 8)
											{
											    DriveToHotspot(PlayZero, brain->ThisVeh, brain->Friendly);
											    brain->FindPlayer = GameTime + RandomInt(45000, 90000);
											}
											else
											{
											    brain->ThisEnemy = FindAFight(brain);
											    if (brain->ThisEnemy == NULL)
											    {
											        brain->ThisEnemy = PLAYER::PLAYER_PED_ID();
											        PickFight(PlayZero, brain->ThisVeh, brain->ThisEnemy, brain->PrefredVehicle);
											    }
											    else
											        PickFight(PlayZero, brain->ThisVeh, brain->ThisEnemy, brain->PrefredVehicle);

											    brain->FindPlayer = GameTime + RandomInt(30000, 45000);
											}
										}
									}
								}
							}
						}
					}
					else if (brain->Passenger)
					{
						if (!(bool)PED::IS_PED_IN_ANY_VEHICLE(PlayZero, 0))
							brain->Passenger = false;
					}
					else
					{
						// Health-based flee: hostile on-foot NPCs with low HP disengage and run
						if (!brain->Friendly && !brain->Follower && MySettings.Aggression <= 5)
						{
							int iHealth = ENTITY::GET_ENTITY_HEALTH(PlayZero);
							int iMaxHealth = ENTITY::GET_ENTITY_MAX_HEALTH(PlayZero);
							if (iHealth > 0 && iHealth < (iMaxHealth / 3) && brain->FindPlayer < GameTime + 5000)
							{
								AI::CLEAR_PED_TASKS(PlayZero);
								AI::TASK_SMART_FLEE_PED(PlayZero, ThePlayer, 200.0f, -1, false, false);
								PED::SET_PED_KEEP_TASK(PlayZero, true);
								brain->ThisEnemy = NULL;
								brain->FindPlayer = GameTime + RandomInt(20000, 40000);
							}
						}


						// --- POLICE INTERACTION (checked every 10 s) ---
						// Friendly peds surrender; hostile peds resist/attack depending on aggression tier.
						if (!brain->Follower && brain->CopReactionTimer < GameTime)
						{
							brain->CopReactionTimer = GameTime + 10000;
							if ((bool)AI::IS_PED_BEING_ARRESTED(PlayZero))
							{
								if (brain->Friendly)
								{
									// Civilian surrender: raise hands, allow arrest
									AI::CLEAR_PED_TASKS(PlayZero);
									AI::TASK_HANDS_UP(PlayZero, 15000, NULL, -1, false);
									PED::SET_PED_KEEP_TASK(PlayZero, true);
									PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(PlayZero, true);
									brain->FindPlayer = GameTime + 16000;
								}
								else
								{
									// Criminal: fight back or sprint-flee based on zone aggression tier
									AI::CLEAR_PED_TASKS(PlayZero);
									if (brain->AggressionTier >= 3)
									{
										// High-crime zone: attack the arresting officer
										Ped srcCop = PED::_GET_PED_KILLER(PlayZero);
										Ped fightTarget = (srcCop != NULL && (bool)ENTITY::DOES_ENTITY_EXIST(srcCop)) ? srcCop : ThePlayer;
										GreefWar(PlayZero, fightTarget);
										brain->ThisEnemy = fightTarget;
									}
									else
									{
										// Lower zone: flee
										AI::TASK_SMART_FLEE_PED(PlayZero, ThePlayer, 300.0f, -1, false, false);
										PED::SET_PED_KEEP_TASK(PlayZero, true);
										brain->ThisEnemy = NULL;
									}
									brain->FindPlayer = GameTime + RandomInt(20000, 35000);
								}
							}
						}

						// --- FRIENDLY PEDS FLEE NEARBY GUNFIGHTS ---
						// When any hostile PZ ped within 60 m is actively firing, civilians scatter.
						if (brain->Friendly && !brain->Follower && brain->FindPlayer < GameTime + 10000)
						{
							for (int fi = 0; fi < (int)PedList.size(); fi++)
							{
								PlayerBrain& threat = PedList[fi];
								if (!threat.Friendly && !threat.YoDeeeed &&
								    threat.ThisPed != NULL &&
								    DistanceTo(threat.ThisPed, PedPos) < 60.0f &&
								    (bool)PED::IS_PED_SHOOTING(threat.ThisPed))
								{
									AI::CLEAR_PED_TASKS(PlayZero);
									AI::TASK_SMART_FLEE_PED(PlayZero, threat.ThisPed, 200.0f, -1, false, false);
									PED::SET_PED_KEEP_TASK(PlayZero, true);
									brain->ThisEnemy = NULL;
									brain->FindPlayer = GameTime + RandomInt(25000, 45000);
									break;
								}
							}
						}
						if (brain->FindPlayer < GameTime)
						{
							brain->FindPlayer = GameTime + 1000;

							if (brain->ThisEnemy != NULL)
							{
								if (!(bool)ENTITY::DOES_ENTITY_EXIST(brain->ThisEnemy))
									brain->ThisEnemy = NULL;
								else if ((bool)ENTITY::IS_ENTITY_DEAD(brain->ThisEnemy))
									brain->ThisEnemy = NULL;
								else if (DistanceTo((Entity)brain->ThisEnemy, PedPos) > brain->EnemyPos)
								{
									if (MySettings.Aggression > 5)
									{
										brain->EWO = true;
										EasyWayOut(PlayZero);
									}
									else
									{
										brain->ThisEnemy = NULL;
										brain->FindPlayer = GameTime + RandomInt(15000, 20000);
									}
								}
							}
							else
							{
								// Low aggression: fight rarely, wander most of the time
								if (MySettings.Aggression <= 3 && RandomInt(1, 10) < 8)
								{
									brain->FindPlayer = GameTime + RandomInt(60000, 120000);
									PickNextAction(brain);
								}
								else
								{
									brain->FindPlayer = GameTime + RandomInt(60000, 120000);
									brain->ThisEnemy = FindAFight(brain);
									if (brain->ThisEnemy == NULL)
									{
										PickNextAction(brain);
									}
									else
									{
										brain->EnemyPos = DistanceTo((Entity)brain->ThisEnemy, PedPos) - 1.0f;
										GreefWar(PlayZero, brain->ThisEnemy);
									}
								}
							}
						}
						else if ((bool)PED::IS_PED_IN_COMBAT(ThePlayer, PlayZero) && brain->ThisEnemy != ThePlayer)
						{
							brain->ThisEnemy = ThePlayer;
							brain->EnemyPos = DistanceTo((Entity)brain->ThisEnemy, PedPos) + 10.0f;
							GreefWar(PlayZero, brain->ThisEnemy);
						}
					}
				}
			}
		}
	}
}

int AiPedCount = 0;
int AiAppCount = 0;
void PlayerZerosAI()
{
	if (PlayerZinSesh() > MySettings.MaxPlayers)
	{
		if ((int)AFKList.size() > 1)
		{
			if (AFKList[0].TheHacker)
				AFKList[1].TimeOn = 0;
			else
				AFKList[0].TimeOn = 0;

		}
		else if ((int)PedList.size() > 1)
		{
			if (PedList[0].TheHacker)
				PedList[1].TimeOn = 0;
			else
				PedList[0].TimeOn = 0;

		}
	}
	
	if ((int)PedList.size() > 0)
	{
		if (AiPedCount < (int)PedList.size())
		{
			if (PedList[AiPedCount].TimeToGo)
				PedList.erase(PedList.begin() + AiPedCount);
			else
				ProcessPZ(&PedList[AiPedCount]);
			AiPedCount++;
		}
		else
			AiPedCount = 0;
	}

	if ((int)AFKList.size() > 0)
	{
		if (AiAppCount < (int)AFKList.size())
		{
			if (AFKList[AiAppCount].TimeOn < InGameTime())
			{
				if ((bool)UI::DOES_BLIP_EXIST(AFKList[AiAppCount].ThisBlip))
					UI::REMOVE_BLIP(&AFKList[AiAppCount].ThisBlip);

				if (!AFKList[AiAppCount].MoveToOpen)
					GVM::BottomLeft("~h~" + AFKList[AiAppCount].MyName + "~s~ left");
				AFKList.erase(AFKList.begin() + AiAppCount);
			}
			else if (MySettings.BackChat)
				ProcessAfk(&AFKList[AiAppCount]);
			AiAppCount++;
		}
		else
			AiAppCount = 0;
	}

	if (!ClosedSession)
	{
		if (!MySettings.InviteOnly && NewPedTime < InGameTime() && PlayerZinSesh() < MySettings.MaxPlayers)
			NewPlayer();
	}
}

int FindKeyBinds(bool Control)
{
	WAIT(4000);
	int iReturn = -1;
	int TestCase = 0;

	while (true)
	{
		if (PlayDead)
			break;

		GVM::TopLeft(PZTranslate[10]);

		if (Control)
		{
			if (TestCase < (int)ControlSym.size())
			{
				if (GVM::ButtonDown(TestCase))
				{
					GVM::BottomLeft(PZTranslate[11]);
					iReturn = TestCase;
					break;
				}
				TestCase++;
			}
			else
				TestCase = 0;
		}
		else
		{
			if (TestCase < (int)KeyFind.size())
			{
				if (IsKeyDown(KeyFind[TestCase]))
				{
					GVM::BottomLeft(PZTranslate[11]);
					iReturn = TestCase;
					break;
				}
				TestCase++;
			}
			else
				TestCase = 0;
		}
		WAIT(1);
	}
	return iReturn;
}

bool NotWanted = false;

void PartyContactCall(PhoneContact* contact, Vector4 local)
{
	LoggerLight("PartyContactCall");
	if (contact != nullptr)
	{
		if (FileExists(contact->ConMobileAdd))
			FileRemoval(contact->ConMobileAdd);

		contact->InSession = true;
		if (ReteaveBrain(contact->YourFriend.MyIdentity) == -1)
		{
			contact->YourFriend.Follower = true;
			contact->YourFriend.WanBeFriends = false;
			contact->YourFriend.BlipColour = 38;
			contact->YourFriend.TimeOn = (MySettings.MaxSession * 60) * 1000 + InGameTime();
			PedList.push_back(contact->YourFriend);
			Ped Psit = PlayerPedGen(local, &PedList[(int)PedList.size() - 1], true);
		}
	}
}
void ConectContact(PhoneContact* contact)
{
	LoggerLight("ConectContact");
	if (contact != nullptr)
	{
		if (FileExists(contact->ConMobileAdd))
			FileRemoval(contact->ConMobileAdd);

		contact->InSession = true;
		if (ReteaveBrain(contact->YourFriend.MyIdentity) == -1)
		{
			Vector3 LandHere = FowardOf(PLAYER::PLAYER_PED_ID(), 5, true);
			contact->YourFriend.Follower = true;
			contact->YourFriend.WanBeFriends = false;
			contact->YourFriend.BlipColour = 38;
			contact->YourFriend.TimeOn = (MySettings.MaxSession * 60) * 1000 + InGameTime();

			if (contact->YourFriend.PrefredVehicle != 0 && contact->YourFriend.PrefredVehicle < 8)
			{
				contact->YourFriend.ApprochPlayer = true;
				contact->YourFriend.Driver = true;
				PedList.push_back(contact->YourFriend);
				contact->YourFriend.ThisVeh = SpawnVehicle(&PedList[(int)PedList.size() -1], true, false);
			}
			else
			{
				PedList.push_back(contact->YourFriend);
				PlayerPedGen(FindPedSpPoint(), &PedList[(int)PedList.size() - 1], false);
			}
		}
	}
}
bool bSnow = false;
bool HideTrafic = false;

std::vector<std::string> TranlatesTo = {
	"English",
	"French",
	"German",
	"Italian",
	"Spanish",
	"Portuguese",
	"Polish",
	"Russian",
	"Korean",
	"Chinese",
	"Japanese",
	"Mexican",
	"ChineseSimplifyed"
};

void Pz_MenuStart();
void Pz_Settings();

void PassiveProg()
{
	//if (PlayerNoZero != PLAYER::PLAYER_PED_ID())
	//{
	//	PlayerNoZero = PLAYER::PLAYER_PED_ID();
	//	ENTITY::SET_ENTITY_ONLY_DAMAGED_BY_PLAYER((Entity)PLAYER::PLAYER_PED_ID(), true);
	//}
	for (int i = 0; i < (int)PedList.size(); i++)
	{
		Entity PlayVeh = GetPlayersVehicle();
		if (PlayVeh != NULL)
		{
			if (PedList[i].Driver)
			{
				if (PedList[i].ThisVeh != NULL)
				{
					ENTITY::SET_ENTITY_NO_COLLISION_ENTITY(PlayVeh, (Entity)PedList[i].ThisVeh, 1);
					ENTITY::SET_ENTITY_NO_COLLISION_ENTITY((Entity)PedList[i].ThisVeh, PlayVeh, 1);
				}
			}
			else if (PedList[i].ThisPed != NULL)
			{
				ENTITY::SET_ENTITY_NO_COLLISION_ENTITY(PlayVeh, (Entity)PedList[i].ThisPed, 1);
				ENTITY::SET_ENTITY_NO_COLLISION_ENTITY((Entity)PedList[i].ThisPed, PlayVeh, 1);
			}
		}
		else
		{
			if (PedList[i].Driver)
			{
				if (PedList[i].ThisVeh != NULL)
				{
					ENTITY::SET_ENTITY_NO_COLLISION_ENTITY((Entity)PLAYER::PLAYER_PED_ID(), (Entity)PedList[i].ThisVeh, 1);
					ENTITY::SET_ENTITY_NO_COLLISION_ENTITY((Entity)PedList[i].ThisVeh, (Entity)PLAYER::PLAYER_PED_ID(), 1);
				}
			}
			else if (PedList[i].ThisPed != NULL)
			{
				ENTITY::SET_ENTITY_NO_COLLISION_ENTITY((Entity)PLAYER::PLAYER_PED_ID(), (Entity)PedList[i].ThisPed, 1);
				ENTITY::SET_ENTITY_NO_COLLISION_ENTITY((Entity)PedList[i].ThisPed, (Entity)PLAYER::PLAYER_PED_ID(), 1);
			}
		}
	}
}
void PasiveModeSitch()
{
	if (InPasiveMode)
	{
		if (!MySettings.PassiveMode)
		{
			SetRelationType(MySettings.FriendlyFire);
			PLAYER::DISABLE_PLAYER_FIRING(PLAYER::PLAYER_ID(), false);
			InPasiveMode = false;
		}
	}
	else
	{
		if (MySettings.PassiveMode)
		{
			PassiveDontShoot();
			PLAYER::DISABLE_PLAYER_FIRING(PLAYER::PLAYER_ID(), true);
			InPasiveMode = true;
		}
	}
	ReBuildIni();
}
void FriendlyFireSwitch()
{
	SetRelationType(MySettings.FriendlyFire);
}
void AggressionChange()
{
	if (MySettings.Aggression > 4)
	{
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, GP_Player, GP_Attack);
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, GP_Attack, GP_Player);
	}
	else
	{
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(3, GP_Player, GP_Attack);
		PED::SET_RELATIONSHIP_BETWEEN_GROUPS(3, GP_Attack, GP_Player);
	}
}
void ChangeWaitTime(int iTim)
{
	if (iTim == 9)
	{
		if (MySettings.MaxPlayers < MySettings.AirVeh)
			MySettings.AirVeh = MySettings.MaxPlayers;
		Pz_MenuList[(int)Pz_MenuList.size() - 1].Menu_Form[iTim + 1].Max = MySettings.MaxPlayers;
	}
	else if (iTim == 11)
		Pz_MenuList[(int)Pz_MenuList.size() - 1].Menu_Form[iTim + 1].Min = MySettings.MinWait;
	else if (iTim == 12)
		Pz_MenuList[(int)Pz_MenuList.size() - 1].Menu_Form[iTim - 1].Max = MySettings.MaxWait;
	else if (iTim == 13)
		Pz_MenuList[(int)Pz_MenuList.size() - 1].Menu_Form[iTim + 1].Min = MySettings.MinSession;
	else if (iTim == 14)
		Pz_MenuList[(int)Pz_MenuList.size() - 1].Menu_Form[iTim - 1].Max = MySettings.MaxSession;
	else if (iTim == 15)
		Pz_MenuList[(int)Pz_MenuList.size() - 1].Menu_Form[iTim + 1].Min = MySettings.AccMin;
	else if (iTim == 16)
		Pz_MenuList[(int)Pz_MenuList.size() - 1].Menu_Form[iTim - 1].Max = MySettings.AccMax;
}
void MeunLaggOut()
{
	WAIT(1000);
	Pz_MenuList.clear();
	LaggOut(false);
}
void KeysBound(int index)
{
	if (Pz_MenuList.size() > 0)
		Pz_MenuList.pop_back();

	if (index == 0)
		MySettings.Keys_Open_Menu = FindKeyBinds(false);
	else if (index == 1)
		MySettings.Keys_Clear_Session = FindKeyBinds(false);
	else if (index == 2)
		MySettings.Keys_Invite_Only = FindKeyBinds(false);
}
void Pz_KeyBindsKeys()
{
	if (Pz_MenuList.size() > 0)
		Pz_MenuList.pop_back();

	std::vector<GVM::GVMFields> PzMenuz = {
		GVM::GVMFields(PZTranslate[158], "", &KeysBound),
		GVM::GVMFields(PZTranslate[159], "", &KeysBound),
		GVM::GVMFields(PZTranslate[160], "", &KeysBound)
	};
	GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + PZTranslate[148] + "--", PzMenuz);
	Pz_MenuList.push_back(MyMenu);
}
void ControlsBound(int index)
{
	if (Pz_MenuList.size() > 0)
		Pz_MenuList.pop_back();

	if (index == 0)
	{
		MySettings.Control_A_Open_Menu = FindKeyBinds(true);
		MySettings.Control_B_Open_Menu = FindKeyBinds(true);
	}
	else if (index == 1)
	{
		MySettings.Control_A_Clear_Session = FindKeyBinds(true);
		MySettings.Control_B_Clear_Session = FindKeyBinds(true);
	}
	else if (index == 2)
	{
		MySettings.Control_A_Invite_Only = FindKeyBinds(true);
		MySettings.Control_B_Invite_Only = FindKeyBinds(true);
	}
}
void Pz_KeyBindsControl(int index)
{
	if (Pz_MenuList.size() > 0)
		Pz_MenuList.pop_back();

	std::vector<GVM::GVMFields> PzMenuz = {
		GVM::GVMFields(PZTranslate[158], "", &ControlsBound),
		GVM::GVMFields(PZTranslate[159], "", &ControlsBound),
		GVM::GVMFields(PZTranslate[160], "", &ControlsBound)
	};
	GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + PZTranslate[146] + "--", PzMenuz);
	Pz_MenuList.push_back(MyMenu);
}
void MultiControls(int index)
{
	if (Pz_MenuList.size() > 0)
		Pz_MenuList.pop_back();

	if (index == 2)
		MySettings.Control_Keys_Players_List = FindKeyBinds(true);
	else if (index == 3)
		MySettings.Control_Keys_AddPed = FindKeyBinds(true);
	else if (index == 4)
		MySettings.Control_Keys_DissmisPed = FindKeyBinds(true);
	else if (index == 5)
		MySettings.Control_Keys_EnterVeh = FindKeyBinds(true);
}
void Pz_KeyBinds()
{
	std::vector<GVM::GVMFields> PzMenuz = {
		GVM::GVMFields(PZTranslate[146], PZTranslate[147], &Pz_KeyBindsControl),
		GVM::GVMFields(PZTranslate[148], PZTranslate[149], &Pz_KeyBindsKeys),
		GVM::GVMFields(PZTranslate[150], PZTranslate[151], &MultiControls),
		GVM::GVMFields(PZTranslate[152], PZTranslate[153], &MultiControls),
		GVM::GVMFields(PZTranslate[154], PZTranslate[155], &MultiControls),
		GVM::GVMFields(PZTranslate[156], PZTranslate[157], &MultiControls)
	};
	GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + PZTranslate[143] + "--", PzMenuz);
	Pz_MenuList.push_back(MyMenu);
}
void SwapLanguage()
{
	LoadLang(MySettings.Pz_Lang);
	Pz_MenuList.clear();
	Pz_MenuStart();
	Pz_Settings();
}
void SwapingMenuSides()
{
	LoggerLight("SwapingMenuSides, Left == " + BoolToString(MySettings.MenuLeftSide));
	if (MySettings.MenuLeftSide)
		GVM::DefaultRatio.MenuLeftSide();
	else
		GVM::DefaultRatio.MenuRightSide();
}
void Pz_Settings()
{
	std::vector<GVM::GVMFields> PzMenuz = {
		GVM::GVMFields(PZTranslate[109], PZTranslate[110], &MySettings.Aggression, 1, 11, false, &AggressionChange, false),
		GVM::GVMFields(PZTranslate[111], PZTranslate[112], &MySettings.NoNotify),
		GVM::GVMFields(PZTranslate[113], PZTranslate[114], &MySettings.PlayerzBlips),
		GVM::GVMFields(PZTranslate[115], PZTranslate[116], &MySettings.SpaceWeaps),
		GVM::GVMFields(PZTranslate[117], PZTranslate[118], &MySettings.NameTags),
		GVM::GVMFields(PZTranslate[119], PZTranslate[120], &MySettings.PassiveMode),
		GVM::GVMFields(PZTranslate[121], PZTranslate[122], &MySettings.FriendlyFire, &FriendlyFireSwitch, true),
		GVM::GVMFields(PZTranslate[123], PZTranslate[124], &MySettings.BackChat),
		GVM::GVMFields(PZTranslate[125], PZTranslate[126], &MySettings.Debugger),
		GVM::GVMFields(PZTranslate[127], PZTranslate[128], &MySettings.MaxPlayers, 5, 50, false, &ChangeWaitTime, true, true),
		GVM::GVMFields(PZTranslate[129], PZTranslate[130], &MySettings.AirVeh, 0, MySettings.MaxPlayers, false),
		GVM::GVMFields(PZTranslate[131], PZTranslate[132], &MySettings.MinWait, 1, MySettings.MaxWait, false,& ChangeWaitTime, true, true),
		GVM::GVMFields(PZTranslate[133], PZTranslate[134], &MySettings.MaxWait, MySettings.MinWait, 240, false, &ChangeWaitTime, true, true),
		GVM::GVMFields(PZTranslate[135], PZTranslate[136], &MySettings.MinSession, 1, MySettings.MaxSession, false, &ChangeWaitTime, true, true),
		GVM::GVMFields(PZTranslate[137], PZTranslate[138], &MySettings.MaxSession, MySettings.MinSession, 1000, false, &ChangeWaitTime, true, true),
		GVM::GVMFields(PZTranslate[139], PZTranslate[140], &MySettings.AccMin, 1, MySettings.AccMax, false, &ChangeWaitTime, true, true),
		GVM::GVMFields(PZTranslate[141], PZTranslate[142], &MySettings.AccMax, MySettings.AccMin, 100, false, &ChangeWaitTime, true, true),
		GVM::GVMFields(PZTranslate[42], PZTranslate[43], &MySettings.MenuLeftSide, &SwapingMenuSides, true),
		GVM::GVMFields(PZTranslate[143], PZTranslate[144], &Pz_KeyBinds),
		GVM::GVMFields(PZTranslate[145], TranlatesTo[MySettings.Pz_Lang], TranlatesTo, &MySettings.Pz_Lang, &SwapLanguage, false, false)
	};
	GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + PZTranslate[36] + "--", PzMenuz, &PasiveModeSitch, true);
	Pz_MenuList.push_back(MyMenu);
}

int iVehList = 0;
void Pz_PickFaveVeh(void* obj, void* obj2)
{
	LoggerLight("ChangeFaveVeh");
	PhoneContact* Friend = static_cast<PhoneContact*>(obj);
	std::string* Vic = static_cast<std::string*>(obj2);
	if (Friend != nullptr && Vic != nullptr)
	{
		Friend->YourFriend.FaveVehicle = *Vic;
		Friend->YourFriend.PrefredVehicle = iVehList;
		SaveContacts(*Friend);
		if (Pz_MenuList.size() > 1)
		{
			Pz_MenuList.pop_back();
			Pz_MenuList.pop_back();
		}
	}
}
void Pz_PickFaveVeh(int index, void* obj)
{
	LoggerLight("ChangeFaveVeh");
	PhoneContact* Friend = static_cast<PhoneContact*>(obj);
	if (Friend != nullptr)
	{
		iVehList = index;
		std::vector<GVM::GVMFields> PzMenuz = {};
		if (index == 0)
		{
			if (Pz_MenuList.size() > 0)
				Pz_MenuList.pop_back();

			Friend->YourFriend.FaveVehicle = "";
			Friend->YourFriend.PrefredVehicle = 0;	
			SaveContacts(*Friend);
		}
		else
		{
			if (index == 1)
			{
				for (int i = 0; i < (int)PreVeh_01.size(); i++)
					PzMenuz.push_back(GVM::GVMFields(PreVeh_01[i], GetVehName(PreVeh_01[i]), &Pz_PickFaveVeh, obj, &PreVeh_01[i]));
			}
			else if (index == 2)
			{
				for (int i = 0; i < (int)PreVeh_02.size(); i++)
					PzMenuz.push_back(GVM::GVMFields(PreVeh_02[i], GetVehName(PreVeh_02[i]), &Pz_PickFaveVeh, obj, &PreVeh_02[i]));
			}
			else if (index == 3)
			{
				for (int i = 0; i < (int)PreVeh_03.size(); i++)
					PzMenuz.push_back(GVM::GVMFields(PreVeh_03[i], GetVehName(PreVeh_03[i]), &Pz_PickFaveVeh, obj, &PreVeh_03[i]));
			}
			else if (index == 4)
			{
				for (int i = 0; i < (int)PreVeh_04.size(); i++)
					PzMenuz.push_back(GVM::GVMFields(PreVeh_04[i], GetVehName(PreVeh_04[i]), &Pz_PickFaveVeh, obj, &PreVeh_04[i]));
			}
			else if (index == 5)
			{
				for (int i = 0; i < (int)PreVeh_05.size(); i++)
					PzMenuz.push_back(GVM::GVMFields(PreVeh_05[i], GetVehName(PreVeh_05[i]), &Pz_PickFaveVeh, obj, &PreVeh_05[i]));
			}
			else
			{
				iVehList = 6;
				for (int i = 0; i < (int)PreVeh_06.size(); i++)
					PzMenuz.push_back(GVM::GVMFields(PreVeh_06[i], GetVehName(PreVeh_06[i]), &Pz_PickFaveVeh, obj, &PreVeh_06[i]));
			}
			GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + Friend->Name + "--", PzMenuz);
			Pz_MenuList.push_back(MyMenu);
		}
	}
}
void Pz_ChangeFaveVeh(void* obj)
{
	LoggerLight("ChangeFaveVeh");
	PhoneContact* Friend = static_cast<PhoneContact*>(obj);
	if (Friend != nullptr)
	{
		std::vector<GVM::GVMFields> PzMenuz = {
			GVM::GVMFields(PZTranslate[102], Friend->YourFriend.FaveVehicle, &Pz_PickFaveVeh, obj),
			GVM::GVMFields(PZTranslate[103], Friend->YourFriend.FaveVehicle, &Pz_PickFaveVeh, obj),
			GVM::GVMFields(PZTranslate[104], Friend->YourFriend.FaveVehicle, &Pz_PickFaveVeh, obj),
			GVM::GVMFields(PZTranslate[105], Friend->YourFriend.FaveVehicle, &Pz_PickFaveVeh, obj),
			GVM::GVMFields(PZTranslate[106], Friend->YourFriend.FaveVehicle, &Pz_PickFaveVeh, obj),
			GVM::GVMFields(PZTranslate[107], Friend->YourFriend.FaveVehicle, &Pz_PickFaveVeh, obj),
			GVM::GVMFields(PZTranslate[108], Friend->YourFriend.FaveVehicle, &Pz_PickFaveVeh, obj)
		};
		GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + Friend->Name  + "--", PzMenuz);
		Pz_MenuList.push_back(MyMenu);
	}
}
void DeleteContact(void* obj)
{
	LoggerLight("DeleteContact");
	PhoneContact* Friend = static_cast<PhoneContact*>(obj);
	if (Friend != nullptr)
	{
		FileRemoval(DirectContacts + "/" + Friend->Name + ".ini");
		LoadContacts();
		Pz_MenuList.clear();
	}
}
void RenamingContact(void* obj)
{
	LoggerLight("RenamingContact");
	PhoneContact* Friend = static_cast<PhoneContact*>(obj);
	if (Friend != nullptr)
	{
		std::string nameIt = GVM::CaptureScreenText(PZTranslate[27]);
		if (nameIt != "")
		{
			FileRemoval(DirectContacts + "/" + Friend->Name + ".ini");
			Friend->Name = nameIt;
			Friend->YourFriend.MyName = nameIt;
			SaveContacts(*Friend);
			int iBe = ReteaveBrain(Friend->YourFriend.MyIdentity);
			if (iBe > 0 && iBe < (int)PedList.size())
				PedList[iBe].MyName = nameIt;
		}
		if (Pz_MenuList.size() > 0)
			Pz_MenuList[(int)Pz_MenuList.size() -1].Banner_Title = "--" + Friend->Name + "--";
	}
}
void AddMobileContact(void* obj)
{
	LoggerLight("AddMobileContact");
	PhoneContact* Friend = static_cast<PhoneContact*>(obj);
	if (Friend != nullptr)
		SaveContacts(*Friend);

	AddPhoneFriends();
}
void CallupContact(void* obj)
{
	LoggerLight("CallupContact");
	PhoneContact* myCon = static_cast<PhoneContact*>(obj);
	if (myCon != nullptr)
	{
		ConectContact(myCon);
		Pz_MenuList.clear();
	}
}
void Pz_ContactEdit(void* obj)
{
	LoggerLight("Pz_ContactEdit");
	PhoneContact* Friend = static_cast<PhoneContact*>(obj);
	if (Friend != nullptr)
	{
		if (Pz_MenuList.size() > 0)
			Pz_MenuList.pop_back();

		if (Friend->InSession)
		{
			std::vector<GVM::GVMFields> PzMenuz = {
				GVM::GVMFields(PZTranslate[27], PZTranslate[95], &RenamingContact, obj),
				GVM::GVMFields(PZTranslate[96], PZTranslate[97], &DeleteContact, obj),
				GVM::GVMFields(PZTranslate[98], PZTranslate[99], &Pz_ChangeFaveVeh, obj),
				GVM::GVMFields(PZTranslate[90], PZTranslate[91], &Friend->YourFriend.IsMobileCont, &AddMobileContact, obj, true)
			};
			GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + Friend->Name + "--", PzMenuz);
			Pz_MenuList.push_back(MyMenu);
		}
		else
		{
			std::vector<GVM::GVMFields> PzMenuz = {
				GVM::GVMFields(PZTranslate[27], PZTranslate[95], &RenamingContact, obj),
				GVM::GVMFields(PZTranslate[96], PZTranslate[97], &DeleteContact, obj),
				GVM::GVMFields(PZTranslate[98], PZTranslate[99], &Pz_ChangeFaveVeh, obj),
				GVM::GVMFields(PZTranslate[100], PZTranslate[101], &CallupContact, obj),
				GVM::GVMFields(PZTranslate[90], PZTranslate[91], &Friend->YourFriend.IsMobileCont, &AddMobileContact, obj, true)
			};
			GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + Friend->Name + "--", PzMenuz);
			Pz_MenuList.push_back(MyMenu);
		}
	}
}
void Pz_ContactList()
{
	LoadContacts();
	std::vector<GVM::GVMFields> PzMenuz = {};
	for (int i = 0; i < (int)PhoneContacts.size(); i++)
		PzMenuz.push_back(GVM::GVMFields(PhoneContacts[i].Name, "", &Pz_ContactEdit, &PhoneContacts[i]));

	if (PzMenuz.size() == 0)
		PzMenuz.push_back(GVM::GVMFields(PZTranslate[93], ""));
	GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + PZTranslate[92] + "--", PzMenuz);
	Pz_MenuList.push_back(MyMenu);
}
void AddNewContact(void* obj)
{
	LoggerLight("AddNewContact");
	PlayerBrain* Friend = static_cast<PlayerBrain*>(obj);
	if (Friend != nullptr)
	{
		std::string ConMobAdd = DirectMobileNet + "/PzContacts" + Friend->MyName + ".ini";
		std::string ConRetAdd = DirectMain + "/PzContacts" + Friend->MyName + ".ini";

		Friend->IsInContacts = true;
		PhoneContact NewCon = PhoneContact(Friend->MyName, ConMobAdd, ConRetAdd, *Friend);
		NewCon.InSession = true;
		SaveContacts(NewCon);
		if (Pz_MenuList.size() > 0)
			Pz_MenuList.pop_back();
	}
}
void Pz_AddNewContact()
{
	std::vector<GVM::GVMFields> PzMenuz = {};

	for (int i = 0; i < (int)PedList.size(); i++)
	{
		if (PedList[i].Follower && !PedList[i].IsInContacts)
			PzMenuz.push_back(GVM::GVMFields(PedList[i].MyName, "", &AddNewContact, &PedList[i]));
	}
	if (PzMenuz.size() == 0)
		PzMenuz.push_back(GVM::GVMFields(PZTranslate[92], ""));

	GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + PZTranslate[88] + "--", PzMenuz);
	Pz_MenuList.push_back(MyMenu);
}
void Pz_Contacts()
{
	std::vector<GVM::GVMFields> PzMenuz = {
		GVM::GVMFields(PZTranslate[86], PZTranslate[87], &Pz_ContactList),
		GVM::GVMFields(PZTranslate[88], PZTranslate[89], &Pz_AddNewContact)
		//GVM::GVMFields(PZTranslate[90], PZTranslate[91], &MySettings.MobileData)
	};
	GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + PZTranslate[34] + "--", PzMenuz);
	Pz_MenuList.push_back(MyMenu);
}

void Pz_TrollPed(void* obj);

void HaCK001(void* obj)
{
	PlayerBrain* Friend = static_cast<PlayerBrain*>(obj);
	if (Friend != nullptr)
	{
		Pz_MenuList.clear();
		Friend->Driver = false;
		Friend->Passenger = false;
		Friend->IsPlane = false;
		Friend->HackReaction = true;
		Vector3 LandHere = FowardOf(PLAYER::PLAYER_PED_ID(), 5, true);
		MoveEntity(Friend->ThisPed, LandHere);
		Pz_TrollPed(obj);
	}
}
void HaCK002(void* obj)
{
	PlayerBrain* Friend = static_cast<PlayerBrain*>(obj);
	if (Friend != nullptr)
	{
		Pz_MenuList.clear();
		FIRE::START_ENTITY_FIRE(Friend->ThisPed);
		Friend->HackReaction = true;
		FightPlayer(Friend);
	}
}
void HaCK003(void* obj)
{
	PlayerBrain* Friend = static_cast<PlayerBrain*>(obj);
	if (Friend != nullptr)
	{
		Pz_MenuList.clear();
		Friend->TimeOn = 0;
	}
}
void HaCK004(void* obj)
{
	PlayerBrain* Friend = static_cast<PlayerBrain*>(obj);
	if (Friend != nullptr)
	{
		Pz_MenuList.clear();
		Vector3 Above = ENTITY::GET_ENTITY_COORDS(Friend->ThisPed, true);
		Above.z += 25.0;
		Friend->HackReaction = true;
		DropObjects(Above);
	}
}
void HaCK005(void* obj)
{
	PlayerBrain* Friend = static_cast<PlayerBrain*>(obj);
	if (Friend != nullptr)
	{
		Pz_MenuList.clear();
		FireOrb(nullptr, Friend);
		Friend->HackReaction = true;
		FightPlayer(Friend);
	}
}
void HaCK006(void* obj)
{
	PlayerBrain* Friend = static_cast<PlayerBrain*>(obj);
	if (Friend != nullptr)
	{
		int iAmPig = BackOffPig();
		Ped ThePlayer = PLAYER::PLAYER_PED_ID();

		if (Friend->PiggyBackin)
		{
			ENTITY::DETACH_ENTITY(ThePlayer, 0, 0);
			AI::CLEAR_PED_TASKS_IMMEDIATELY(ThePlayer);
			Friend->PiggyBackin = false;
			Friend->HackReaction = false;
		}
		else
		{
			if (iAmPig != -1)
			{
				ENTITY::DETACH_ENTITY(ThePlayer, 0, 0);
				AI::CLEAR_PED_TASKS_IMMEDIATELY(ThePlayer);
				PedList[iAmPig].PiggyBackin = false;
			}

			ForceAnim(ThePlayer, "amb@code_human_in_bus_passenger_idles@female@sit@idle_a", "idle_a", PlayerPosi(), NewVector3(0.0, 0.0, 0.0));
			ENTITY::ATTACH_ENTITY_TO_ENTITY(ThePlayer, Friend->ThisPed, 31086, 0.10, 0.15, 0.61, 0.00, 0.00, 180.00, 0, 0, 0, 0, 2, 1);
			Friend->PiggyBackin = true;
			Friend->HackReaction = true;
		}
	}
}
void HaCK007(void* obj)
{
	PlayerBrain* Friend = static_cast<PlayerBrain*>(obj);
	if (Friend != nullptr)
	{
		if (Friend->ThisPed != NULL)
		{
			Friend->DropMoneyBags = true;
			Pz_MenuList.clear();
		}
	}
}
void HaCK008(void* obj)
{
	PlayerBrain* Friend = static_cast<PlayerBrain*>(obj);
	if (Friend != nullptr)
	{
		Pz_MenuList.clear();
		PedBlimp(&Friend->ThisBlip, Friend->ThisPed, 303, Friend->MyName, 1, false);
		Friend->Bounty = true;
	}
}
void HaCK009(void* obj)
{
	PlayerBrain* Friend = static_cast<PlayerBrain*>(obj);
	if (Friend != nullptr)
	{
		Pz_MenuList.clear();
		Friend->Driver = false;
		Friend->Passenger = false;
		Friend->HackReaction = true;
		if (Friend->ThisVeh != NULL && Friend->ThisPed != NULL)
			GetOutVehicle(Friend->ThisPed);
	}
}
void HaCK010(void* obj)
{
	PlayerBrain* Friend = static_cast<PlayerBrain*>(obj);
	if (Friend != nullptr)
	{
		if (Friend->ThisVeh != NULL)
		{
			if (HasASeat(Friend->ThisVeh))
			{
				Pz_MenuList.clear();
				GetOutVehicle(PLAYER::PLAYER_PED_ID());

				if (Friend->Follower)
					FollowOn(Friend, true, false);
				else
					GetInVehicle(PLAYER::PLAYER_PED_ID(), Friend->ThisVeh, 0);
			}
			else
				GetInVehicle(PLAYER::PLAYER_PED_ID(), Friend->ThisVeh, -1);
		}
	}
}
void HaCK011(void* obj)
{
	PlayerBrain* Friend = static_cast<PlayerBrain*>(obj);
	if (Friend != nullptr)
	{
		if (Friend->XmasTree != NULL)
		{
			ENTITY::DELETE_ENTITY(&Friend->XmasTree);
			Friend->HackReaction = false;
			Friend->XmasTree = NULL;
		}
		else if (Friend->ThisPed != NULL)
		{
			Vector3 Vme = ENTITY::GET_ENTITY_COORDS(Friend->ThisPed, 0);
			Friend->XmasTree = BuildProps("prop_xmas_tree_int", Vme, NewVector3(0.0, 90.0, 0.0), false);
			ENTITY::SET_ENTITY_COLLISION(Friend->XmasTree, false, false);
			ENTITY::ATTACH_ENTITY_TO_ENTITY(Friend->XmasTree, Friend->ThisPed, PED::GET_PED_BONE_INDEX(Friend->ThisPed, 24818), 0.0, 0.0, 0.0, 0.0, 90.0, 0.0, 0, 0, 0, 1, 2, 1);
			Friend->HackReaction = true;
		}
	}
}
void HaCK012(void* obj)
{
	PlayerBrain* Friend = static_cast<PlayerBrain*>(obj);
	if (Friend != nullptr)
	{
		Pz_MenuList.clear();
		if (Friend->ThisPed != NULL)
		{
			PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(Friend->ThisPed, true);
			Friend->EWO = true;
			EasyWayOut(Friend->ThisPed);

			Friend->HackReaction = true;
			FightPlayer(Friend);
		}
	}
}
void LeaveYourApp(int index, void* obj)
{
	AfkPlayer* Friend = static_cast<AfkPlayer*>(obj);
	if (Friend != nullptr)
	{
		if (index == 0)
		{
			int iPedLocate = ReteaveAfk(Friend->MyIdentity);

			int PZSetMinSession = (MySettings.MinSession * 60) * 1000;
			int PZSetMaxSession = (MySettings.MaxSession * 60) * 1000;

			ClothBank MB = NewClothBank();
			int iNat = RandomInt(1, 5);
			int iGetGunnin = YourGunNum();
			Vector3 LandHere = FowardOf(PLAYER::PLAYER_PED_ID(), 5, true);
			PlayerBrain newBrain = PlayerBrain(Friend->MyName, Friend->MyIdentity, MB, InGameTime() + RandomInt(PZSetMinSession, PZSetMaxSession), Friend->Level, false, false, iNat, iGetGunnin, 0, "", -1);
			PedList.push_back(newBrain);
			PlayerPedGen(Vector4(LandHere.x, LandHere.y, LandHere.z, 0.0), &PedList[PedList.size() - 1], false);
			AFKList[iPedLocate].MoveToOpen = true;
			AFKList[iPedLocate].TimeOn = 0;
		}
		else if (index == 3)
			LocalBlip(&Friend->ThisBlip, AFKPlayers[Friend->App], 303, Friend->MyName, 1);
		else if (index == 4)
		{
			GVM::BottomLeft(Friend->MyName + PZTranslate[25]);
			Friend->TimeOn = 0;
		}
		else
			Friend->TimeOn = 0;
	}

}
void Pz_TrollPed(void* obj)
{
	LoggerLight("Pz_TrollThis");
	PlayerBrain* Friend = static_cast<PlayerBrain*>(obj);
	if (Friend != nullptr)
	{
		if (Pz_MenuList.size() > 0)
			Pz_MenuList.pop_back();

		if (Friend->ThisPed == NULL)
		{
			std::vector<GVM::GVMFields> PzMenuz = { GVM::GVMFields(PZTranslate[161], "") };
			GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + PZTranslate[44] + "--", PzMenuz);
			Pz_MenuList.push_back(MyMenu);
		}
		else if (Friend->YoDeeeed)
		{
			std::vector<GVM::GVMFields> PzMenuz = { GVM::GVMFields(PZTranslate[161], "") };
			GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + PZTranslate[44] + "--", PzMenuz);
			Pz_MenuList.push_back(MyMenu);
		}
		else if (Friend->Driver)
		{
			std::vector<GVM::GVMFields> PzMenuz = {
				GVM::GVMFields(PZTranslate[62], PZTranslate[63], &HaCK001, obj),//HaCK001(ThisPed);//"Bring Player to Self",
				GVM::GVMFields(PZTranslate[64], PZTranslate[65], &HaCK002, obj),//HaCK002(ThisPed);//"Burn Player",
				GVM::GVMFields(PZTranslate[66], PZTranslate[67], &HaCK003, obj),//HaCK003(ThisPed);//"Crash Player",
				GVM::GVMFields(PZTranslate[68], PZTranslate[69], &HaCK004, obj),//HaCK004(ThisPed);//"Drop Objects On Player",
				GVM::GVMFields(PZTranslate[76], PZTranslate[77], &HaCK008, obj),//HaCK008(ThisPed);//"Add Bounty to Player",
				GVM::GVMFields(PZTranslate[78], PZTranslate[79], &HaCK009, obj),//HaCK009(ThisPed);//"Eject Player Vehicle",
				GVM::GVMFields(PZTranslate[80], PZTranslate[81], &HaCK010, obj),//HaCK010(ThisPed);//"Enter Player Vehicle",
				GVM::GVMFields(PZTranslate[82], PZTranslate[83], &HaCK011, obj),//HaCK011(ThisPed);//"Add Christmas Tree",
				GVM::GVMFields(PZTranslate[84], PZTranslate[85], &HaCK012, obj),//HaCK012(ThisPed);//"EWO",*/
			};
			GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + PZTranslate[44] + "--", PzMenuz);
			Pz_MenuList.push_back(MyMenu);
		}
		else if (Friend->Passenger)
		{
			std::vector<GVM::GVMFields> PzMenuz = {
				GVM::GVMFields(PZTranslate[62], PZTranslate[63], &HaCK001, obj),//HaCK001(ThisPed);//"Bring Player to Self",
				GVM::GVMFields(PZTranslate[64], PZTranslate[65], &HaCK002, obj),//HaCK002(ThisPed);//"Burn Player",
				GVM::GVMFields(PZTranslate[66], PZTranslate[67], &HaCK003, obj),//HaCK003(ThisPed);//"Crash Player",
				GVM::GVMFields(PZTranslate[68], PZTranslate[69], &HaCK004, obj),//HaCK004(ThisPed);//"Drop Objects On Player",
				GVM::GVMFields(PZTranslate[76], PZTranslate[77], &HaCK008, obj),//HaCK008(ThisPed);//"Add Bounty to Player",
				GVM::GVMFields(PZTranslate[78], PZTranslate[79], &HaCK009, obj),//HaCK009(ThisPed);//"Eject Player Vehicle",
				GVM::GVMFields(PZTranslate[82], PZTranslate[83], &HaCK011, obj),//HaCK011(ThisPed);//"Add Christmas Tree",
				GVM::GVMFields(PZTranslate[84], PZTranslate[85], &HaCK012, obj),//HaCK012(ThisPed);//"EWO",*/
			};
			GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + PZTranslate[44] + "--", PzMenuz);
			Pz_MenuList.push_back(MyMenu);
		}
		else
		{
			std::vector<GVM::GVMFields> PzMenuz = {
				GVM::GVMFields(PZTranslate[62], PZTranslate[63], &HaCK001, obj),//HaCK001(ThisPed);//"Bring Player to Self",
				GVM::GVMFields(PZTranslate[64], PZTranslate[65], &HaCK002, obj),//HaCK002(ThisPed);//"Burn Player",
				GVM::GVMFields(PZTranslate[66], PZTranslate[67], &HaCK003, obj),//HaCK003(ThisPed);//"Crash Player",
				GVM::GVMFields(PZTranslate[68], PZTranslate[69], &HaCK004, obj),//HaCK004(ThisPed);//"Drop Objects On Player",
				GVM::GVMFields(PZTranslate[70], PZTranslate[71], &HaCK005, obj),//HaCK005(ThisPed);//"Orbital Strike Player",
				GVM::GVMFields(PZTranslate[72], PZTranslate[73], &HaCK006, obj),//HaCK006(ThisPed);//"PiggyBack Player",
				GVM::GVMFields(PZTranslate[74], PZTranslate[75], &HaCK007, obj),//HaCK007(ThisPed);//"Drop Moneys on Player",
				GVM::GVMFields(PZTranslate[76], PZTranslate[77], &HaCK008, obj),//HaCK008(ThisPed);//"Add Bounty to Player",
				GVM::GVMFields(PZTranslate[82], PZTranslate[83], &HaCK011, obj),//HaCK011(ThisPed);//"Add Christmas Tree",
				GVM::GVMFields(PZTranslate[84], PZTranslate[85], &HaCK012, obj),//HaCK012(ThisPed);//"EWO",*/
			};
			GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + PZTranslate[44] + "--", PzMenuz);
			Pz_MenuList.push_back(MyMenu);
		}
	}
}
void Pz_TrollAdd(void* obj)
{
	LoggerLight("Pz_TrollThis");
	AfkPlayer* Friend = static_cast<AfkPlayer*>(obj);
	if (Friend != nullptr)
	{
		if (Pz_MenuList.size() > 0)
			Pz_MenuList.pop_back();
		std::vector<GVM::GVMFields> PzMenuz = {
			GVM::GVMFields(PZTranslate[62], PZTranslate[63], &LeaveYourApp, obj),//HaCK001(ThisPed);//"Bring Player to Self",
			GVM::GVMFields(PZTranslate[64], PZTranslate[65], &LeaveYourApp, obj),//HaCK002(ThisPed);//"Burn Player",
			GVM::GVMFields(PZTranslate[66], PZTranslate[67], &LeaveYourApp, obj),//HaCK003(ThisPed);//"Crash Player",
			GVM::GVMFields(PZTranslate[76], PZTranslate[77], &LeaveYourApp, obj),//HaCK008(ThisPed);//"Add Bounty to Player",
			GVM::GVMFields(PZTranslate[84], PZTranslate[85], &LeaveYourApp, obj)//HaCK012(ThisPed);//"EWO",*/
		};
		GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + PZTranslate[44] + "--", PzMenuz);
		Pz_MenuList.push_back(MyMenu);			
	}
}
void Pz_TrollPlayerz()
{
	std::vector<GVM::GVMFields> PzMenuz = {};

	for (int i = 0; i < PedList.size(); i++)
		PzMenuz.push_back(GVM::GVMFields(PedList[i].MyName, "", &Pz_TrollPed, &PedList[i]));

	for (int i = 0; i < AFKList.size(); i++)
		PzMenuz.push_back(GVM::GVMFields(AFKList[i].MyName, "", &Pz_TrollAdd, &AFKList[i]));

	if (PzMenuz.size() == 0)
		PzMenuz.push_back(GVM::GVMFields(PZTranslate[61], ""));

	GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + PZTranslate[44] + "--", PzMenuz);
	Pz_MenuList.push_back(MyMenu);
}
void BurnPlayers()
{
	for (int i = 0; i < (int)PedList.size(); i++)
	{
		if (!PedList[i].Follower)
			HaCK002(&PedList[i]);
		if (DistanceTo(PedList[i].ThisPed, PLAYER::PLAYER_PED_ID()) > 70.0)
			ENTITY::SET_ENTITY_HEALTH(PedList[i].ThisPed, 0);
	}

	for (int i = 0; i < (int)AFKList.size(); i++)
	{
		GVM::BottomLeft("~h~" + AFKList[i].MyName + "~s~ " + PZTranslate[28]);
		AFKList[i].TimeOn = 0;
	}
}
void CashFlow()
{
	for (int i = 0; i < (int)PedList.size(); i++)
		HaCK007(&PedList[i]);
}
void GetingOffHere()
{
	for (int i = 0; i < (int)PedList.size(); i++)
	{
		PedList[i].Driver = false;
		PedList[i].Passenger = false;
		if (PedList[i].ThisPed != NULL)
			GetOutVehicle(PedList[i].ThisPed);
	}
}

bool ItsSnowing = false;
void SnowTime()
{
	if (ItsSnowing)
	{
		AccessLetItSnowType(true);
		if (WEAPON::HAS_PED_GOT_WEAPON(PLAYER::PLAYER_PED_ID(), MyHashKey("WEAPON_snowball"), false))
			WEAPON::GIVE_WEAPON_TO_PED(PLAYER::PLAYER_PED_ID(), MyHashKey("WEAPON_snowball"), 9999, false, true);
	}
	else
	{
		AccessLetItSnowType(false);
		if (WEAPON::HAS_PED_GOT_WEAPON(PLAYER::PLAYER_PED_ID(), MyHashKey("WEAPON_snowball"), false))
			WEAPON::REMOVE_WEAPON_FROM_PED(PLAYER::PLAYER_PED_ID(), MyHashKey("WEAPON_snowball"));
	}
}
void PlayerEWO()
{
	Pz_MenuList.clear();
	EasyWayOut(PLAYER::PLAYER_PED_ID());
	GVM::BottomLeft(PZTranslate[26]);
}
void Pz_TrollMenu()
{
	ItsSnowing = AccessSnowFallType();
	std::vector<GVM::GVMFields> PzMenuz = {
		GVM::GVMFields(PZTranslate[45], PZTranslate[46], &Pz_TrollPlayerz),
		GVM::GVMFields(PZTranslate[47], PZTranslate[48], &BurnPlayers),
		GVM::GVMFields(PZTranslate[49], PZTranslate[50], &CashFlow),
		GVM::GVMFields(PZTranslate[51], PZTranslate[52], &GetingOffHere),
		GVM::GVMFields(PZTranslate[53], PZTranslate[54], &GotWindMill , &EclipsWindMill, true),
		GVM::GVMFields(PZTranslate[55], PZTranslate[56], &NotWanted),
		GVM::GVMFields(PZTranslate[57], PZTranslate[58], &ItsSnowing),
		GVM::GVMFields(PZTranslate[59], PZTranslate[60], &PlayerEWO)
	};
	GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + PZTranslate[32] + "--", PzMenuz, &SnowTime, false);
	Pz_MenuList.push_back(MyMenu);
}

void SavePzSetts()
{
	ReBuildIni();
	if (MySettings.InviteOnly)
		LaggOut(true);
}

void Pz_MenuStart()
{
	std::vector<GVM::GVMFields> PzMenuz = {
		GVM::GVMFields(PZTranslate[32], PZTranslate[33], &Pz_TrollMenu),
		GVM::GVMFields(PZTranslate[34], PZTranslate[35], &Pz_Contacts),
		GVM::GVMFields(PZTranslate[36], PZTranslate[37], &Pz_Settings),
		GVM::GVMFields(PZTranslate[38], PZTranslate[39], &MeunLaggOut),
		GVM::GVMFields(PZTranslate[40], PZTranslate[41], &MySettings.InviteOnly)
	};
	GVM::GVMSystem MyMenu = GVM::GVMSystem("--" + PZTranslate[44] + "--", PzMenuz, &SavePzSetts, true);
	Pz_MenuList.push_back(MyMenu);
}

int GotHacked = 1;
int OrigLang = 0;
Prop PlayTree = NULL;

void GotHacked006()
{
	GotHacked++;
}
void GotHacked005()
{
	Pz_MenuList.clear();
	GotHacked++;
	ItsSnowing = false;
	SnowTime();
	MySettings.Pz_Lang = OrigLang;
	LoadLang(MySettings.Pz_Lang);
	MeunLaggOut();
	HackAttacks = false;
}
void GotHacked004()
{
	Pz_MenuList.clear();
	int MoreHacks = LessRandomInt("MissingCat", 1, 5);
	if (MoreHacks == 1)
	{
		Vector3 PlPos = EntityPosition(PLAYER::PLAYER_PED_ID());
		MoveEntity(PLAYER::PLAYER_PED_ID(), NewVector3(PlPos.x, PlPos.y, PlPos.z + 400.0f));
	}
	else if (MoreHacks == 2)
	{
		int iPick = -1;
		for (int i = 0; i < (int)PedList.size(); i++)
		{
			if (PedList[i].Driver && PedList[i].ThisVeh != NULL)
				iPick = i;
		}
		if (iPick > 0)
			HaCK010(&PedList[iPick]);
		else
			DropObjects(EntityPosition(PLAYER::PLAYER_PED_ID()));
	}
	else if (MoreHacks == 3)
	{
		FIRE::START_ENTITY_FIRE(PLAYER::PLAYER_PED_ID());
	}
	else if (MoreHacks == 4)
	{
		MoneyBags Mbags = MoneyBags();

		while (Mbags.BagsDroped < 10)
		{
			if (Mbags.BagTimer < InGameTime())
			{
				Mbags.BagsDroped++;
				Mbags.BagTimer = InGameTime() + 500;
				Vector4 AboutHere = InAreaOf(ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true), 0.01f, 4.0f);
				Prop MoreBag = BuildProps(Mbags.Bags, NewVector3(AboutHere.X, AboutHere.Y, AboutHere.Z + 5.0f), NewVector3(0.0, 0.0, 0.0), true);
				Mbags.TheseBags.push_back(MoreBag);
			}
			for (int i = 0; i < (int)Mbags.TheseBags.size(); i++)
			{
				if (DistanceTo(PLAYER::PLAYER_PED_ID(), Mbags.TheseBags[i]) < 1.25f)
				{
					if ((bool)ENTITY::DOES_ENTITY_EXIST(Mbags.TheseBags[i]))
						ENTITY::DELETE_ENTITY(&Mbags.TheseBags[i]);
				}
			}
			WAIT(0);
		}
		WAIT(2000);
		
		TakeAwayYourMoney(&Mbags);
	}
	else if (MoreHacks == 5)
	{
		PLAYER::SET_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID(), 5, true);
		PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(PLAYER::PLAYER_ID(), true);
	}

	if (HackerIsInSession)
	{
		GotHacked++;
	}
}
void GotHacked003()
{
	Pz_MenuList.clear();
	PlayerEWO();
}
void GotHacked002()
{
	Pz_MenuList.clear();

	if (PlayTree != NULL)
	{
		ENTITY::DELETE_ENTITY(&PlayTree);
		PlayTree = NULL;
		GotHacked++;
	}
	else if (PlayTree == NULL)
	{
		ItsSnowing = true;
		SnowTime();
		Vector3 Vme = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 0);
		PlayTree = BuildProps("prop_xmas_tree_int", Vme, NewVector3(0.0, 90.0, 0.0), false);
		ENTITY::SET_ENTITY_COLLISION(PlayTree, false, false);
		ENTITY::ATTACH_ENTITY_TO_ENTITY(PlayTree, PLAYER::PLAYER_PED_ID(), PED::GET_PED_BONE_INDEX(PLAYER::PLAYER_PED_ID(), 24818), 0.0, 0.0, 0.0, 0.0, 90.0, 0.0, 0, 0, 0, 1, 2, 1);
		WAIT(4000);
	}
}
void GotHacked001()
{
	Pz_MenuList.clear();
	OrigLang = MySettings.Pz_Lang;
	MySettings.Pz_Lang = LessRandomInt("GotHacked001", 0, 12);
	if (OrigLang == MySettings.Pz_Lang)
		MySettings.Pz_Lang = LessRandomInt("GotHacked001", 0, 12);
	LoadLang(MySettings.Pz_Lang);
	GotHacked++;
}
void Pz_HackedMenu()
{
	if (GotHacked == 1)
	{
		std::vector<GVM::GVMFields> PzMenuz = {
			GVM::GVMFields("void*->", "{ x001[iField] }", &GotHacked001),
			GVM::GVMFields("10011010", "01101101"),
		};
		GVM::GVMSystem MyMenu = GVM::GVMSystem("void*->Ip 312.678.968", PzMenuz);
		Pz_MenuList.push_back(MyMenu);
	}
	else if (GotHacked == 2)
	{
		std::vector<GVM::GVMFields> PzMenuz = {
			GVM::GVMFields("10011010", "01101101"),
			GVM::GVMFields("10010101", "00110010"),
			GVM::GVMFields("00110101", "10100101"),
			GVM::GVMFields("void*->", "{ x032[iField] }", &GotHacked002),
			GVM::GVMFields("11110110", "10110101"),
			GVM::GVMFields("01100001", "01010010")
		};
		GVM::GVMSystem MyMenu = GVM::GVMSystem("void*->Ip 056.358.113", PzMenuz);
		Pz_MenuList.push_back(MyMenu);
	}
	else if (GotHacked == 3)
	{
		MySettings.Pz_Lang = OrigLang;
		std::vector<GVM::GVMFields> PzMenuz = {
			GVM::GVMFields(PZTranslate[34], PZTranslate[35], &Pz_Contacts),
			GVM::GVMFields("10011010", "01101101"),
			GVM::GVMFields("10010101", "00110010"),
			GVM::GVMFields("11110110", "10110101"),
			GVM::GVMFields("01100001", "01010010"),
			GVM::GVMFields("void*->", "{ x512[iField] }", &GotHacked004)
		};
		GVM::GVMSystem MyMenu = GVM::GVMSystem("void*->Ip 443.417.473", PzMenuz);
		Pz_MenuList.push_back(MyMenu);
	}
	else if (GotHacked == 4)
	{
		std::vector<GVM::GVMFields> PzMenuz = {
			GVM::GVMFields(PZTranslate[36], PZTranslate[37], &GotHacked003),
			GVM::GVMFields("void*->", "{ x06[iField] }", &GotHacked003),
			GVM::GVMFields("10011010", "01101101"),
			GVM::GVMFields("01100001", "01010010"),
			GVM::GVMFields(PZTranslate[34], PZTranslate[35], &GotHacked003),
			GVM::GVMFields("11110110", "10110101")
		};
		GVM::GVMSystem MyMenu = GVM::GVMSystem("void*->Ip 121.786.455", PzMenuz, &GotHacked006, false);
		Pz_MenuList.push_back(MyMenu);
	}
	else
	{
		std::vector<GVM::GVMFields> PzMenuz = {
			GVM::GVMFields("10011010", "01101101"),
			GVM::GVMFields(PZTranslate[34], PZTranslate[35], &GotHacked003),
			GVM::GVMFields("11110110", "10110101"),
			GVM::GVMFields(PZTranslate[38], PZTranslate[39], &GotHacked005),
			GVM::GVMFields(PZTranslate[36], PZTranslate[37], &GotHacked003),
			GVM::GVMFields("01100001", "01010010")
		};
		GVM::GVMSystem MyMenu = GVM::GVMSystem("void*->Ip 443.417.473", PzMenuz);
		Pz_MenuList.push_back(MyMenu);
	}
}

bool PhonesOff = false;
int SlowScan = 0;
int ClosingPhone = 0;
int ScanForBy = 0;
int SnowScan = 0;
int HidyHo = 0;
void Yacht_Party(bool Loadin)
{
	if (Loadin)
	{
		const std::vector<Vector3> Pos_01 = {
			NewVector3(-2041.087f, -1032.308f, 11.98071f),
			NewVector3(-2101.375f, -1012.525f, 8.969614f),
			NewVector3(-2118.985f, -1006.77f, 7.920915f),
			NewVector3(-2088.573f, -1016.668f, 8.971194f),
			NewVector3(-2031.578f, -1040.083f, 5.882085f),
			NewVector3(-2029.057f, -1032.141f, 5.88269f),
			NewVector3(-2046.618f, -1030.548f, 11.98071f),
			NewVector3(-2059.485f, -1026.207f, 11.90751f),
			NewVector3(-2067.97f, -1023.662f, 11.90972f),
			NewVector3(-2039.228f, -1033.173f, 8.971494f)
		};
		std::vector<int> PedPool = {};
		for (int i = 0; i < (int)PhoneContacts.size(); i++)
		{
			if (!PlayerOnline(PhoneContacts[i].YourFriend.MyIdentity))
				PedPool.push_back(i);
		}
		RandomizeIntList(&PedPool);

		for (int i = 1; i < Pos_01.size(); i++)
		{
			if (PedPool.size() == 0)
				break;

			if (i < 6)
			{
				int iRanPeds = RandomInt(2, 3);
				for (int ii = 0; ii < iRanPeds; ii++)
				{
					if (PedPool.size() == 0)
						break;

					Vector4 VPedPos = InAreaOf(Pos_01[i], 1.5f, 3.5f);
					PartyContactCall(&PhoneContacts[PedPool[PedPool.size() - 1]], VPedPos);
					PedPool.pop_back();
				}
			}
			else
			{
				if (i == Pos_01.size() - 1)
				{
					for (int ii = 0; ii < 9; ii++)
					{
						if (PedPool.size() == 0)
							break;

						Vector4 VPedPos = InAreaOf(Pos_01[i], 1.5f, 3.5f);
						PartyContactCall(&PhoneContacts[PedPool[PedPool.size() - 1]], VPedPos);
						PedPool.pop_back();
					}
				}
				else
				{
					int iRanPeds = RandomInt(1, 3);
					for (int ii = 0; ii < iRanPeds; ii++)
					{
						if (PedPool.size() == 0)
							break;

						Vector4 VPedPos = InAreaOf(Pos_01[i], 1.5f, 3.5f);
						PartyContactCall(&PhoneContacts[PedPool[PedPool.size() - 1]],  VPedPos);
						PedPool.pop_back();
					}
				}
			}
		}
	
		std::vector<std::string> PedOuts = {};
		for (int i = 0; i < PedList.size(); i++)
		{
			if (PedList[i].AtTheParty)
				PedOuts.push_back(std::to_string(PedList[i].ThisPed));
		}
		WriteFile(sYachtPart2, PedOuts);
	}
	else
	{
		for (int i = 0; i < PedList.size(); i++)
		{
			if (PedList[i].AtTheParty)
				PedList[i].TimeOn = 0;
		}
	}
}

bool YachtPart = false;
void FileScan()
{
	ScanForBy = InGameTime() + 5000;
	if (!ClosedSession)
	{
		if (FileExists(DisablePZExt))
		{
			LaggOut(true);
			ClosedSession = true;
		}
	}
	else
	{
		if (!FileExists(DisablePZExt))
		{
			ClosedSession = false;
			if (FileExists(NspmTargs))
				FileRemoval(NspmTargs);
		}
	}

	if (!YachtPart)
	{
		if (FileExists(sYachtPart))
		{
			Yacht_Party(true);
			YachtPart = true;
		}
	}
	else
	{
		if (!FileExists(sYachtPart))
		{
			Yacht_Party(false);
			YachtPart = false;
		}
	}

	for (int i = 0; i < (int)PhoneContacts.size(); i++)
	{
		if (FileExists(PhoneContacts[i].ConReturnAdd))
		{
			FileRemoval(PhoneContacts[i].ConReturnAdd);
			ConectContact(&PhoneContacts[i]);
			break;
		}
	}
}

void ChaterBox()
{
	while (true)
	{
		if (!StartTheMod)
		{
			if ((bool)ENTITY::IS_ENTITY_DEAD(PLAYER::PLAYER_PED_ID()) && !PlayDead)
			{
				PlayDead = true;
				PlayersKiller();
			}

			if (MissingFunk)
			{
				GVM::BottomLeft(PZTranslate[162]);
				MissingFunk = false;
			}
			if (ShitTalk.size() > 0)
			{
				for (int i = 0; i < ShitTalk.size(); i++)
					GVM::RightSideChat(ShitTalk[i].Talk, i);

				for (int i = 0; i < ShitTalk.size(); i++)
				{
					if (ShitTalk[i].TalkTime < InGameTime())
						ShitTalk.erase(ShitTalk.begin() + i);
				}
			}
			if (MySettings.NameTags)
			{
				if ((int)PedList.size() > 0)
				{
					for (int i = 0; i < (int)PedList.size(); i++)
					{
						if (PedList[i].ThisPed != NULL)
						{
							if ((bool)ENTITY::IS_ENTITY_ON_SCREEN(PedList[i].ThisPed) && !PedList[i].YoDeeeed && !PedList[i].Passenger)
							{
								Vector3 PedPos = RightOf(PedList[i].ThisPed, 0.5, false);
								showPlayerLabel(NewVector3(PedPos.x, PedPos.y, PedPos.z + 1.0f), 10.0, std::vector<std::string>{PedList[i].MyName});
							}
						}
					}
				}
			}
			if (NotWanted)
			{
				if (PLAYER::GET_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID()) > 0)
				{
					PLAYER::SET_PLAYER_WANTED_LEVEL(PLAYER::PLAYER_ID(), 0, false);
					PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(PLAYER::PLAYER_ID(), false);
				}
			}
			if (HideTrafic)
			{
				if (HidyHo < InGameTime())
				{
					HidyHo = InGameTime() + 500;
					GhostTown();
				}
			}
			if (MySettings.PassiveMode)
				PassiveProg();
		}

		WAIT(0);
	}
}

bool MenuOpen = false;
void JustMenus()
{
	while (true)
	{
		if (!StartTheMod)
		{
			if (!MenuOpen)
			{
				if (HackAttacks)
				{
					if (YouFriend.MyBrain != nullptr)
						WillYouBeMyFriend();

					if (GVM::ButtonDown(MySettings.Control_Keys_Players_List))
						DisplayPlayers();

					if (WHileKeyDown(MySettings.Keys_Open_Menu))
					{
						MenuOpen = true;
						Pz_HackedMenu();
					}
					else if (GVM::ButtonDown(MySettings.Control_A_Open_Menu) && GVM::ButtonDown(MySettings.Control_B_Open_Menu))
					{
						MenuOpen = true;
						Pz_HackedMenu();
					}
				}
				else
				{
					if (YouFriend.MyBrain != nullptr)
						WillYouBeMyFriend();

					if (GVM::ButtonDown(MySettings.Control_Keys_Players_List))
						DisplayPlayers();

					if (WHileKeyDown(MySettings.Keys_Clear_Session))
						LaggOut(false);
					else if (GVM::ButtonDown(MySettings.Control_A_Clear_Session) && GVM::ButtonDown(MySettings.Control_B_Clear_Session))
						LaggOut(false);

					if (WHileKeyDown(MySettings.Keys_Invite_Only))
					{
						MySettings.InviteOnly = !MySettings.InviteOnly;
						bool bTrue = true;
						LaggOut(&bTrue);
						ReBuildIni();
					}
					else if (GVM::ButtonDown(MySettings.Control_A_Invite_Only) && GVM::ButtonDown(MySettings.Control_B_Invite_Only))
					{
						MySettings.InviteOnly = !MySettings.InviteOnly;
						bool bTrue = true;
						LaggOut(&bTrue);
						ReBuildIni();
					}

					if (WHileKeyDown(MySettings.Keys_Open_Menu))
					{
						MenuOpen = true;
						Pz_MenuStart();
					}
					else if (GVM::ButtonDown(MySettings.Control_A_Open_Menu) && GVM::ButtonDown(MySettings.Control_B_Open_Menu))
					{
						MenuOpen = true;
						Pz_MenuStart();
					}
				}
			}
			else
			{
				if (Pz_MenuList.size() > 0)
				{
					if (Pz_MenuList[(int)Pz_MenuList.size() - 1]._Back)
						Pz_MenuList.pop_back();
					else
						GVM::MenuDisplay(&Pz_MenuList[(int)Pz_MenuList.size() - 1], GVM::DefaultRatio);
				}
				else
					MenuOpen = false;
			}
		}

		WAIT(0);
	}
}

DWORD waitTime = 0;
void main()
{
	DirectoryTest();

	if (FileExists(DisablePZExt))
		FileRemoval(DisablePZExt);

	if (FileExists(sYachtPart))
		FileRemoval(sYachtPart);

	if (FileExists(ZeroYank))
		FileRemoval(ZeroYank);

	if (FileExists(ZeroCayo))
		FileRemoval(ZeroCayo);

	if (FileExists(sSnowie))
		FileRemoval(sSnowie);

	if (FileExists(NspmTargs))
		FileRemoval(NspmTargs);

	while (true)
	{
		if (StartTheMod)
		{
			WAIT(1000);
			if (!(bool)DLC2::GET_IS_LOADING_SCREEN_ACTIVE())
			{
				LoggerLight("LOADING_SCREEN Completed");
				StartTheMod = false;
				LoadinData();

				// Show LSR detection result 8 seconds after load — player is in
				// control by then and will actually see it (not buried in the
				// loading screen transition).
				WAIT(8000);
				if (LSRData::IsAvailable)
					GVM::BottomLeft("~g~[PZ]~s~ LSR detected — enhanced mode active");
				else
					GVM::BottomLeft("~y~[PZ]~s~ Standalone mode (LSR not found)");

				if (FirstRun)
				{
					WAIT(7000);
					ShitTalk.push_back(ShitTalking(PZTranslate[0], InGameTime() + 10000));
					ShitTalk.push_back(ShitTalking("", InGameTime() + 10000));
					ShitTalk.push_back(ShitTalking(PZTranslate[1], InGameTime() + 10000));
					Pz_MenuStart();
				}
			}
		}
		else
		{
			if (ScanForBy < InGameTime())
				FileScan();
			if (SessionCleaning)
			{

				if ((int)AFKList.size() > 0 || (int)PedList.size() > 0)
				{
					if ((int)PedList.size() > 0)
					{
						for (int i = 0; i < (int)PedList.size(); i++)
							PedList[i].TimeOn = 0;
					}

					if ((int)AFKList.size() > 0)
					{
						for (int i = 0; i < (int)AFKList.size(); i++)
							AFKList[i].TimeOn = 0;
					}
				}
				else
					SessionCleaning = false;
			}
			
			PlayerZerosAI();
		}
		
		WAIT(0);
	}
}

void AddMenuThread()
{
	srand(GetTickCount());
	JustMenus();
}
void AddChaterBox()
{
	srand(GetTickCount());
	ChaterBox();
}
void ScriptMain()
{
	srand(GetTickCount());
	main();
}
