/*
    LSRData.cpp - Los Santos Red XML integration for Player Zero
*/
#include "LSRData.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>

// ---------------------------------------------------------------------------
// Static member definitions
// ---------------------------------------------------------------------------
bool LSRData::IsAvailable = false;
std::unordered_map<std::string, std::string>              LSRData::ZoneGangMap;
std::unordered_map<std::string, std::string>              LSRData::ZoneEconomyMap;
std::unordered_map<std::string, LSRGangProfile>           LSRData::GangProfiles;
std::unordered_map<std::string, std::vector<LSRLocation>> LSRData::Locations;
std::unordered_map<int, LSRInterior>                        LSRData::Interiors;
std::vector<std::string>                                    LSRData::IntoxicantNames;

// ---------------------------------------------------------------------------
// XML helpers
// ---------------------------------------------------------------------------

std::string LSRData::GetTagValue(const std::string& line, const std::string& tag) {
    std::string open  = "<"  + tag + ">";
    std::string close = "</" + tag + ">";
    size_t s = line.find(open);
    if (s == std::string::npos) return "";
    s += open.size();
    size_t e = line.find(close, s);
    if (e == std::string::npos) return "";
    return line.substr(s, e - s);
}

float LSRData::GetTagFloat(const std::string& line, const std::string& tag) {
    std::string v = GetTagValue(line, tag);
    if (v.empty()) return 0.0f;
    try { return std::stof(v); } catch (...) { return 0.0f; }
}

int LSRData::GetTagInt(const std::string& line, const std::string& tag) {
    std::string v = GetTagValue(line, tag);
    if (v.empty()) return 0;
    try { return std::stoi(v); } catch (...) { return 0; }
}

bool LSRData::GetTagBool(const std::string& line, const std::string& tag) {
    return GetTagValue(line, tag) == "true";
}

