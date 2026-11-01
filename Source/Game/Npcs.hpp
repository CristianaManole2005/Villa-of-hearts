#pragma once
// Npcs.hpp
// Contestants (Gabriela and Bianca): dialogue, challenge queue, relationship score

#include <vector>
#include <string>
#include <cmath>
#include "glm/glm.hpp"

#include "Challenges.hpp"
#include "PixelArt.hpp"

// Contestants (NPCs)
enum NpcId { GABRIELA = 0, BIANCA = 1 };

struct Npc {
	std::string name;
	glm::vec2 pos;
	glm::vec4 hair, top, bottom;
	const char** bitmap;         // GIRL_BITMAP or CHARACTER_BITMAP
	int relationship;            // 0 - 100
	bool talkedOnce;             // the first conversation has answer choices
	bool heartGiven;             // all challenges of this contestant finished

	// Challenges still to do. The first one is offered next.
	// Finished challenges are removed, so they are never offered again.
	std::vector<int> queue;
	int totalChallenges;
	int completed;

	// First conversation
	std::string greeting;
	std::string answer1, answer2;
	int answer1Effect, answer2Effect;
	std::string reaction1, reaction2;
	std::string thanks;

	// Special dialogue, unlocked with the big heart AND 100/100 relationship
	std::string special1, special2;
	std::string specialAnswer1, specialAnswer2;    // 1 = couple up, 2 = friends
	std::string specialReaction1, specialReaction2;
	std::string almostThere;                       // heart but not 100 yet
	int specialChoice;                             // 0 = not seen, 1 = couple, 2 = friends
};

std::vector<Npc> npcs;

