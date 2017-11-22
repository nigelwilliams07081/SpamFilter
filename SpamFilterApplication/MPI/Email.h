#pragma once

struct Email
{
	bool IsValid;
	char Sender[256];
	char Subject[998];
	char Body[65535];
	char Attachments[10][255];
	float SpamPercentage;
};