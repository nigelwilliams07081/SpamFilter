#include "stdafx.h"

double _TimeCout::m_startTime = 0.0;

_TimeCout::_TimeCout() {
	
	if (m_startTime == 0.0) {
		m_startTime = MPI::Wtime();
	}
	
	s << "[+" << std::fixed << std::setprecision(6) << (MPI::Wtime() - m_startTime) << "] ";
}

_TimeCout::~_TimeCout() {
	std::cout << s.str();
}