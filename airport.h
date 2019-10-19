#pragma once
#include "queue.h"
#include <iostream>

struct randomInput {
	int timeUnit;
	int numPlanesTakeOff;
	int numPlanesLanding;
	int* remainingFlyTime;
};

struct UseRunway {
	bool takeoff_landing;
	int IDPlane;
	int start;
	int end;
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

class Airport
{
public:
	Airport();
	Airport(int landingplanes, int takingoffplanes, int maxwaitingline = 3);
	~Airport(){}
	int generateInputData(struct randomInput* random);
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

private:
	LandingPlane* LPs;
	TakeoffPlane* TPs;
	UseRunway *R1, *R2, *R3;
	
	int MAXWAITINGPLANES;
	int index1, index2, index3, R1End, R2End, R3End;
	int lnd, toff, landcomplete, flightcomplete, nofuel, crash;
	double landed, tookoff, remTime;
};

Airport::Airport()
	:index1(-1), index2(-1), index3(-1), crash(0), R1End(0), R2End(0), R3End(0), lnd(0), toff(0), landcomplete(0), flightcomplete(0), nofuel(0),
	landed(0), tookoff(0), remTime(0), MAXWAITINGPLANES(0)
{
	R1 = new UseRunway[0];
	R2 = new UseRunway[0];
	R3 = new UseRunway[0];
	LPs = new LandingPlane[0];
	TPs = new TakeoffPlane[0];
}
Airport::Airport(int landingplanes, int takingoffplanes, int maxwaitingline)
	:index1(-1), index2(-1), index3(-1), crash(0), R1End(0), R2End(0), R3End(0), lnd(0), toff(0), landcomplete(0), flightcomplete(0), nofuel(0),
	landed(0), tookoff(0), remTime(0), MAXWAITINGPLANES(maxwaitingline)
{
	R1 = new UseRunway[maxwaitingline];
	R2 = new UseRunway[maxwaitingline];
	R3 = new UseRunway[maxwaitingline];
	LPs = new LandingPlane[landingplanes];
	TPs = new TakeoffPlane[takingoffplanes];
}

int generateInputData(struct randomInput* random)
{
	for (int i = 0; i < random->numPlanesLanding; i++)
	{
		LPs[i].arrivalTime = random->timeUnit;
		LPs[i].IDofLandingPlane = i * 2 + 1;
		LPs[i].remainingFlyingTime = rand() % 50 + 1;
		random->remainingFlyTime[i] = LPs[i].remainingFlyingTime;
		/*
		LPs[i].arrivalTime = random->timeUnit;
		LPs[i].IDofLandingPlane = i * 2 + 1;
		cout << "Enter the remaining Flying time of Plane[" << LPs[i].IDofLandingPlane
			<< "]" << endl << ">>";
		cin >> LPs[i].remainingFlyingTime;
		random->remainingFlyTime[i] = LPs[i].remainingFlyingTime;
		*/

	}

	for (int i = 0; i < random->numPlanesTakeOff; i++)
	{
		TPs[i].takeoffTime = rand() % 3 + 1;
		TPs[i].IDofTakeoffPlane = i * 2 + 2;
	}
	return random->numPlanesLanding + random->numPlanesTakeOff;
}