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
std::unordered_map<std::string, bool*> DeveloperConsole::s_BoolCommands;

void DeveloperConsole::Init()
{
	Instance = std::make_shared<DeveloperConsole>();
}

void DeveloperConsole::AddBoolCommand(const std::string& command, bool* outResult)
{
	s_BoolCommands[StringEncodingUtil::ToLower(command)] = outResult;
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
		RestoreFromHistory(true);
	if (Input::GetSpecialKeyDown(Input::SpecialKey::DOWN))
		RestoreFromHistory(false);
}

void DeveloperConsole::CreateUI()
{
	m_Root = UIElement::Create(nullptr, Vector2(0, 0), Vector2(0, 0));
	m_Background = UIImage::Create(m_Root, Vector2(0, 0), Vector2(0, 0), Texture2D::White());
	m_TextField = UITextField::Create(m_Root, Vector2(0, 0), Vector2(0, 0), 20, Texture2D::White());

	const Vector4 backgroundColor = Vector4(0, 0, 0, 0.7f);
	m_Background->Color = backgroundColor;
	m_TextField->SetBackgroundColor(backgroundColor);
	m_TextField->SetTextColor(Vector4(1, 1, 1, 1));

	m_TextField->KeepFocusOnDone = true;
	m_TextField->OnFinish = [&](const std::wstring& command) { HandleCommand(command); };
}

void DeveloperConsole::UpdateUI()
{
	m_Root->Active = m_Enabled;
	if (!m_Enabled)
		return;

	const std::shared_ptr<UIElement> rootParent = m_Root->GetParent();
	m_Root->Size = Vector2(rootParent->Size.x, rootParent->Size.y * 0.5f);
	m_Root->Position = Vector2(0, rootParent->Size.y - m_Root->Size.y);

	m_Background->Size = m_Root->Size;
	m_TextField->Size = Vector2(m_Root->Size.x, 25);

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
	const std::string cmd = StringEncodingUtil::ToLower(StringEncodingUtil::WStringToString(split[0]));

	const auto& it = s_BoolCommands.find(cmd);
	if (it != s_BoolCommands.end())
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
	else
		AddUITextHistory(L"Unknown command");

	AddCommandHistory(command);
	m_TextField->SetText(L"");
}

void DeveloperConsole::AddUITextHistory(const std::wstring& text)
{
	const std::shared_ptr<UIText> historyText = UIText::Create(m_Root, Vector2(0, 0), Vector2(0, 20), text, 15);
	m_HistoryTexts.insert(m_HistoryTexts.begin(), historyText);
}

void DeveloperConsole::AddCommandHistory(const std::wstring& text)
{
	m_CommandHistory.push_back(text);
	m_CurrentCommandHistoryIndex = -1;
}

void DeveloperConsole::RestoreFromHistory(bool up)
{
	if (m_CommandHistory.empty())
		return;

	m_CurrentCommandHistoryIndex += up ? 1 : -1;
	m_CurrentCommandHistoryIndex = std::clamp(m_CurrentCommandHistoryIndex, -1, static_cast<int>(m_CommandHistory.size()) - 1);

	if (m_CurrentCommandHistoryIndex >= 0)
	{
		const std::wstring& text = m_CommandHistory[m_CommandHistory.size() - m_CurrentCommandHistoryIndex - 1];
		m_TextField->SetText(text);
		m_TextField->SetCursorPosition(text.size());
	}
	else
		m_TextField->SetText(L"");
}
