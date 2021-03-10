#include "creds.h"
#include "untis.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const* argv[]) {
	untis u(SRV, SCHOOL, USER, PASS);
	timetable t = u.getLessons();
	for(int i = 0; i < t.get_lessonsAmt(); i++){
		lesson l = *t[i];
		printf("%s in Raum %s von %s bis %s\n", l.get_subject(), l.get_room(), l.get_startTime(), l.get_endTime());
	}
	system("pause");
	return 0;
}
