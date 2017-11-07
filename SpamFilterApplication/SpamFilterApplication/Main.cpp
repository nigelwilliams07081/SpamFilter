#include "stdafx.h"
#include <time.h>

int main()
{
	SpamFilter spamFilter;
	Bayesian bayesianCalculator;

	spamFilter.PerformSpamSearch();

	bayesianCalculator.SetAnyMsgIsSpam(0.8f);
	bayesianCalculator.SetWordAppearsInSpam(spamFilter.GetOverallSpamConfidence());
	bayesianCalculator.CalculateTheorem();

	std::cout << "Our spam confidence is: " << spamFilter.GetOverallSpamConfidence() << " -> " << spamFilter.GetOverallSpamConfidence() * 100.0f << "%" << std::endl;
	std::cout << "The Bayesian confidence is: " << bayesianCalculator.GetGivenMsgIsSpam() << " -> " << bayesianCalculator.GetGivenMsgIsSpam() * 100.0f << "%" << std::endl;
	
	system("pause");
	return 0;
}