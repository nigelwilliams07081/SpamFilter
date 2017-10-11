#include "SpamFilter.h"
#include "Bayesian.h"

int main()
{
	SpamFilter spamFilter;
	Bayesian bayesian;

	spamFilter.OpenFile("File.txt");

	bayesian.SetAnyMsgIsSpam(0.8f);

	bayesian.AddToWordAppearsInSpamList(0.95f);
	bayesian.AddToWordAppearsInSpamList(0.75f);
	bayesian.AddToWordAppearsInSpamList(0.85f);

	bayesian.CalculateTheoremME();

	printf("%f\n", bayesian.GetGivenMsgIsSpamME());

	system("pause");
	return 0;
}