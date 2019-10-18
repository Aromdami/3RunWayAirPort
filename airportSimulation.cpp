/* airportSimulation.cpp - to simulate an airport landing and takeoff pattern
학번: 201724650
이름: 김선우
github id: Aromdami
*/
/*
airport에 3개의 runway1,2,3
airplane는 4개의 대기 queue 중의 하나에서 대기한다. runway1에 2개의 대기 queue, runway2에 대하여 2개의 대기 queue - 최대 3대
- arriving airplane는 4개의 대기 queue 중의 하나에 Add(id, 연료로 인하여 대기 가능한 시간), 평균적으로 4개의 queue의 길이는 거의 비슷하게 유지
-  3개의 활주로 각각에 대하여 각각 takeoff queue :  3개의 takeoff queue(size는 거의 비슷하게) - 최대 3대
- 각 runway는 각 time slot에 takeoff 또는 landing
- runway3은 주로 takeoff에 사용, 단 low fuel일 경우에는 landing(예외적으로 적용)
- landing 대기 queue에 있는 비행기의 low fuel이면 high priority 부여하고 runway3을 사용, 일정 수준 이하의 low fuel이 동시 발생시에 다른 runway를 사용, 각
  time slot에서 3대만 허용
  - id 부여 방법: landing은 연속적인 odd 숫자, takeoff는 even 번호 부여
- 출력
 1) 각 time slot의 상태를 display - 1) 각 queue의 내용, 2) 평균 이륙 대기 시간, 3) 평균 착륙 대기 시간, 4) 착륙시 잔여 평균 비행 시간, 5) no fuel 상태의 비행기 댓수
- 입력: 난수 사용, 각 time slot에서 이륙 queue에 도달하는 비행기 댓수, 착륙 queue에 도달하는 비행기 댓수, 착륙 queue에 도달하는 비행기의 잔여 비행 가능 시간
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
	//활주로 사용 기록
	bool takeoff_landing;
	int IDPlane;
	int start;
	int end;
};
const int MAXWAITINGPLANES = 3;
const int LANDINGTIME = 3;

int generateInputData(struct randomInput*);//착륙비행기, 이륙비행기 대기 queue를 생성

void landingEnqueue(randomInput& planes, Queue<LandingPlane>* landingQ);
void takeoffEnqueue(randomInput& planes, Queue<TakeoffPlane>* landingQ);
int findSmallLandingQueue(Queue<LandingPlane>* landingQueue);
int findSmallTakeoffQueue(Queue<TakeoffPlane>* takeoffQueue);

int landingProcedure(Queue<LandingPlane>* landingQ, int pivot, int currentTime);
int takeOffProcedure(Queue<TakeoffPlane>* takeOff, int pivot, int currentTime);


bool isAllEmpty(Queue<LandingPlane>* landingQ);
bool isAllEmpty(Queue<TakeoffPlane>* takeoffQ);
void showQueueStatus(Queue<LandingPlane>* landingQueue);
void showQueueStatus(Queue<TakeoffPlane>* takeoffQueue);
void recordOfRunways();
void recordOfRunways(int currentTime);

/*Global Variables (Planes, index of Runways)*/
LandingPlane* LPs;
TakeoffPlane* TPs;
static int index1 = -1, index2 = -1, index3 = -1, crash = 0;
static int R1End = 0, R2End = 0, R3End = 0;
static int lnd = 0, toff = 0, landcomplete = 0, flightcomplete = 0;
struct UseRunway R1[100], R2[100], R3[100];
int main()
{
	struct randomInput planes;
	Queue<LandingPlane> landingQ[4];
	Queue<TakeoffPlane> takeoffQ[3]	;
	int nop = generateInputData(&planes);
	int currentTime = planes.timeUnit;
	
	
	/*Debugging : Generate Input Data*/
	/*
	{
		cout << "===================================================" << endl;
		cout << "          Test of Generate Input Data" << endl;
		cout << "===================================================" << endl;
		cout << "TimeUnit : " << setw(4) << setfill('0') << planes.timeUnit << endl
			<< "Number of Planes Taking off : " << planes.numPlanesTakeOff << endl
			<< "Number of Planes Landing    : " << planes.numPlanesLanding << endl
			<< "Array of Remaining Times    : ";
		for (int i = 0; i < planes.numPlanesLanding; i++)
			cout << planes.remainingFlyTime[i] << " ";
		cout << endl << endl;
		cout << "[List of Taking off Planes]" << endl;
		for (int i = 0; i < planes.numPlanesTakeOff; i++)
			cout << "Plane ID           : " << TPs[i].IDofTakeoffPlane << endl
			<< "TakeOff Time       : " << setw(4) << setfill('0') << TPs[i].takeoffTime << endl << endl;
		cout << "------------------------" << endl;
		cout << "[List of Landing Planes]" << endl;
		for (int i = 0; i < planes.numPlanesLanding; i++)
			cout << "Plane ID           : " << LPs[i].IDofLandingPlane << endl
			<< "Arrival Time       : " << setw(4) << setfill('0') << LPs[i].arrivalTime << endl
	 << "Remaining Fly Time : " << LPs[i].remainingFlyingTime << endl << endl;
	}
	*/
	/*Testing for distributed Queue*/
	/*
	cout << "===================================================" << endl;
	cout << "          Test of Enqueue Distribution" << endl;
	cout << "===================================================" << endl;
	for (int i = 0; i < 4; i++)
	{
		if (!landingQ[i].IsEmpty())
		{
			cout << "[Landing Planes for Q" << i << "]" << endl;
			ind = landingQ[i].Size();
			for (int j = 0; j < ind; j++)
			{
				LandingPlane temp;
				temp = landingQ[i].Pop();
				cout << "Plane ID           : " << temp.IDofLandingPlane << endl
					<< "Arrival Time       : " << setw(4) << setfill('0') << temp.arrivalTime << endl
					<< "Remaining Fly Time : " << temp.remainingFlyingTime << endl << endl;
			}
		}
	}
	*/
	
	/*Selecting RUNWAYS*/
	while (1)
	{
		landingEnqueue(planes, landingQ);
		takeoffEnqueue(planes, takeoffQ);
		int landP = -1;
		int flightP = -1;
		
		if (!isAllEmpty(landingQ))
		{
			while (1)
			{
				landP = findSmallLandingQueue(landingQ);
				//cout << "Recieved Value is " << pivot << endl;
				if (landP == -1001)
					break;
				if (landingProcedure(landingQ, landP, currentTime) == 0)
					break;
			}
		}

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

		recordOfRunways(currentTime++);

		if (isAllEmpty(landingQ) && isAllEmpty(takeoffQ))
			break;
	}
	while (currentTime <= R1End || currentTime <= R2End || currentTime <= R3End)
		recordOfRunways(currentTime++);
	recordOfRunways(currentTime);
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
		LPs[i].remainingFlyingTime = rand()% 50 + 1;
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
		TPs[i].takeoffTime = rand()%3 + 1;
		TPs[i].IDofTakeoffPlane = i * 2 + 2;
	}
	return random->numPlanesLanding + random->numPlanesTakeOff;
}

