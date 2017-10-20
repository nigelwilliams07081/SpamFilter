#include "stdafx.h"
#include <time.h>

using namespace EAGetMailObjLib;

int main()
{
	SpamFilter spamFilter;
	Bayesian bayesian;
	EmailReceiver emailReceiver;
	spamFilter.OpenFile("File.txt");
	bayesian.SetAnyMsgIsSpam(0.8f);
	
	bayesian.SetWordAppearsInSpam(0.505f);

	bayesian.CalculateTheorem();

	printf("%f\n", bayesian.GetGivenMsgIsSpam());

	emailReceiver.RetrieveEmail();

	system("pause");
	return 0;
}