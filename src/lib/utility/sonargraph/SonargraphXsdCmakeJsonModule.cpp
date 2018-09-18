#include "SonargraphXsdCmakeJsonModule.h"

#include "tinyxml.h"

#include "IndexerCommandCxx.h"
#include "ApplicationSettings.h"
#include "LanguageType.h"
#include "SourceGroupSettings.h"
#include "SourceGroupSettingsCxxSonargraph.h"
#include "SourceGroupSettingsWithCppStandard.h"
#include "SourceGroupSettingsWithCStandard.h"
#include "FileSystem.h"
#include "logging.h"
#include "SonargraphSoftwareSystem.h"
#include "SonargraphXsdCppSystemSettings.h"
#include "utilitySonargraph.h"
#include "utility.h"
#include "utilityString.h"
#include "utilityXml.h"
#include "OrderedCache.h"

namespace Sonargraph
{
	std::string XsdCmakeJsonModule::getXsdTypeName()
	{
		return "xsdCmakeJsonModule";
	}

	std::shared_ptr<XsdCmakeJsonModule> XsdCmakeJsonModule::create(const TiXmlElement* element, std::weak_ptr<SoftwareSystem> parent)
	{
		if (!utility::xmlElementHasAttribute(element, "xsi:type") || utility::sonargraphXmlElementIsType(element, getXsdTypeName()))
		{
			std::shared_ptr<XsdCmakeJsonModule> module = std::shared_ptr<XsdCmakeJsonModule>(new XsdCmakeJsonModule());
			if (module->init(element, parent))
			{
				return module;
			}
		}
		return std::shared_ptr<XsdCmakeJsonModule>();
	}

	LanguageType XsdCmakeJsonModule::getSupportedLanguage() const
	{
		return LANGUAGE_CPP;
	}

	std::set<FilePath> XsdCmakeJsonModule::getAllSourcePaths() const
	{
		FilePath baseDirectory;
		if (std::shared_ptr<const SoftwareSystem> softwareSystem = getSoftwareSystem())
		{
			baseDirectory = softwareSystem->getBaseDirectory();
		}

		std::set<FilePath> sourcePaths;
		for (std::shared_ptr<XsdRootPath> rootPath : getRootPaths())
		{
			sourcePaths.insert(rootPath->getFilePath(baseDirectory));
		}

		for (std::shared_ptr<XsdRootPathWithFiles> rootPath : m_rootPathWithFiles)
		{
			sourcePaths.insert(rootPath->getFilePath(baseDirectory));
		}

		return sourcePaths;
	}

	std::set<FilePath> XsdCmakeJsonModule::getAllSourceFilePathsCanonical() const
	{
		FilePath baseDir;
		if (std::shared_ptr<const SoftwareSystem> softwareSystem = getSoftwareSystem())
		{
			baseDir = softwareSystem->getBaseDirectory();
		}

		const std::set<FilePathFilter> excludeFilters = utility::toSet(getDerivedExcludeFilters());
		const std::set<FilePathFilter> includeFilters = utility::toSet(getDerivedIncludeFilters());

		std::set<FilePath> sourceFilePaths;
		for (std::shared_ptr<XsdRootPath> rootPath : getRootPaths())
		{
			for (const XsdRootPathWithFiles::SourceFile& sourceFile : getIncludedSourceFilesForRootPath(rootPath, baseDir, excludeFilters, includeFilters))
			{
				sourceFilePaths.insert(sourceFile.getFilePath(baseDir.getConcatenated(rootPath->getName())));
			}
		}

		for (std::shared_ptr<XsdRootPath> rootPath : m_rootPathWithFiles)
		{
			for (const XsdRootPathWithFiles::SourceFile& sourceFile : getIncludedSourceFilesForRootPath(rootPath, baseDir, excludeFilters, includeFilters))
			{
				sourceFilePaths.insert(sourceFile.getFilePath(baseDir.getConcatenated(rootPath->getName())));
			}
		}

		return sourceFilePaths;
	}

