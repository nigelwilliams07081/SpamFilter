#include "stdafx.h"
#include <time.h>

using namespace EAGetMailObjLib;

int main()
{
	//Bayesian bayesian;
	EmailReceiver emailReceiver;
	/*bayesian.SetAnyMsgIsSpam(0.8f);
	
	bayesian.SetWordAppearsInSpam(0.505f);

	bayesian.CalculateTheorem();

	printf("%f\n", bayesian.GetGivenMsgIsSpam());*/

	emailReceiver.RetrieveEmail();

	system("pause");
	return 0;
}