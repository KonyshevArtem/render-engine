#include "developer_console.h"

#include "texture_2d/texture_2d.h"
#include "ui/ui_element.h"
#include "ui/ui_image.h"
#include "ui/ui_text_field.h"
#include "ui/ui_text.h"
#include "ui/ui_manager.h"
#include "input/input.h"
#include "string_split.h"
#include "string_encoding_util.h"

#include <cwchar>
#include <algorithm>

std::shared_ptr<DeveloperConsole> DeveloperConsole::Instance = nullptr;
std::unordered_map<std::wstring, bool*> DeveloperConsole::s_BoolCommands;
std::unordered_map<std::wstring, std::function<void(const std::string&)>> DeveloperConsole::s_FunctionCommands;

void DeveloperConsole::Init()
{
	Instance = std::make_shared<DeveloperConsole>();
}

void DeveloperConsole::AddBoolCommand(const std::wstring& command, bool* outResult)
{
	s_BoolCommands[StringEncodingUtil::ToLower(command)] = outResult;
}

void DeveloperConsole::AddFunctionCommand(const std::wstring& command, std::function<void(const std::string&)> func)
{
	s_FunctionCommands[StringEncodingUtil::ToLower(command)] = func;
}

void DeveloperConsole::Update()
{
	if (m_Root)
	{
		UpdateUI();

		if (m_Enabled != m_WasEnabledLastFrame)
		{
			if (m_Enabled)
				UIManager::ChangeFocus(m_TextField);
			else
				UIManager::ResetFocus();
		}

		m_WasEnabledLastFrame = m_Enabled;
	}
	else if (m_Enabled)
		CreateUI();

	if (Input::GetSpecialKeyDown(Input::SpecialKey::TILDE))
		m_Enabled = !m_Enabled;
	if (Input::GetSpecialKeyDown(Input::SpecialKey::UP))
		AutoFillCommand(true);
	if (Input::GetSpecialKeyDown(Input::SpecialKey::DOWN))
		AutoFillCommand(false);
}

void DeveloperConsole::CreateUI()
{
	m_Root = UIElement::Create(nullptr, Vector2(0, 0), Vector2(0, 0));
	m_Background = UIImage::Create(m_Root, Vector2(0, 0), Vector2(0, 0), Texture2D::White());
	m_TextField = UITextField::Create(m_Root, Vector2(0, 0), Vector2(0, 0), 20, Texture2D::White());
	m_PromptText = UIText::Create(m_Root, Vector2(0, -20), Vector2(0, 0), L"", 15);

	const Vector4 backgroundColor = Vector4(0, 0, 0, 0.7f);
	m_Background->Color = backgroundColor;
	m_TextField->SetBackgroundColor(backgroundColor);
	m_TextField->SetTextColor(Vector4(1, 1, 1, 1));

	m_TextField->KeepFocusOnDone = true;
	m_TextField->OnFinish = [&](const std::wstring& command) { HandleCommand(command); };
	m_TextField->OnTextChanged = [&](const std::wstring& text) { UpdatePrompt(text); };
}

void DeveloperConsole::UpdateUI()
{
	m_Root->Active = m_Enabled;
	if (!m_Enabled)
		return;

	const std::shared_ptr<UIElement> rootParent = m_Root->GetParent();
	m_Root->Size = Vector2(rootParent->Size.x, rootParent->Size.y * 0.5f);
	m_Root->Position = Vector2(0, rootParent->Size.y - m_Root->Size.y);

	m_Background->Position = Vector2(0, -m_PromptText->GetTextSize().y);
	m_Background->Size = Vector2(m_Root->Size.x, m_Root->Size.y + m_PromptText->GetTextSize().y);
	m_TextField->Size = Vector2(m_Root->Size.x, 25);
	m_PromptText->Size = Vector2(m_Root->Size.x, 20);

	float historyTextPosY = 0;
	for (const std::shared_ptr<UIText>& historyText : m_HistoryTexts)
	{
		historyText->Position = Vector2(0, m_TextField->Size.y + historyTextPosY);
		historyText->Size = Vector2(m_Root->Size.x, historyText->Size.y);
		historyTextPosY += historyText->Size.y;
	}

	if (!m_HistoryTexts.empty())
	{
		for (auto it = m_HistoryTexts.end() - 1; it >= m_HistoryTexts.begin(); --it)
		{
			const std::shared_ptr<UIText> historyText = *it;
			if (historyText->Position.y > m_Root->Size.y)
			{
				it = m_HistoryTexts.erase(it);
				historyText->Destroy();
			}
			else
				break;
		}
	}
}

