#define BUILD_EXPORT
#include "untis.h"
#include "rpc.h"
#include "str.h"

inline cstr permString(string s) { return eternalstr(s.c_str()); }

cstr fixTime(int time) {
	int hr = time / 100;
	int min = time - hr * 100;
	return eternalstr(hr + (min < 10 ? ":0" : "0") + min);
}

void CreateUntisInst(untis& u, cstr server, cstr schoolName, cstr user, cstr password) {
	json params = {{"user", user}, {"password", password}};

	Str serv = strn("https://" + server + ".webuntis.com");
	serv.doNotDestroy = true;

	Str path = strn("/WebUntis/jsonrpc.do?school=" + schoolName);

	RPCClient::Set(serv.c_str(), path.c_str());

	json result = RPCClient::Request("authenticate", params)["result"];

	u.sessKey = permString(result["sessionId"].get<string>());
	u.pID = result["personId"].get<int>();
	u.pType = result["personType"].get<int>();
}

void GetLessonsFor(int date, timetable& timetable, const untis& inst) {
	json params = {{"options",
					{
						{"element",
						 {
							 {"type", inst.pType},
							 {"id", inst.pID},
						 }},
						{"showSubstText", true},
						{"showLsText", true},
						{"roomFields", {"name"}},
						{"subjectFields", {"name"}},
						{"teacherFields", {"name"}},
					}}};

	if(date != 0) {
		params["options"]["startDate"] = date;
		params["options"]["endDate"] = date;
	}

	RPCClient::Path("/WebUntis/jsonrpc.do");
	json result = RPCClient::Request("getTimetable", params, {{"Cookie", str("JSESSIONID=" + inst.sessKey)}})["result"];

	int resSize = result.size();

	lesson* array = new lesson[resSize];
	for(int i = 0; i < resSize; i++) {
		array[i] = {0};
		json& el = result[i];
		array[i].subject = permString(el["su"][0]["name"].get<string>());
		array[i].room = permString(el["ro"][0]["name"].get<string>());
		array[i].startTime = fixTime(el["startTime"].get<int>());
		array[i].endTime = fixTime(el["endTime"].get<int>());
	}

	timetable.date = date;
	timetable.lessons = array;
	timetable.lessonsAmt = resSize;
}