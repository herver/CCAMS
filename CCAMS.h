#pragma once
#include <vector>
#include <string>
#include <regex>
#include <future>
#include <thread>
#include <exception>
#include <map>
#include <cstdio>
#include <EuroScopePlugIn.h>
#include "Helpers.h"

using namespace std;
using namespace EuroScopePlugIn;

#define MY_PLUGIN_NAME			"CCAMS"
#ifdef _DEBUG
#define MY_PLUGIN_VERSION		"2.0.0 DEV"
#else
#define MY_PLUGIN_VERSION		"2.0.0"
#endif
#define MY_PLUGIN_VERSIONCODE	11
#define MY_PLUGIN_UPDATE_URL	"https://raw.githubusercontent.com/kusterjs/CCAMS/master/config.txt"
//#define MY_PLUGIN_UPDATE_URL	"https://raw.githubusercontent.com/kusterjs/CCAMS/1.8/config.txt"
#define MY_PLUGIN_DEVELOPER		"Jonas Kuster, Pierre Ferran, Oliver Gr�tzmann"
#define MY_PLUGIN_COPYRIGHT		"GPL v3"
//#define MY_PLUGIN_VIEW      "Standard ES radar screen"

struct ItemCodes
{
	enum ItemTypes : int
	{
		TAG_ITEM_ISMODES = 501,
		TAG_ITEM_EHS_HDG,
		TAG_ITEM_EHS_ROLL,
		TAG_ITEM_EHS_GS,
		TAG_ITEM_ERROR_MODES_USE,
		TAG_ITEM_SQUAWK
	};

	enum ItemFunctions : int
	{
		TAG_FUNC_SQUAWK_POPUP = 869,
		TAG_FUNC_ASSIGN_SQUAWK,
		TAG_FUNC_ASSIGN_SQUAWK_AUTO,
		TAG_FUNC_ASSIGN_SQUAWK_MANUAL,
		TAG_FUNC_ASSIGN_SQUAWK_VFR,
		TAG_FUNC_ASSIGN_SQUAWK_MODES,
		TAG_FUNC_ASSIGN_SQUAWK_DISCRETE
	};
};

struct EquipmentCodes
{
	string FAA{ "HLEGWQS" };
	string ICAO_MODE_S{ "EHILS" };
	string ICAO_EHS{ "EHLS" };
};

struct SquawkCodes
{
	const char* MODE_S{ "1000" };
	const char* VFR{ "7000" };
};

static const regex MODE_S_AIRPORTS("^((E([BDHLT]|P(?!CE|DA|DE|IR|KS|LK|LY|MB|MI|MM|OK|PR|PW|SN|TM))|L[DFHIKORZ])[A-Z]{2}|LS(G[CG]|Z[BGHR]))", regex::icase);


class CCAMS :
	public EuroScopePlugIn::CPlugIn
{
public:
	explicit CCAMS(const EquipmentCodes&& ec = EquipmentCodes(), const SquawkCodes&& sc = SquawkCodes());
	virtual ~CCAMS();

	bool OnCompileCommand(const char* command);
	void OnGetTagItem(CFlightPlan FlightPlan, CRadarTarget RadarTarget,
					  int ItemCode,
					  int TagData,
					  char sItemString[16],
					  int * pColorCode,
					  COLORREF * pRGB,
					  double * pFontSize);

	void OnFlightPlanFlightPlanDataUpdate(CFlightPlan FlightPlan);
	void OnFlightPlanDisconnect(CFlightPlan FlightPlan);

	void OnRefreshFpListContent(CFlightPlanList AcList);
	void OnFunctionCall(int FunctionId,
						const char * sItemString,
						POINT Pt,
						RECT Area);

	void OnTimer(int Counter);

	bool PluginCommands(const char* Command);

private:
	future<string> fUpdateString;
	vector<string> ProcessedFlightPlans;
	regex ModeSAirports;
	CFlightPlanList FpListEHS;
	string EquipmentCodesFAA;
	string EquipmentCodesICAO;
	string EquipmentCodesICAOEHS;
	const char* squawkModeS;
	const char* squawkVFR;
	bool pluginVersionRestricted;
	bool acceptEquipmentICAO;
	bool acceptEquipmentFAA;
	bool autoAssign;
	int APTcodeMaxGS;
	int APTcodeMaxDist;

	void AssignAutoSquawk(CFlightPlan& FlightPlan);
	void AssignPendingSquawks();
	void DoInitialLoad(future<string> & message);
	bool IsFlightPlanProcessed(CFlightPlan & FlightPlan);
	bool isAcModeS(const CFlightPlan& FlightPlan) const;
	bool isApModeS(const string& icao) const;
	bool isEHS(const CFlightPlan& FlightPlan) const;
	bool hasEquipment(const CFlightPlan& FlightPlan, bool acceptEquipmentFAA, bool acceptEquipmentICAO, string CodesICAO) const;
	bool isADEPvicinity(const CFlightPlan& FlightPlan) const;
	bool isEligibleSquawkModeS(const CFlightPlan& FlightPlan) const;
	bool hasValidSquawk(const CFlightPlan& FlightPlan);

	map<const char*, future<string>> PendingSquawks;
	vector<const char*> collectUsedCodes(const CFlightPlan& FlightPlan);
};