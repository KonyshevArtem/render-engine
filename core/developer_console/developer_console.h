#ifndef RENDER_ENGINE_DEVELOPER_CONSOLE_H
#define RENDER_ENGINE_DEVELOPER_CONSOLE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

class UIElement;
class UIImage;
class UITextField;
class UIText;

class DeveloperConsole
{
public:
	static std::shared_ptr<DeveloperConsole> Instance;

	static void Init();

	DeveloperConsole() = default;
	~DeveloperConsole() = default;

	static void AddBoolCommand(const std::wstring& command, bool* outResult);

	void Update();

private:
	static std::unordered_map<std::wstring, bool*> s_BoolCommands;

	std::shared_ptr<UIElement> m_Root;
	std::shared_ptr<UIImage> m_Background;
	std::shared_ptr<UITextField> m_TextField;
	std::shared_ptr<UIText> m_PromptText;

	std::vector<std::shared_ptr<UIText>> m_HistoryTexts;
	std::vector<std::wstring> m_CommandHistory;
	std::vector<std::wstring> m_Prompts;
	int m_CurrentAutoFillIndex = 0;
	bool m_FreezePrompts = false;

	bool m_Enabled = false;
	bool m_WasEnabledLastFrame = false;

	void CreateUI();
	void UpdateUI();

	void HandleCommand(const std::wstring& command);
	void UpdatePrompt(const std::wstring& text);
	void AddUITextHistory(const std::wstring& text);
	void AddCommandHistory(const std::wstring& text);
	void AutoFillCommand(bool up);
};

#endif // RENDER_ENGINE_DEVELOPER_CONSOLE_H