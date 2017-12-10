#pragma once
#ifndef _BAYESIAN_H
#define _BAYESIAN_H

class Bayesian
{
public:
	Bayesian();
	
	/**
	: [float]
	: Returns the probability that any message is spam
	: Pr(S)
	*/
	float GetAnyMsgIsSpam() const;

	/**
	: [float]
	: Returns the probability that any message is not spam (is legitimate)
	: ~Pr(S)
	*/
	float GetAnyMsgIsNotSpam() const;

	/**
	: [float]
	: Returns the probability that a given word appears in spam messages
	: Pr(W|S) (For Single Event)
	*/
	float GetWordAppearsInSpam() const;

	/**
	: [float]
	: Returns the probability that a given word does not appear in spam messages
	: ~Pr(W|S) (For Single Event)
	*/
	float GetWordNotAppearsInSpam() const;

	/**
	: [float]
	: Returns the probability that the given message you're checking is spam
	: Pr(S|W) (For Single Event)
	*/
	float GetGivenMsgIsSpam() const;

	/**
	: [float]
	: Returns the probability that the given message you're checking is spam
	: (For Multiple Event)
	*/
	float GetGivenMsgIsSpamME() const;

	/**
	: [vector<float>]
	: Returns a list of probabilities that the corresponding given messages being checked
		are spam
	: (For Multiple Events)
	*/
	std::vector<float> GetWordAppearsInSpamList() const;

	/**
	: [vector<float>]
	: Returns a list of probabilities that the corresponding given messages being checked
		are not spam
	: (For Multiple Events)
	*/
	std::vector<float> GetWordNotAppearsInSpamList() const;

	/**
	: Appends <amount> to the m_WordAppearInSpamList and 1 - <amount> to the m_WordNotAppearInSpamList
	*/
	void AddToWordAppearsInSpamList(float amount);

	/**
	: Sets both the m_AnyMsgIsSpam value and m_AnyMsgIsNotSpam value
	*/
	void SetAnyMsgIsSpam(float isSpam);

	/**
	: Sets both the m_WordAppearsInSpam value and m_WordNotAppearsInSpam value
	: (For Single Event)
	*/
	void SetWordAppearsInSpam(float inSpam);

	/**
	: Performs the Bayesian Theorem calculation to determine the probability that
	message being checked is spam and assigns it to m_GivenMsgIsSpam variable
	: (For Single Event)
	*/
	void CalculateTheorem();

	/**
	: Performs the Bayesian Theorem calculation to determine the probability that
	message being checked is spam and assigns it to m_GivenMsgIsSpam variable
	: (For Multiple Events)
	*/
	void CalculateTheoremME();
	
private:
	void SetNumerator();
	void SetDenominator();

	void SetNumeratorME();
	void SetDenominatorME();

	float m_AnyMsgIsSpam;
	float m_AnyMsgIsNotSpam;
	float m_WordAppearsInSpam;
	float m_WordNotAppearsInSpam;

	std::vector<float> m_WordAppearsInSpamList;
	std::vector<float> m_WordNotAppearsInSpamList;

	float m_GivenMsgIsSpam;
	float m_GivenMsgIsSpamME;
	
	float m_BayesianNumerator;
	float m_BayesianDenominator;
	float m_BayesianNumeratorME;
	float m_BayesianDenominatorME;
};

#endif