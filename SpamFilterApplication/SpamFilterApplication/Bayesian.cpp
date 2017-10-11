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

float Bayesian::GetGivenMsgIsSpamME() const
{
	return m_GivenMsgIsSpamME;
}

std::vector<float> Bayesian::GetWordAppearsInSpamList() const
{
	return m_WordAppearsInSpamList;
}

std::vector<float> Bayesian::GetWordNotAppearsInSpamList() const
{
	return m_WordNotAppearsInSpamList;
}

void Bayesian::AddToWordAppearsInSpamList(float amount)
{
	m_WordAppearsInSpamList.push_back(amount);
	m_WordNotAppearsInSpamList.push_back(1.0f - amount);
}

void Bayesian::SetAnyMsgIsSpam(float isSpam)
{
	m_AnyMsgIsSpam = isSpam;
	m_AnyMsgIsNotSpam = 1.0f - isSpam;
}

void Bayesian::SetWordAppearsInSpam(float inSpam)
{
	m_WordAppearsInSpam = inSpam;
	m_WordNotAppearsInSpam = 1.0f - inSpam;
}

void Bayesian::CalculateTheorem()
{
	m_GivenMsgIsSpam = Numerator() / Denominator();
}

void Bayesian::CalculateTheoremME()
{
	m_GivenMsgIsSpamME = NumeratorME() / DenominatorME();
}

/**
: [float]
: Returns the Numerator of the Bayesian Theorem calculation
: Pr(W|S) * Pr(S)
: (Single Event)
*/
float Bayesian::Numerator()
{
	return m_WordAppearsInSpam * m_AnyMsgIsSpam;
}

/**
: [float]
: Returns the Denominator of the Bayesian Theorem calculation
: Pr(W|S) * Pr(S) + ~Pr(W|S) * ~Pr(S)
: (Single Event)
*/
float Bayesian::Denominator()
{
	return Numerator() + (m_WordNotAppearsInSpam * m_AnyMsgIsNotSpam);
}

/**
: [float]
: Returns the Numerator of the Bayesian Theorem calculation (Needs to be checked)
: (Multiple Events)
*/
float Bayesian::NumeratorME()
{
	float tempNumerator = 1.0f;

	if (m_WordAppearsInSpamList.size() > 0)
	{
		for (int i = 0; i < m_WordAppearsInSpamList.size(); i++)
		{
			tempNumerator *= m_WordAppearsInSpamList.at(i);
		}
		tempNumerator *= m_AnyMsgIsSpam;
	}
	else
	{
		printf("%s", "Could not calculate numerator m_WordAppearsInSpamList is empty");
	}

	return tempNumerator;
}

/**
: [float]
: Returns the Denominator of the Bayesian Theorem calculation (Needs to be checked)
: (Multiple Events)
*/
float Bayesian::DenominatorME()
{
	float tempDenominator = 1.0f;
	
	if (m_WordNotAppearsInSpamList.size() > 0)
	{
		for (int i = 0; i < m_WordNotAppearsInSpamList.size(); i++)
		{
			tempDenominator *= m_WordNotAppearsInSpamList.at(i);
		}
		tempDenominator *= m_AnyMsgIsNotSpam;
		tempDenominator += NumeratorME();
	}
	else
	{
		printf("%s", "Could not calculate numerator m_WordNotAppearsInSpamList is empty");
	}

	return tempDenominator;
}
