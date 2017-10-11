#include "SpamFilter.h"
#include "Bayesian.h"

int main()
{
	SpamFilter spamFilter;
	Bayesian bayesian;

	spamFilter.OpenFile("File.txt");
	
	bayesian.SetAnyMsgIsSpam(0.8f);
	bayesian.SetWordAppearsInSpam(0.3f);
	bayesian.CalculateTheorem();

	spamFilter.SetSpamConfidence(bayesian.GetGivenMsgIsSpam());
	if (spamFilter.GetSpamConfidence() > spamFilter.POSSIBLE_SPAM_THRESHOLD &&
		spamFilter.GetSpamConfidence() < spamFilter.SPAM_THRESHOLD)
	{
		printf("%s", "This word might indicate this message is spam\n");
	}
	else if (spamFilter.GetSpamConfidence() > spamFilter.SPAM_THRESHOLD)
	{
		printf("%s", "This word strongly indicates this message is spam\n");
	}

	printf("%f\n", bayesian.GetGivenMsgIsSpam());

	system("pause");
	return 0;
}