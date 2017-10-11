#include "Bayesian.h"

Bayesian::Bayesian() :
	m_AnyMsgIsSpam(0.0f), m_AnyMsgIsNotSpam(0.0f), m_WordAppearsInSpam(0.0f),
	m_WordNotAppearsInSpam(0.0f), m_GivenMsgIsSpam(0.0f)
{
}

float Bayesian::GetAnyMsgIsSpam() const
{
	return m_AnyMsgIsSpam;
}

float Bayesian::GetAnyMsgIsNotSpam() const
{
	return m_AnyMsgIsNotSpam;
}

float Bayesian::GetWordAppearsInSpam() const
{
	return m_WordAppearsInSpam;
}

float Bayesian::GetWordNotAppearsInSpam() const
{
	return m_WordNotAppearsInSpam;
}

float Bayesian::GetGivenMsgIsSpam() const
{
	return m_GivenMsgIsSpam;
}

void Bayesian::SetAnyMsgIsSpam(float isSpam)
{
	m_AnyMsgIsSpam = isSpam;
	m_AnyMsgIsNotSpam = 1 - isSpam;
}

void Bayesian::SetWordAppearsInSpam(float inSpam)
{
	m_WordAppearsInSpam = inSpam;
	m_WordNotAppearsInSpam = 1 - inSpam;
}

void Bayesian::CalculateTheorem()
{
	m_GivenMsgIsSpam = Numerator() / Denominator();
}

/**
: [float]
: Returns the Numerator of the Bayesian Theorem calculation
: Pr(W|S) * Pr(S)
*/
float Bayesian::Numerator()
{
	return m_WordAppearsInSpam * m_AnyMsgIsSpam;
}

/**
: [float]
: Returns the Denominator of the Bayesian Theorem calculation
: Pr(W|S) * Pr(S) + ~Pr(W|S) * ~Pr(S)
*/
float Bayesian::Denominator()
{
	return Numerator() + (m_WordNotAppearsInSpam * m_AnyMsgIsNotSpam);
}