void DeveloperConsole::HandleCommand(const std::wstring& command)
{
	const std::vector<std::wstring> split = StringSplit::Split(command, ' ');
	const std::wstring cmd = StringEncodingUtil::ToLower(split[0]);

	if (const auto& it = s_BoolCommands.find(cmd); it != s_BoolCommands.end())
	{
		if (split.size() > 1)
		{
			*it->second = std::wcstol(split[1].c_str(), nullptr, 10) > 0;
			AddUITextHistory(command);
		}
		else
		{
			const bool currentState = *it->second;
			AddUITextHistory(split[0] + (currentState ? L" true" : L" false"));
		}
	}
	else if (const auto& it = s_FunctionCommands.find(cmd); it != s_FunctionCommands.end())
	{
		if (split.size() > 1)
			it->second(StringEncodingUtil::WStringToString(split[1]));

		AddUITextHistory(command);
	}
	else
		AddUITextHistory(L"Unknown command");

	AddCommandHistory(command);
	m_TextField->SetText(L"");
}

void DeveloperConsole::UpdatePrompt(const std::wstring& text)
{
	if (!m_FreezePrompts)
	{
		if (text.empty())
		{
			m_Prompts.clear();
			m_PromptText->SetText(L"");
			return;
		}

		const std::vector<std::wstring> split = StringSplit::Split(text, ' ');

		auto CheckCommand = [&](const std::wstring& command)
			{
				bool matching = true;
				for (const std::wstring& pattern : split)
				{
					if (command.find(pattern) == std::wstring::npos)
					{
						matching = false;
						break;
					}
				}

				if (matching)
					m_Prompts.push_back(command);
			};

		m_Prompts.clear();
		for (const auto& it : s_BoolCommands)
			CheckCommand(it.first);
		for (const auto& it : s_FunctionCommands)
			CheckCommand(it.first);

		std::ranges::sort(m_Prompts);
	}

	const int shownPromptsCount = std::min<int>(m_Prompts.size(), 10);
	const int currentPromptIndex = std::max<int>(-m_CurrentAutoFillIndex - 1, 0);

	std::wstring prompt;
	for (int i = 0; i < shownPromptsCount; ++i)
	{
		prompt.append(m_Prompts[std::min<int>(currentPromptIndex + i, m_Prompts.size() - shownPromptsCount + i)]);
		if (i < shownPromptsCount - 1)
			prompt.append(L"\n");
	}

	m_PromptText->SetText(prompt);
}

void DeveloperConsole::AddUITextHistory(const std::wstring& text)
{
	const std::shared_ptr<UIText> historyText = UIText::Create(m_Root, Vector2(0, 0), Vector2(0, 20), text, 15);
	m_HistoryTexts.insert(m_HistoryTexts.begin(), historyText);
}

void DeveloperConsole::AddCommandHistory(const std::wstring& text)
{
	m_CommandHistory.push_back(text);
	m_CurrentAutoFillIndex = 0;
	m_FreezePrompts = false;
}

void DeveloperConsole::AutoFillCommand(bool up)
{
	if (m_CommandHistory.empty() && m_Prompts.empty())
		return;

	int newAutoFillIndex = m_CurrentAutoFillIndex + (up ? 1 : -1);
	newAutoFillIndex = std::clamp(newAutoFillIndex, -static_cast<int>(m_Prompts.size()), static_cast<int>(m_CommandHistory.size()));

	if (m_CurrentAutoFillIndex != newAutoFillIndex)
	{
		m_CurrentAutoFillIndex = newAutoFillIndex;

		if (newAutoFillIndex > 0)
		{
			const std::wstring& text = m_CommandHistory[m_CommandHistory.size() - newAutoFillIndex];
			m_TextField->SetText(text);
			m_TextField->SetCursorPosition(text.size());
		}
		else if (newAutoFillIndex < 0)
		{
			m_FreezePrompts = true;
			const std::wstring& text = m_Prompts[-newAutoFillIndex - 1];
			m_TextField->SetText(text);
			m_TextField->SetCursorPosition(text.size());
		}
		else
		{
			m_FreezePrompts = false;
			m_TextField->SetText(L"");
		}
	}
}