void landingEnqueue(randomInput &planes, Queue<LandingPlane> *landingQ)
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
		LPs[lnd].remainingFlyingTime--;
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
	cout <<endl<<"[Current Landing Queue Status]" << endl;
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
		for (int i = 0; i < (landingQueue+q)->Size(); i++)
		{
			if (cmp > 0 && cmp > (landingQueue+q)->Find(i).arrivalTime + (landingQueue+q)->Find(i).remainingFlyingTime)
			{
				cmp = (landingQueue+q)->Find(i).arrivalTime + (landingQueue+q)->Find(i).remainingFlyingTime;
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

int landingProcedure(Queue<LandingPlane>* landingQ, int pivot, int currentTime)
{
	int ind = 0;
	LandingPlane land;
	land.IDofLandingPlane = pivot % 1000;
	
	land = landingQ[pivot / 1000].searchPop(land);
	
	if (index1 == -1 || currentTime > R1End)
	{
		if (currentTime  > land.arrivalTime + land.remainingFlyingTime + LANDINGTIME)
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
		if (currentTime> land.arrivalTime + land.remainingFlyingTime + LANDINGTIME)
			goto landFailure;
		if (R2End < currentTime + LANDINGTIME)
			R2End = currentTime + LANDINGTIME;
		index2++;
		R2[index2].takeoff_landing = 1;
		R2[index2].IDPlane = land.IDofLandingPlane;
		R2[index2].start = currentTime;
		R2[index2].end = currentTime  + LANDINGTIME;
		ind++;
	}
	
	else if (land.remainingFlyingTime + land.arrivalTime- currentTime <= LANDINGTIME + 3 && currentTime > R3End)
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
		if (land.remainingFlyingTime + land.arrivalTime > currentTime)
			landingQ[pivot / 1000].Push(land);
		else
			crash++;
		
		return 0;
	}
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
	
	flightcomplete++;
	return 1;
}

void recordOfRunways()
{
	/*OUTPUT OF RUNWAYS*/
	cout << "===================================================" << endl;
	cout << "             FULL Record of 3 Runways" << endl;
	cout << "===================================================" << endl;
	cout << "Total " << landcomplete << " Planes landed, and "<< flightcomplete << " took off." << endl
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

void recordOfRunways(int currentTime)
{
	/*OUTPUT OF RUNWAYS by TIME*/
	static int landed = 0, tookoff = 0;
	cout << "===================================================" << endl;
	cout << "          Record of 3 Runways at " << setw(4) << setfill('0') << currentTime <<endl;
	cout << "===================================================" << endl;
	cout << "Total " << landcomplete << " Plane landing Records , and " << flightcomplete << " taking off." << endl
		<< crash << " Plane(s) crashed." << endl;

	/*For Runway 1*/
	cout << endl << "[Runway #1]" << endl;
	if (R1[index1].start <= currentTime && R1[index1].end >= currentTime )
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