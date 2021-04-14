#ifndef UNTIS_H_
#define UNTIS_H_

#ifdef BUILD_EXPORT
#define EXPORTED extern "C" __declspec(dllexport)
#else
#define EXPORTED extern "C" __declspec(dllimport)
#endif

typedef const char* cstr; // lazy

typedef struct lesson_struct {
	cstr subject;
	cstr room;
	cstr startTime;
	cstr endTime;
	bool isWeird;
	cstr reason;
} lesson;

typedef struct timetable_struct {
	lesson* lessons;
	int lessonsAmt;
	int date;
} timetable;

typedef struct untis_struct {
	cstr sessKey;
	cstr server;
	int pType;
	int pID;
} untis;

EXPORTED void CreateUntisInst(untis& inst, cstr server, cstr schoolName, cstr user, cstr password);
EXPORTED void GetLessonsFor(int date, timetable& timetable, const untis& inst);

#endif