/**
MQ2Relocate by Sic & ChatWithThisName
Purpose & Scope: Plugin to allow easier use of relocation items/aas, as well as specific logic to handle "converting" travel items like Wishing Lamp: and Zeuria Slide:
Usage:
	/relocate help will give a list of available options
**/
#include <mq/Plugin.h>

PreSetup("MQ2Relocate");
PLUGIN_VERSION(2.0);

#define PLUGIN_MSG "\aw[\agRelocate\aw]\ao::\aw "

CAltAbilityData* AltAbility(const char* szAltName);
PlayerClient* MyTarget();
unsigned int MyTargetID();
PlayerClient* Me();
void ReloCmd(PlayerClient* pChar, char* szLine);
void StatusItemCheck(const char* szItemName);
void TransloCmd(PlayerClient* pChar, char* szLine);
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
bool canGateClicky = false;
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

void ReloCmd(PlayerClient* pChar, char* szLine) {
	if (!InGame()) {
		return;
	}

	//Get our parameters
	char Arg[MAX_STRING] = { 0 };
	GetMaybeQuotedArg(Arg, MAX_STRING, szLine, 1);
	if (strlen(Arg)) {
		char temp[MAX_STRING] = "/useitem ";

		if (ci_equals(Arg, "help")) { // Output available arguments for /relocate
			WriteChatf(PLUGIN_MSG "Welcome to MQ2Relocate!");
			WriteChatf(PLUGIN_MSG "By \agSic\aw & \aoChatWithThisName\aw Exclusively for \arRedGuides\aw.");
			WriteChatf(PLUGIN_MSG "Valid \aoRelocate\ax options are:\aw");
			WriteChatf(PLUGIN_MSG "/relocate \agair\aw, \agfire\aw, and \agstone\aw for \ayZephyr's Lamp\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \agstonebrunt\aw, \agdreadlands\aw, \aggreatdivide\aw, \agnek\aw, \agnro\aw, and \agskyfire\aw for \ayZueria Slide\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \agpok\aw for \ayPlane of Knowledge\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \aggate\aw to use your \ayGate AA\aw or \ayTranslocation Potion\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \agorigin\aw to use your \ayOrigin AA\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \agbrell\aw to use your \ayMark of Brell\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \agbronze\aw to use your \ayHarbinger's Staff\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \aganchor\aw to use your \ayPrimary \awor \aySecondary Anchor\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \aganchor1\aw to use your \ayPrimary Anchor\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \aganchor2\aw to use your \aySecondary Anchor\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \agfellow\aw or \agfellowship\aw to use your \ayFellowship Insignia\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \aglobby\aw to use your \ay Throne of Heroes AA\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \agblood\aw to use your \ay Theatre of Blood AA\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \agcrystal\aw to use your \ayFroststone Crystal Resonator\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \aglaurion\aw to use your \ayLaurion Inn Lute\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \agumbral\aw to use your \ayUmbral Plains Mushroom\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \agshadow haven\aw to use your \ayLost Turnip Sign\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \agskyshrine\aw to use your \aySkyshrine Crystal\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \agevac\aw to use your \ay Group Evac AA\aw (if you are in a group) or your \ay Personal Evac AA\aw.");
			WriteChatf(PLUGIN_MSG "/relocate \agteleport\aw to use your \ay AoE Teleport AA\aw.");
			WriteChatf(PLUGIN_MSG "Valid \aoTranslocate\ax options are:\aw");
			WriteChatf(PLUGIN_MSG "/translocate\aw to \ayTranslocate\aw your target to their bind.");
			WriteChatf(PLUGIN_MSG "/translocate\aw \arRedBot\aw to \ayTranslocate\aw \arRedBot\aw to their bind.");
			return;
		}

		if (ci_equals(Arg, "air") || ci_equals(Arg, "fire") || ci_equals(Arg, "stone")) { // Use Wishing Lamp:
			const char reloClicky[64] = "Wishing Lamp:";
			const char air[64] = "Wishing Lamp: Zephyr's Flight";
			const char fire[64] = "Wishing Lamp: Palace of Embers";
			const char stone[64] = "Wishing Lamp: The Stone Demesne";

			if (!FindItemByName(reloClicky)) {
				WriteChatf(PLUGIN_MSG "\arYou do not appear to have a \ayWishing Lamp.");
				return;
			}

			if (ci_equals(Arg, "air")) {
				if (FindItemByName(air)) {
					strcat_s(temp, air);
					EzCommand(temp);
					WriteChatf(PLUGIN_MSG "\agRelocating with: \ay%s ", air);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, air);
				}
			}

			if (ci_equals(Arg, "fire")) {
				if (FindItemByName(fire)) {
					strcat_s(temp, fire);
					EzCommand(temp);
					WriteChatf(PLUGIN_MSG "\agRelocating with: \ay%s ", fire);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, fire);
				}
			}

			if (ci_equals(Arg, "stone")) {
				if (FindItemByName(stone)) {
					strcat_s(temp, stone);
					EzCommand(temp);
					WriteChatf(PLUGIN_MSG "\agRelocating with: \ay%s ", stone);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, stone);
				}
			}
			return;
		}

		if (ci_equals(Arg, "stonebrunt") || ci_equals(Arg, "dreadlands") || ci_equals(Arg, "nek") || ci_equals(Arg, "greatdivide") || ci_equals(Arg, "nro") || ci_equals(Arg, "skyfire")) { // Zueria Slide
			const char reloClicky[64] = "Zueria Slide:";
			const char stonebrunt[64] = "Zueria Slide: Stonebrunt";
			const char dreadlands[64] = "Zueria Slide: Dreadlands";
			const char greatdivide[64] = "Zueria Slide: Great Divide";
			const char nek[64] = "Zueria Slide: Nektulos";
			const char nro[64] = "Zueria Slide: North Ro";
			const char nro2[64] = "Northern Desert Outlook Device";
			const char skyfire[64] = "Zueria Slide: Skyfire";

			if (!FindItemByName(reloClicky)) {
				WriteChatf(PLUGIN_MSG "\arYou do not appear to have a Zueria Slide.");
				return;
			}

			if (ci_equals(Arg, "stonebrunt")) {
				if (FindItemByName(stonebrunt)) {
					strcat_s(temp, stonebrunt);
					EzCommand(temp);
					WriteChatf(PLUGIN_MSG "\agRelocating with: \ay%s ", stonebrunt);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, stonebrunt);
				}
			}

			if (ci_equals(Arg, "dreadlands")) {
				if (FindItemByName(dreadlands)) {
					strcat_s(temp, dreadlands);
					EzCommand(temp);
					WriteChatf(PLUGIN_MSG "\agRelocating with: \ay%s ", dreadlands);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, dreadlands);
				}
			}

			if (ci_equals(Arg, "greatdivide")) {
				if (FindItemByName(greatdivide)) {
					strcat_s(temp, greatdivide);
					EzCommand(temp);
					WriteChatf(PLUGIN_MSG "\agRelocating with: \ay%s ", greatdivide);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, greatdivide);
				}
			}

			if (ci_equals(Arg, "nek")) {
				if (FindItemByName(nek)) {
					strcat_s(temp, nek);
					EzCommand(temp);
					WriteChatf(PLUGIN_MSG "\agRelocating with: \ay%s ", nek);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, nek);
				}
			}

			if (ci_equals(Arg, "nro")) {
				if (FindItemByName(nro)) {
					strcat_s(temp, nro);
					EzCommand(temp);
					WriteChatf(PLUGIN_MSG "\agRelocating with: \ay%s ", nro);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, nro);
				}
			}

			if (ci_equals(Arg, "skyfire")) {
				if (FindItemByName(skyfire)) {
					strcat_s(temp, skyfire);
					EzCommand(temp);
					WriteChatf(PLUGIN_MSG "\agRelocating with: \ay%s ", skyfire);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, skyfire);
				}
			}
			return;
		}

		if (ci_equals(Arg, "pok")) { // Plane of Knowledge
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
				WriteChatf(PLUGIN_MSG "\aoDOH!\ar You don't have a \aypok \arclicky that is ready!");
				return;
			}

			if (FindItemByName(reloClicky)) {
				if (UseClickyByItemName(reloClicky)) {
					//WriteChatf("Casting \ag %s \aw ", reloClicky);
				}
			}
			else {
				// This else for error checking and should never happen.
				WriteChatf(PLUGIN_MSG "\arI don't have a %s", reloClicky);
			}
			return;
		}

		if (ci_equals(Arg, "gate")) { // Use gate AA if you have it, otherwise try and use a gate potion
			if (AltAbility("Gate") && AltAbility("Gate")->CurrentRank > 0) {
				canGateAA = true;
				return;
			}

			if (!canGateAA) {
				// Bulwark of Many portals is a mage summoned item, it has charges, but easy to come by
				if (FindItemByName("Bulwark of Many Portals")) {
					canGateClicky = true;
				}
				// Check if i have a Philter of Major Translocation
				else if (FindItemByName("Philter of Major Translocation")) {
					canGateClicky = true;
				}
				// "Gate Potion" is a progression server version
				else if (FindItemByName("Gate Potion"), true) {
					canGateClicky = true;
				}
				else {
					WriteChatf(PLUGIN_MSG "\arI don't seem to have the ability to \aygate\ax, nor do you have a \aygate potion\ax!\aw");
				}
			}
			return;
		}

		if (ci_equals(Arg, "origin")) { // Try and use Origin AA, otherwise try and use Sceptre of Draconic Calling
			if (AltAbility("Origin") && AltAbility("Origin")->CurrentRank > 0){
				if (AltAbilityReady("Origin")) {
					canOriginAA = true;
				}
				else {
					WriteChatf(PLUGIN_MSG "Origin \arisn't ready right now\aw.");
				}

				if ((pLocalPC->StartingCity == 394) && FindItemByName("Sceptre of Draconic Calling")) {
					if (IsClickyReadyByItemName("Sceptre of Draconic Calling")) {
						UseClickyByItemName("Sceptre of Draconic Calling");
					}
					else {
						WriteChatf(PLUGIN_MSG "\aySceptre of Draconic Calling \arisn't ready right now\aw!");
					}
				}
			}
			else {
				WriteChatf(PLUGIN_MSG "\arWe don't have a way to \ayOrigin\ax at the moment\aw.");
			}
			return;
		}

		if (ci_equals(Arg, "brell")) { // Mark of Brell for Brell's Rest
			StatusItemCheck("Mark of Brell");
			return;
		}

		if (ci_equals(Arg, "bronze")) { // Harbinger's Staff for City of Bronze
			StatusItemCheck("Harbinger's Staff");
			return;
		}

		if (ci_equals(Arg, "anchor")) { // Use Primary or Secondary Anchor
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
				WriteChatf(PLUGIN_MSG "\arDOH!\aw I don't have an \ayanchor\ax clicky that is ready");
			}
			return;
		}

		if (ci_equals(Arg, "anchor1")) { // Use specifically Primary Anchor
			StatusItemCheck("Primary Anchor Transport Device");
			return;
		}

		if (ci_equals(Arg, "anchor2")) { // Use specifically Secondary Anchor
			StatusItemCheck("Secondary Anchor Transport Device");
			return;
		}

		if (ci_equals(Arg, "fellow") || ci_equals(Arg, "fellowship")) { // Use fellowship Insignia
			if (FindItemByName("Fellowship Registration Insignia")) {
				if (IsClickyReadyByItemName("Fellowship Registration Insignia")) {
					if (pChar->HideMode) { // Fellowship insignia requires being visible to use
						MakeMeVisible(pLocalPlayer, "");
					}
					if (pLocalPlayer->Campfire) {
						UseClickyByItemName("Fellowship Registration Insignia");
					}
					else {
						WriteChatf(PLUGIN_MSG "\arYou do not have a campfire up\aw.");
					}
				}
				else {
					WriteChatf(PLUGIN_MSG "\ayFellowship Registration Insignia \aris not ready!\aw");
				}
			}
			else {
				WriteChatf(PLUGIN_MSG "\arYou do not have a Fellowship Registration Insigna!\aw");
			}
			return;
		}

		if (ci_equals(Arg, "lobby")) {
			if (AltAbility("Throne of Heroes") && AltAbility("Throne of Heroes")->CurrentRank > 0) {
				if (AltAbilityReady("Throne of Heroes")) {
					canLobbyAA = true;
				}
				else {
					WriteChatf(PLUGIN_MSG "\ayThrone of Heroes \arisn't ready right now\aw.");
				}
			}
			else {
				WriteChatf(PLUGIN_MSG "\arI don't have the \ayThrone of Heroes \arAA\aw!");
			}
			return;
		}

		if (ci_equals(Arg, "blood")) {
			if (AltAbility("Harmonic Dissonance") && AltAbility("Harmonic Dissonance")->CurrentRank > 0) {
				if (AltAbilityReady("Harmonic Dissonance")) {
					canHarmonicAA = true;
				}
				else {
					WriteChatf(PLUGIN_MSG "\ayHarmonic Dissonance \arisn't ready right now\aw.");
				}
			}
			else {
				WriteChatf(PLUGIN_MSG "\arI don't have the \ayHarmonic Dissonance \arAA\aw!");
			}
			return;
		}

		if (ci_equals(Arg, "evac")) { // use evac AA if you have it
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
			else if (pLocalPC->GetClass() == Druid || pLocalPC->GetClass() == Wizard) { // only Wizards/Druids can use the Di`Zok Escape Staff
				StatusItemCheck("Di`Zok Escape Staff");
				return;
			}

			if (!canEvacAA && !canGroupEvacAA) {
				WriteChatf(PLUGIN_MSG "\arI don't seem to have the ability to \ayevac\ax!\aw");
			}

			return;
		}

		if (ci_equals(Arg, "teleport")) {
			if (AltAbility("Teleport") && AltAbility("Teleport")->CurrentRank > 0) {
				if (AltAbilityReady("Teleport")) {
					canTeleportAA = true;
				}
				else {
					WriteChatf(PLUGIN_MSG "Teleport \arisn't ready right now\aw.");
				}
			}
			else {
				WriteChatf(PLUGIN_MSG "\arI don't have the \ayTeleport \arAA\aw!");
			}
			return;
		}

		if (ci_equals(Arg, "crystal")) { // Froststone Crystal Resonator ToV pre-order item
			StatusItemCheck("Froststone Crystal Resonator");
			return;
		}

		if (!_stricmp(Arg, "laurion")) { // Laurion Inn LS self clicky
			StatusItemCheck("Laurion Inn Lute");
			return;
		}

		if (!_stricmp(Arg, "umbral")) { // Umbral Plains Mushroom ToL self clicky
			StatusItemCheck("Umbral Plains Mushroom");
			return;
		}

		if (!_stricmp(Arg, "shadow haven")) { // Lost Turnip Sign NoS self clicky
			StatusItemCheck("Lost Turnip Sign");
			return;
		}

		if (!_stricmp(Arg, "skyshrine")) { // Lost Turnip Sign CoV self clicky
			StatusItemCheck("Skyshrine Crystal");
			return;
		}
	}
	WriteChatf(PLUGIN_MSG "\arYou didn't provide a valid option for /relocate.\aw");
	WriteChatf(PLUGIN_MSG "\arPlease do \ao\"\ay/relocate help\ao\"\ax for more information.\aw");
	return;
}

