/* airportSimulation.cpp - to simulate an airport landing and takeoff pattern
�й�: 201724650
�̸�: �輱��
github id: Aromdami
*/
/*
airport�� 3���� runway1,2,3
airplane�� 4���� ��� queue ���� �ϳ����� ����Ѵ�. runway1�� 2���� ��� queue, runway2�� ���Ͽ� 2���� ��� queue - �ִ� 3��
- arriving airplane�� 4���� ��� queue ���� �ϳ��� Add(id, ����� ���Ͽ� ��� ������ �ð�), ��������� 4���� queue�� ���̴� ���� ����ϰ� ����
-  3���� Ȱ�ַ� ������ ���Ͽ� ���� takeoff queue :  3���� takeoff queue(size�� ���� ����ϰ�) - �ִ� 3��
- �� runway�� �� time slot�� takeoff �Ǵ� landing
- runway3�� �ַ� takeoff�� ���, �� low fuel�� ��쿡�� landing(���������� ����)
- landing ��� queue�� �ִ� ������� low fuel�̸� high priority �ο��ϰ� runway3�� ���, ���� ���� ������ low fuel�� ���� �߻��ÿ� �ٸ� runway�� ���, ��
  time slot���� 3�븸 ���
  - id �ο� ���: landing�� �������� odd ����, takeoff�� even ��ȣ �ο�
- ���
 1) �� time slot�� ���¸� display - 1) �� queue�� ����, 2) ��� �̷� ��� �ð�, 3) ��� ���� ��� �ð�, 4) ������ �ܿ� ��� ���� �ð�, 5) no fuel ������ ����� ���
- �Է�: ���� ���, �� time slot���� �̷� queue�� �����ϴ� ����� ���, ���� queue�� �����ϴ� ����� ���, ���� queue�� �����ϴ� ������� �ܿ� ���� ���� �ð�
*/
#include "queue.h"
#include <iomanip>
#define DEBUG
using namespace std;

struct randomInput {
	int timeUnit;
	int numPlanesTakeOff;
	int numPlanesLanding;
	int* remainingFlyTime;
};
struct LandingPlane {
	int arrivalTime;
	int IDofLandingPlane;
	int remainingFlyingTime;
	bool operator==(LandingPlane b) {
		return IDofLandingPlane == b.IDofLandingPlane;
	}
};
struct TakeoffPlane {
	int takeoffTime;
	int IDofTakeoffPlane;
	bool operator==(TakeoffPlane b) {
		return IDofTakeoffPlane == b.IDofTakeoffPlane;
	}
};
struct UseRunway {
	//Ȱ�ַ� ��� ���
	bool takeoff_landing;
	int IDPlane;
	int start;
	int end;
};
const int MAXWAITINGPLANES = 3;
const int LANDINGTIME = 3;

int generateInputData(struct randomInput*);//���������, �̷������ ��� queue�� ����

void landingEnqueue(randomInput& planes, Queue<LandingPlane>* landingQ);
void takeoffEnqueue(randomInput& planes, Queue<TakeoffPlane>* landingQ);
int findSmallLandingQueue(Queue<LandingPlane>* landingQueue);
int findSmallTakeoffQueue(Queue<TakeoffPlane>* takeoffQueue);

int landingProcedure(Queue<LandingPlane>* landingQ, int pivot, int currentTime, randomInput& planes);
int takeOffProcedure(Queue<TakeoffPlane>* takeOff, int pivot, int currentTime);


bool isAllEmpty(Queue<LandingPlane>* landingQ);
bool isAllEmpty(Queue<TakeoffPlane>* takeoffQ);
void showQueueStatus(Queue<LandingPlane>* landingQueue);
void showQueueStatus(Queue<TakeoffPlane>* takeoffQueue);
void recordOfRunways();
void recordOfRunways(int currentTime, Queue<LandingPlane>* landingQ, Queue<TakeoffPlane>* takeoffQ);
void selectRunways(int piv, Queue<TakeoffPlane>* takeoffQ, int currentTime);
void selectRunways(int piv, Queue<LandingPlane>* landingQ, int currentTime, randomInput& planes);

