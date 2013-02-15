#ifndef HANDMONITOR_H
#define HANDMONITOR_H

class HandMonitor
{
public:
	HandMonitor();
	~HandMonitor();

	void init();
	void monitor();
private:
	bool checkTimeout(Uint32 &);

	float x,y,z;
	bool done;
};
#endif