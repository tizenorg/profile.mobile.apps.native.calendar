
#include "Utils/Logger.h"
#include "CalendarApp.h"

#include "tizen.h"


EXPORT_API int main(int argc, char *argv[])
{
	DBG("Start!");

	int retcode = Calendar::CalendarApp().run(argc, argv);

	DBG("Exit %d", retcode);
	DBG("Finish!");
}