/*Global Variables (Planes, index of Runways)*/
LandingPlane* LPs;
TakeoffPlane* TPs;
static int index1 = -1, index2 = -1, index3 = -1, crash = 0;
static int R1End = 0, R2End = 0, R3End = 0;
static int lnd = 0, toff = 0, landcomplete = 0, flightcomplete = 0, nofuel = 0;
static double landed = 0, tookoff = 0, remTime = 0;

struct UseRunway R1[100], R2[100], R3[100];
int main()
{
	struct randomInput planes;
	Queue<LandingPlane> landingQ[4];
	Queue<TakeoffPlane> takeoffQ[3];
	int nop = generateInputData(&planes);
	int nol = nop - planes.numPlanesTakeOff;
	int currentTime = planes.timeUnit;

	/*Selecting RUNWAYS*/
	while (1)
	{
		landingEnqueue(planes, landingQ);
		takeoffEnqueue(planes, takeoffQ);
		int landP = -1;
		int flightP = -1;

		selectRunways(landP, landingQ, currentTime, planes);
		selectRunways(flightP, takeoffQ, currentTime);

		for (int i = 0; i < nol; i++)
		{
			planes.remainingFlyTime[i] --;
			if (planes.remainingFlyTime[i] < 6 && planes.remainingFlyTime[i] > 0)
				nofuel++;
		}
		recordOfRunways(currentTime++, landingQ, takeoffQ);
		nofuel = 0;
		if (isAllEmpty(landingQ) && isAllEmpty(takeoffQ))
			break;
	}
	while (currentTime <= R1End || currentTime <= R2End || currentTime <= R3End)
		recordOfRunways(currentTime++, landingQ, takeoffQ);
	recordOfRunways(currentTime, landingQ, takeoffQ);
	//recordOfRunways();
	return 0;
}

/*==========================================================================*/
/*================================FUNCTIONS=================================*/
/*==========================================================================*/
bool isAllEmpty(Queue<LandingPlane>* landingQ)
{
	int result = 0;
	for (int i = 0; i < 4; i++)
		result += landingQ[i].IsEmpty();

	if (result == 4)
		return true;

	else
		return false;
}

bool isAllEmpty(Queue<TakeoffPlane>* takeoffQ)
{
	int result = 0;
	for (int i = 0; i < 3; i++)
		result += takeoffQ[i].IsEmpty();

	if (result == 3)
		return true;

	else
		return false;
}

int generateInputData(struct randomInput* random)
{
	random->timeUnit = rand() % 2400 + 1;
	random->numPlanesLanding = rand() % 20 + 1;
	random->numPlanesTakeOff = rand() % 20 + 1;
	random->remainingFlyTime = new int[random->numPlanesLanding];
	LPs = new LandingPlane[random->numPlanesLanding];
	TPs = new TakeoffPlane[random->numPlanesTakeOff];

	for (int i = 0; i < random->numPlanesLanding; i++)
	{
#ifdef DEBUG
		LPs[i].arrivalTime = random->timeUnit;
		LPs[i].IDofLandingPlane = i * 2 + 1;
		LPs[i].remainingFlyingTime = rand() % 50 + 1;
		random->remainingFlyTime[i] = LPs[i].remainingFlyingTime;
#endif
#ifndef DEBUG
		LPs[i].arrivalTime = random->timeUnit;
		LPs[i].IDofLandingPlane = i * 2 + 1;
		cout << "Enter the remaining Flying time of Plane[" << LPs[i].IDofLandingPlane
			<< "]" << endl << ">>";
		cin >> LPs[i].remainingFlyingTime;
		random->remainingFlyTime[i] = LPs[i].remainingFlyingTime;
#endif
	}

	for (int i = 0; i < random->numPlanesTakeOff; i++)
	{
		TPs[i].takeoffTime = rand() % 3 + 1;
		TPs[i].IDofTakeoffPlane = i * 2 + 2;
	}
	return random->numPlanesLanding + random->numPlanesTakeOff;
}

