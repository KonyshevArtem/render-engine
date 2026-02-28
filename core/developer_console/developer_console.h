#ifndef RENDER_ENGINE_DEVELOPER_CONSOLE_H
#define RENDER_ENGINE_DEVELOPER_CONSOLE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

class UIElement;
class UIImage;
class UITextField;

class DeveloperConsole
{
public:
	static std::shared_ptr<DeveloperConsole> Instance;

	static void Init();

	DeveloperConsole() = default;
	~DeveloperConsole() = default;

	static void AddBoolCommand(const std::string& command, bool* outResult);

	void Update();

private:
	static std::unordered_map<std::string, bool*> s_BoolCommands;

	std::shared_ptr<UIElement> m_Root;
	std::shared_ptr<UIImage> m_Background;
	std::shared_ptr<UITextField> m_TextField;

	bool m_Enabled = false;
	bool m_WasEnabledLastFrame = false;

	void CreateUI();
	void UpdateUI() const;

	void HandleCommand(const std::wstring& command);
};

#endif // RENDER_ENGINE_DEVELOPER_CONSOLE_H