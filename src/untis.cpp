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

class Timetable : public ITimetable {
  private:
	ILesson* lessons;
	int lessonsAmt;
	int date;

  public:
	Timetable(ILesson* lessonArray, int lenght, int date) {
		lessons = lessonArray;
		lessonsAmt = lenght;
		this->date = date;
	}

	int LessonAmt() { return lessonsAmt; }

	const ILesson& operator[](unsigned index) {
		if(index >= lessonsAmt)
			throw;
		return lessons[index];
	}
};

class Untis : public IUntis {
  private:
	cstr sessKey;
	cstr server;
	int pType;
	int pID;

  public:
	Untis(cstr server, cstr schoolName, cstr user, cstr password) {
		json params = {{"user", user}, {"password", password}};
		json result = RPCReq(str(server + ".webuntis.com"), str("/WebUntis/jsonrpc.do?" + uriComponent("school", schoolName)),
							 "authenticate", params)["result"];

		sessKey = permString(result["sessionId"].get<string>());
		this->server = server;
		pID = result["personId"].get<int>();
		pType = result["personType"].get<int>();
	}

	cstr GetServer() { return server; }

	ITimetable* TimetableFor(int date) {
		json params = {{"options",
						{
							{"element",
							 {
								 {"type", pType},
								 {"id", pID},
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

		json result = RPCReq(str(server + ".webuntis.com"), "/WebUntis/jsonrpc.do", "getTimetable", params,
							 str("Cookie: JSESSIONID=" + sessKey))["result"];

		int resSize = result.size();

		ILesson* array = new ILesson[resSize];
		for(int i = 0; i < resSize; i++) {
			array[i] = ILesson();
			json& el = result[i];
			array[i].isWeird = false;
			array[i].subject = permString(el["su"][0]["name"].get<string>());
			array[i].room = permString(el["ro"][0]["name"].get<string>());
			array[i].startTime = fixTime(el["startTime"].get<int>());
			array[i].endTime = fixTime(el["endTime"].get<int>());

			if(el.contains("code")) {
				array[i].isWeird = true;
				array[i].reason = permString(el["code"].get<string>());
			}
		}

		return new Timetable(array, resSize, date);
	}
};

bool CreateUntisInst(IUntis** u, cstr server, cstr schoolName, cstr user, cstr password) {
	*u = new Untis(server, schoolName, user, password);
	return true;
}