void landingEnqueue(randomInput& planes, Queue<LandingPlane>* landingQ)
{
	while (planes.numPlanesLanding > 0)
	{
		if (((landingQ[0].Size() == MAXWAITINGPLANES) && (landingQ[1].Size() == MAXWAITINGPLANES)
			&& (landingQ[2].Size() == MAXWAITINGPLANES) && (landingQ[3].Size() == MAXWAITINGPLANES)))
			break;

		int avg = (landingQ[0].Size() + landingQ[1].Size() + landingQ[2].Size() + landingQ[3].Size()) / 4;
		int piv = rand() % 4;
		while (landingQ[piv].Size() > avg || landingQ[piv].Size() > MAXWAITINGPLANES)
			piv = rand() % 4;

		landingQ[piv].Push(LPs[lnd]);
		planes.numPlanesLanding--;
		lnd++;
	}
}

void takeoffEnqueue(randomInput& planes, Queue<TakeoffPlane>* takeoffQ)
{
	while (planes.numPlanesTakeOff > 0)
	{
		if (((takeoffQ[0].Size() == MAXWAITINGPLANES) && (takeoffQ[1].Size() == MAXWAITINGPLANES)
			&& (takeoffQ[2].Size() == MAXWAITINGPLANES)))
			break;

		int avg = (takeoffQ[0].Size() + takeoffQ[1].Size() + takeoffQ[2].Size()) / 3;
		int piv = rand() % 3;
		while (takeoffQ[piv].Size() > avg || takeoffQ[piv].Size() > MAXWAITINGPLANES)
			piv = rand() % 3;

		takeoffQ[piv].Push(TPs[toff]);
		planes.numPlanesTakeOff--;
		toff++;
	}
}
void showQueueStatus(Queue<LandingPlane>* landingQueue)
{
	cout << endl << "[Current Landing Queue Status]" << endl;
	for (int q = 0; q < 4; q++)
	{
		cout << "|QUEUE" << q << "|" << endl;
		if ((landingQueue + q)->Size() == 0)
			cout << "Empty" << endl << endl;
		for (int i = 0; i < (landingQueue + q)->Size(); i++)
		{
			cout << "Plane ID           : " << (landingQueue + q)->Find(i).IDofLandingPlane << endl
				<< "Arrival Time       : " << setw(4) << setfill('0') << (landingQueue + q)->Find(i).arrivalTime << endl
				<< "Remaining Fly Time : " << (landingQueue + q)->Find(i).remainingFlyingTime << endl << endl;
		}
	}
}
void showQueueStatus(Queue<TakeoffPlane>* takeoffQueue)
{
	cout << endl << "[Current Takeoff Queue Status]" << endl;
	for (int q = 0; q < 3; q++)
	{
		cout << "|QUEUE" << q << "|" << endl;
		if ((takeoffQueue + q)->Size() == 0)
			cout << "Empty" << endl << endl;
		for (int i = 0; i < (takeoffQueue + q)->Size(); i++)
		{
			cout << "Plane ID           : " << (takeoffQueue + q)->Find(i).IDofTakeoffPlane << endl
				<< "Takeoff Time       : " << setw(4) << setfill('0') << (takeoffQueue + q)->Find(i).takeoffTime << endl << endl;
		}
	}
}

int findSmallLandingQueue(Queue<LandingPlane>* landingQueue)
{
	int cmp = INT_MAX;
	int id = -1;
	int qno = -1;
	for (int q = 0; q < 4; q++)
	{
		for (int i = 0; i < (landingQueue + q)->Size(); i++)
		{
			if (cmp > 0 && cmp > (landingQueue + q)->Find(i).arrivalTime + (landingQueue + q)->Find(i).remainingFlyingTime)
			{
				cmp = (landingQueue + q)->Find(i).arrivalTime + (landingQueue + q)->Find(i).remainingFlyingTime;
				id = (landingQueue + q)->Find(i).IDofLandingPlane;
				qno = q;
			}
		}
	}
	return 1000 * qno + id;
}

