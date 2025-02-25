#include "Dialogue.h"
#include "ResourceManager/ResourceManager.h"
#include "../ChangeText/ChangeText.h"
#include "../Core/AssetManager/FilePaths.h"

Dialogue g_DialogueText;
Serialization dialogueText;

void Dialogue::OnInitialize()
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

	g_ChangeText.OnShutdown();
}

std::string Dialogue::getDialogue()
{
	switch (m_CurrentState)
	{
	case DialogueState::DEFAULT:
		return "";

	case DialogueState::TUTORIALSTART:
		return "I have to get out of here… One step at a time.";

	case DialogueState::TUTORIALEND:
		return "Wait… I smell something… It’s coming from-there! Could this be a way out?";

	case DialogueState::TOUCHBALL:
		return "Oh! My tennis ball! Master and I used to play with this all the time…";

	case DialogueState::TOUCHBONE:
		return "My favorite snack!!";

	case DialogueState::DONTWASTETIME:
		return "No, I can’t waste time—I have to get out before Rex finds me!";

	case DialogueState::OUTOFLIBRARY:
		return "Phew… We’re out of the library. But it won’t be long before Rex realizes I’m gone… I need to stay sharp. One wrong move, and I’ll be his next chew toy.";

	case DialogueState::REXSAWYOU:
		return "Oh no, he saw me!! Gotta hide—NOW!";

	case DialogueState::SEARCHINGFORPUPS:
		return "Where are my kiddos at…? Hang in there, I’m coming!";

	case DialogueState::FOUNDPUP1:
		return "I found you! But this stupid lock… Time to bite it to pieces!! Grr!!";

	case DialogueState::TWOMORETOGO:
		return "One down… Two more to go. Stay close, kiddo!";

	case DialogueState::DISGUSTED:
		return "Ewww!! Seriously, Rex?! Why pee all over the place?! I gotta clean this off before he sniffs me out!";

	case DialogueState::SEARCHINGFORPUPS2:
		return "If I were a pup, where would I be…? Gotta check everywhere—can’t leave without all of them!";

	case DialogueState::FOUNDPUP2:
		return "There you are! I knew I’d find you!";

	case DialogueState::ONEMORETOGO:
		return "That’s two! Stay close—we’ve got one more to save!";

	case DialogueState::SEARCHINGFORPUPS3:
		return "Hmm… That last pup has to be around here somewhere. Maybe if I climb up there…";

	case DialogueState::FOUNDPUP3:
		return "Locked… again?! Alright, let’s do this the hard way—grr!!";

	case DialogueState::ALLPUPSFOUND:
		return "That’s all three… We’re finally together again! Time to escape this hellhole!";

	case DialogueState::STAYCLOSE:
		return "Stay close, little ones. We’re almost there… Just gotta avoid that brute.";

	case DialogueState::REXSAWYOU2:
		return "Oh no—RUN! Don’t let him catch us!";

	case DialogueState::BREAKROPES:
		return "We made it… But we’re not safe yet. I need to break these ropes!";

	case DialogueState::BROKEROPE1:
		return "One down—one more to go!";

	case DialogueState::FREED:
		return "We did it… We’re free!";
	}

	return std::string();
}

void Dialogue::setDialogue(DialogueState newState)
{
	m_CurrentState = newState;

	if (g_Coordinator.HaveComponent<FontComponent>(m_D1)) {
		g_Coordinator.GetComponent<FontComponent>(m_D1).set_text(getDialogue());
	}
}

//void Dialogue::setDialogue(DialogueState newState, Entity entity)
//{
//	m_CurrentState = newState;
//
//	if (g_Coordinator.HaveComponent<FontComponent>(entity)) {
//		g_Coordinator.GetComponent<FontComponent>(entity).set_text(getDialogue());
//	}
//}

void Dialogue::Reset()
{
	//setDialogue(DialogueState::DEFAULT, m_D1);
	setDialogue(DialogueState::DEFAULT);
}