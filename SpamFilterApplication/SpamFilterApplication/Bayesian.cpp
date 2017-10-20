#include "stdafx.h"

Bayesian::Bayesian() :
	m_BayesianNumerator(1.0f), m_BayesianDenominator(1.0f),
	m_BayesianNumeratorME(1.0f), m_BayesianDenominatorME(1.0f)
{
	m_AnyMsgIsSpam = 0.0f;
	m_AnyMsgIsNotSpam = 0.0f;
	m_WordAppearsInSpam = 0.0f;
	m_WordNotAppearsInSpam = 0.0f;
	m_GivenMsgIsSpam = 0.0f;
	m_GivenMsgIsSpamME = 0.0f;
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
	SetNumerator();
	SetDenominator();

	m_GivenMsgIsSpam = m_BayesianNumerator / m_BayesianDenominator;
}

void Bayesian::CalculateTheoremME()
{
	SetNumeratorME();
	SetDenominatorME();

	m_GivenMsgIsSpamME = m_BayesianNumeratorME / m_BayesianDenominatorME;
}

/**
: Sets the Numerator of the Bayesian Theorem calculation
: Pr(W|S) * Pr(S)
: (Single Event)
*/
void Bayesian::SetNumerator()
{
	m_BayesianNumerator = m_WordAppearsInSpam * m_AnyMsgIsSpam;
}

/**
: Sets the Denominator of the Bayesian Theorem calculation (MUST BE CALLED AFTER SetNumerator())
: Pr(W|S) * Pr(S) + ~Pr(W|S) * ~Pr(S)
: (Single Event)
*/
void Bayesian::SetDenominator()
{
	m_BayesianDenominator = m_BayesianNumerator + (m_WordNotAppearsInSpam * m_AnyMsgIsNotSpam);
}

/**
: Sets the Numerator of the Bayesian Theorem calculation
: (Multiple Events) *NEEDS TO BE CHECKED
*/
void Bayesian::SetNumeratorME()
{
	if (m_WordAppearsInSpamList.size() > 0)
	{
		for (int i = 0; i < static_cast<int>(m_WordAppearsInSpamList.size()); i++)
		{
			m_BayesianNumeratorME *= m_WordAppearsInSpamList.at(i);
		}
		m_BayesianNumeratorME *= m_AnyMsgIsSpam;
	}
	else
	{
		printf("%s", "Could not calculate numerator m_WordAppearsInSpamList is empty");
	}
}

/**
: Sets the Denominator of the Bayesian Theorem calculation (MUST BE CALLED AFTER SetNumeratorME())
: (Multiple Events) *NEEDS TO BE CHECKED
*/
void Bayesian::SetDenominatorME()
{
	if (m_WordNotAppearsInSpamList.size() > 0)
	{
		for (int i = 0; i < static_cast<int>(m_WordNotAppearsInSpamList.size()); i++)
		{
			m_BayesianDenominatorME *= m_WordNotAppearsInSpamList.at(i);
		}
		m_BayesianDenominatorME *= m_AnyMsgIsNotSpam;
		m_BayesianDenominatorME += m_BayesianNumeratorME;
	}
	else
	{
		printf("%s", "Could not calculate numerator m_WordNotAppearsInSpamList is empty");
	}
}
