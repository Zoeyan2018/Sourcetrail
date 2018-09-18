#ifndef QT_PROJECT_WIZZARD_CONTENT_SELECT_H
#define QT_PROJECT_WIZZARD_CONTENT_SELECT_H

#include "QtProjectWizzardContent.h"
#include "LanguageType.h"
#include "SourceGroupType.h"

class QButtonGroup;
class SolutionParserManager;

class QtProjectWizzardContentSelect
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentSelect(QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	void populate(QGridLayout* layout, int& row) override;

	void save() override;
	bool check() override;

	bool isScrollAble() const override;

signals:
	void selected(SourceGroupType);

private:
	QButtonGroup* m_languages;
	std::map<LanguageType, QButtonGroup*> m_buttons;

	QLabel* m_title;
	QLabel* m_description;

	std::map<SourceGroupType, std::wstring> m_sourceGroupTypeIconName;
	std::map<SourceGroupType, std::string> m_sourceGroupTypeDescriptions;

	std::vector<std::string> m_solutionDescription;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_SELECT_H
