#include "untis.h"
#include "rpc.h"
#include "str.h"

cstr retardedStringFix(string s) {
	Str key(s.c_str());
	key.doNotDestroy = true;
	return key.c_str();
}

timetable::timetable(lesson* lessons, int amt, int date) : date(date), lessons(lessons), lessonsAmt(amt) {}

lesson* timetable::operator[](unsigned idx) { return idx < lessonsAmt ? &(lessons[idx]) : 0; }

void lesson::set(cstr subject, cstr room, int startTime, int endTime) {
	this->subject = subject;
	this->room = room;
	int hr = startTime / 100;
	int min = startTime - hr * 100;
	Str start;
	if(min < 10)
		start = strn(hr + ":0" + min);
	else
		start = strn(hr + ":" + min);
	start.doNotDestroy = true;
	hr = endTime / 100;
	min = endTime - hr * 100;
	Str end;
	if(min < 10)
		end = strn(hr + ":0" + min);
	else
		end = strn(hr + ":" + min);
	end.doNotDestroy = true;
	this->startTime = start.c_str();
	this->endTime = end.c_str();
}

untis::untis(cstr server, cstr schoolName, cstr user, cstr password) {
	json params = {{"user", user}, {"password", password}};

	Str serv = strn("https://" + server + ".webuntis.com");
	serv.doNotDestroy = true;

	Str path = strn("/WebUntis/jsonrpc.do?school=" + schoolName);

	RPCClient::Set(serv.c_str(), path.c_str());

	json result = RPCClient::Request("authenticate", params)["result"];

	sessKey = retardedStringFix(result["sessionId"].get<string>());
	pID = result["personId"].get<int>();
	pType = result["personType"].get<int>();
}

timetable untis::getLessons(int date) {
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

	RPCClient::Path("/WebUntis/jsonrpc.do");
	json result = RPCClient::Request("getTimetable", params, {{"Cookie", str("JSESSIONID=" + sessKey)}})["result"];
	DWORD written;
	const char* str = result.dump(4).c_str();
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), (void*)str, cmplx::strlen(str), &written, 0);
	lesson* array = new lesson[result.size()];
	for(int i = 0; i < result.size(); i++) {
		json& el = result[i];
		cstr subject = retardedStringFix(el["su"][0]["name"].get<string>());
		cstr room = retardedStringFix(el["ro"][0]["name"].get<string>());
		array[i].set(subject, room, el["startTime"].get<int>(), el["endTime"].get<int>());
	}

	return timetable(array, result.size(), date);
}