void initNpcs()
{
	npcs.clear();

	Npc gabriela;
	gabriela.name = "GABRIELA";
	gabriela.pos = glm::vec2(26.6f, 12.3f);          // next to the pool
	gabriela.hair = glm::vec4(0.95f, 0.80f, 0.35f, 1.0f);
	gabriela.top = glm::vec4(0.85f, 0.15f, 0.15f, 1.0f);
	gabriela.bottom = glm::vec4(0.98f, 0.98f, 0.98f, 1.0f);   // white skirt
	gabriela.bitmap = GIRL_BITMAP;                         // Gabriela is a girl
	gabriela.relationship = 10;
	gabriela.talkedOnce = false;
	gabriela.heartGiven = false;
	gabriela.queue = { OBSTACLE_COURSE, POOL_CORNERS, RACE };
	gabriela.totalChallenges = (int)gabriela.queue.size();
	gabriela.completed = 0;
	gabriela.greeting = "HEY NEWBIE. I'M GABRIELA. I NEVER LOSE.";
	gabriela.answer1 = "1 - BRING IT ON, I NEVER LOSE EITHER!";
	gabriela.answer2 = "2 - RELAX, IT'S JUST A GAME.";
	gabriela.answer1Effect = +15;                    // Gabriela likes confidence
	gabriela.answer2Effect = -5;
	gabriela.reaction1 = "GABRIELA: HA! I LIKE YOUR ATTITUDE.";
	gabriela.reaction2 = "GABRIELA: JUST A GAME? WE'LL SEE...";
	gabriela.thanks = "GABRIELA: RESPECT. YOU ALREADY HAVE MY HEART.";
	gabriela.special1 = "YOU'RE THE FIRST PERSON WHO EVER KEPT UP WITH ME.";
	gabriela.special2 = "I DON'T SAY THIS OFTEN... I THINK I'M FALLING FOR YOU.";
	gabriela.specialAnswer1 = "1 - I FEEL THE SAME. LET'S BE A POWER COUPLE!";
	gabriela.specialAnswer2 = "2 - YOU'RE AMAZING, BUT LET'S BE BEST FRIENDS.";
	gabriela.specialReaction1 = "GABRIELA: THE VILLA WON'T KNOW WHAT HIT THEM!";
	gabriela.specialReaction2 = "GABRIELA: BEST FRIENDS AND RIVALS. DEAL!";
	gabriela.almostThere = "GABRIELA: YOU'RE GOOD... BUT I'VE SEEN FASTER.";
	gabriela.specialChoice = 0;
	npcs.push_back(gabriela);

	Npc bianca;
	bianca.name = "BIANCA";
	bianca.pos = glm::vec2(11.5f, 13.2f);           // near the fire pit
	bianca.hair = glm::vec4(0.15f, 0.10f, 0.08f, 1.0f);    // dark hair
	bianca.top = glm::vec4(0.15f, 0.65f, 0.55f, 1.0f);     // teal top
	bianca.bottom = glm::vec4(0.55f, 0.80f, 0.95f, 1.0f);  // light blue skirt
	bianca.bitmap = GIRL_BITMAP;                           // Bianca is a girl
	bianca.relationship = 10;
	bianca.talkedOnce = false;
	bianca.heartGiven = false;
	bianca.queue = { TREASURE_HUNT, CATCH_CRAB, FOLLOW_TRAIL };
	bianca.totalChallenges = (int)bianca.queue.size();
	bianca.completed = 0;
	bianca.greeting = "HI! I'M BIANCA. THIS ISLAND IS FULL OF SECRETS.";
	bianca.answer1 = "1 - SHOW ME, I LOVE EXPLORING!";
	bianca.answer2 = "2 - I'D RATHER STAY BY THE POOL.";
	bianca.answer1Effect = +15;                     // Bianca likes curiosity
	bianca.answer2Effect = -5;
	bianca.reaction1 = "BIANCA: YES! A FELLOW ADVENTURER!";
	bianca.reaction2 = "BIANCA: OH... MAYBE I CAN CHANGE YOUR MIND.";
	bianca.thanks = "BIANCA: YOU ALREADY HAVE MY HEART, ADVENTURER.";
	bianca.special1 = "EVERY ADVENTURE IS BETTER WITH YOU BY MY SIDE.";
	bianca.special2 = "WILL YOU EXPLORE THE ISLAND WITH ME... AS MORE THAN FRIENDS?";
	bianca.specialAnswer1 = "1 - YES! LET'S COUPLE UP, BIANCA.";
	bianca.specialAnswer2 = "2 - LET'S STAY ADVENTURE BUDDIES.";
	bianca.specialReaction1 = "BIANCA: THE BEST ADVENTURE STARTS NOW!";
	bianca.specialReaction2 = "BIANCA: BUDDIES FOREVER! HIGH FIVE!";
	bianca.almostThere = "BIANCA: THANKS, FRIEND. MAYBE ONE DAY WE'LL BE MORE...";
	bianca.specialChoice = 0;
	npcs.push_back(bianca);
}

// Relationship formula: keeps the score between 0 and 100
void changeRelationship(int npc, int amount)
{
	npcs[npc].relationship = glm::clamp(npcs[npc].relationship + amount, 0, 100);
}

// Reward for a finished challenge: base 20 points + up to 10 bonus points
// for finishing fast (proportional to the time that was left, rounded).
// Max score: 10 start + 15 good answer + 3 x 25 (half the time left on
// average) = 100, so a good, fast run reaches a full relationship.
const int REWARD_BASE = 20;
const int REWARD_SPEED_BONUS = 10;

int challengeReward(float timeLeft, float timeLimit)
{
	float fraction = glm::clamp(timeLeft / timeLimit, 0.0f, 1.0f);
	return REWARD_BASE + (int)(REWARD_SPEED_BONUS * fraction + 0.5f);
}

// Moves the first challenge of the queue to the end, so the contestant
// offers a different one next time (used after refusing or failing)
void rotateQueue(Npc& n)
{
	if (n.queue.size() > 1) {
		int first = n.queue.front();
		n.queue.erase(n.queue.begin());
		n.queue.push_back(first);
	}
}
