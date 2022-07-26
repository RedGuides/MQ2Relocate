/**
MQ2Relocate by Sic & ChatWithThisName
Purpose & Scope: Plugin to allow easier use of relocation items/aas, as well as specific logic to handle "converting" travel items like Wishing Lamp: and Zeuria Slide:
Usage:
	/relocate help will give a list of available options
**/
#include <mq/Plugin.h>

PreSetup("MQ2Relocate");
PLUGIN_VERSION(2.0);

constexpr auto PLUGINMSG = "\aw[\agRelocate\aw]\ao::\aw ";

CAltAbilityData* AltAbility(const std::string szAltName);
PlayerClient* MyTarget();
unsigned int MyTargetID();
PlayerClient* Me();
void ReloCmd(PlayerClient* pChar, char* szLine);
void StatusItemCheck(const char* szItemName);
void TransloCmd(PlayerClient* pChar, const char* szLine);
void UseItem(const char* pItem);
bool HaveAlias(const std::string& aliasName);
bool UseClickyByItemName(const char* pItem);
bool IAmDead();
bool Invis(PlayerClient* pSpawn);
bool IsClickyReadyByItemName(const char* pItem);
bool IsSpellBookOpen();
bool IsTargetPlayer();
bool ItemReady(const char* szItem);
bool ImDucking();
bool AltAbilityReady(const char*, const unsigned long TargetID = 0);
bool Casting();
bool CastingCheck();
bool DiscReady(EQ_Spell*);
bool Moving(PlayerClient* pSpawn);
inline bool InGame();
int GroupSize();

char convertoption[MAX_STRING] = { 0 };
char reloClicky[128] = { 0 };
bool bAmConverting = false;
bool needsUsing = false;
bool canGatePotion = false;
bool canGateAA = false;
bool canOriginAA = false;
bool canLobbyAA = false;
bool canHarmonicAA = false;
bool canEvacAA = false;
bool canTranslocate = false;
bool canTeleportAA = false;
bool canGroupEvacAA = false;
bool bDebugging = true;

uint64_t GlobalLastTimeUsed = GetTickCount64();

int iPulse = 0;
int iPulseDelay = 75;
int GlobalSkillDelay = 600;