	std::set<FilePath> XsdCmakeJsonModule::getAllCxxHeaderSearchPathsCanonical() const
	{
		std::set<Id> usedCompilerOptionSetIds;

		for (std::shared_ptr<XsdRootPath> rootPath : getRootPaths())
		{
			if (std::shared_ptr<XsdRootPathWithFiles> rootPathWithFiles = std::dynamic_pointer_cast<XsdRootPathWithFiles>(rootPath))
			{
				for (const XsdRootPathWithFiles::SourceFile& sourceFile : rootPathWithFiles->getSourceFiles())
				{
					usedCompilerOptionSetIds.insert(sourceFile.compilerOptionSetId);
				}
			}
		}

		for (std::shared_ptr<XsdRootPathWithFiles> rootPath : m_rootPathWithFiles)
		{
			for (const XsdRootPathWithFiles::SourceFile& sourceFile : rootPath->getSourceFiles())
			{
				usedCompilerOptionSetIds.insert(sourceFile.compilerOptionSetId);
			}
		}

		std::shared_ptr<const SoftwareSystem> softwareSystem = getSoftwareSystem();
		if (!softwareSystem)
		{
			return std::set<FilePath>();
		}

		std::set<std::wstring> usedCompilerOptions;
		for (Id compilerOptionSetId : usedCompilerOptionSetIds)
		{
			for (std::shared_ptr<const XsdCppSystemSettings> systemExtension :
				softwareSystem->getSpecificSystemExtensions<XsdCppSystemSettings>()
			)
			{
				if (systemExtension->hasCompilerOptionsForId(compilerOptionSetId))
				{
					utility::append(
						usedCompilerOptions,
						utility::toSet(systemExtension->getCompilerOptionsForId(compilerOptionSetId))
					);
					break;
				}
			}
		}

		// make sure that none of these prefixes is the prefix of a prefix that appears further down in the list
		const std::vector<std::wstring> optionPrefixes = {
			L"--include-directory=",
			L"--include-directory",
			L"-cxx-isystem",
			L"-iquote",
			L"-isystem-after",
			L"-isystem",
			L"-I"
		};

		std::set<FilePath> headerSearchPaths;
		for (const std::wstring& compilerOption : usedCompilerOptions)
		{
			for (const std::wstring& optionPrefix : optionPrefixes)
			{
				if (utility::isPrefix(optionPrefix, compilerOption))
				{
					FilePath headerSearchPath(utility::trim(compilerOption.substr(optionPrefix.size())));
					if (headerSearchPath.isAbsolute())
					{
						headerSearchPaths.insert(headerSearchPath);
					}
					else
					{
						headerSearchPaths.insert(softwareSystem->getBaseDirectory().getConcatenated(headerSearchPath));
					}
					break;
				}
			}
		}

		return headerSearchPaths;
	}

	std::vector<std::shared_ptr<IndexerCommand>> XsdCmakeJsonModule::getIndexerCommands(
		std::shared_ptr<const SourceGroupSettings> sourceGroupSettings,
		std::shared_ptr<const ApplicationSettings> appSettings) const
	{
		std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;

		std::set<FilePath> indexedHeaderPaths;
		std::wstring languageStandard = SourceGroupSettingsWithCppStandard::getDefaultCppStandardStatic();

		if (std::shared_ptr<const SourceGroupSettingsCxxSonargraph> sonargraphSettings =
			std::dynamic_pointer_cast<const SourceGroupSettingsCxxSonargraph>(sourceGroupSettings))
		{
			indexedHeaderPaths = utility::toSet(sonargraphSettings->getIndexedHeaderPathsExpandedAndAbsolute());
			languageStandard = sonargraphSettings->getCppStandard();
		}
		else
		{
			LOG_ERROR(L"Source group doesn't specify language standard. Falling back to \"" + languageStandard + L"\".");
			LOG_ERROR(L"Source group doesn't specify any indexed header paths");
		}

		std::vector<std::wstring> compilerFlags;
		compilerFlags.emplace_back(IndexerCommandCxx::getCompilerFlagLanguageStandard(languageStandard));
		utility::append(compilerFlags, IndexerCommandCxx::getCompilerFlagsForSystemHeaderSearchPaths(appSettings ? appSettings->getHeaderSearchPathsExpanded() : std::vector<FilePath>()));
		utility::append(compilerFlags, IndexerCommandCxx::getCompilerFlagsForFrameworkSearchPaths(appSettings ? appSettings->getFrameworkSearchPathsExpanded() : std::vector<FilePath>()));

		for (std::shared_ptr<XsdRootPath> rootPath : getRootPaths())
		{
			if (std::shared_ptr<XsdRootPathWithFiles> rootPathWithFiles = std::dynamic_pointer_cast<XsdRootPathWithFiles>(rootPath))
			{
				utility::append(indexerCommands, getIndexerCommandsForRootPath(
					rootPathWithFiles, indexedHeaderPaths, compilerFlags
				));
			}
		}

		for (std::shared_ptr<XsdRootPathWithFiles> rootPath : m_rootPathWithFiles)
		{
			utility::append(indexerCommands, getIndexerCommandsForRootPath(
				rootPath, indexedHeaderPaths, compilerFlags
			));
		}

		return indexerCommands;
	}

