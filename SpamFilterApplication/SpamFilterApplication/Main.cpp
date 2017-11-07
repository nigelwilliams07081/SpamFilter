#include "stdafx.h"
#include <time.h>

int main()
{
	EmailReceiver emailReceiver;
	SpamFilter spamFilter;
	Bayesian bayesianCalculator;

	spamFilter.PerformSpamSearch();
	std::cout << "Our Overall Spam Confidence: " << spamFilter.GetOverallSpamConfidence() << std::endl;

	bayesianCalculator.SetAnyMsgIsSpam(0.8f);

	bayesianCalculator.SetWordAppearsInSpam(spamFilter.GetOverallSpamConfidence());

	bayesianCalculator.CalculateTheorem();

	std::cout << "The Overall Spam Confidence After Calculation: " << bayesianCalculator.GetGivenMsgIsSpam() << std::endl;

	// Retrieve email from server and assign parts of it to Email struct
	/*emailReceiver.RetrieveEmail();

	if (emailReceiver.GetEmail().IsValid)
	{
		printf("Sender:\n%s\nSubject:\n%s\nBody:\n%s\n", emailReceiver.GetEmail().Sender, emailReceiver.GetEmail().Subject,
			emailReceiver.GetEmail().Body);

		for (int i = 0; i < sizeof(emailReceiver.GetEmail().Attachments) / sizeof(emailReceiver.GetEmail().Attachments[0]); i++)
		{
			if (emailReceiver.GetEmail().Attachments[i] != "")
			{
				printf("Attachments:\n%s\n", emailReceiver.GetEmail().Attachments[i]);
			}
		}
	}
	else
	{
		printf("%s\n", "This is not a valid email");
	}*/
	
	system("pause");
	return 0;
}