void ReloCmd(PlayerClient* pChar, char* szLine)
{
	if (!InGame())
		return;
	//Get our parameters
	char Arg[MAX_STRING] = { 0 };
	GetArg(Arg, szLine, 1);
	if (strlen(Arg)) {
		char temp[MAX_STRING] = "/useitem ";

		if (!_stricmp(Arg, "help")) { // Output available arguments for /relocate
			WriteChatf("%sWelcome to MQ2Relocate!", PLUGINMSG);
			WriteChatf("%sBy \agSic\aw & \aoChatWithThisName\aw Exclusively for \arRedGuides\aw.", PLUGINMSG);
			WriteChatf("%s\agValid Relocate options are:\aw", PLUGINMSG);
			WriteChatf("%s\ar/relocate \agair\aw, \agfire\aw, and \agstone\aw for \ayZephyr's Lamp\aw.", PLUGINMSG);
			WriteChatf("%s\ar/relocate \agstonebrunt\aw, \agdreadlands\aw, \aggreatdivide\aw, \agnek\aw, \agnro\aw, and \agskyfire\aw for \ayZueria Slide\aw.", PLUGINMSG);
			WriteChatf("%s\ar/relocate \agpok\aw for \ayPlane of Knowledge\aw.", PLUGINMSG);
			WriteChatf("%s\ar/relocate \aggate\aw to use your \ayGate AA\aw or \ayTranslocation Potion\aw.", PLUGINMSG);
			WriteChatf("%s\ar/relocate \agorigin\aw to use your \ayOrigin AA\aw.", PLUGINMSG);
			WriteChatf("%s\ar/relocate \agbrell\aw to use your \ayMark of Brell\aw.", PLUGINMSG);
			WriteChatf("%s\ar/relocate \agbronze\aw to use your \ayHarbinger's Staff\aw.", PLUGINMSG);
			WriteChatf("%s\ar/relocate \aganchor\aw to use your \ayPrimary \awor \aySecondary Anchor\aw.", PLUGINMSG);
			WriteChatf("%s\ar/relocate \aganchor1\aw to use your \ayPrimary Anchor\aw.", PLUGINMSG);
			WriteChatf("%s\ar/relocate \aganchor2\aw to use your \aySecondary Anchor\aw.", PLUGINMSG);
			WriteChatf("%s\ar/relocate \agfellow\aw or \agfellowship\aw to use your \ayFellowship Insignia\aw.", PLUGINMSG);
			WriteChatf("%s\ar/relocate \aglobby\aw to use your \ay Throne of Heroes AA\aw.", PLUGINMSG);
			WriteChatf("%s\ar/relocate \agblood\aw to use your \ay Theatre of Blood AA\aw.", PLUGINMSG);
			WriteChatf("%s\ar/relocate \agcrystal\aw to use your \ayFroststone Crystal Resonator\aw.", PLUGINMSG);
			WriteChatf("%s\ar/relocate \agevac\aw to use your \ay Group Evac AA\aw (if you are in a group) or your \ay Personal Evac AA\aw.", PLUGINMSG);
			WriteChatf("%s\ar/relocate \agteleport\aw to use your \ay AoE Teleport AA\aw.", PLUGINMSG);
			WriteChatf("%s\agValid Translocate options are:\aw", PLUGINMSG);
			WriteChatf("%s\ay/translocate\aw to \ayTranslocate\aw your target to their bind.", PLUGINMSG);
			WriteChatf("%s\ay/translocate\aw \arRedBot\aw to \ayTranslocate\aw \arRedBot\aw to their bind.", PLUGINMSG);
			return;
		}
		if (!_stricmp(Arg, "air") || !_stricmp(Arg, "fire") || !_stricmp(Arg, "stone")) { // Use Wishing Lamp:
			char reloClicky[64] = "Wishing Lamp:";
			char air[64] = "Wishing Lamp: Zephyr's Flight";
			char fire[64] = "Wishing Lamp: Palace of Embers";
			char stone[64] = "Wishing Lamp: The Stone Demesne";
			if (!FindItemByName(reloClicky)) {
				WriteChatf("%s\arYou do not appear to have a Wishing Lamp.", PLUGINMSG);
				return;
			}
			if (!_stricmp(Arg, "air")) {
				if (FindItemByName(air)) {
					strcat_s(temp, air);
					EzCommand(temp);
					WriteChatf("%s\arRelocating with: \ag%s ", PLUGINMSG, air);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, air);
				}
			}
			if (!_stricmp(Arg, "fire")) {
				if (FindItemByName(fire)) {
					strcat_s(temp, fire);
					EzCommand(temp);
					WriteChatf("%s\arRelocating with: \ag%s ", PLUGINMSG, fire);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, fire);
				}
			}
			if (!_stricmp(Arg, "stone")) {
				if (FindItemByName(stone)) {
					strcat_s(temp, stone);
					EzCommand(temp);
					WriteChatf("%s\arRelocating with: \ag%s ", PLUGINMSG, stone);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, stone);
				}
			}
			return;
		}
		if (!_stricmp(Arg, "stonebrunt") || !_stricmp(Arg, "dreadlands") || !_stricmp(Arg, "nek") || !_stricmp(Arg, "greatdivide") || !_stricmp(Arg, "nro") || !_stricmp(Arg, "skyfire")) { // Zueria Slide
			char reloClicky[64] = "Zueria Slide:";
			char stonebrunt[64] = "Zueria Slide: Stonebrunt";
			char dreadlands[64] = "Zueria Slide: Dreadlands";
			char greatdivide[64] = "Zueria Slide: Great Divide";
			char nek[64] = "Zueria Slide: Nektulos";
			char nro[64] = "Zueria Slide: North Ro";
			char skyfire[64] = "Zueria Slide: Skyfire";
			if (!FindItemByName(reloClicky)) {
				WriteChatf("%s\arYou do not appear to have a Zueria Slide.", PLUGINMSG);
				return;
			}
			if (!_stricmp(Arg, "stonebrunt")) {
				if (FindItemByName(stonebrunt)) {
					strcat_s(temp, stonebrunt);
					EzCommand(temp);
					WriteChatf("%s\arRelocating with: \ag%s ", stonebrunt, PLUGINMSG);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, stonebrunt);
				}
			}
			if (!_stricmp(Arg, "dreadlands")) {
				if (FindItemByName(dreadlands)) {
					strcat_s(temp, dreadlands);
					EzCommand(temp);
					WriteChatf("%s\arRelocating with: \ag%s ", dreadlands, PLUGINMSG);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, dreadlands);
				}
			}
			if (!_stricmp(Arg, "greatdivide")) {
				if (FindItemByName(greatdivide)) {
					strcat_s(temp, greatdivide);
					EzCommand(temp);
					WriteChatf("%s\arRelocating with: \ag%s ", PLUGINMSG, greatdivide);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, greatdivide);
				}
			}
			if (!_stricmp(Arg, "nek")) {
				if (FindItemByName(nek)) {
					strcat_s(temp, nek);
					EzCommand(temp);
					WriteChatf("%s\arRelocating with: \ag%s ", PLUGINMSG, nek);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, nek);
				}
			}
			if (!_stricmp(Arg, "nro")) {
				if (FindItemByName(nro)) {
					strcat_s(temp, nro);
					EzCommand(temp);
					WriteChatf("%s\arRelocating with: \ag%s ", PLUGINMSG, nro);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, nro);
				}
			}
			if (!_stricmp(Arg, "skyfire")) {
				if (FindItemByName(skyfire)) {
					strcat_s(temp, skyfire);
					EzCommand(temp);
					WriteChatf("%s\arRelocating with: \ag%s ", PLUGINMSG, skyfire);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, skyfire);
				}
			}
			return;
		}
		if (!_stricmp(Arg, "pok")) { // Plane of Knowledge
			if (FindItemByName("Drunkard's Stein") && IsClickyReadyByItemName("Drunkard's Stein")) {
				sprintf_s(reloClicky, "Drunkard's Stein");
			}
			else if (FindItemByName("Brick of Knowledge") && IsClickyReadyByItemName("Brick of Knowledge")) {
				sprintf_s(reloClicky, "Brick of Knowledge");
			}
			else if (FindItemByName("The Binden Concerrentia") && FindItemCountByName("Quintessence of Knowledge") && IsClickyReadyByItemName("The Binden Concerrentia")) {
				sprintf_s(reloClicky, "The Binden Concerrentia");
			}
			else if (FindItemByName("The Fabled Binden Concerrentia") && FindItemCountByName("Quintessence of Knowledge") && IsClickyReadyByItemName("The Fabled Binden Concerrentia")) {
				sprintf_s(reloClicky, "The Fabled Binden Concerrentia");
			}
			else {
				WriteChatf("%s\aoDOH!\ar You don't have a \aypok \arclicky that is ready!", PLUGINMSG);
				return;
			}
			if (FindItemByName(reloClicky)) {
				if (UseClickyByItemName(reloClicky)) {
					//WriteChatf("Casting \ag %s \aw ", reloClicky);
				}
			}
			else {
				// This else for error checking and should never happen.
				WriteChatf("%s\arI don't have a %s", PLUGINMSG, reloClicky);
			}
			return;
		}
		if (!_stricmp(Arg, "gate")) { // Use gate AA if you have it, otherwise try and use a gate potion
			if (AltAbility("Gate") && AltAbility("Gate")->CurrentRank > 0) {
				canGateAA = true;
				return;
			}
			if (!canGateAA) {
				// Check if i have a Philter of Major Translocation - if so, turn canGatePotion to true
				if (FindItemByName("Philter of Major Translocation")) {
					canGatePotion = true;
				}
				// "Gate Potion" is a progression server version
				else if (FindItemByName("Gate Potion"), true) {
					canGatePotion = true;
				}
				else {
					WriteChatf("%s\arI don't seem to have the ability to gate, nor do you have a gate potion!\aw", PLUGINMSG);
				}
			}
			return;
		}
		if (!_stricmp(Arg, "origin")) { // Try and use Origin AA, otherwise try and use Sceptre of Draconic Calling
			if (AltAbility("Origin") && AltAbility("Origin")->CurrentRank > 0){
				if (AltAbilityReady("Origin")) {
					canOriginAA = true;
				}
				else {
					WriteChatf("%s\ayOrigin \arisn't ready right now\aw.", PLUGINMSG);
				}
				if ((GetCharInfo()->StartingCity == 394) && FindItemByName("Sceptre of Draconic Calling")) {
					if (IsClickyReadyByItemName("Sceptre of Draconic Calling")) {
						UseClickyByItemName("Sceptre of Draconic Calling");
					}
					else {
						WriteChatf("%s\aySceptre of Draconic Calling \arisn't ready right now\aw!", PLUGINMSG);
					}
				}
			}
			else {
				WriteChatf("%s\arWe don't have a way to Origin at the moment\ar.");
			}
			return;
		}

		if (!_stricmp(Arg, "brell")) { // Mark of Brell for Brell's Rest
			StatusItemCheck("Mark of Brell");
			return;
		}
		if (!_stricmp(Arg, "bronze")) { // Harbinger's Staff for City of Bronze
			StatusItemCheck("Harbinger's Staff");
			return;
		}

		if (!_stricmp(Arg, "anchor")) { // Use Primary or Secondary Anchor
			// Not doing a "Find Item" because IsClickyReady function already does that
			// We are also verifying that we don't have the "anchor" in our inventory by ID 52584 for Primary Anchor
			if (IsClickyReadyByItemName("Primary Anchor Transport Device") && !FindItemCountByID(52584)) {
				sprintf_s(reloClicky, "Primary Anchor Transport Device");
				UseClickyByItemName(reloClicky);
			}
			// Not doing a "Find Item" because IsClickyReady function already does that
			// We are also verifying that we don't have the "anchor" in our inventory by ID 52585 for Secondary Anchor
			else if (IsClickyReadyByItemName("Secondary Anchor Transport Device") && !FindItemCountByID(52585)) {
					sprintf_s(reloClicky, "Secondary Anchor Transport Device");
					UseClickyByItemName(reloClicky);
			}
			else {
				WriteChatf("%s\arDOH!\aw I don't have an anchor clicky that is ready", PLUGINMSG);
			}
			return;
		}
		if (!_stricmp(Arg, "anchor1")) { // Use specifically Primary Anchor
			StatusItemCheck("Primary Anchor Transport Device");
			return;
		}
		if (!_stricmp(Arg, "anchor2")) { // Use specifically Secondary Anchor
			StatusItemCheck("Secondary Anchor Transport Device");
			return;
		}
		if (!_stricmp(Arg, "fellow") || !_stricmp(Arg, "fellowship")) { // Use fellowship Insignia
			if (FindItemByName("Fellowship Registration Insignia")) {
				if (IsClickyReadyByItemName("Fellowship Registration Insignia")) {
					if (pChar->HideMode) { // Fellowship insignia requires being visible to use
						MakeMeVisible(GetCharInfo()->pSpawn, "");
					}
					if (pLocalPlayer && pLocalPlayer->Campfire) {
						UseClickyByItemName("Fellowship Registration Insignia");
					}
					else {
							WriteChatf("%s\arYou do not have a campfire up\aw.", PLUGINMSG);
					}
				}
				else {
					WriteChatf("%s\arFellowship Registration Insignia is not ready!\aw", PLUGINMSG);
				}
			}
			else {
				WriteChatf("%s\arYou do not have a Fellowship Registration Insigna!\aw", PLUGINMSG);
			}
			return;
		}
		if (!_stricmp(Arg, "lobby")) {
			if (AltAbility("Throne of Heroes") && AltAbility("Throne of Heroes")->CurrentRank > 0) {
				if (AltAbilityReady("Throne of Heroes")) {
					canLobbyAA = true;
				}
				else {
					WriteChatf("%s\ayThrone of Heroes \arisn't ready right now\aw.", PLUGINMSG);
				}
			}
			else {
				WriteChatf("%s\arI don't have the \ayThrone of Heroes \arAA\aw!", PLUGINMSG);
			}
			return;
		}
		if (!_stricmp(Arg, "blood")) {
			if (AltAbility("Harmonic Dissonance") && AltAbility("Harmonic Dissonance")->CurrentRank > 0) {
				if (AltAbilityReady("Harmonic Dissonance")) {
					canHarmonicAA = true;
				}
				else {
					WriteChatf("%s\ayHarmonic Dissonance \arisn't ready right now\aw.", PLUGINMSG);
				}
			}
			else {
				WriteChatf("%s\arI don't have the \ayHarmonic Dissonance \arAA\aw!", PLUGINMSG);
			}
			return;
		}
		if (!_stricmp(Arg, "evac")) { // use evac AA if you have it

			if (GroupSize() > 1 && AltAbility("Exodus") && AltAbility("Exodus")->CurrentRank > 0 && AltAbilityReady("Exodus")) { // Exodus GROUP evac 43
				canGroupEvacAA = true;
			}
			else if (AltAbility("Egress") && AltAbility("Egress")->CurrentRank > 0 && AltAbilityReady("Egress")) { // Egress SELF evac 8602
				canEvacAA = true;
			}
			else if (AltAbility("Levant") && AltAbility("Levant")->CurrentRank > 0 && AltAbilityReady("Levant")) { // Self Evac
				canEvacAA = true;
			}
			else if (AltAbility("Stealthy Getaway") && AltAbility("Stealthy Getaway")->CurrentRank > 0 && AltAbilityReady("Stealthy Getaway")) { // Self Evac 789
				canEvacAA = true;
			}
			else if (GroupSize() > 1 && AltAbility("Exodus") && AltAbility("Exodus")->CurrentRank > 0 && AltAbilityReady("Exodus")) { // Exodus GROUP evac 43
				canGroupEvacAA = true;
			}
			else if (AltAbility("Abscond") && AltAbility("Abscond")->CurrentRank > 0 && AltAbilityReady("Abscond")) { // Abscond SELF evac
				canEvacAA = true;
			}
			else if (GetPcProfile()->Class == Druid || GetPcProfile()->Class == Wizard) { // only Wizards/Druids can use the Di`Zok Escape Staff
				StatusItemCheck("Di`Zok Escape Staff");
				return;
			}

			if (!canEvacAA && !canGroupEvacAA) {
				WriteChatf("%s\arI don't seem to have the ability to evac!\aw", PLUGINMSG);
			}

			return;
		}
		if (!_stricmp(Arg, "teleport")) {
			if (AltAbility("Teleport") && AltAbility("Teleport")->CurrentRank > 0) {
				if (AltAbilityReady("Teleport")) {
					canTeleportAA = true;
				}
				else {
					WriteChatf("%s\ayTeleport \arisn't ready right now\aw.", PLUGINMSG);
				}
			}
			else {
				WriteChatf("%s\arI don't have the \ayTeleport \arAA\aw!", PLUGINMSG);
			}
			return;
		}
		if (!_stricmp(Arg, "crystal")) { // Froststone Crystal Resonator ToV pre-order item
			StatusItemCheck("Froststone Crystal Resonator");
			return;
		}
	}
	WriteChatf("%s\arYou didn't provide a valid option for /relocate.\aw", PLUGINMSG);
	WriteChatf("%s\arPlease do \"/relocate help\" for more information.\aw", PLUGINMSG);
	return;
}