	bool XsdCmakeJsonModule::init(const TiXmlElement* element, std::weak_ptr<SoftwareSystem> parent)
	{
		if (!XsdAbstractModule::init(element, parent))
		{
			return false;
		}

		if (element != nullptr)
		{
			for (const TiXmlElement* rootPathWithFilesElement : utility::getXmlChildElementsWithName(element, "rootPathWithFiles"))
			{
				if (std::shared_ptr<XsdRootPathWithFiles> rootPath = XsdRootPathWithFiles::create(rootPathWithFilesElement))
				{
					m_rootPathWithFiles.push_back(rootPath);
				}
				else
				{
					LOG_ERROR("Unable to parse \"rootPathWithFiles\" element of Sonargraph " + getXsdTypeName() + ".");
					return false;
				}
			}
			return true;
		}
		return false;
	}

	std::vector<XsdRootPathWithFiles::SourceFile> XsdCmakeJsonModule::getIncludedSourceFilesForRootPath(
		std::shared_ptr<XsdRootPath> rootPath,
		const FilePath& baseDir,
		const std::set<FilePathFilter>& excludeFilters,
		const std::set<FilePathFilter>& includeFilters) const
	{
		std::vector<XsdRootPathWithFiles::SourceFile> sourceFiles;
		if (std::shared_ptr<XsdRootPathWithFiles> rootPathWithFiles = std::dynamic_pointer_cast<XsdRootPathWithFiles>(rootPath))
		{
			for (const XsdRootPathWithFiles::SourceFile& sourceFile : rootPathWithFiles->getSourceFiles())
			{
				const FilePath sourceFilePath = sourceFile.getFilePath(baseDir).makeCanonical();

				bool excludeMatches = false;
				for (const FilePathFilter& excludeFilter : excludeFilters)
				{
					if (excludeFilter.isMatching(sourceFilePath))
					{
						excludeMatches = true;
						break;
					}
				}
				if (excludeMatches)
				{
					for (const FilePathFilter& includeFilter : includeFilters)
					{
						if (includeFilter.isMatching(sourceFilePath))
						{
							excludeMatches = false;
							break;
						}
					}
				}
				if (!excludeMatches)
				{
					sourceFiles.push_back(sourceFile);
				}
			}
		}
		return sourceFiles;
	}

	std::vector<std::shared_ptr<IndexerCommand>> XsdCmakeJsonModule::getIndexerCommandsForRootPath(
		std::shared_ptr<XsdRootPathWithFiles> rootPath,
		const std::set<FilePath>& indexedHeaderPaths,
		const std::vector<std::wstring>& compilerFlags) const
	{
		std::vector<std::shared_ptr<IndexerCommand>> indexerCommands;
		if (rootPath)
		{
			std::shared_ptr<const SoftwareSystem> softwareSystem = getSoftwareSystem();
			if (!softwareSystem)
			{
				return std::vector<std::shared_ptr<IndexerCommand>>();
			}

			const FilePath baseDir = rootPath->getFilePath(softwareSystem->getBaseDirectory());

			const std::set<FilePathFilter> excludeFilters = utility::toSet(getDerivedExcludeFilters());
			const std::set<FilePathFilter> includeFilters = utility::toSet(getDerivedIncludeFilters());

			OrderedCache<Id, std::vector<std::wstring>> compilerOptionCache([&](const Id& id) {
				if (std::shared_ptr<const SoftwareSystem> softwareSystem = getSoftwareSystem())
				{
					for (std::shared_ptr<const XsdCppSystemSettings> systemExtension : softwareSystem->getSpecificSystemExtensions<XsdCppSystemSettings>())
					{
						if (systemExtension->hasCompilerOptionsForId(id))
						{
							return utility::concat(systemExtension->getCompilerOptionsForId(id), compilerFlags);
						}
					}
				}
				return compilerFlags;
			});

			for (const XsdRootPathWithFiles::SourceFile& sourceFile : getIncludedSourceFilesForRootPath(
					rootPath, baseDir, excludeFilters, includeFilters)
				)
			{
				const FilePath sourceFilePath = sourceFile.getFilePath(baseDir).makeCanonical();
				indexerCommands.push_back(std::make_shared<IndexerCommandCxx>(
					sourceFilePath,
					utility::concat(indexedHeaderPaths, { sourceFilePath }),
					excludeFilters,
					includeFilters,
					softwareSystem->getBaseDirectory(),
					utility::concat(
						compilerOptionCache.getValue(sourceFile.compilerOptionSetId),
						sourceFilePath.wstr()
					)
				));
			}
		}
		return indexerCommands;
	}
}
