#ifndef RENDER_ENGINE_DEVELOPER_CONSOLE_H
#define RENDER_ENGINE_DEVELOPER_CONSOLE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

#include "string_encoding_util.h"

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

	template<typename T>
	static void AddCommand(const std::wstring& command, T* outResult)
	{
		Command cmd;
		cmd.Getter = [outResult]
			{
				if constexpr (std::is_same_v<T, bool>)
					return *outResult ? L"true" : L"false";
				else
					return std::to_wstring(*outResult);
			};
		cmd.Setter = [outResult](const std::string& value)
			{
				if constexpr (std::is_same_v<T, bool>)
					*outResult = std::stoi(value) > 0;
				else if constexpr (std::is_integral_v<T>)
					*outResult = std::stoll(value);
				else if constexpr (std::is_floating_point_v<T>)
					*outResult = std::stod(value);
			};

		s_Commands[StringEncodingUtil::ToLower(command)] = cmd;
	}

	static void AddFunctionCommand(const std::wstring& command, std::function<void(const std::string&)> func);

	void Update();

private:
	struct Command
	{
		std::function<const std::wstring()> Getter;
		std::function<void(const std::string&)> Setter;
	};

	static std::unordered_map<std::wstring, Command> s_Commands;

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