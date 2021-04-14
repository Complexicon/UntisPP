#define BUILD_EXPORT
#include "untis.h"

#include "json.hpp"
using json = nlohmann::json;

#include "winreq.h"
#include "urihelper.h"
#include "str.h"

inline cstr permString(string s) { return eternalstr(s.c_str()); }

cstr fixTime(int time) {
	int hr = time / 100;
	int min = time - hr * 100;
	return eternalstr(hr + (min < 10 ? ":0" : ":") + min);
}

json RPCReq(cstr domain, cstr url, cstr method, const json& params, cstr extraHdr = "") {
	json request = {{"method", method}};
	request["id"] = "JRPC++";
	request["jsonrpc"] = "2.0";
	request["params"] = params;
	auto x = HttpsWebRequestPost(domain, url, request.dump().c_str(), extraHdr);

	json result = json::parse(x.c_str());

	// err detection

	return result;
}

void CreateUntisInst(untis& u, cstr server, cstr schoolName, cstr user, cstr password) {
	json params = {{"user", user}, {"password", password}};
	json result = RPCReq(str(server + ".webuntis.com"), str("/WebUntis/jsonrpc.do?" + uriComponent("school", schoolName)),
						 "authenticate", params)["result"];

	u.sessKey = permString(result["sessionId"].get<string>());
	u.server = server;
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
						{"showStudentgroup", true},
						{"roomFields", {"name"}},
						{"subjectFields", {"name"}},
						{"teacherFields", {"name"}},
					}}};

	if(date != 0) {
		params["options"]["startDate"] = date;
		params["options"]["endDate"] = date;
	}

	json result = RPCReq(str(inst.server + ".webuntis.com"), "/WebUntis/jsonrpc.do", "getTimetable", params,
						 str("Cookie: JSESSIONID=" + inst.sessKey))["result"];

	int resSize = result.size();

	lesson* array = new lesson[resSize];
	for(int i = 0; i < resSize; i++) {
		array[i] = {0};
		array[i].startTime = "invalid";
		array[i].endTime = "invalid";
		array[i].room = "invalid";
		array[i].teacher = "invalid";
		array[i].subject = "invalid";
		json& el = result[i];
		try {
			array[i].room = permString(el["ro"][0]["name"].get<string>());
			array[i].startTime = fixTime(el["startTime"].get<int>());
			array[i].endTime = fixTime(el["endTime"].get<int>());
			array[i].subject = permString(el["su"][0]["name"].get<string>()); // frau weber fix

			Str t = el["sg"].get<string>().c_str();
			t.substring(t.lastIndexOf('_') + 1);
			t.doNotDestroy = true;
			array[i].teacher = t.c_str();
		} catch(json::exception e) {}
	}

	timetable.date = date;
	timetable.lessons = array;
	timetable.lessonsAmt = resSize;
}