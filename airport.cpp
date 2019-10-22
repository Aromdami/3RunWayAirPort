#include "queue.h"
#include "airport.h"
using namespace std;

int main()
{
	Airport airport;
	struct randomInput planes;
	Queue<LandingPlane> landingQ[4];
	Queue<TakeoffPlane> takeoffQ[3];
	int nol = airport.generateInputData(&planes);

	airport.showtimeslot(planes, landingQ, takeoffQ, planes.timeUnit, nol);

	return 0;
}