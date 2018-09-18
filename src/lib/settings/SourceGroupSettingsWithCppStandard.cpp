#include "SourceGroupSettingsWithCppStandard.h"

#include "ProjectSettings.h"

std::wstring SourceGroupSettingsWithCppStandard::getDefaultCppStandardStatic()
{
	return L"c++17";
}

bool SourceGroupSettingsWithCppStandard::equals(std::shared_ptr<SourceGroupSettingsWithCppStandard> other) const
{
	return (
		other &&
		m_cppStandard == other->m_cppStandard
	);
}

void SourceGroupSettingsWithCppStandard::load(std::shared_ptr<const ConfigManager> config, const std::string& key)
{
	setCppStandard(config->getValueOrDefault<std::wstring>(key + "/cpp_standard", L""));
}

void SourceGroupSettingsWithCppStandard::save(std::shared_ptr<ConfigManager> config, const std::string& key)
{
	config->setValue(key + "/cpp_standard", getCppStandard());
}

std::wstring SourceGroupSettingsWithCppStandard::getCppStandard() const
{
	if (m_cppStandard.empty())
	{
		return getDefaultCppStandard();
	}
	return m_cppStandard;
}

void SourceGroupSettingsWithCppStandard::setCppStandard(const std::wstring& standard)
{
	m_cppStandard = standard;
}

std::vector<std::wstring> SourceGroupSettingsWithCppStandard::getAvailableCppStandards() const
{
	return {
		L"c++2a",
		L"gnu++2a",
		L"c++17",
		L"gnu++17",
		L"c++14",
		L"gnu++14",
		L"c++11",
		L"gnu++11",
		L"c++03",
		L"gnu++03",
		L"c++98",
		L"gnu++98"
	};
}

std::wstring SourceGroupSettingsWithCppStandard::getDefaultCppStandard() const
{
	return getDefaultCppStandardStatic();
}
