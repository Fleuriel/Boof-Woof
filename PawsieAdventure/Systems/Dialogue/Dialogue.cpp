#include "Dialogue.h"
#include "ResourceManager/ResourceManager.h"
#include "../ChangeText/ChangeText.h"
#include "../Core/AssetManager/FilePaths.h"
#include "../../../BoofWoof/Utilities/ForGame/TimerTR/TimerTR.h"

Dialogue g_DialogueText;
Serialization dialogueText;

void Dialogue::OnInitialize()
{
	if (!m_DialogueActive) 
	{
		g_SceneManager.LoadScene(FILEPATH_ASSET_SCENES + "/Dialogue.json");
		storage = dialogueText.GetStored();

		std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

		for (auto entity : entities)
		{
			if (g_Coordinator.HaveComponent<MetadataComponent>(entity))
			{
				if (g_Coordinator.GetComponent<MetadataComponent>(entity).GetName() == "Row1")
				{
					m_D1 = entity;
					break;
				}
			}
		}
		m_DialogueActive = true;
	}
}

void Dialogue::OnUpdate(double deltaTime)
{
	// Click on the screen to move to the close dialogue
	if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_LEFT))
	{
		OnShutdown();
	}
}

void Dialogue::OnShutdown()
{
	std::vector<Entity> entities = g_Coordinator.GetAliveEntitiesSet();

	for (auto i = entities.begin(); i != entities.end(); i++)
	{
		for (auto k = storage.begin(); k != storage.end(); k++)
		{
			if (*k == *i)
			{
				g_Coordinator.DestroyEntity(*i);
			}
		}
	}

	m_DialogueActive = false;
	g_ChangeText.OnShutdown();
}

std::string Dialogue::getDialogue()
{
	switch (m_CurrentState)
	{
	case DialogueState::DEFAULT:
		return "";

	/* Starting Room Dialogues */
	case DialogueState::TUTORIALSTART:
		return "I have to get out of here.. One step at a time..";

	case DialogueState::TUTORIALEND:
		return "I can smell something.. It's coming from-there! Could this be a way out?";
	
	/* TimeRush Dialogues */
	case DialogueState::ENTEREDLIBRARY:
		return "The library?! No, no, no.. If Rex finds me here, I'm done for.. I have to find a way out-FAST!";

	case DialogueState::TOUCHBALL:
		return "Oh! My tennis ball! Master and I used to play with this all the time..";

	case DialogueState::TOUCHBONE:
		return "My favorite snack!!";

	case DialogueState::DONTWASTETIME:
		return "No, I can't waste time-I have to get out before Rex finds me!";

	/* MainHall Lvl1 Dialogues */
	case DialogueState::OUTOFLIBRARY:
		return "Phew.. We're out, but Rex will notice soon. I need to stay sharp! One wrong move, and I'm his next chew toy.";

	case DialogueState::REXSAWYOU:
		return "Oh no, he saw me!! Gotta hide-NOW!";

	case DialogueState::SEARCHINGFORPUPS:
		return "Where are my kiddos at..? Hang in there, I'm coming!";

	case DialogueState::FOUNDPUP1:
		return "I found you! But this stupid lock.. Time to bite it to pieces!! Grr!!";

	case DialogueState::TWOMORETOGO:
		return "One down.. Two more to go. Stay close, kiddo!";

	case DialogueState::DISGUSTED:
		return "Eww!! Seriously, Rex?! Why pee all over the place?! I gotta clean this off before he sniffs me out!";

	/* MainHall Lvl2 Dialogues ? */
	case DialogueState::SEARCHINGFORPUPS2:
		return "If I were a pup, where would I be..? Gotta check everywhere—can't leave without all of them!";

	case DialogueState::FOUNDPUP2:
		return "There you are! I knew I'd find you!";

	case DialogueState::ONEMORETOGO:
		return "That's two! Stay close-we've got one more to save!";

	case DialogueState::SEARCHINGFORPUPS3:
		return "Hmm.. That last pup has to be around here somewhere. Maybe if I climb up there..";

	case DialogueState::FOUNDPUP3:
		return "Locked.. again?! Alright, let’s do this the hard way—grr!!";

	case DialogueState::ALLPUPSFOUND:
		return "That's all three.. We’re finally together again! Time to escape this hellhole!";

	case DialogueState::STAYCLOSE:
		return "Stay close, little ones. We're almost there.. Just gotta avoid that brute.";

	case DialogueState::REXSAWYOU2:
		return "Oh no—RUN! Don't let him catch us!";

	/* RopeBreaker & Escape Dialogues */
	case DialogueState::BREAKROPES:
		return "We made it.. But we're not safe yet. I need to break these ropes!";

	case DialogueState::BROKEROPE1:
		return "One down—one more to go!";

	case DialogueState::FREED:
		return "We did it.. We're free!";
	}

	return std::string();
}

void Dialogue::setDialogue(DialogueState newState)
{
	m_CurrentState = newState;

	if (g_Coordinator.HaveComponent<FontComponent>(m_D1)) {
		g_Coordinator.GetComponent<FontComponent>(m_D1).set_text(textWrap(getDialogue()));
	}
}

std::string Dialogue::textWrap(const std::string& input)
{
	size_t nws = input.find_first_not_of(" \t\n\r");
	std::string format = input.substr(nws);

	// Find the last whitespace character at or before the 42nd char
	for (size_t currentPos{ 42 }; currentPos <= format.length(); currentPos += 42)
	{
		// Find the last whitespace before this position
		size_t insPt = format.rfind(' ', currentPos);
		if (insPt == std::string::npos) {
			break;  // No more whitespace found, exit the loop
		}

		// Insert newline at the found space position
		format.insert(insPt, "\n");

		// Remove space after the inserted newline if it exists
		if (insPt + 1 < format.length() && std::isspace(format[insPt + 1])) {
			format.erase(insPt + 1, 1);
		}
	}
	return format;
}

void Dialogue::checkCollision(Entity player, double dt)
{
	if (!g_Coordinator.HaveComponent<CollisionComponent>(player))
		return;

	std::string collidedObject = g_Coordinator.GetComponent<CollisionComponent>(player).GetLastCollidedObjectName();

	// Tennis Ball Dialogue Trigger
	if (g_TimerTR.touched && collidedObject == "TennisBall")
	{
		if (!m_TouchedBall)  // Only trigger if not already interacting
		{
			OnInitialize();
			setDialogue(m_FirstTimeTouchBall ? DialogueState::TOUCHBALL : DialogueState::DONTWASTETIME);
			m_TouchedBall = true;  // Track interaction with the ball
			m_FirstTimeTouchBall = false;
		}
	}

	// Bone Dialogue Trigger
	if (g_TimerTR.touched && collidedObject == "Bone")
	{
		if (!m_TouchedBone)  // Only trigger if not already interacting
		{
			OnInitialize();
			setDialogue(m_FirstTimeTouchBone ? DialogueState::TOUCHBONE : DialogueState::DONTWASTETIME);
			m_TouchedBone = true;  // Track interaction with the bone
			m_FirstTimeTouchBone = false;
		}
	}

	// Ensure reset works based on real touch
	if (!g_TimerTR.touched)  
	{
		m_TouchedBall = false;
		m_TouchedBone = false;
	}
}


void Dialogue::Reset()
{
	setDialogue(DialogueState::DEFAULT);
	m_DialogueActive = m_TouchedBall = m_TouchedBone = false;
	m_FirstTimeTouchBall = m_FirstTimeTouchBone = true;
	m_CollisionResetTimer = 2.0f;
}