void TransloCmd(PlayerClient* pChar, char* szLine)
{
	if (!InGame())
		return;
	char Arg[MAX_STRING] = { 0 };
	GetArg(Arg, szLine, 1);

	if (!_stricmp(Arg, "help")) {
		WriteChatf("%s\agValid Translocate options are:\aw", PLUGINMSG);
		WriteChatf("%s\ay/translocate\aw \arRedBot\aw to Translocate \arRedBot\aw to their bind.", PLUGINMSG);
		return;
	}

	if (GetPcProfile()->Class == Wizard) { // If I am a wizard?
		PlayerClient* me = GetCharInfo()->pSpawn;
		EQ_Spell* pSpell = GetSpellByName("Translocate");
		if (!MyTarget()) {
			if (strlen(Arg) == 0) {
				WriteChatf("%s\arPlease provide a target for translocate\aw.", PLUGINMSG);
				WriteChatf("%s\arYou can physically target a player, or provide a name like: \ag/translocate RedBot\aw.", PLUGINMSG);
				return;
			}
			else {
				//Let us see if we can find the target that was provided as an argument.
				PlayerClient* desiredTarget = GetSpawnByName(Arg);
				bool haveAA = false;
				if (pSpell && AltAbility(pSpell->Name) && AltAbility(pSpell->Name)->CurrentRank > 0)
					haveAA = true;
				if (!desiredTarget || !haveAA || !me) {
					if (!desiredTarget)
						WriteChatf("%s\arI can't find a player with the name \ay%s\aw", Arg);
					if (!haveAA)
						WriteChatf("%s\arI don't have the AA Translocate");
					if (!me)
						WriteChatf("%s\arI'm not in game, knock that off!");
					return;
				}
				if (desiredTarget && me && GetDistance3D(desiredTarget->X, desiredTarget->Y, desiredTarget->Z, me->X, me->Y, me->Z) > pSpell->Range) {
					WriteChatf("%s\arIt seems \ay%s\aw is out of range of \ay%s\aw.", PLUGINMSG, Arg, pSpell->Name);
				}

				pTarget = desiredTarget;

				if (!MyTarget()) {
					WriteChatf("%s\arWe do not have a target.", PLUGINMSG);
				}
				else if (MyTarget() && MyTarget()->Type != SPAWN_PLAYER) {
					WriteChatf("%s\ay%s\aw \aris not a Player!\aw", PLUGINMSG, MyTarget()->Type);
				}
				else if (MyTargetID() == Me()->SpawnID) {
					WriteChatf("%s\arYou can't translocate yourself, knock it off!", PLUGINMSG);
				}
				else {
					canTranslocate = true;
				}
				return;
			}
		}
		else {
			if (strlen(Arg) == 0) {
				PlayerClient* desiredTarget = GetSpawnByName(MyTarget()->Name);
				if (desiredTarget && me && GetDistance3D(desiredTarget->X, desiredTarget->Y, desiredTarget->Z, me->X, me->Y, me->Z) > pSpell->Range) {
					WriteChatf("%s\arIt seems \ay%s\aw \aris out of range of \ay%s\aw.", MyTarget()->Name, pSpell->Name, PLUGINMSG);
					return;
				}

				if (MyTarget()->Type != SPAWN_PLAYER) {
					WriteChatf("%s\ay%s\aw \aris not a Player!\aw", PLUGINMSG, MyTarget()->Name);
					return;
				}

				if (MyTargetID() == Me()->SpawnID) {
					WriteChatf("%s\arYou can't translocate yourself, knock it off!\aw", PLUGINMSG);
					return;
				}

				canTranslocate = true;
				return;
			}
			else {
				PlayerClient* desiredTarget = GetSpawnByName(Arg);
				if (!desiredTarget) {
					WriteChatf("%s\arI can't find a player with the name >>> \ay%s\ar <<<\aw.", PLUGINMSG, Arg);
					return;
				}

				if (desiredTarget && desiredTarget->Type != SPAWN_PLAYER) {
					WriteChatf("%s\ay%s\aw \aris not a player.\aw", PLUGINMSG, desiredTarget);
					return;
				}

				if (desiredTarget->SpawnID == Me()->SpawnID) {
					WriteChatf("%s\arYou can't translocate yourself, knock it off!\aw", PLUGINMSG);
					return;
				}

				if (desiredTarget && me && GetDistance3D(desiredTarget->X, desiredTarget->Y, desiredTarget->Z, me->X, me->Y, me->Z) > pSpell->Range) {
					WriteChatf("%s\arIt seems \ay%s\ar is out of range of \ay%s\aw.", PLUGINMSG, Arg, pSpell->Name);
					return;
				}

				pTarget = desiredTarget;

				if (!MyTarget()) {
					WriteChatf("We do not have a target.", PLUGINMSG);
					return;
				}

				if (MyTarget()->Type != SPAWN_PLAYER) {
					WriteChatf("%s\ay%s\aw \aris not a Player!\aw", PLUGINMSG, MyTarget()->Name);
					return;
				}

				canTranslocate = true;
				return;
			}
		}

	}
	WriteChatf("%s\arYou are not a Wizard! No Translocate for you\aw!", PLUGINMSG);
}

