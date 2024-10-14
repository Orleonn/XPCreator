#include "XPCreator.hpp"
#include "XPPropsContent.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>



#define XP_CONFIG_CONTENT L"\
[%s]\n\
name			= st_exp_%s_name\n\
description		= st_exp_%s_desc\n\
icon			= %s\n\
req_level		= %d\n\
max_level		= %d\n\
each_level		= %d\n\
req_prestige	= %d\n\
"

#define XP_TEXTURE_DESCR_CONTENT L"\
<w>\n\
\t<file name=\"%s\">\n\
\t\t<texture id=\"ui_icon_experience_skill_%s\" x=\"0\" y=\"0\" width=\"%u\" height=\"%u\" />\n\
\t</file>\n\
</w>\n\
"

#define XP_LANG_CONTENT L"\
<?xml version=\"1.0\" encoding=\"windows-1251\"?>\n\n\
<!-- %s -->\n\n\n\
<string_table>\n\
%s\
</string_table>\n\
"

#define XP_LANG_ELEMENT L"\
\t<string id=\"%s\">\n\
\t\t<text>%s\n\
\t\t</text>\n\
\t</string>\n\
"

#define XP_SCRIPT_HEADER L"\
-- %s\n\n\
if not (experience_utils and experience_utils.CanStartFramework()) then\n\
\treturn\n\
end\n\n\
"

#define XP_SCRIPT_ON_GAME_START L"\
function on_game_start()\n\
%s\
end\n\
"

