#pragma once
#ifndef _TIMECOUT_H
#define _TIMECOUT_H

#define TimeCout _TimeCout() 

#include <sstream>
#include <iostream>

class _TimeCout {

private:
	std::stringstream s;
	static double m_startTime;
	
public:
	_TimeCout();
	~_TimeCout();
	
	template<typename T>
	_TimeCout& operator<< (const T &data) {
		s << data;
		return *this;
	}
};

#endif