// ---------------------------------------------------------------------------
// Block-level parser
// Streams the file, accumulating text between <blockTag> and </blockTag>,
// and fires callback(blockText) for each complete block.
// ---------------------------------------------------------------------------
void LSRData::ParseBlocks(const std::string& path, const std::string& blockTag,
    std::function<void(const std::string&)> callback)
{
    std::ifstream f(path);
    if (!f.is_open()) return;

    std::string openTag  = "<"  + blockTag + ">";
    std::string closeTag = "</" + blockTag + ">";
    std::string line, block;
    bool inBlock = false;

    while (std::getline(f, line)) {
        if (!inBlock) {
            if (line.find(openTag) != std::string::npos) {
                inBlock = true;
                block   = line + "\n";
            }
        } else {
            block += line + "\n";
            if (line.find(closeTag) != std::string::npos) {
                callback(block);
                block.clear();
                inBlock = false;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Loaders
// ---------------------------------------------------------------------------

void LSRData::LoadTerritories(const std::string& path) {
    ParseBlocks(path, "GangTerritory", [](const std::string& blk) {
        std::string zone, gang;
        std::istringstream ss(blk);
        std::string line;
        while (std::getline(ss, line)) {
            if (zone.empty()) zone = GetTagValue(line, "ZoneInternalGameName");
            if (gang.empty()) gang = GetTagValue(line, "GangID");
        }
        // Only keep the first entry per zone (highest priority in LSR ordering)
        if (!zone.empty() && !gang.empty())
            if (ZoneGangMap.find(zone) == ZoneGangMap.end())
                ZoneGangMap[zone] = gang;
    });
}

void LSRData::LoadZones(const std::string& path) {
    ParseBlocks(path, "Zone", [](const std::string& blk) {
        std::string name, econ;
        std::istringstream ss(blk);
        std::string line;
        while (std::getline(ss, line)) {
            if (name.empty()) name = GetTagValue(line, "InternalGameName");
            if (econ.empty()) econ = GetTagValue(line, "Economy");
        }
        if (!name.empty() && !econ.empty())
            ZoneEconomyMap[name] = econ;
    });
}

void LSRData::LoadGangs(const std::string& path) {
    ParseBlocks(path, "Gang", [](const std::string& blk) {
        LSRGangProfile gp;
        std::istringstream ss(blk);
        std::string line;
        while (std::getline(ss, line)) {
            if (gp.gangID.empty())
                gp.gangID = GetTagValue(line, "ID");
            if (line.find("<FightPercentage>") != std::string::npos)
                gp.fightPercentage      = GetTagInt(line, "FightPercentage");
            if (line.find("<DrugDealerPercentage>") != std::string::npos)
                gp.drugDealerPercentage = GetTagInt(line, "DrugDealerPercentage");
            if (line.find("<PercentageWithLongGuns>") != std::string::npos)
                gp.pctLongGuns          = GetTagInt(line, "PercentageWithLongGuns");
            if (line.find("<PercentageWithSidearms>") != std::string::npos)
                gp.pctSidearms          = GetTagInt(line, "PercentageWithSidearms");
            if (line.find("<PercentageWithMelee>") != std::string::npos)
                gp.pctMelee             = GetTagInt(line, "PercentageWithMelee");
            if (line.find("<VehicleSpawnPercentage>") != std::string::npos)
                gp.vehicleSpawnPct      = GetTagInt(line, "VehicleSpawnPercentage");
            if (line.find("<HostileRepLevel>") != std::string::npos)
                gp.hostileRepLevel      = GetTagInt(line, "HostileRepLevel");
        }
        if (!gp.gangID.empty())
            GangProfiles[gp.gangID] = gp;
    });
}

// Locations.xml is very large (~125k lines) so we stream it line-by-line
// rather than loading the whole file into a string buffer.
void LSRData::LoadLocations(const std::string& path) {
    // Container element name -> individual item element name
    static const std::pair<std::string, std::string> containers[] = {
        { "GangDens",            "GangDen"           },
        { "Restaurants",         "Restaurant"        },
        { "Bars",                "Bar"               },
        { "Banks",               "Bank"              },
        { "ConvenienceStores",   "ConvenienceStore"  },
        { "GasStations",         "GasStation"        },
        { "LiquorStores",        "LiquorStore"       },
        { "DriveThrus",          "DriveThru"         },
        { "Pharmacies",          "Pharmacy"          },
        { "Dispensaries",        "Dispensary"        },
        { "IllicitMarketplaces", "IllicitMarketplace"},
        { "Hotels",              "Hotel"             },
        { "Hospitals",           "Hospital"          },
    };
    static const int containerCount = (int)(sizeof(containers) / sizeof(containers[0]));

    std::ifstream f(path);
    if (!f.is_open()) return;

    std::string currentContainer;
    std::string currentItem;
    bool inItem      = false;
    int  posState    = 0; // 0=waiting for EntrancePosition, 1=gotX, 2=gotXY, 3=complete
    float curX = 0, curY = 0, curZ = 0;
    int   curOpen = 0, curClose = 24;

    std::string line;
    while (std::getline(f, line)) {

        // --- Container transitions (only checked when we're not mid-item) ---
        if (!inItem) {
            for (int i = 0; i < containerCount; ++i) {
                const std::string& cname = containers[i].first;
                const std::string& iname = containers[i].second;

                if (line.find("<" + cname + ">") != std::string::npos) {
                    currentContainer = cname;
                    currentItem      = iname;
                    break;
                }
                if (!currentContainer.empty() &&
                    line.find("</" + currentContainer + ">") != std::string::npos)
                {
                    currentContainer.clear();
                    currentItem.clear();
                    break;
                }
            }
        }

        if (currentItem.empty()) continue;

        // --- Item open tag ---
        if (!inItem && line.find("<" + currentItem + ">") != std::string::npos) {
            inItem    = true;
            posState  = 0;
            curX = curY = curZ = 0.0f;
            curOpen   = 0;
            curClose  = 24;
            continue;
        }

        // --- Item close tag ---
        if (inItem && line.find("</" + currentItem + ">") != std::string::npos) {
            if (posState == 3) { // we captured a valid position
                LSRLocation loc;
                loc.typeName  = currentItem;
                loc.x         = curX;
                loc.y         = curY;
                loc.z         = curZ;
                loc.openTime  = curOpen;
                loc.closeTime = curClose;
                Locations[currentItem].push_back(loc);
            }
            inItem = false;
            continue;
        }

        if (!inItem) continue;

        // --- Field extraction ---
        // Track EntrancePosition via state machine to avoid picking up
        // coordinates from PossiblePedSpawns etc. within the same item.
        if (posState == 0 && line.find("<EntrancePosition>") != std::string::npos)
            posState = 1;
        else if (posState == 1 && line.find("<X>") != std::string::npos) {
            curX     = GetTagFloat(line, "X");
            posState = 2;
        } else if (posState == 2 && line.find("<Y>") != std::string::npos) {
            curY     = GetTagFloat(line, "Y");
            posState = 3; // Z will come next but mark complete once we have X+Y+Z
        } else if (posState == 3 && line.find("<Z>") != std::string::npos) {
            curZ     = GetTagFloat(line, "Z");
            // posState stays 3; we now have a full position
        }

        if (line.find("<OpenTime>") != std::string::npos)
            curOpen  = GetTagInt(line, "OpenTime");
        if (line.find("<CloseTime>") != std::string::npos)
            curClose = GetTagInt(line, "CloseTime");
    }
}

// ---------------------------------------------------------------------------
// Init
// ---------------------------------------------------------------------------
void LSRData::Init(const std::string& lsrRoot) {
    IsAvailable = false;

    // Quick sentinel check — if Locations.xml isn't here LSR isn't installed.
    {
        std::ifstream check(lsrRoot + "/Locations.xml");
        if (!check.is_open()) {
            // Write to PZ log directly (no PZSys dependency).
            std::ofstream log("PlayerZero/LoggerLight.txt", std::ios::app);
            if (log) log << "[LSRData] Locations.xml not found at: " << lsrRoot << "\n";
            return;
        }
    }

    LoadTerritories(lsrRoot + "/GangTerritories.xml");
    LoadZones      (lsrRoot + "/Zones.xml");
    LoadGangs      (lsrRoot + "/Gangs.xml");
    LoadLocations  (lsrRoot + "/Locations.xml");
    LoadInteriors  (lsrRoot + "/Interiors.xml");
    LoadIntoxicants(lsrRoot + "/Itoxicants.xml");

    IsAvailable = true;

    // Diagnostic summary so we can confirm the right data was loaded.
    {
        std::ofstream log("PlayerZero/LoggerLight.txt", std::ios::app);
        if (log) {
            log << "[LSRData] Loaded OK — "
                << "Zones: "       << ZoneGangMap.size()
                << ", Gangs: "     << GangProfiles.size()
                << ", EconZones: " << ZoneEconomyMap.size()
                << ", Restaurants: " << Locations["Restaurant"].size()
                << ", Bars: "        << Locations["Bar"].size()
                << ", GangDens: "    << Locations["GangDen"].size()
                << "\n";
        }
    }
}

// ---------------------------------------------------------------------------
// Public lookups
// ---------------------------------------------------------------------------

std::string LSRData::GetGangForZone(const std::string& zoneName) {
    auto it = ZoneGangMap.find(zoneName);
    return (it != ZoneGangMap.end()) ? it->second : "";
}

std::string LSRData::GetEconomyForZone(const std::string& zoneName) {
    auto it = ZoneEconomyMap.find(zoneName);
    return (it != ZoneEconomyMap.end()) ? it->second : "";
}

int LSRData::GetEconomyCode(const std::string& economy) {
    if (economy == "Poor")   return 1;
    if (economy == "Middle") return 2;
    if (economy == "Rich")   return 3;
    return 0;
}

const LSRGangProfile* LSRData::GetGangProfile(const std::string& gangID) {
    auto it = GangProfiles.find(gangID);
    return (it != GangProfiles.end()) ? &it->second : nullptr;
}

std::vector<const LSRLocation*> LSRData::GetLocationsForHour(
    const std::string& typeName, int hour)
{
    std::vector<const LSRLocation*> result;
    auto it = Locations.find(typeName);
    if (it == Locations.end()) return result;

    for (const auto& loc : it->second) {
        if (hour < 0) {
            result.push_back(&loc);
            continue;
        }
        bool open;
        if (loc.openTime <= loc.closeTime)
            open = (hour >= loc.openTime && hour < loc.closeTime);
        else // wraps midnight e.g. open=20, close=4
            open = (hour >= loc.openTime || hour < loc.closeTime);
        if (open) result.push_back(&loc);
    }
    return result;
}

const LSRLocation* LSRData::GetRandomLocation(const std::string& typeName, int hour) {
    auto locs = GetLocationsForHour(typeName, hour);
    if (locs.empty()) return nullptr;
    return locs[rand() % (int)locs.size()];
}

const LSRLocation* LSRData::GetNearestLocation(
    float px, float py, float pz, const std::string& typeName)
{
    auto it = Locations.find(typeName);
    if (it == Locations.end()) return nullptr;

    const LSRLocation* best = nullptr;
    float bestDistSq = 1e12f;
    for (const auto& loc : it->second) {
        float dx = loc.x - px, dy = loc.y - py, dz = loc.z - pz;
        float d = dx*dx + dy*dy + dz*dz;
        if (d < bestDistSq) { bestDistSq = d; best = &loc; }
    }
    return best;
}

const LSRLocation* LSRData::GetNightlifeLocation(int hour) {
    // 22:00-05:59 -> bars only; 18:00-21:59 -> bars or restaurants; else restaurants
    if (hour >= 22 || hour < 6)
        return GetRandomLocation("Bar", hour);
    if (hour >= 18)
        return GetRandomLocation((rand() % 2 == 0) ? "Bar" : "Restaurant", hour);
    return GetRandomLocation("Restaurant", hour);
}

const LSRLocation* LSRData::GetRobableStore() {
    // 60% convenience store, 40% liquor store (both always open)
    const char* type = (rand() % 10 < 6) ? "ConvenienceStore" : "LiquorStore";
    return GetRandomLocation(type, -1);
}

const LSRLocation* LSRData::GetNearestLocationWithin(
    float px, float py, float pz, const std::string& typeName, float maxDist)
{
    const LSRLocation* nearest = GetNearestLocation(px, py, pz, typeName);
    if (!nearest) return nullptr;
    float dx = nearest->x - px, dy = nearest->y - py;
    float distSq = dx*dx + dy*dy;
    return (distSq <= maxDist * maxDist) ? nearest : nullptr;
}

int LSRData::LocationCount(const std::string& typeName) {
    if (!IsAvailable) return 0;
    auto it = Locations.find(typeName);
    return (it != Locations.end()) ? (int)it->second.size() : 0;
}

// ---------------------------------------------------------------------------
// LoadInteriors â€” reads Interiors.xml into the Interiors map.
// Stores LocalID, Name, and IsWeaponRestricted for each interior entry.
// ---------------------------------------------------------------------------
void LSRData::LoadInteriors(const std::string& path) {
    ParseBlocks(path, "Interior", [](const std::string& blk) {
        LSRInterior interior;
        bool gotID = false;
        std::istringstream ss(blk);
        std::string line;
        while (std::getline(ss, line)) {
            if (!gotID && line.find("<LocalID>") != std::string::npos) {
                std::string v = GetTagValue(line, "LocalID");
                if (!v.empty()) {
                    try { interior.localID = std::stoi(v); gotID = true; } catch (...) {}
                }
            }
            if (interior.name.empty() && line.find("<Name>") != std::string::npos)
                interior.name = GetTagValue(line, "Name");
            if (line.find("<IsWeaponRestricted>") != std::string::npos)
                interior.isWeaponRestricted = GetTagBool(line, "IsWeaponRestricted");
        }
        if (gotID)
            Interiors[interior.localID] = interior;
    });
}

const LSRInterior* LSRData::GetInterior(int localID) {
    auto it = Interiors.find(localID);
    return (it != Interiors.end()) ? &it->second : nullptr;
}

const LSRLocation* LSRData::GetNearestLocationWithInterior(
    float px, float py, float pz, float maxDist)
{
    if (!IsAvailable) return nullptr;
    const LSRLocation* best = nullptr;
    float bestDistSq = maxDist * maxDist;
    for (const auto& kv : Locations) {
        for (const auto& loc : kv.second) {
            if (loc.interiorID == -1) continue;
            float dx = loc.x - px, dy = loc.y - py;
            float d = dx*dx + dy*dy;
            if (d < bestDistSq) { bestDistSq = d; best = &loc; }
        }
    }
    return best;
}

// ---------------------------------------------------------------------------
// LoadIntoxicants — reads Itoxicants.xml (and optionally LSRPDRUGS variant)
// Populates IntoxicantNames with every <Name> found in <Intoxicant> blocks.
// ---------------------------------------------------------------------------
void LSRData::LoadIntoxicants(const std::string& path)
{
    // Parse main file
    ParseBlocks(path, "Intoxicant", [](const std::string& blk) {
        std::istringstream ss(blk);
        std::string line;
        while (std::getline(ss, line)) {
            if (line.find("<Name>") != std::string::npos) {
                std::string name = GetTagValue(line, "Name");
                if (!name.empty())
                    IntoxicantNames.push_back(name);
                break;
            }
        }
    });

    // Also pull in the LSRPDRUGS expansion pack if present
    std::string plusPath = path.substr(0, path.rfind('/') + 1) + "Itoxicants+_LSRPDRUGS.xml";
    ParseBlocks(plusPath, "Intoxicant", [](const std::string& blk) {
        std::istringstream ss(blk);
        std::string line;
        while (std::getline(ss, line)) {
            if (line.find("<Name>") != std::string::npos) {
                std::string name = GetTagValue(line, "Name");
                if (!name.empty())
                    IntoxicantNames.push_back(name);
                break;
            }
        }
    });
}

// Returns a random intoxicant name. Falls back to "something" if the list is empty.
const std::string& LSRData::GetRandomIntoxicant()
{
    static std::string fallback = "something";
    if (IntoxicantNames.empty()) return fallback;
    return IntoxicantNames[(size_t)std::rand() % IntoxicantNames.size()];
}