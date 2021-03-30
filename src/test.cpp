#include "creds.h"
#include "str.h"
#include "untis.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const* argv[]) {
	untis u = {0};
	CreateUntisInst(u, SRV, SCHOOL, USER, PASS);
	timetable t = {0};
	GetLessonsFor(0, t, u);
	for(int i = 0; i < t.lessonsAmt; i++) {
		const lesson l = t.lessons[i];
		printf("%s in Raum %s von %s bis %s\n", l.subject, l.room, l.startTime, l.endTime);
	}

	system("pause");
	return 0;
}