int findSmallTakeoffQueue(Queue<TakeoffPlane>* takeoffQueue)
{

	int piv = rand() % 3;
	int avg = (takeoffQueue[0].Size() + takeoffQueue[1].Size() + takeoffQueue[2].Size()) / 3;
	while (takeoffQueue[piv].Size() < avg && takeoffQueue[piv].IsEmpty())
		piv = rand() % 3;

	if (takeoffQueue[piv].IsEmpty())
		return -1;

	return piv;
}

int landingProcedure(Queue<LandingPlane>* landingQ, int pivot, int currentTime, randomInput& planes)
{
	int ind = 0;
	LandingPlane land;
	land.IDofLandingPlane = pivot % 1000;

	land = landingQ[pivot / 1000].searchPop(land);

	if (index1 == -1 || currentTime > R1End)
	{
		if (currentTime > land.arrivalTime + land.remainingFlyingTime + LANDINGTIME)
			goto landFailure;
		if (R1End < currentTime + LANDINGTIME)
			R1End = currentTime + LANDINGTIME;
		index1++;
		R1[index1].takeoff_landing = 1;
		R1[index1].IDPlane = land.IDofLandingPlane;
		R1[index1].start = currentTime;
		R1[index1].end = currentTime + LANDINGTIME;
		ind++;
	}

	else if (index2 == -1 || currentTime > R2End)
	{
		if (currentTime > land.arrivalTime + land.remainingFlyingTime + LANDINGTIME)
			goto landFailure;
		if (R2End < currentTime + LANDINGTIME)
			R2End = currentTime + LANDINGTIME;
		index2++;
		R2[index2].takeoff_landing = 1;
		R2[index2].IDPlane = land.IDofLandingPlane;
		R2[index2].start = currentTime;
		R2[index2].end = currentTime + LANDINGTIME;
		ind++;
	}

	else if (land.remainingFlyingTime - (currentTime - land.arrivalTime) <= 3
		&& currentTime > R3End)
	{
		if (currentTime > land.arrivalTime + land.remainingFlyingTime + LANDINGTIME)
			goto landFailure;
		if (R3End < currentTime + LANDINGTIME)
			R3End = currentTime + LANDINGTIME;
		index3++;
		R3[index3].takeoff_landing = 1;
		R3[index3].IDPlane = land.IDofLandingPlane;
		R3[index3].start = currentTime;
		R3[index3].end = currentTime + LANDINGTIME;
		ind++;
	}

landFailure:
	if (ind == 0)
	{
		if (land.remainingFlyingTime + land.arrivalTime > currentTime + LANDINGTIME)
			landingQ[pivot / 1000].Push(land);
		else
			crash++;

		return 0;
	}

	landed += (double)(currentTime - land.arrivalTime + LANDINGTIME);
	remTime += (double)(land.remainingFlyingTime - (currentTime - land.arrivalTime + LANDINGTIME));
	planes.remainingFlyTime[(land.IDofLandingPlane - 1) / 2] = -1;
	landcomplete++;
	return 1;
}