void TransloCmd(PlayerClient* pChar, char* szLine) {
	if (!InGame()) {
		return;
	}
	char Arg[MAX_STRING] = { 0 };
	GetArg(Arg, szLine, 1);

	if (ci_equals(Arg, "help")) {
		WriteChatf(PLUGIN_MSG "\agValid Translocate options are:\aw");
		WriteChatf(PLUGIN_MSG "\ay/translocate \arRedBot\aw to Translocate \arRedBot\aw to their bind.");
		return;
	}

	if (pLocalPC->GetClass() == Wizard) { // If I am a wizard?
		PlayerClient* me = pLocalPlayer;
		EQ_Spell* pSpell = GetSpellByName("Translocate");
		if (!MyTarget()) {
			if (strlen(Arg) == 0) {
				WriteChatf(PLUGIN_MSG "\arPlease provide a target for \aytranslocate\aw.");
				WriteChatf(PLUGIN_MSG "\arYou can physically target a player, or provide a name like: \ag/translocate RedBot\aw.");
				return;
			}
			else {
				//Let us see if we can find the target that was provided as an argument.
				PlayerClient* desiredTarget = GetSpawnByName(Arg);
				bool haveAA = false;
				if (pSpell && AltAbility(pSpell->Name) && AltAbility(pSpell->Name)->CurrentRank > 0) {
					haveAA = true;
				}

				if (!desiredTarget || !haveAA || !me) {
					if (!desiredTarget) {
						WriteChatf(PLUGIN_MSG "\arI can't find a player with the name \ay%s\aw", Arg);
					}
					if (!haveAA) {
						WriteChatf(PLUGIN_MSG "\arI don't have the AA Translocate");
					}
					if (!me) {
						WriteChatf(PLUGIN_MSG "\arI'm not in game, knock that off!");
					} 
					return;
				}

				if (desiredTarget && me && GetDistance3D(desiredTarget->X, desiredTarget->Y, desiredTarget->Z, me->X, me->Y, me->Z) > pSpell->Range) {
					WriteChatf(PLUGIN_MSG "\arIt seems \ay%s\aw is out of range of \ay%s\aw.", Arg, pSpell->Name);
				}

				pTarget = desiredTarget;

				if (!MyTarget()) {
					WriteChatf(PLUGIN_MSG "\arWe do not have a target.");
				}
				else if (MyTarget() && MyTarget()->Type != SPAWN_PLAYER) {
					WriteChatf(PLUGIN_MSG "\ay%s\aw \aris not a Player!\aw", MyTarget()->Type);
				}
				else if (MyTargetID() == Me()->SpawnID) {
					WriteChatf(PLUGIN_MSG "\arYou can't translocate yourself, knock it off!");
				}
				else {
					canTranslocate = true;
				}
				return;
			}
		}
		else {
			if (strlen(Arg) == 0) {
				PlayerClient* desiredTarget = MyTarget();
				if (desiredTarget && me && GetDistance3D(desiredTarget->X, desiredTarget->Y, desiredTarget->Z, me->X, me->Y, me->Z) > pSpell->Range) {
					WriteChatf(PLUGIN_MSG "\arIt seems \ay%s\aw \aris out of range of \ay%s\aw.", MyTarget()->Name, pSpell->Name);
					return;
				}

				if (MyTarget()->Type != SPAWN_PLAYER) {
					WriteChatf(PLUGIN_MSG "\ay%s\aw \aris not a Player!\aw", MyTarget()->Name);
					return;
				}

				if (MyTargetID() == Me()->SpawnID) {
					WriteChatf(PLUGIN_MSG "\arYou can't translocate yourself, knock it off!\aw");
					return;
				}

				canTranslocate = true;
				return;
			}
			else {
				PlayerClient* desiredTarget = GetSpawnByName(Arg);
				if (!desiredTarget) {
					WriteChatf(PLUGIN_MSG "\arI can't find a player with the name >>> \ay%s\ar <<<\aw.", Arg);
					return;
				}

				if (desiredTarget && desiredTarget->Type != SPAWN_PLAYER) {
					WriteChatf(PLUGIN_MSG "\ay%s\aw \aris not a player.\aw", desiredTarget);
					return;
				}

				if (desiredTarget->SpawnID == Me()->SpawnID) {
					WriteChatf(PLUGIN_MSG "\arYou can't translocate yourself, knock it off!\aw");
					return;
				}

				if (desiredTarget && me && GetDistance3D(desiredTarget->X, desiredTarget->Y, desiredTarget->Z, me->X, me->Y, me->Z) > pSpell->Range) {
					WriteChatf(PLUGIN_MSG "\arIt seems \ay%s\ar is out of range of \ay%s\aw.", Arg, pSpell->Name);
					return;
				}

				pTarget = desiredTarget;

				if (!MyTarget()) {
					WriteChatf("\arWe do not have a target.");
					return;
				}

				if (MyTarget()->Type != SPAWN_PLAYER) {
					WriteChatf(PLUGIN_MSG "\ay%s\aw \aris not a Player!\aw", MyTarget()->Name);
					return;
				}

				canTranslocate = true;
				return;
			}
		}

	}
	WriteChatf(PLUGIN_MSG "\arYou are not a Wizard! No Translocate for you\aw!");
}

