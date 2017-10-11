#pragma once

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
	: Pr(W|S)
	*/
	float GetWordAppearsInSpam() const;

	/**
	: [float]
	: Returns the probability that a given word does not appear in spam messages
	: ~Pr(W|S)
	*/
	float GetWordNotAppearsInSpam() const;

	/**
	: [float]
	: Returns the probability that the given message you're checking is spam
	: Pr(S|W)
	*/
	float GetGivenMsgIsSpam() const;

	/**
	: Sets both the m_AnyMsgIsSpam value and m_AnyMsgIsNotSpam value
	*/
	void SetAnyMsgIsSpam(float isSpam);

	/**
	: Sets both the m_WordAppearsInSpam value and m_WordNotAppearsInSpam value
	*/
	void SetWordAppearsInSpam(float inSpam);

	/**
	: Performs the Bayesian Theorem calculation to determine the probability that
	message being checked is spam and assigns it to m_GivenMsgIsSpam variable
	*/
	void CalculateTheorem();

private:
	float Numerator();
	float Denominator();

	float m_AnyMsgIsSpam;
	float m_AnyMsgIsNotSpam;
	float m_WordAppearsInSpam;
	float m_WordNotAppearsInSpam;

	float m_GivenMsgIsSpam;
};