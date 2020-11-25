/**
MQ2Relocate by Sic & ChatWithThisName
Purpose & Scope: Plugin to allow easier use of relocation items/aas, as well as specific logic to handle "converting" travel items like Wishing Lamp: and Zeuria Slide:
Usage:
	/relocate help will give a list of available options
**/
#include "../MQ2Plugin.h"
#include <string>

PreSetup("MQ2Relocate");
PLUGIN_VERSION(2.0);

ALTABILITY* AltAbility(std::string szAltName);
SPAWNINFO* MyTarget();
unsigned long MyTargetID();
SPAWNINFO* Me();
void ReloCmd(SPAWNINFO* pChar, char* szLine);
void StatusItemCheck(char* szItemName);
void TransloCmd(SPAWNINFO* pChar, char* szLine);
void UseItem(char* pItem);
bool TargetSpawn(SPAWNINFO* pSpawn);
bool HaveAlias(const std::string& aliasName);
bool UseClickyByItemName(char* pItem);
bool IAmDead();
bool Invis(SPAWNINFO* pSpawn);
bool IsClickyReadyByItemName(char* pItem);
bool IsSpellBookOpen();
bool IsTargetPlayer();
bool ItemReady(char* szItem);
bool ImDucking();
bool AltAbilityReady(const char*, unsigned long TargetID = 0);
bool Casting();
bool DiscReady(SPELL*);
bool Moving(SPAWNINFO* pSpawn);
bool FindPlugin(const char* pChar);
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

