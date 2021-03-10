#ifndef UNTIS_H_
#define UNTIS_H_
#define getter(variable)                                                                                                       \
	get_##variable() { return variable; };

typedef const char* cstr;

/*

enum day {
		MONTAG,
		DIENSTAG,
		MITTWOCH,
		DONNERSTAG,
		FREITAG
};

*/

class lesson {
  private:
	cstr subject;
	cstr room;
	cstr startTime;
	cstr endTime;

  public:
	void set(cstr subject, cstr room, int startTime, int endTime);
	cstr getter(subject);
	cstr getter(room);
	cstr getter(startTime);
	cstr getter(endTime);
};

class timetable {
  private:
	lesson* lessons;
	int lessonsAmt;
	int date;

  public:
	int getter(date);
	int getter(lessonsAmt);
	timetable(lesson* lessons, int amt, int date);
	lesson* operator[](unsigned idx);
};

class untis {
  private:
	cstr sessKey;
	int pType;
	int pID;

  public:
	untis(cstr server, cstr schoolName, cstr user, cstr password);
	timetable getLessons(int date = 0);
};

#endif