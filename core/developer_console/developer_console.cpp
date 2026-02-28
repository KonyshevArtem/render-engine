#include "developer_console.h"

#include "texture_2d/texture_2d.h"
#include "ui/ui_element.h"
#include "ui/ui_image.h"
#include "ui/ui_text_field.h"
#include "ui/ui_manager.h"
#include "input/input.h"
#include "string_split.h"
#include "string_encoding_util.h"

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

void DeveloperConsole::UpdateUI() const
{
	m_Root->Active = m_Enabled;
	if (!m_Enabled)
		return;

	const std::shared_ptr<UIElement> rootParent = m_Root->GetParent();
	m_Root->Size = Vector2(rootParent->Size.x, rootParent->Size.y * 0.5f);
	m_Root->Position = Vector2(0, rootParent->Size.y - m_Root->Size.y);

	m_Background->Size = m_Root->Size;
	m_TextField->Size = Vector2(m_Root->Size.x, 25);
}

void DeveloperConsole::HandleCommand(const std::wstring& command)
{
	const std::string cmd = StringEncodingUtil::WStringToString(command);
	std::vector<std::string> split = StringSplit::Split(cmd, ' ');

	split[0] = StringEncodingUtil::ToLower(split[0]);

	const auto& it = s_BoolCommands.find(split[0]);
	if (it != s_BoolCommands.end())
		*it->second = split.size() > 1 && strtol(split[1].c_str(), nullptr, 10) > 0;

	m_TextField->SetText(L"");
}