int takeOffProcedure(Queue<TakeoffPlane>* takeOff, int pivot, int currentTime)
{
	int ind = 0;
	TakeoffPlane flight;
	flight = takeOff[pivot].Find(0);

	if (index3 == -1 || R3End < currentTime || (R3[index3].end > flight.takeoffTime + currentTime
		&& R3[index3].start < flight.takeoffTime + currentTime && R3[index3].takeoff_landing == 1))
	{
		flight = takeOff[pivot].Pop();
		if (R3End < currentTime + flight.takeoffTime)
			R3End = currentTime + flight.takeoffTime;
		index3++;
		R3[index3].takeoff_landing = 0;
		R3[index3].IDPlane = flight.IDofTakeoffPlane;
		R3[index3].start = currentTime;
		R3[index3].end = currentTime + flight.takeoffTime;
		ind++;
	}
	else if (index1 == -1 || R1End < currentTime ||
		(R1[index1].end > flight.takeoffTime + currentTime && R1[index1].start < currentTime
			&& R1[index1].takeoff_landing == 1))
	{
		flight = takeOff[pivot].Pop();
		if (R1End < currentTime + flight.takeoffTime)
			R1End = currentTime + flight.takeoffTime;
		index1++;
		R1[index1].takeoff_landing = 0;
		R1[index1].IDPlane = flight.IDofTakeoffPlane;
		R1[index1].start = currentTime;
		R1[index1].end = currentTime + flight.takeoffTime;
		ind++;
	}
	else if (index2 == -1 || R2End < currentTime ||
		(R2[index2].end > flight.takeoffTime + currentTime && R2[index2].start < currentTime
			&& R2[index2].takeoff_landing == 1))
	{
		flight = takeOff[pivot].Pop();
		if (R2End < currentTime + flight.takeoffTime)
			R2End = currentTime + flight.takeoffTime;
		index2++;
		R2[index2].takeoff_landing = 0;
		R2[index2].IDPlane = flight.IDofTakeoffPlane;
		R2[index2].start = currentTime;
		R2[index2].end = currentTime + flight.takeoffTime;
		ind++;
	}

	if (ind == 0)
		return 0;

	tookoff += (double)(flight.takeoffTime);
	flightcomplete++;
	return 1;
}

void selectRunways(int landP, Queue<LandingPlane>* landingQ, int currentTime, randomInput& planes)
{
	if (!isAllEmpty(landingQ))
	{
		while (1)
		{
			landP = findSmallLandingQueue(landingQ);
			//cout << "Recieved Value is " << pivot << endl;
			if (landP == -1001)
				break;
			if (landingProcedure(landingQ, landP, currentTime, planes) == 0)
				break;
		}
	}
}

void selectRunways(int flightP, Queue<TakeoffPlane>* takeoffQ, int currentTime)
{
	if (!isAllEmpty(takeoffQ))
	{
		while (1)
		{
			//showQueueStatus(takeoffQ);
			flightP = findSmallTakeoffQueue(takeoffQ);
			if (flightP == -1)
				break;
			if (takeOffProcedure(takeoffQ, flightP, currentTime) == 0)
				break;
		}
	}

}

void recordOfRunways()
{
	/*OUTPUT OF RUNWAYS*/
	cout << "===================================================" << endl;
	cout << "             FULL Record of 3 Runways" << endl;
	cout << "===================================================" << endl;
	cout << "Total " << landcomplete << " Planes landed, and " << flightcomplete << " took off." << endl
		<< crash << " Plane(s) crashed." << endl;
	/*For Runway 1*/
	cout << endl << "[Runway #1]" << endl;
	if (index1 == -1)
		cout << "Not used" << endl;
	else
	{
		for (int i = 0; i <= index1; i++)
		{
			cout << "Record No." << i + 1;
			if (R1[i].takeoff_landing == 0)	cout << "(TakeOff)";
			else							cout << "(Landing)";
			cout << endl
				<< "Plane ID         : " << R1[i].IDPlane << endl
				<< "Used Runway From : " << setw(4) << setfill('0') << R1[i].start << endl
				<< "              to : " << setw(4) << setfill('0') << R1[i].end << endl << endl;
		}
	}
	/*For Runway 2*/
	cout << endl << "[Runway #2]" << endl;
	if (index2 == -1)
		cout << "Not used" << endl;
	else
	{
		for (int i = 0; i <= index2; i++)
		{
			cout << "Record No." << i + 1;
			if (R2[i].takeoff_landing == 0)	cout << "(TakeOff)";
			else							cout << "(Landing)";
			cout << endl
				<< "Plane ID         : " << R2[i].IDPlane << endl
				<< "Used Runway From : " << setw(4) << setfill('0') << R2[i].start << endl
				<< "              to : " << setw(4) << setfill('0') << R2[i].end << endl << endl;
		}
	}
	/*For Runway 3*/
	cout << endl << "[Runway #3]" << endl;
	if (index3 == -1)
		cout << "Not used" << endl;
	else
	{
		for (int i = 0; i <= index3; i++)
		{
			cout << "Record No." << i + 1;
			if (R3[i].takeoff_landing == 0)	cout << "(TakeOff)";
			else							cout << "(Landing)";
			cout << endl
				<< "Plane ID         : " << R3[i].IDPlane << endl
				<< "Used Runway From : " << setw(4) << setfill('0') << R3[i].start << endl
				<< "              to : " << setw(4) << setfill('0') << R3[i].end << endl << endl;
		}
	}
}