bool HaveAlias(const std::string& aliasName) {
	const std::string alias = GetPrivateProfileString("Aliases", aliasName, "None", gPathMQini);
	if (alias == "None") {
		return false;
	}
	return true;
}

PLUGIN_API void InitializePlugin()
{
	if (HaveAlias("/relocate")) { // Check our aliases
		WriteChatf("%s\arIt appears you already have an Alias for \ap/relocate\ar  please type \"\ay/alias /relocate delete\ar\" then reload this plugin.", PLUGINMSG);
		EzCommand("/timed 10 /plugin MQ2Relocate Unload");
	}

	if (HaveAlias("/translocate")) { // Check our aliases
		WriteChatf("%s\arIt appears you already have an Alias for \ap/translocate\ar  please type \"\ay/alias /translocate delete\ar\" then reload this plugin.", PLUGINMSG);
		EzCommand("/timed 10 /plugin MQ2Relocate Unload");
	}
	else {
		AddCommand("/relocate", ReloCmd);
		AddCommand("/translocate", TransloCmd);
	}
}

PLUGIN_API void ShutdownPlugin()
{
	RemoveCommand("/relocate");
	RemoveCommand("/translocate");
}

PLUGIN_API void OnPulse()
{
	if (++iPulse < iPulseDelay) return;
	iPulse = 0;

	//Base Cases
	if (!InGame()) {
		return;
	}

	//DON'T FORGET TO CHANGE THE FREAKING BASE CASES SO THAT YOU ACTUALLY CONTINUE INTO THE FUNCTION! (WTB back many minutes of my life)
	if (!strlen(convertoption) && !bAmConverting && !needsUsing && !canGatePotion && !canGateAA && !canOriginAA && !canLobbyAA && !canHarmonicAA && !canEvacAA && !canGroupEvacAA && !canTranslocate && !canTeleportAA) {
		return;
	}

	if (ImDucking() || Casting() || Moving(GetCharInfo()->pSpawn)) return;


	char temp[MAX_STRING] = "";

	if (bAmConverting) {
		if (!FindItemByName(convertoption) && FindItemByName(reloClicky)) {
			sprintf_s(temp, "/convertitem %s", convertoption);
			WriteChatf("%sConverting Item", PLUGINMSG); // Debug purposes
			EzCommand(temp);
		}
		else {
			//Converting is done at this point and I need to swap to using the item.
			bAmConverting = false;
			needsUsing = true;
		}
	}

	if (needsUsing && FindItemByName(convertoption)) {
		// This is where I trigger the usage of the converted item.
		sprintf_s(temp, "/useitem \"%s\"", convertoption);
		EzCommand(temp);
		WriteChatf("%sRelocating with: \ag%s\aw", PLUGINMSG, convertoption);
		needsUsing = false; // Turn off needsusing, because i've used it.
		sprintf_s(convertoption, ""); // Clear the convertitem string so that it's blank and ready for modification again.
	}

	if (canGateAA) {
		WriteChatf("%sRelocating with: \ayGate AA\aw.", PLUGINMSG);
		EzCommand("/alt act 1217");
		canGateAA = false;
	}

	if (canGatePotion) {
		if (UseClickyByItemName("Philter of Major Translocation")) {
			canGatePotion = false;
		}
		else if (UseClickyByItemName("Gate Potion")) {
			canGatePotion = false;
		}
	}

	if (canOriginAA) {
		WriteChatf("%sRelocating with: \ayOrigin AA\aw.", PLUGINMSG);
		EzCommand("/alt act 331");
		canOriginAA = false;
	}

	if (canLobbyAA) {
		WriteChatf("%sRelocating to \agGuild Lobby\aw with: \ayThrone of Heroes AA\aw.", PLUGINMSG);
		EzCommand("/alt act 511");
		canLobbyAA = false;
	}

	if (canHarmonicAA) {
		WriteChatf("%sRelocating to \agTheater of Blood\aw with: \ayHarmonic Dissonance AA\aw.", PLUGINMSG);
		EzCommand("/alt act 574");
		canHarmonicAA = false;
	}

	if (canEvacAA) { // Check each evac AA
		if (AltAbilityReady("Steathly Getaway")) {
			WriteChatf("%sSelf Evac with: \ayStealthy Getaway AA\aw.", PLUGINMSG);
			EzCommand("/alt act 789");
			canEvacAA = false;
		}

		if (AltAbilityReady("Abscond")) {
			WriteChatf("%sSelf Evac with: \ayAbscond AA\aw.", PLUGINMSG);
			EzCommand("/alt act 490");
			canEvacAA = false;
		}

		if (AltAbilityReady("Egress")) {
			WriteChatf("Self Evac with: \ayEgress AA\aw.");
			EzCommand("/alt act 8602");
			canEvacAA = false;
		}

		if (AltAbilityReady("Levant")) {
			WriteChatf("%sSelf Evac with: \ayLevant AA\aw.", PLUGINMSG);
			EzCommand("/alt act 2899");
			canEvacAA = false;
		}
	}

	if (canGroupEvacAA) { // Check each evac AA
		WriteChatf("%sGroup Evac with: \ayExodus AA\aw.", PLUGINMSG);
		EzCommand("/alt act 43");
		canGroupEvacAA = false;
	}

	if (canTranslocate) {
		WriteChatf("%sTranslocating \ay%s\aw with: \ayTranslocate AA\aw.", PLUGINMSG, pTarget->Name);
		EzCommand("/alt act 9703");
		canTranslocate = false;
	}

	if (canTeleportAA) {
		WriteChatf("%sTeleporting surrounding allies with: \ayTeleport AA\aw.", PLUGINMSG);
		EzCommand("/alt act 9704");
		canTeleportAA = false;
	}
}

