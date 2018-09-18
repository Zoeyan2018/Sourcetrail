#include "CxxFrameworkPathDetector.h"

#include "FilePath.h"
#include "utilityCxxHeaderDetection.h"
#include "utilityString.h"

CxxFrameworkPathDetector::CxxFrameworkPathDetector(const std::string& compilerName)
	: PathDetector(compilerName)
	, m_compilerName(compilerName)
{
}

std::vector<FilePath> CxxFrameworkPathDetector::getPaths() const
{
	std::vector<std::string> paths = utility::getCxxHeaderPaths(m_compilerName);
	std::vector<FilePath> frameworkPaths;
	for (const std::string& path : paths)
	{
		if (utility::isPostfix<std::string>(" (framework directory)", path))
		{
			frameworkPaths.push_back(FilePath(utility::replace(path, " (framework directory)", "")).makeCanonical());
		}
	}
	return frameworkPaths;
}