#define XP_SCRIPT_REGISTER_CALLBACK_ELEMENT L"\
\tRegisterScriptCallback(\"%s\", %s)\n\
"

#define XP_SCRIPT_SKILL_UPDATE_ELEMENT L"\
local function experience_skill_on_update(skill_id, skill_data)\n\
\tif skill_id ~= \"%s\" then return end\n\n\
%s\
end\n\n\
"


static bool build_icon_content(const XPSkillInfo& info, const std::wstring& root_dir);
static bool build_config_content(const XPSkillInfo& info, const std::wstring& root_dir, const bool& has_icon);
static bool build_translation_content(const XPSkillInfo& info, const std::wstring& root_dir);
static bool build_script_content(const XPSkillInfo& info, const std::wstring& root_dir);


bool XPCreate(const XPSkillInfo& info)
{
	const std::wstring root_dir = info.create_path + L"\\" + info.id;

	const bool has_icon = build_icon_content(info, root_dir);

	if (!build_config_content(info, root_dir, has_icon))
		return false;

	if (!build_translation_content(info, root_dir))
		return false;

	if (!build_script_content(info, root_dir))
		return false;

	return true;
}



static bool build_icon_content(const XPSkillInfo& info, const std::wstring& root_dir)
{
	if (info.icon_path.empty())
		return false;

	std::ifstream dds_file(info.icon_path, std::ios::binary);

	if (!dds_file.is_open())
	{
		XPShowErrorBox(XPSprintf(L"Can't find: %s", info.icon_path.c_str()));
		return false;
	}

	std::size_t fileSize = 0;

	dds_file.seekg(0, std::ios::end);
	fileSize = dds_file.tellg();
	dds_file.seekg(0, std::ios::beg);

	if (fileSize <= 128)
	{
		dds_file.close();
		return false;
	}

	std::string fileData(fileSize, '\0');
	dds_file.read(fileData.data(), fileSize);
	dds_file.close();

	const uint32_t headerSize = *reinterpret_cast<const uint32_t*>(fileData.c_str() + 4);
	const uint32_t iconHeight = *reinterpret_cast<const uint32_t*>(fileData.c_str() + 12);
	const uint32_t iconWidth = *reinterpret_cast<const uint32_t*>(fileData.c_str() + 16);

	if (memcmp(fileData.c_str(), "DDS ", 4) != 0 || headerSize != 124)
		return false;

	const std::wstring texture_dir = root_dir + L"\\gamedata\\textures\\ui\\experience_framework\\" + info.id;
	const std::wstring textures_descr_dir = root_dir + L"\\gamedata\\configs\\ui\\textures_descr";
	const std::wstring texture_path = texture_dir + L"\\icon.dds";

	std::filesystem::create_directories(texture_dir);
	std::filesystem::create_directories(textures_descr_dir);

	{
		std::ofstream out_dds_file(texture_path, std::ios::binary | std::ios::trunc);
		if (!out_dds_file.is_open())
			return false;
		out_dds_file.write(fileData.c_str(), fileSize);
		out_dds_file.close();
	}
	{
		const std::wstring textures_descr_path = textures_descr_dir + L"\\ui_experience_framework_" + info.id + L".xml";
		std::wofstream textures_descr_file(textures_descr_path, std::ios::binary | std::ios::trunc);
		if (!textures_descr_file.is_open())
			return false;

		std::wstring temp_data = XPSprintf(L"<!-- %s -->\n\n", XPCREATOR_GENERATED_TEXT);
		textures_descr_file.write(temp_data.c_str(), temp_data.length());

		const std::wstring texture_game_path = L"ui\\experience_framework\\" + info.id + L"\\icon";
		temp_data = XPSprintf(XP_TEXTURE_DESCR_CONTENT, texture_game_path.c_str(), info.id.c_str(), iconWidth, iconHeight);
		textures_descr_file.write(temp_data.c_str(), temp_data.length());
		textures_descr_file.close();
	}
	return true;
}

static bool build_config_content(const XPSkillInfo& info, const std::wstring& root_dir, const bool& has_icon)
{
	std::wstring icon_id;
	if (has_icon)
		icon_id = L"ui_icon_experience_skill_" + info.id;

	const std::wstring config_dir = root_dir + L"\\gamedata\\configs\\experience_framework\\skills";
	std::filesystem::create_directories(config_dir);

	std::wofstream cfg_file(config_dir + L"\\skills_" + info.id + L".ltx", std::ios::binary | std::ios::trunc);
	if (!cfg_file.is_open())
		return false;

	std::wstring temp_data = XPSprintf(L"; %s\n\n", XPCREATOR_GENERATED_TEXT);
	cfg_file.write(temp_data.c_str(), temp_data.length());

	temp_data = XPSprintf(XP_CONFIG_CONTENT,
		info.id.c_str(),		// id
		info.id.c_str(),		// name
		info.id.c_str(),		// description
		icon_id.c_str(),		// icon
		info.req_level,			// req_level
		info.max_level,			// max_level
		info.each_level,		// each_level
		info.req_prestige		// req_prestige
	);

	cfg_file.write(temp_data.c_str(), temp_data.length());
	cfg_file.close();
	return true;
}


static std::wstring get_lang_element(const std::wstring& text_id, const std::wstring& text)
{
	std::wstring buf((text_id.length() + text.length() + (sizeof(XP_LANG_ELEMENT) / sizeof(wchar_t))), L'\0');
	std::swprintf(buf.data(), buf.length(), XP_LANG_ELEMENT, text_id.c_str(), text.c_str());
	return { buf.c_str() };
}

static std::wstring get_lang_content(const std::wstring& body_text)
{
	std::wstring buf((body_text.length() +
		(sizeof(XPCREATOR_GENERATED_TEXT) / sizeof(wchar_t)) +
		(sizeof(XP_LANG_CONTENT) / sizeof(wchar_t))),
		L'\0');
	std::swprintf(buf.data(), buf.length(), XP_LANG_CONTENT, XPCREATOR_GENERATED_TEXT, body_text.c_str());
	return { buf.c_str() };
}

static bool build_translation_content(const XPSkillInfo& info, const std::wstring& root_dir)
{
	std::wstringstream tmp_body, temp_desc_text;

	const std::wstring desc_id_prefix = L"st_exp_" + info.id + L"_desc_";
	const std::wstring desc_text_prefix = info.descr + L" \\n \\n\n\t\t%c[ui_gray_3]ABILITY:";
	std::wstring tmp_desc_id;

	// Name
	tmp_body << get_lang_element(L"st_exp_" + info.id + L"_name", info.name);

	// Descriptions
	for (int i = 1; i <= info.max_level; ++i)
	{
		tmp_desc_id = desc_id_prefix + std::to_wstring(i);

		temp_desc_text.str(L"");
		temp_desc_text << desc_text_prefix;
		for (const auto& prop : info.props)
		{
			temp_desc_text << L" \\n\n\t\t" << prop->GetInGameLevelDescription(i);
		}

		tmp_body << get_lang_element(tmp_desc_id, temp_desc_text.str());
	}

	const std::wstring file_content = get_lang_content(tmp_body.str());

	const std::wstring lang_dir_eng = root_dir + L"\\gamedata\\configs\\text\\eng";
	const std::wstring lang_dir_rus = root_dir + L"\\gamedata\\configs\\text\\rus";
	const std::wstring lang_file_name = L"\\ui_st_experience_" + info.id + L".xml";

	std::filesystem::create_directories(lang_dir_eng);
	std::filesystem::create_directories(lang_dir_rus);

	std::wofstream file_eng(lang_dir_eng + lang_file_name, std::ios::binary | std::ios::trunc);
	if (!file_eng.is_open())
		return false;

	std::wofstream file_rus(lang_dir_rus + lang_file_name, std::ios::binary | std::ios::trunc);
	if (!file_rus.is_open())
	{
		file_eng.close();
		return false;
	}

	const std::locale loc(".1251");

	file_eng.imbue(loc);
	file_rus.imbue(loc);

	file_eng.write(file_content.c_str(), file_content.length());
	file_eng.close();
	file_rus.write(file_content.c_str(), file_content.length());
	file_rus.close();
	return true;
}

static std::wstring get_script_header()
{
	std::wstring buf(
		(sizeof(XP_SCRIPT_HEADER) / sizeof(wchar_t)) +
		(sizeof(XPCREATOR_GENERATED_TEXT) / sizeof(wchar_t)),
		L'\0');
	std::swprintf(buf.data(), buf.length(), XP_SCRIPT_HEADER, XPCREATOR_GENERATED_TEXT);
	return { buf.c_str() };
}

static std::wstring get_script_update_element(const std::wstring& id_text, const std::wstring& body_text)
{
	std::wstring buf(id_text.length() + body_text.length() +
		(sizeof(XP_SCRIPT_SKILL_UPDATE_ELEMENT) / sizeof(wchar_t)),
		L'\0');
	std::swprintf(buf.data(), buf.length(), XP_SCRIPT_SKILL_UPDATE_ELEMENT, id_text.c_str(), body_text.c_str());
	return { buf.c_str() };
}

static std::wstring get_script_on_game_start_element(const std::wstring& body_text)
{
	std::wstring buf(body_text.length() +
		(sizeof(XP_SCRIPT_ON_GAME_START) / sizeof(wchar_t)),
		L'\0');
	std::swprintf(buf.data(), buf.length(), XP_SCRIPT_ON_GAME_START, body_text.c_str());
	return { buf.c_str() };
}

static bool build_script_content(const XPSkillInfo& info, const std::wstring& root_dir)
{
	std::wstringstream script_content, impl_content, update_content, callbacks_content;

	callbacks_content << XPSprintf(
		XP_SCRIPT_REGISTER_CALLBACK_ELEMENT, L"experience_skill_on_update", L"experience_skill_on_update");

	std::vector<std::pair<std::wstring, std::wstring>> callbacks;
	for (const auto& prop : info.props)
	{
		impl_content << prop->GetScriptImplementation() << L"\n";
		update_content << L"\t" << prop->GetUpdateFunctionName() << L"(skill_data.curr_level)\n";

		callbacks = prop->GetRegisterCallbacks();
		for (const auto& ele : callbacks)
		{
			callbacks_content << XPSprintf(XP_SCRIPT_REGISTER_CALLBACK_ELEMENT, ele.first.c_str(), ele.second.c_str());
		}
	}

	script_content << get_script_header();
	script_content << impl_content.str();
	script_content << get_script_update_element(info.id, update_content.str());
	script_content << get_script_on_game_start_element(callbacks_content.str());

	const std::wstring scripts_dir = root_dir + L"\\gamedata\\scripts";
	const std::wstring scripr_name = L"\\experience_skill_" + info.id + L".script";

	std::filesystem::create_directories(scripts_dir);

	std::wofstream script_file(scripts_dir + scripr_name, std::ios::binary | std::ios::trunc);
	if (!script_file.is_open())
		return false;

	const std::wstring file_content = script_content.str();
	script_file.write(file_content.c_str(), file_content.length());
	script_file.close();
	return true;
}
