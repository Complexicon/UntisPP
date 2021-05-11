#ifndef UNTIS_H_
#define UNTIS_H_

#ifdef BUILD_EXPORT
#define EXPORTED extern "C" __declspec(dllexport)
#else
#define EXPORTED extern "C" __declspec(dllimport)
#endif

#define API __cdecl

typedef const char* cstr; // lazy

class ILesson {
  public:
	cstr subject;
	cstr room;
	cstr startTime;
	cstr endTime;
	bool isWeird;
	cstr reason;
};

class ITimetable {
  public:
	virtual int API LessonAmt() = 0;
	virtual const ILesson& API operator[](unsigned index) = 0;
};

class IUntis {
  public:
	virtual cstr API GetServer() = 0;
	virtual ITimetable* API TimetableFor(int date = 0) = 0;
};

#ifdef DYNAMIC_LOAD

#include <windows.h>

inline bool CreateUntisInst(IUntis** inst, cstr server, cstr schoolName, cstr user, cstr password) {
	HINSTANCE dll = LoadLibraryA("untis32.dll");
	if(!dll)
		return false;
	return ((bool (*)(IUntis**, cstr, cstr, cstr, cstr))GetProcAddress(dll, "CreateUntisInst"))(inst, server, schoolName, user,
																								password);
}

#else

EXPORTED bool CreateUntisInst(IUntis** inst, cstr server, cstr schoolName, cstr user, cstr password);

#endif
#endif