void ReloCmd(SPAWNINFO* pChar, char* szLine)
{
	if (!InGame())
		return;
	//Get our parameters
	CHAR Arg[MAX_STRING] = { 0 };
	GetArg(Arg, szLine, 1);
	if (strlen(Arg)) {
		char temp[MAX_STRING] = { 0 };
		if (GetCharInfo2()->Class == EQData::Bard && FindPlugin("mq2twist")) { // We already verify that GetCharInfo2 exists in our InGame(), so only checking its member here.
			sprintf_s(temp, "/multiline ; /twist off ; /timed 5 ; /useitem ");
			WriteChatf("\agWe are going to /twist off, to use our relocate items.");
		}
		else {
			sprintf_s(temp, "/useitem ");
		}
		if (!_stricmp(Arg, "help")) { // Output available arguments for /relocate
			WriteChatf("Welcome to MQ2Relocate!");
			WriteChatf("By \agSic\aw & \aoChatWithThisName\aw Exclusively for \arRedGuides\aw.");
			WriteChatf("\agValid Relocate options are:\aw");
			WriteChatf("/relocate \agair\aw, \agfire\aw, and \agstone\aw for \ayZephyr's Lamp\aw.");
			WriteChatf("/relocate \agstonebrunt\aw, \agdreadlands\aw, \aggreatdivide\aw, \agnek\aw, \agnro\aw, and \agskyfire\aw for \ayZueria Slide\aw.");
			WriteChatf("/relocate \agpok\aw for \ayPlane of Knowledge\aw.");
			WriteChatf("/relocate \aggate\aw to use your \ayGate AA\aw or \ayTranslocation Potion\aw.");
			WriteChatf("/relocate \agorigin\aw to use your \ayOrigin AA\aw.");
			WriteChatf("/relocate \agbrell\aw to use your \ayMark of Brell\aw.");
			WriteChatf("/relocate \agbronze\aw to use your \ayHarbinger's Staff\aw.");
			WriteChatf("/relocate \aganchor\aw to use your \ayPrimary \awor \aySecondary Anchor\aw.");
			WriteChatf("/relocate \aganchor1\aw to use your \ayPrimary Anchor\aw.");
			WriteChatf("/relocate \aganchor2\aw to use your \aySecondary Anchor\aw.");
			WriteChatf("/relocate \agfellow\aw or \agfellowship\aw to use your \ayFellowship Insignia\aw.");
			WriteChatf("/relocate \aglobby\aw to use your \ay Throne of Heroes AA\aw.");
			WriteChatf("/relocate \agblood\aw to use your \ay Theatre of Blood AA\aw.");
			WriteChatf("/relocate \agcrystal\aw to use your \ayFroststone Crystal Resonator\aw.");
			WriteChatf("/relocate \agevac\aw to use your \ay Group Evac AA\aw (if you are in a group) or your \ay Personal Evac AA\aw.");
			WriteChatf("/relocate \agteleport\aw to use your \ay AoE Teleport AA\aw.");
			WriteChatf("\agValid Translocate options are:\aw");
			WriteChatf("\ay/translocate\aw to \ayTranslocate\aw your target to their bind.");
			WriteChatf("\ay/translocate\aw \arRedBot\aw to \ayTranslocate\aw \arRedBot\aw to their bind.");
			return;
		}
		if (!_stricmp(Arg, "air") || !_stricmp(Arg, "fire") || !_stricmp(Arg, "stone")) { // Use Wishing Lamp:
			char reloClicky[64] = "Wishing Lamp:";
			char air[64] = "Wishing Lamp: Zephyr's Flight";
			char fire[64] = "Wishing Lamp: Palace of Embers";
			char stone[64] = "Wishing Lamp: The Stone Demesne";
			if (!FindItemByName(reloClicky)) {
				WriteChatf("You do not appear to have a Wishing Lamp.");
				return;
			}
			if (!_stricmp(Arg, "air")) {
				if (FindItemByName(air)) {
					strcat_s(temp, air);
					EzCommand(temp);
					WriteChatf("Relocating with: \ag%s ", air);
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
					WriteChatf("Relocating with: \ag%s ", fire);
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
					WriteChatf("Relocating with: \ag%s ", stone);
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
				WriteChatf("You do not appear to have a Zueria Slide.");
				return;
			}
			if (!_stricmp(Arg, "stonebrunt")) {
				if (FindItemByName(stonebrunt)) {
					strcat_s(temp, stonebrunt);
					EzCommand(temp);
					WriteChatf("Relocating with: \ag%s ", stonebrunt);
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
					WriteChatf("Relocating with: \ag%s ", dreadlands);
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
					WriteChatf("Relocating with: \ag%s ", greatdivide);
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
					WriteChatf("Relocating with: \ag%s ", nek);
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
					WriteChatf("Relocating with: \ag%s ", nro);
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
					WriteChatf("Relocating with: \ag%s ", skyfire);
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
			else if (FindItemByName("The Binden Concerrentia") && FindItemCountByName("Quintessence of Knowledge", 1) && IsClickyReadyByItemName("The Binden Concerrentia")) {
				sprintf_s(reloClicky, "The Binden Concerrentia");
			}
			else if (FindItemByName("The Fabled Binden Concerrentia") && FindItemCountByName("Quintessence of Knowledge", 1) && IsClickyReadyByItemName("The Fabled Binden Concerrentia")) {
				sprintf_s(reloClicky, "The Fabled Binden Concerrentia");
			}
			else {
				WriteChatf("\aoDOH!\ar You don't have a \aypok \arclicky that is ready!");
				return;
			}
			if (FindItemByName(reloClicky)) {
				if (UseClickyByItemName(reloClicky)) {
					//WriteChatf("Casting \ag %s \aw ", reloClicky);
				}
			}
			else {
				// This else for error checking and should never happen.
				WriteChatf("I don't have a %s", reloClicky);
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
				else {
					WriteChatf("\arI don't seem to have the ability to gate, nor do you have a gate potion!\aw");
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
					WriteChatf("\ayOrigin \arisn't ready right now\aw.");
				}
				if ((GetCharInfo()->StartingCity == 394) && FindItemByName("Sceptre of Draconic Calling")) {
					if (IsClickyReadyByItemName("Sceptre of Draconic Calling")) {
						UseClickyByItemName("Sceptre of Draconic Calling");
					}
					else {
						WriteChatf("\aySceptre of Draconic Calling \arisn't ready right now\aw!");
					}
				}
			}
			else {
				WriteChatf("\arWe don't have a way to Origin at the moment\ar.");
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
				WriteChatf("\arDOH!\aw I don't have an anchor clicky that is ready");
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
					if (pLocalPlayer && ((SPAWNINFO*)pLocalPlayer)->Campfire) {
						UseClickyByItemName("Fellowship Registration Insignia");
					}
					else {
							WriteChatf("\arYou do not have a campfire up\aw.");
					}
				}
				else {
					WriteChatf("\arFellowship Registration Insignia is not ready!\aw");
				}
			}
			else {
				WriteChatf("\arYou do not have a Fellowship Registration Insigna!\aw");
			}
			return;
		}
		if (!_stricmp(Arg, "lobby")) {
			if (AltAbility("Throne of Heroes") && AltAbility("Throne of Heroes")->CurrentRank > 0) {
				if (AltAbilityReady("Throne of Heroes")) {
					canLobbyAA = true;
				}
				else {
					WriteChatf("\ayThrone of Heroes \arisn't ready right now\aw.");
				}
			}
			else {
				WriteChatf("\arI don't have the \ayThrone of Heroes \arAA\aw!");
			}
			return;
		}
		if (!_stricmp(Arg, "blood")) {
			if (AltAbility("Harmonic Dissonance") && AltAbility("Harmonic Dissonance")->CurrentRank > 0) {
				if (AltAbilityReady("Harmonic Dissonance")) {
					canHarmonicAA = true;
				}
				else {
					WriteChatf("\ayHarmonic Dissonance \arisn't ready right now\aw.");
				}
			}
			else {
				WriteChatf("\arI don't have the \ayHarmonic Dissonance \arAA\aw!");
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

			if (!canEvacAA && !canGroupEvacAA) {
				WriteChatf("\arI don't seem to have the ability to evac!\aw");
			}

			return;
		}
		if (!_stricmp(Arg, "teleport")) {
			if (AltAbility("Teleport") && AltAbility("Teleport")->CurrentRank > 0) {
				if (AltAbilityReady("Teleport")) {
					canTeleportAA = true;
				}
				else {
					WriteChatf("\ayTeleport \arisn't ready right now\aw.");
				}
			}
			else {
				WriteChatf("\arI don't have the \ayTeleport \arAA\aw!");
			}
			return;
		}
		if (!_stricmp(Arg, "crystal")) { // Froststone Crystal Resonator ToV pre-order item
			StatusItemCheck("Froststone Crystal Resonator");
			return;
		}
	}
	WriteChatf("\arYou didn't provide a valid option for /relocate.\aw");
	WriteChatf("\arPlease do \"/relocate help\" for more information.\aw");
	return;
}

void TransloCmd(SPAWNINFO* pChar, char* szLine)
{
	if (!InGame())
		return;
	CHAR Arg[MAX_STRING] = { 0 };
	GetArg(Arg, szLine, 1);

	if (!_stricmp(Arg, "help")) {
		WriteChatf("\agValid Translocate options are:\aw");
		WriteChatf("\ay/translocate\aw \arRedBot\aw to Translocate \arRedBot\aw to their bind.");
		return;
	}

	if (GetCharInfo2()->Class == Wizard) { // If I am a wizard?
		if (!MyTarget()) {
			if (strlen(Arg) == 0) {
				WriteChatf(static_cast<char*>("\arPlease provide a target for translocate\aw."));
				WriteChatf(static_cast<char*>("\arYou can physically target a player, or provide a name like: \ag/translocate RedBot\aw."));
				return;
			}
			else {
				//Let us see if we can find the target that was provided as an argument.
				SPAWNINFO* desiredTarget = (SPAWNINFO*)GetSpawnByName(Arg);
				SPAWNINFO* me = GetCharInfo()->pSpawn;
				SPELL* pSpell = GetSpellByName("Translocate");
				bool haveAA = false;
				if (pSpell && AltAbility(pSpell->Name) && AltAbility(pSpell->Name)->CurrentRank > 0)
					haveAA = true;
				if (!desiredTarget || !haveAA || !me) {
					if (!desiredTarget)
						WriteChatf("\arI can't find a player with the name \ay%s\aw", Arg);
					if (!haveAA)
						WriteChatf("\arI don't have the AA Translocate");
					if (!me)
						WriteChatf("\arI'm not in game, knock that off!");
					return;
				}
				if (desiredTarget && me && GetDistance3D(desiredTarget->X, desiredTarget->Y, desiredTarget->Z, me->X, me->Y, me->Z) > pSpell->Range) {
					WriteChatf("\arIt seems \ay%s\aw is out of range of \ay%s\aw.", Arg, pSpell->Name); // Double check this spits out both %s
					return;
				}

				TargetSpawn(desiredTarget);

				if (!MyTarget()) {
					WriteChatf("\arWe do not have a target.");
				}
				else if (MyTarget() && MyTarget()->Type != SPAWN_PLAYER) {
					WriteChatf("\ay%s\aw \aris not a Player!\aw", MyTarget()->Type);
				}
				else if (MyTargetID() == Me()->SpawnID) {
					WriteChatf("\arYou can't translocate yourself, knock it off!");
				}
				else {
					canTranslocate = true;
				}
				return;
			}
		} else {
			if (strlen(Arg) == 0) {
				SPAWNINFO* desiredTarget = (SPAWNINFO*)GetSpawnByName(MyTarget()->Name);
				SPAWNINFO* me = GetCharInfo()->pSpawn;
				SPELL* pSpell = GetSpellByName("Translocate");
				if (desiredTarget && me && GetDistance3D(desiredTarget->X, desiredTarget->Y, desiredTarget->Z, me->X, me->Y, me->Z) > pSpell->Range) {
					WriteChatf("\arIt seems \ay%s\aw \aris out of range of \ay%s\aw.", MyTarget()->Name, pSpell->Name);
					return;
				}

				if (MyTarget()->Type != SPAWN_PLAYER) {
					WriteChatf("\ay%s\aw \aris not a Player!\aw", MyTarget()->Name);
					return;
				}

				if (MyTargetID() == Me()->SpawnID) {
					WriteChatf("\arYou can't translocate yourself, knock it off!\aw");
					return;
				}

				canTranslocate = true;
				return;
			}
			else {
				SPAWNINFO* desiredTarget = (SPAWNINFO*)GetSpawnByName(Arg);
				SPAWNINFO* me = GetCharInfo()->pSpawn;
				SPELL* pSpell = GetSpellByName("Translocate");
				if (!desiredTarget) {
					WriteChatf("\arI can't find a player with the name >>> \ay%s\ar <<<\aw.", Arg);
					return;
				}

				if (desiredTarget && desiredTarget->Type != SPAWN_PLAYER) {
					WriteChatf("\ay%s\aw \aris not a player.\aw", desiredTarget);
					return;
				}

				if (desiredTarget->SpawnID == Me()->SpawnID) {
					WriteChatf("\arYou can't translocate yourself, knock it off!\aw");
					return;
				}

				if (desiredTarget && me && GetDistance3D(desiredTarget->X, desiredTarget->Y, desiredTarget->Z, me->X, me->Y, me->Z) > pSpell->Range) {
					WriteChatf("\arIt seems \ay%s\ar is out of range of \ay%s\aw.", Arg, pSpell->Name);
					return;
				}

				TargetSpawn(desiredTarget);

				if (!MyTarget()) {
					WriteChatf("We do not have a target.");
					return;
				}

				if (MyTarget()->Type != SPAWN_PLAYER) {
					WriteChatf("\ay%s\aw \aris not a Player!\aw", MyTarget()->Name);
					return;
				}

				canTranslocate = true;
				return;
			}
		}

	}
	WriteChatf("\arYou are not a Wizard! No Translocate for you\aw!");
}

bool HaveAlias(const std::string& aliasName)
{
	char szTemp[MAX_STRING] = { 0 };
	char mq2dir[128] = "";
	sprintf_s(mq2dir, 128, "%s\\MacroQuest2.ini", gszINIPath);
	GetPrivateProfileString("Aliases", aliasName.c_str(), "None", szTemp, MAX_STRING, mq2dir);

	if (!_stricmp(szTemp, "None")) {
		return false;
	}
	return true;
}

PLUGIN_API void InitializePlugin()
{
	if (HaveAlias("/relocate")) { // Check our aliases
		WriteChatf("\ar[\a-tMQ2Relocate\ar]\ao:: \arIt appears you already have an Alias for \ap/relocate\ar  please type \"\ay/alias /relocate delete\ar\" then reload this plugin.");
		EzCommand("/timed 10 /plugin MQ2Relocate Unload");
	}

	if (HaveAlias("/translocate")) { // Check our aliases
		WriteChatf("\ar[\a-tMQ2Relocate\ar]\ao:: \arIt appears you already have an Alias for \ap/translocate\ar  please type \"\ay/alias /translocate delete\ar\" then reload this plugin.");
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
			WriteChatf("Converting Item"); // Debug purposes
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
		WriteChatf("Relocating with: \ag%s\aw", convertoption);
		needsUsing = false; // Turn off needsusing, because i've used it.
		sprintf_s(convertoption, ""); // Clear the convertitem string so that it's blank and ready for modification again.
	}

	if (canGateAA && AltAbilityReady("Gate")) {
		WriteChatf("Relocating with: \ayGate AA\aw.");
		EzCommand("/alt act 1217");
		canGateAA = false;
	}

	if (canGatePotion) {
		if (UseClickyByItemName("Philter of Major Translocation")) {
			canGatePotion = false;
		}
	}

	if (canOriginAA && AltAbilityReady("Origin")) {
		WriteChatf("Relocating with: \ayOrigin AA\aw.");
		EzCommand("/alt act 331");
		canOriginAA = false;
	}

	if (canLobbyAA && AltAbilityReady("Throne of Heroes")) {
		WriteChatf("Relocating to \agGuild Lobby\aw with: \ayThrone of Heroes AA\aw.");
		EzCommand("/alt act 511");
		canLobbyAA = false;
	}

	if (canHarmonicAA && AltAbilityReady("Harmonic Dissonance")) {
		WriteChatf("Relocating to \agTheater of Blood\aw with: \ayHarmonic Dissonance AA\aw.");
		EzCommand("/alt act 574");
		canHarmonicAA = false;
	}

	if (canEvacAA) { // Check each evac AA
		if (AltAbilityReady("Steathly Getaway")) {
			WriteChatf("Self Evac with: \ayStealthy Getaway AA\aw.");
			EzCommand("/alt act 789");
			canEvacAA = false;
		}

		if (AltAbilityReady("Abscond")) {
			WriteChatf("Self Evac with: \ayAbscond AA\aw.");
			EzCommand("/alt act 490");
			canEvacAA = false;
		}

		if (AltAbilityReady("Egress")) {
			WriteChatf("Self Evac with: \ayEgress AA\aw.");
			EzCommand("/alt act 8602");
			canEvacAA = false;
		}

		if (AltAbilityReady("Levant")) {
			WriteChatf("Self Evac with: \ayLevant AA\aw.");
			EzCommand("/alt act 2899");
			canEvacAA = false;
		}
	}
	if (canGroupEvacAA) { // Check each evac AA
		if (AltAbilityReady("Exodus")) {
			WriteChatf("Group Evac with: \ayExodus AA\aw.");
			EzCommand("/alt act 43");
			canGroupEvacAA = false;
		}
	}

	if (canTranslocate && AltAbilityReady("Translocate")) {
		WriteChatf("Translocating \ay%s\aw with: \ayTranslocate AA\aw.", ((SPAWNINFO*)pTarget)->Name);
		EzCommand("/alt act 9703");
		canTranslocate = false;
	}

	if (canTeleportAA && AltAbilityReady("Teleport")) {
		WriteChatf("Teleporting surrounding allies with: \ayTeleport AA\aw.");
		EzCommand("/alt act 9704");
		canTeleportAA = false;
	}
}

bool UseClickyByItemName(char* pItem)
{
	if (FindItemCountByName(pItem)) {
		if (PCONTENTS item = FindItemByName(pItem)) {
			if (PITEMINFO pIteminf = GetItemFromContents(item)) {
				if ((PVOID)&GetItemFromContents(item)->Clicky) {
					if (pSpellMgr && ItemReady(pIteminf->Name)) {
						UseItem(pIteminf->Name);
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool IsClickyReadyByItemName(char* pItem)
{
	if (FindItemCountByName(pItem)) {
		if (CONTENTS* pItemContents = FindItemByName(pItem)) {
			if (ITEMINFO* pItemInfo = GetItemFromContents(pItemContents)) {
				if ((void*)&GetItemFromContents(pItemContents)->Clicky) {
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
	if (SPAWNINFO* target = (SPAWNINFO*)pTarget) {
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

void StatusItemCheck(char* szItemName)
{
	if (FindItemByName(szItemName)) {
		if (IsClickyReadyByItemName(szItemName)) {
			UseClickyByItemName(szItemName);
			return;
		}
		else {
			WriteChatf("\ay%s \aris not ready\aw!", szItemName);
			return;
		}
	}
	WriteChatf("\arYou do not have a \ay%s\aw!", szItemName);
	return;
}

inline bool InGame()
{
	return(GetGameState() == GAMESTATE_INGAME && GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo2());
}

ALTABILITY* AltAbility(std::string szAltName)
{
	int level = -1;
	if (SPAWNINFO* pMe = (SPAWNINFO*)pLocalPlayer) {
		level = pMe->Level;
	}
	for (auto nAbility = 0; nAbility < AA_CHAR_MAX_REAL; nAbility++) {
		if (ALTABILITY* pAbility = GetAAByIdWrapper(pPCData->GetAlternateAbilityId(nAbility), level)) {
			if (const char* pName = pCDBStr->GetString(pAbility->nName, 1, NULL)) {
				if (!_stricmp(szAltName.c_str(), pName)) {
					return pAbility;
				}
			}
		}
	}
	return nullptr;
}

bool AltAbilityReady(const char* szLine, unsigned long TargetID)
{
	if (!InGame() || IsSpellBookOpen() || IAmDead() || Casting()) return false;
	SPAWNINFO* me = GetCharInfo()->pSpawn;

	if (!me || GlobalLastTimeUsed >= GetTickCount64() || Casting() || Invis(me)) return false;
	int level = -1;

	if (SPAWNINFO* pMe = (SPAWNINFO*)pLocalPlayer) {
		level = pMe->Level;
	}
	for (auto nAbility = 0; nAbility < AA_CHAR_MAX_REAL; nAbility++) {
		if (ALTABILITY* pAbility = GetAAByIdWrapper(pPCData->GetAlternateAbilityId(nAbility), level)) {
			if (const char* pName = pCDBStr->GetString(pAbility->nName, 1, NULL)) {
				if (!_stricmp(szLine, pName)) {
					if (pAbility->SpellID != -1) {
						if (SPELL* myAltAbility = GetSpellByID(pAbility->SpellID)) {
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
	if (InGame()) {
		int n = 0;
		if (!GetCharInfo()->pGroupInfo) {
			return 0;
		}
		for (int i = 1; i < 6; i++) {
			if (GetCharInfo()->pGroupInfo->pMember[i]) n++;
		}

		if (n) n++;
		return n;
	}
	return false;
}

SPAWNINFO* MyTarget()
{
	if (!pTarget) {
		return nullptr;
	}
	if (SPAWNINFO* target = (SPAWNINFO*)pTarget) {
		return target;
	}
	return nullptr;
}

unsigned long MyTargetID()
{
	if (pTarget) {
		return ((SPAWNINFO*)pTarget)->SpawnID;
	}
	return 0;
}

SPAWNINFO* Me()
{
	if (SPAWNINFO* me = GetCharInfo()->pSpawn) {
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

bool Moving(SPAWNINFO* pSpawn)
{
	if (FindSpeed(pSpawn))
		return true;
	else
		return false;
}

bool ItemReady(char* szItem)
{
	if (GlobalLastTimeUsed >= GetTickCount64()) return false;
	if (GetCharInfo2()->Class != EQData::Bard && Casting()) return false;
	if (CONTENTS* item = FindItemByName(szItem, true)) {
		if (ITEMINFO* pIteminf = GetItemFromContents(item)) {
			if (pIteminf->Clicky.TimerID != -1) {
				unsigned long timer = GetItemTimer(item);
				if (timer == 0 && !Moving((SPAWNINFO*)GetCharInfo()->pSpawn))
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

void UseItem(char* pItem)
{
	if (GlobalLastTimeUsed >= GetTickCount64()) return;
	char temp[MAX_STRING] = "/useitem \"";
	strcat_s(temp, MAX_STRING, pItem);
	char temp2[MAX_STRING] = "\"";
	strcat_s(temp, MAX_STRING, temp2);
	EzCommand(temp);
	WriteChatf("Using Item: \ay%s", pItem);
	GlobalLastTimeUsed = GetTickCount64() + GlobalSkillDelay;
}

bool IsSpellBookOpen()
{
	return (CSIDLWND*)pSpellBookWnd->IsVisible();
}

bool IAmDead()
{
	if (SPAWNINFO* Me = GetCharInfo()->pSpawn) {
		if (Me->RespawnTimer) {
			return true;
		}
	}
	return false;
}

bool Invis(SPAWNINFO* pSpawn)
{
	return pSpawn->HideMode;
}

bool DiscReady(SPELL* pSpell)
{
	if (!InGame()) return false;
	unsigned long timeNow = (unsigned long)time(NULL);
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

bool FindPlugin(const char* szPluginName)
{
	if (!strlen(szPluginName)) return false;
	MQPLUGIN* pPlugin = pPlugins;
	while (pPlugin) {
		if (!_stricmp(szPluginName, pPlugin->szFilename)) {
			return true;
		}
		pPlugin = pPlugin->pNext;
	}
	return false;
}

bool TargetSpawn(SPAWNINFO* pSpawn)
{
	if (!pSpawn)
		return false;

	pTarget = (EQPlayer*)pSpawn;

	return true;
}