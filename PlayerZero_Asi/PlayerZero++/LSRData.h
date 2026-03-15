/*
    LSRData - Los Santos Red XML integration for Player Zero
    Reads LSR's data files at startup to drive zone-aware NPC behavior.
    All methods are safe to call even if LSR is not installed (IsAvailable == false).
*/
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

// ---------------------------------------------------------------------------
// Data structures
// ---------------------------------------------------------------------------

struct LSRGangProfile {
    std::string gangID;
    int fightPercentage      = 50;
    int drugDealerPercentage = 25;
    int pctLongGuns          = 10;
    int pctSidearms          = 30;
    int pctMelee             = 20;
    int vehicleSpawnPct      = 50;
    int hostileRepLevel      = -200;
};

struct LSRInterior {
    int         localID            = -1;
    std::string name;
    bool        isWeaponRestricted = false;
};

struct LSRIntoxicant {
    std::string name; // e.g. "Marijuana", "Cocaine", "SPANK"
};

struct LSRLocation {
    std::string typeName; // "GangDen", "Restaurant", "Bar", etc.
    float x         = 0.0f;
    float y         = 0.0f;
    float z         = 0.0f;
    int   openTime  = 0;  // in-game hour 0-24
    int   closeTime = 24;
    int   interiorID = -1; // LocalID in Interiors.xml; -1 = no linked interior
};

// ---------------------------------------------------------------------------
// Loader / query class  (all members static — no instance needed)
// ---------------------------------------------------------------------------

class LSRData {
public:
    // True after Init() successfully locates and parses LSR's XML files.
    static bool IsAvailable;

    // Call once from LoadinData() after the loading screen completes.
    // lsrRoot example: "plugins/LosSantosRED"
    static void Init(const std::string& lsrRoot);

    // ---- Zone / gang lookups -----------------------------------------------

    // Gang ID string (e.g. "AMBIENT_GANG_BALLAS") for a GTA zone name, or "".
    static std::string GetGangForZone(const std::string& zoneName);

    // Economy string: "Poor", "Middle", "Rich", or "" if unknown.
    static std::string GetEconomyForZone(const std::string& zoneName);

    // Economy as int: 0=unknown, 1=Poor, 2=Middle, 3=Rich.
    static int GetEconomyCode(const std::string& economy);

    // Pointer to a gang's behavioral profile, or nullptr if not found.
    static const LSRGangProfile* GetGangProfile(const std::string& gangID);

    // ---- Location lookups --------------------------------------------------

    // All locations of typeName open at the given in-game hour.
    // Pass hour = -1 to skip the time filter (return all of that type).
    static std::vector<const LSRLocation*> GetLocationsForHour(
        const std::string& typeName, int hour = -1);

    // Random open location of the given type, or nullptr if none available.
    static const LSRLocation* GetRandomLocation(
        const std::string& typeName, int hour = -1);

    // Nearest location of the given type to the given world position.
    static const LSRLocation* GetNearestLocation(
        float px, float py, float pz, const std::string& typeName);

    // Nearest location of typeName within maxDist metres, or nullptr if none qualify.
    static const LSRLocation* GetNearestLocationWithin(
        float px, float py, float pz, const std::string& typeName, float maxDist);

    // Nearest location of ANY type with a linked interior within maxDist metres.
    static const LSRLocation* GetNearestLocationWithInterior(
        float px, float py, float pz, float maxDist);

    // Interior data for a given LocalID, or nullptr if not found.
    static const LSRInterior* GetInterior(int localID);

    // Hour-aware nightlife destination: bars at night, restaurants at evening.
    static const LSRLocation* GetNightlifeLocation(int hour);

    // Random robbable store: 60% ConvenienceStore, 40% LiquorStore.
    static const LSRLocation* GetRobableStore();

    // Count of loaded locations for a type (0 if LSR not available).
    static int LocationCount(const std::string& typeName);

    // Random drug/intoxicant name from Itoxicants.xml, e.g. "Marijuana", "Cocaine".
    // Returns "something" if LSR not available or file not found.
    static const std::string& GetRandomIntoxicant();

private:
    static std::unordered_map<std::string, std::string>                ZoneGangMap;
    static std::unordered_map<std::string, std::string>                ZoneEconomyMap;
    static std::unordered_map<std::string, LSRGangProfile>             GangProfiles;
    static std::unordered_map<std::string, std::vector<LSRLocation>>   Locations;
    static std::unordered_map<int, LSRInterior>                        Interiors;
    static std::vector<std::string>                                    IntoxicantNames;

    static void LoadTerritories(const std::string& path);
    static void LoadZones(const std::string& path);
    static void LoadGangs(const std::string& path);
    static void LoadLocations(const std::string& path);
    static void LoadInteriors(const std::string& path);
    static void LoadIntoxicants(const std::string& path);

    // Reads the file at path, accumulates text between <blockTag> and </blockTag>,
    // and fires callback(block) for each complete block found.
    static void ParseBlocks(const std::string& path,
        const std::string& blockTag,
        std::function<void(const std::string&)> callback);

    // Line-level XML helpers — extract inner text of a single tag.
    static std::string GetTagValue(const std::string& line, const std::string& tag);
    static float       GetTagFloat(const std::string& line, const std::string& tag);
    static int         GetTagInt  (const std::string& line, const std::string& tag);
    static bool        GetTagBool (const std::string& line, const std::string& tag);
};