void recordOfRunways(int currentTime, Queue<LandingPlane>* landingQ, Queue<TakeoffPlane>* takeoffQ)
{
	/*
	1) �� queue�� ����, 2) ��� �̷� ��� �ð�,
	3) ��� ���� ��� �ð�, 4) ������ �ܿ� ��� ���� �ð�, 5) no fuel ������ ����� ���
	*/

	/*OUTPUT OF RUNWAYS by TIME*/

	cout << "===================================================" << endl;
	cout << "          Record of 3 Runways at " << setw(4) << setfill('0') << currentTime << endl;
	cout << "===================================================" << endl;

	//OUTPUT CONDITION 1//
	showQueueStatus(landingQ);
	showQueueStatus(takeoffQ);
	cout << "---------------------------------------------------" << endl;
	//OUTPUT CONDITION 2//
	cout << "Average waiting time for take off     : ";
	if (index1 + index2 + index3 == -3)
		cout << "0" << endl;
	else
		cout << tookoff / flightcomplete << endl;
	//OUTPUT CONDITION 3//
	cout << "Average waiting time for landing      : ";
	if (index1 + index2 + index3 == -3)
		cout << "0" << endl;
	else
		cout << landed / landcomplete << endl;
	//OUTPUT CONDITION 4//
	cout << "Average remaining time after landing  : ";
	if (index1 + index2 + index3 == -3)
		cout << "0" << endl;
	else
		cout << remTime / landcomplete << endl;
	//OUTPUT CONDITION 5//
	cout << "Number of planes with no fuel status  : " << nofuel << endl;
	cout << "---------------------------------------------------" << endl;

	/*For Runway 1*/
	cout << endl << "[Runway #1]" << endl;
	if (R1[index1].start <= currentTime && R1[index1].end >= currentTime)
	{
		cout << "Record No." << index1 + 1;
		if (R1[index1].takeoff_landing == 0)	cout << "(TakeOff)";
		else							cout << "(Landing)";
		cout << endl
			<< "Plane ID         : " << R1[index1].IDPlane << endl
			<< "Used Runway From : " << setw(4) << setfill('0') << R1[index1].start << endl
			<< "              to : " << setw(4) << setfill('0') << R1[index1].end << endl << endl;
	}
	else
		cout << "Not used" << endl;

	/*For Runway 2*/
	cout << endl << "[Runway #2]" << endl;
	if (R2[index2].start <= currentTime && R2[index2].end >= currentTime)
	{
		cout << "Record No." << index2 + 1;
		if (R2[index2].takeoff_landing == 0)	cout << "(TakeOff)";
		else							cout << "(Landing)";
		cout << endl
			<< "Plane ID         : " << R2[index2].IDPlane << endl
			<< "Used Runway From : " << setw(4) << setfill('0') << R2[index2].start << endl
			<< "              to : " << setw(4) << setfill('0') << R2[index2].end << endl << endl;
	}
	else
		cout << "Not used" << endl;
	/*For Runway 3*/
	cout << endl << "[Runway #3]" << endl;
	if (R3[index3].start <= currentTime && R3[index3].end >= currentTime)
	{
		cout << "Record No." << index3 + 1;
		if (R3[index3].takeoff_landing == 0)	cout << "(TakeOff)";
		else							cout << "(Landing)";
		cout << endl
			<< "Plane ID         : " << R3[index3].IDPlane << endl
			<< "Used Runway From : " << setw(4) << setfill('0') << R3[index3].start << endl
			<< "              to : " << setw(4) << setfill('0') << R3[index3].end << endl << endl;
	}
	else
		cout << "Not used" << endl;
}