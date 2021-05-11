#include "creds.h"
#include "str.h"
#define DYNAMIC_LOAD
#include "untis.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const* argv[]) {
	IUntis* u;
	CreateUntisInst(&u, SRV, SCHOOL, USER, PASS);
	ITimetable* t = u->TimetableFor(0);
	for(int i = 0; i < t->LessonAmt(); i++) {
		const ILesson l = (*t)[i];
		if(l.isWeird)
			printf("[%s] ", l.reason);
		printf("%s in Raum %s von %s bis %s\n", l.subject, l.room, l.startTime, l.endTime);
	}

	system("pause");
	return 0;
}
