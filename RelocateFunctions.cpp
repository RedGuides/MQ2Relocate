#include "RelocateFunctions.h"

char convertoption[MAX_STRING] = { 0 };
bool bAmConverting = false;
bool needsUsing = false;

void LampCmd(PSPAWNINFO pChar, PCHAR szLine) {
	//Check to see if MQ2EQBC Plugin is loaded, if not output an error and return out without doing anything. 
	if (!FindPlugin("MQ2EQBC")) {
		WriteChatf("\ar[\a-tMQ2Status\ar]\ao:: \arYou don't appear to have MQ2EQBC Loaded!");
		return;
	}
	else {//If the MQ2EQBC was loaded, let's see if we're connected to the EQBC Server, if not output an error and return out without doing anything.
		char amConnected[64] = "${EQBC.Connected}";
		ParseMacroData(amConnected, 64);
		if (!_stricmp(amConnected, "FALSE")) {
			WriteChatf("\ar[\a-tMQ2Status\ar]\ao:: \arYou don't appear to be connected to the EQBC server!");
			return;
		}
	}
	/// Get our Parameters
	CHAR Arg[MAX_STRING] = { 0 };
	GetArg(Arg, szLine, 1);
	/// /status item stuff - this is doing a search for how many of these items we have on our person.
	if (strlen(Arg)) {
		char temp[MAX_STRING] = "/useitem ";
		char lamp[64] = "Wishing Lamp:";
		char air[64] = "Wishing Lamp: Zephyr's Flight";
		char fire[64] = "Wishing Lamp: Palace of Embers";
		char stone[64] = "Wishing Lamp: The Stone Demesne";

		if (!_stricmp(Arg, "air") || !_stricmp(Arg, "fire") || !_stricmp(Arg, "stone")) {
			if (!FindItemByName(lamp)) {
				WriteChatf("You do not appear to have a Wishing Lamp.");
				return;
			}
			if (!_stricmp(Arg, "air")) {
				//We have a lamp for sure at this point. Lets see if it's already the right conversion of it. 
				if (FindItemByName(air)) {
					strcat_s(temp, air);
					EzCommand(temp);
					WriteChatf("Casting %s ", air);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, air);
				}
			}
			if (!_stricmp(Arg, "fire")) {
				//We have a lamp for sure at this point. Lets see if it's already the right conversion of it. 
				if (FindItemByName(fire)) {
					strcat_s(temp, fire);
					EzCommand(temp);
					WriteChatf("Casting %s ", fire);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, fire);
				}
			}
			if (!_stricmp(Arg, "stone")) {
				//We have a lamp for sure at this point. Lets see if it's already the right conversion of it. 
				if (FindItemByName(stone)) {
					strcat_s(temp, stone);
					EzCommand(temp);
					WriteChatf("Casting %s ", stone);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, stone);
				}
			}
			return;
		}

	}
	if (!strlen(Arg)) {
		//It wasn't the wishing lamp, add more options here. 
		WriteChatf("\arNo arguement was provided for /Lamp.\aw", Arg);
	}
	else {
		WriteChatf("\ar>> %s << was not a valid option for /Lamp.\aw", Arg);
	}
	WriteChatf("\arValid options are: air, fire, or stone.\aw");
}

//Check to see if a macro variable is defined.
bool IsDefined(PCHAR szLine) {
	return (FindMQ2DataVariable(szLine) != 0);
}

//Check to see if an alias exists with the name of "ShortCommand"
bool HaveAlias(PCHAR ShortCommand) {
	std::string sName = ShortCommand;
	std::transform(sName.begin(), sName.end(), sName.begin(), tolower);
	if (mAliases.find(sName) != mAliases.end()) {
		return true;
	}
	return false;
}