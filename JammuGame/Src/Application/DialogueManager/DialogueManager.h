#pragma once
#include "../ResourceManager/Resource/DataList/JsonResourceManager.h"

class DialogueManager
{
public:
	static JsonResourceManager& Instance()
	{
		static JsonResourceManager instance;
		return instance;
	}
};
#define DIALOGUE_MGR DialogueManager::Instance()