bool UseClickyByItemName(const char* pItem)
{
	if (FindItemCountByName(pItem)) {
		if (ItemClient* item = FindItemByName(pItem)) {
			if (ItemDefinition* pItemInfo = GetItemFromContents(item)) {
				if (EQ_Spell* itemSpell = GetSpellByID(pItemInfo->Clicky.SpellID)) {
					if (pSpellMgr && ItemReady(pItemInfo->Name)) {
						UseItem(pItemInfo->Name);
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool IsClickyReadyByItemName(const char* pItem)
{
	if (FindItemCountByName(pItem)) {
		if (ItemClient* pItemContents = FindItemByName(pItem)) {
			if (ItemDefinition* pItemInfo = GetItemFromContents(pItemContents)) {
				if (EQ_Spell* itemSpell = GetSpellByID(pItemInfo->Clicky.SpellID)) {
					if (pSpellMgr && ItemReady(pItemInfo->Name)) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool IsTargetPlayer()
{
	if (PlayerClient* target = pTarget) {
		if (target->Type == SPAWN_PLAYER) {
			return true;
		}
	}
	return false;
}

PLUGIN_API void SetGameState(unsigned long GameState)
{
	if (GameState != GAMESTATE_INGAME) {
		if (bAmConverting) bAmConverting = false;
		if (needsUsing) needsUsing = false;
		if (strlen(convertoption)) sprintf_s(convertoption, "");
	}
}

void StatusItemCheck(const char* szItemName)
{
	if (FindItemByName(szItemName)) {
		if (IsClickyReadyByItemName(szItemName)) {
			UseClickyByItemName(szItemName);
			return;
		}
		else {
			WriteChatf("%s\ay%s \aris not ready\aw!", PLUGINMSG, szItemName);
			return;
		}
	}
	WriteChatf("%s\arYou do not have a \ay%s\aw!", PLUGINMSG, szItemName);
	return;
}

inline bool InGame()
{
	return(GetGameState() == GAMESTATE_INGAME && GetCharInfo() && GetCharInfo()->pSpawn && GetPcProfile());
}

CAltAbilityData* AltAbility(const std::string szAltName)
{
	int level = -1;
	if (PlayerClient* pMe = pLocalPlayer) {
		level = pMe->Level;
	}
	for (int nAbility = 0; nAbility < AA_CHAR_MAX_REAL; nAbility++) {
		if (CAltAbilityData* pAbility = GetAAByIdWrapper(pPCData->GetAlternateAbilityId(nAbility), level)) {
			if (const char* pName = pCDBStr->GetString(pAbility->nName, eAltAbilityName)) {
				if (!_stricmp(szAltName.c_str(), pName)) {
					return pAbility;
				}
			}
		}
	}
	return nullptr;
}

void BardStopSinging() {
	if (Me()->GetClass() == Bard) {
		EzCommand("/stopsong");

		if (IsPluginLoaded("mq2twist")) {
			EzCommand("/twist off");
		}
		WriteChatf("%s\agWe are going to /stopsong and /twist off, to use our relocate items.", PLUGINMSG);
	}
}

bool CastingCheck()
{
	PlayerClient* me = GetCharInfo()->pSpawn;
	if (me->GetClass() == Bard) {
		BardStopSinging();
	}
	else if (Casting()) {
		return true;
	}
	return false;
}

bool AltAbilityReady(const char* szLine, const unsigned long TargetID)
{
	if (!InGame() || IsSpellBookOpen() || IAmDead()) return false;
	PlayerClient* me = GetCharInfo()->pSpawn;

	if (CastingCheck())
		return false;

	if (!me || GlobalLastTimeUsed >= GetTickCount64() || Invis(me)) return false;
	int level = -1;

	if (PlayerClient* pMe = pLocalPlayer) {
		level = pMe->Level;
	}
	for (auto nAbility = 0; nAbility < AA_CHAR_MAX_REAL; nAbility++) {
		if (CAltAbilityData* pAbility = GetAAByIdWrapper(pPCData->GetAlternateAbilityId(nAbility), level)) {
			if (const char* pName = pCDBStr->GetString(pAbility->nName, eAltAbilityName)) {
				if (!_stricmp(szLine, pName)) {
					if (pAbility->SpellID != -1) {
						if (EQ_Spell* myAltAbility = GetSpellByID(pAbility->SpellID)) {
							//Am I in motion?
							if (myAltAbility->CastTime && Moving(Me())) {
								return false;
							}
							if (DiscReady(myAltAbility))
								return pAltAdvManager->IsAbilityReady(pPCData, pAbility, 0);
						}
					}
				}
			}
		}
	}
	return false;
}

int GroupSize()
{
	if (pCharData) {
		if (pCharData->Group) {
			return pCharData->Group->GetNumberOfMembers();
		}
	}
	return 0;
}

PlayerClient* MyTarget()
{
	if (!pTarget) {
		return nullptr;
	}
	if (PlayerClient* target = pTarget) {
		return target;
	}
	return nullptr;
}

unsigned int MyTargetID()
{
	return pTarget ? pTarget->SpawnID : 0;
}

PlayerClient* Me()
{
	if (PlayerClient* me = GetCharInfo()->pSpawn) {
		return me;
	}
	return nullptr;
}

bool ImDucking()
{
	return Me()->StandState == STANDSTATE_DUCK;
}

bool Casting()
{
	return GetCharInfo()->pSpawn->CastingData.IsCasting();
}

bool Moving(PlayerClient* pSpawn)
{
	// 0.01f gets reported when levitating
	return FindSpeed(pSpawn) >= 0.01f;
}

bool ItemReady(const char* szItem)
{
	if (GlobalLastTimeUsed >= GetTickCount64()) return false;
	if (GetPcProfile()->Class != Bard && Casting()) return false;
	if (ItemClient* item = FindItemByName(szItem, true)) {
		if (ItemDefinition* pIteminf = GetItemFromContents(item)) {
			if (pIteminf->Clicky.TimerID != -1) {
				uint32_t timer = GetItemTimer(item);
				if (timer == 0 && !Moving(GetCharInfo()->pSpawn))
					return true;
			}
			else if (pIteminf->Clicky.SpellID != -1)
			{
				return true; // insta-click or instant recast
			}
		}
	}
	return false;
}

void UseItem(const char* pItem)
{
	if (GlobalLastTimeUsed >= GetTickCount64()) return;
	char temp[MAX_STRING] = "/useitem \"";
	strcat_s(temp, MAX_STRING, pItem);
	char temp2[MAX_STRING] = "\"";
	strcat_s(temp, MAX_STRING, temp2);
	EzCommand(temp);
	WriteChatf("%sUsing Item: \ay%s", PLUGINMSG, pItem);
	GlobalLastTimeUsed = GetTickCount64() + GlobalSkillDelay;
}

bool IsSpellBookOpen() {
	return pSpellBookWnd->IsVisible();
}

bool IAmDead()
{
	if (PlayerClient* Me = GetCharInfo()->pSpawn) {
		if (Me->RespawnTimer) {
			return true;
		}
	}
	return false;
}

bool Invis(PlayerClient* pSpawn)
{
	return pSpawn->HideMode;
}

bool DiscReady(EQ_Spell* pSpell)
{
	if (!InGame()) return false;
	time_t timeNow = time(NULL);
#if !defined(ROF2EMU) && !defined(UFEMU)
	if (pPCData->GetCombatAbilityTimer(pSpell->ReuseTimerIndex, pSpell->SpellGroup) < timeNow) {
#else
	if (pSpell->ReuseTimerIndex == -1 || pSpell->ReuseTimerIndex > 20) //this matters on emu it will actually crash u if above 20
	{
		return true;
	}
	if (pPCData->GetCombatAbilityTimer(pSpell->ReuseTimerIndex) < timeNow) {
#endif
		return true;
	}
	return false;
}