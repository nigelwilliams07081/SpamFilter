from random import choice, randrange
from string import ascii_lowercase, digits


def randomstring(size=6, chars=ascii_lowercase + digits):
	return ''.join(choice(chars) for _ in range(size))
	
def generate_emailaddress():
	return randomstring(8) + "@" + choice(["gmail.com", "yahoo.com", "mail.com", "aol.com", "bellsouth.net", "att.net", "googlemail.com", "protonmail.com", "outlook.com", "gmx.com"])
	
def generate_body():
	s = ""
	for i in range(randrange(60, 800)):
		s += randomstring(randrange(5, 12), ascii_lowercase) + " "
		
	return s

def generate_attachment():
	return randomstring(randrange(5, 12)) + "." + choice(["exe", "jpg", "jpg.exe", "png", "txt", "pdf", "docx", "crf"])

emailcount = 1000

print("<emailList>")
for i in range(emailcount):
	print("<sender>" + generate_emailaddress() + "</sender>" + "<subject>Test subject #" + str(randrange(0, 10000)) + "</subject><body>" + generate_body() + "</body><attachments>")
	
	for i in range(randrange(0, 10)):
		print("<file>" + generate_attachment() + "</file>")
	
	print("</attachments></email>")
	
print("</emailList>")