bool HaveAlias(const std::string& aliasName) {
	const std::string alias = GetPrivateProfileString("Aliases", aliasName, "None", gPathMQini);
	if (alias == "None") {
		return false;
	}

	return true;
}

PLUGIN_API void InitializePlugin() {
	if (HaveAlias("/relocate")) { // Check our aliases
		WriteChatf(PLUGIN_MSG "\arIt appears you already have an Alias for \ap/relocate\ar  please type \"\ay/alias /relocate delete\ar\" then reload this plugin.");
		EzCommand("/timed 10 /plugin MQ2Relocate Unload");
	}

	if (HaveAlias("/translocate")) { // Check our aliases
		WriteChatf(PLUGIN_MSG "\arIt appears you already have an Alias for \ap/translocate\ar  please type \"\ay/alias /translocate delete\ar\" then reload this plugin.");
		EzCommand("/timed 10 /plugin MQ2Relocate Unload");
	}
	else {
		AddCommand("/relocate", ReloCmd, false, true, true);
		AddCommand("/translocate", TransloCmd, false, true, true);
	}
}

PLUGIN_API void ShutdownPlugin() {
	RemoveCommand("/relocate");
	RemoveCommand("/translocate");
}

PLUGIN_API void OnPulse() {
	if (++iPulse < iPulseDelay) {
		return;
	}
	iPulse = 0;

	//Base Cases
	if (!InGame()) {
		return;
	}

	//DON'T FORGET TO CHANGE THE FREAKING BASE CASES SO THAT YOU ACTUALLY CONTINUE INTO THE FUNCTION! (WTB back many minutes of my life)
	if (!strlen(convertoption) && !bAmConverting && !needsUsing && !canGateClicky && !canGateAA && !canOriginAA && !canLobbyAA && !canHarmonicAA && !canEvacAA && !canGroupEvacAA && !canTranslocate && !canTeleportAA) {
		return;
	}

	if (ImDucking() || Casting() || Moving(pLocalPlayer)) {
		return;
	}

	char temp[MAX_STRING] = "";

	if (bAmConverting) {
		if (!FindItemByName(convertoption) && FindItemByName(reloClicky)) {
			sprintf_s(temp, "/convertitem %s", convertoption);
			WriteChatf(PLUGIN_MSG "Converting Item"); // Debug purposes
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
		WriteChatf(PLUGIN_MSG "\agRelocating with: \ag%s\aw", convertoption);
		needsUsing = false; // Turn off needsusing, because i've used it.
		sprintf_s(convertoption, ""); // Clear the convertitem string so that it's blank and ready for modification again.
	}

	if (canGateAA) {
		WriteChatf(PLUGIN_MSG "\agRelocating with: \ayGate AA\aw.");
		EzCommand("/alt act 1217");
		canGateAA = false;
	}

	if (canGateClicky) {
		if (UseClickyByItemName("Philter of Major Translocation")) {
			canGateClicky = false;
		}
		else if (UseClickyByItemName("Gate Potion")) {
			canGateClicky = false;
		}
	}

	if (canOriginAA) {
		WriteChatf(PLUGIN_MSG "\agRelocating with: \ayOrigin AA\aw.");
		EzCommand("/alt act 331");
		canOriginAA = false;
	}

	if (canLobbyAA) {
		WriteChatf(PLUGIN_MSG "\agRelocating to \agGuild Lobby\aw with: \ayThrone of Heroes AA\aw.");
		EzCommand("/alt act 511");
		canLobbyAA = false;
	}

	if (canHarmonicAA) {
		WriteChatf(PLUGIN_MSG "\agRelocating to \agTheater of Blood\aw with: \ayHarmonic Dissonance AA\aw.");
		EzCommand("/alt act 574");
		canHarmonicAA = false;
	}

	if (canEvacAA) { // Check each evac AA
		if (AltAbilityReady("Steathly Getaway")) {
			WriteChatf(PLUGIN_MSG "\agSelf Evac with: \ayStealthy Getaway AA\aw.");
			EzCommand("/alt act 789");
			canEvacAA = false;
		}

		if (AltAbilityReady("Abscond")) {
			WriteChatf(PLUGIN_MSG "\agSelf Evac with: \ayAbscond AA\aw.");
			EzCommand("/alt act 490");
			canEvacAA = false;
		}

		if (AltAbilityReady("Egress")) {
			WriteChatf("\agSelf Evac with: \ayEgress AA\aw.");
			EzCommand("/alt act 8602");
			canEvacAA = false;
		}

		if (AltAbilityReady("Levant")) {
			WriteChatf(PLUGIN_MSG "\agSelf Evac with: \ayLevant AA\aw.");
			EzCommand("/alt act 2899");
			canEvacAA = false;
		}
	}

	if (canGroupEvacAA) { // Check each evac AA
		WriteChatf(PLUGIN_MSG "\agGroup Evac with: \ayExodus AA\aw.");
		EzCommand("/alt act 43");
		canGroupEvacAA = false;
	}

	if (canTranslocate) {
		WriteChatf(PLUGIN_MSG "\agTranslocating \ay%s\aw with: \ayTranslocate AA\aw.", pTarget->Name);
		EzCommand("/alt act 9703");
		canTranslocate = false;
	}

	if (canTeleportAA) {
		WriteChatf(PLUGIN_MSG "\agTeleporting surrounding allies with: \ayTeleport AA\aw.");
		EzCommand("/alt act 9704");
		canTeleportAA = false;
	}
}

bool UseClickyByItemName(const char* pItem) {
	if (ItemClient* item = FindItemByName(pItem)) {
		if (EQ_Spell* itemSpell = GetSpellByID(item->GetSpellID(ItemSpellType_Clicky))) {
			if (ItemReady(item->GetName())) {
				UseItem(item->GetName());
				return true;
			}
		}
	}
	return false;
}

bool IsClickyReadyByItemName(const char* pItem) {
	if (ItemClient* item = FindItemByName(pItem)) {
		if (EQ_Spell* itemSpell = GetSpellByID(item->GetSpellID(ItemSpellType_Clicky))) {
			if (ItemReady(item->GetName())) {
				return true;
			}
		}
	}
	return false;
}

bool IsTargetPlayer() {
	return pTarget && pTarget->Type == SPAWN_PLAYER;
}

PLUGIN_API void SetGameState(unsigned long GameState) {
	if (GameState != GAMESTATE_INGAME) {
		if (bAmConverting) {
			bAmConverting = false;
		}

		if (needsUsing) {
			needsUsing = false;
		}

		if (strlen(convertoption)) {
			sprintf_s(convertoption, "");
		}
	}
}

void StatusItemCheck(const char* szItemName) {
	if (FindItemByName(szItemName)) {
		if (IsClickyReadyByItemName(szItemName)) {
			UseClickyByItemName(szItemName);
			return;
		}
		else {
			WriteChatf(PLUGIN_MSG "%s \aris not ready\aw!", szItemName);
			return;
		}
	}
	WriteChatf(PLUGIN_MSG "\arYou do not have a \ay%s\aw!", szItemName);
}

inline bool InGame() {
	return (GetGameState() == GAMESTATE_INGAME && pLocalPC && pLocalPlayer && GetPcProfile());
}

CAltAbilityData* AltAbility(const char* szAltName) {
	for (int nAbility = 0; nAbility < AA_CHAR_MAX_REAL; nAbility++) {
		if (CAltAbilityData* pAbility = GetAAById(pPCData->GetAlternateAbilityId(nAbility), pLocalPlayer->Level)) {
			if (const char* pName = pCDBStr->GetString(pAbility->nName, eAltAbilityName)) {
				if (ci_equals(szAltName, pName)) {
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
		WriteChatf(PLUGIN_MSG "We are going to /stopsong and /twist off, to use our relocate items.");
	}
}

bool CastingCheck() {
	if (pLocalPlayer->GetClass() == Bard) {
		BardStopSinging();
	}
	else if (Casting()) {
		return true;
	}
	return false;
}

bool AltAbilityReady(const char* szLine, const unsigned long TargetID) {
	if (!InGame() || !pLocalPlayer || GlobalLastTimeUsed >= GetTickCount64() || IsSpellBookOpen() || IAmDead()) {
		return false;
	}

	if (CastingCheck()) {
		return false;
	}

	for (auto nAbility = 0; nAbility < AA_CHAR_MAX_REAL; nAbility++) {
		if (CAltAbilityData* pAbility = GetAAByIdWrapper(pPCData->GetAlternateAbilityId(nAbility), pLocalPlayer->Level)) {
			if (const char* pName = pCDBStr->GetString(pAbility->nName, eAltAbilityName)) {
				if (ci_equals(szLine, pName)) {
					if (pAbility->SpellID != -1) {
						if (EQ_Spell* myAltAbility = GetSpellByID(pAbility->SpellID)) {
							//Am I in motion?
							if (myAltAbility->CastTime && Moving(Me())) {
								return false;
							}
							if (DiscReady(myAltAbility)) {
								return pAltAdvManager->IsAbilityReady(pPCData, pAbility, 0);
							}
						}
					}
				}
			}
		}
	}
	return false;
}

int GroupSize() {
	if (pCharData) {
		if (pCharData->Group) {
			return pCharData->Group->GetNumberOfMembers();
		}
	}
	return 0;
}

PlayerClient* MyTarget() {
	return pTarget;
}

unsigned int MyTargetID() {
	return pTarget ? pTarget->SpawnID : 0;
}

PlayerClient* Me() {
	return pLocalPlayer;
}

bool ImDucking() {
	return Me()->StandState == STANDSTATE_DUCK;
}

bool Casting() {
	return pLocalPlayer->CastingData.IsCasting();
}

bool Moving(PlayerClient* pSpawn) {
	// 0.01f gets reported when levitating
	return FindSpeed(pSpawn) >= 0.01f;
}

bool ItemReady(const char* szItem) {
	if (GlobalLastTimeUsed >= GetTickCount64()) {
		return false;
	}

	if (pLocalPC->GetClass() != Bard && Casting()) {
		return false;
	}

	if (ItemClient* item = FindItemByName(szItem, true)) {
		if (item->GetSpellRecastTime(ItemSpellType_Clicky) != -1) {
			uint32_t timer = GetItemTimer(item);
			if (timer == 0 && !Moving(pLocalPlayer)) {
				return true;
			}
		}
	}

	return false;
}

void UseItem(const char* pItem) {
	if (GlobalLastTimeUsed >= GetTickCount64()) {
		return;
	}

	char temp[MAX_STRING] = "/useitem \"";
	strcat_s(temp, MAX_STRING, pItem);
	char temp2[MAX_STRING] = "\"";
	strcat_s(temp, MAX_STRING, temp2);
	EzCommand(temp);
	WriteChatf(PLUGIN_MSG "Using Item: \ay%s", pItem);
	GlobalLastTimeUsed = GetTickCount64() + GlobalSkillDelay;
}

bool IsSpellBookOpen() {
	return pSpellBookWnd->IsVisible();
}

bool IAmDead() {
	return pLocalPlayer && pLocalPlayer->RespawnTimer;
}

bool Invis(PlayerClient* pSpawn) {
	return pSpawn->HideMode;
}

bool DiscReady(EQ_Spell* pSpell) {
	if (!InGame()) {
		return false;
	}

	time_t timeNow = time(NULL);

	if (pPCData->GetCombatAbilityTimer(pSpell->ReuseTimerIndex, pSpell->SpellGroup) < timeNow) {
		return true;
	}
	return false;
}