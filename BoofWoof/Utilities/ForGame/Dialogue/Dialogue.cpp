#include "pch.h"
#include "Dialogue.h"
#include "ResourceManager/ResourceManager.h"
#include "../Core/AssetManager/FilePaths.h"
#include "../Utilities/ForGame/TimerTR/TimerTR.h"

Dialogue g_DialogueText;
Serialization dialogueText;

void Dialogue::OnInitialize()
{
	if (!dialogueActive)
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
		dialogueActive = true;
	}
}

void Dialogue::OnUpdate(double deltaTime)
{
	// Only update if dialogue is active.
	if (!dialogueActive)
		return;

	// Reduce cooldown timer every frame.
	if (clickCooldown > 0)
	{
		clickCooldown -= static_cast<float>(deltaTime);
	}

	// Process left click only when cooldown is expired.
	if (g_Input.GetMouseState(GLFW_MOUSE_BUTTON_LEFT) && clickCooldown <= 0.0f)
	{
		ProcessDialogue();
		clickCooldown = 0.2f;  // Prevent multiple clicks in one frame.
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

	storage.clear();
	dialogueActive = false;
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

	case DialogueState::FIRSTROPEBITE:
		return "A rope? I can bite through it.. But if danger's near, I should stop.";

	case DialogueState::INSTRUCTIONS:
		return "I'll use my teeth-Right Mouse Button to bite. And if I need to stop, I'll press ESC!";

	case DialogueState::TUTORIALEND:
		return "I can smell something.. It's coming from-there! Could this be a way out?";
	
	/* TimeRush Dialogues */
	case DialogueState::ENTEREDLIBRARY:
		return "The library?! No, no, no.. If Rex finds me here, I'm done for.. I have to find a way out-FAST!";

	case DialogueState::TOUCHBALL:
		return "Oh! My tennis ball! Master and I used to play with this all the time..";

	case DialogueState::TOUCHBONE:
		return "My favorite toy!!";

	case DialogueState::DONTWASTETIME:
		return "No, I can't waste time-I have to get out before Rex finds me!";

	case DialogueState::REXISHERE:
		return "I messed up.. Rex knows i'm not in the room anymore. If he catches me now, I'm done for!";

	/* Corridor Dialogues*/
	case DialogueState::OUTOFLIBRARY:
		return "Phew.. We're out! But Rex will catch on.. Gotta stay sharp-one slip, and I'm his chew toy.";

	case DialogueState::SEARCHINGFORPUPS:
		return "This is.. the way to the Main Hall! My puppies must be there!";

	/* MainHall Lvl1 Dialogues */
	case DialogueState::INMAINHALL:
		return "This whole place carries their scent.. My three babies!!";

	case DialogueState::REXSAWYOU:
		return "Oh no, he saw me!! Gotta run-NOW!";

	case DialogueState::FOUNDPUP1:
		return "I found you! But this stupid lock.. Time to bite it to pieces!! Grr!!";

	case DialogueState::TWOMORETOGO:
		return "One down.. Two more to go. Stay close, kiddo! Maybe I could try looking on the second floor..";

	case DialogueState::DISGUSTED:
		return "Something stinks.. Smells like pee..?! I should avoid it..";

	case DialogueState::DISGUSTED2:
		return "Eww!! Seriously, Rex?! Get this off me!!";

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
		return "I'm here my little one! Alright, let’s do this the hard way-grr!!";

	case DialogueState::ALLPUPSFOUND:
		return "That's all three.. We’re finally together again! Time to escape this hellhole!";

	case DialogueState::STAYCLOSE:
		return "Stay close, little ones. We're almost there.. Just gotta avoid that brute.";

	case DialogueState::REXSAWYOU2:
		return "Oh no—RUN! Don't let him catch us!";

	/* RopeBreaker & Escape Dialogues */
	case DialogueState::BREAKROPES:
		return "We made it.. But we're not safe yet. I need to bite off these ropes!";

	case DialogueState::BROKEROPE1:
		return "One down! One more to go!!";

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

void Dialogue::ProcessDialogue()
{
	OnShutdown();

	DialogueState nextState = m_CurrentState;  // Default: No transition

	switch (m_CurrentState) 
	{
	case DialogueState::OUTOFLIBRARY:
		nextState = DialogueState::SEARCHINGFORPUPS;
		break;

	case DialogueState::FIRSTROPEBITE:
		nextState = DialogueState::INSTRUCTIONS;
		break;

	default:
		return;
	}

	if (nextState != m_CurrentState)
	{
		m_CurrentState = nextState;
		OnInitialize();  // Only call OnInitialize when transitioning
		setDialogue(m_CurrentState);  // Update dialogue text
	}
}

std::string Dialogue::textWrap(const std::string& input)
{
	size_t nws = input.find_first_not_of(" \t\n\r");
	std::string format = input.substr(nws);

	// Find the last whitespace character at or before the 42nd char
	for (size_t currentPos{ 48 }; currentPos <= format.length(); currentPos += 48)
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
			setDialogue(DialogueState::DONTWASTETIME);
			m_TouchedBall = true;  // Track interaction with the ball
		}
	}

	// Bone Dialogue Trigger
	if (g_TimerTR.touched && collidedObject == "Bone")
	{
		if (!m_TouchedBone)  // Only trigger if not already interacting
		{
			OnInitialize();
			setDialogue(DialogueState::DONTWASTETIME);
			m_TouchedBone = true;  // Track interaction with the bone
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
	dialogueActive = m_TouchedBall = m_TouchedBone = false;
	m_CollisionResetTimer = 2.0f;
}