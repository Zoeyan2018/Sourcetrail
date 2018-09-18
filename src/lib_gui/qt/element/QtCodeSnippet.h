#ifndef QT_CODE_SNIPPET_H
#define QT_CODE_SNIPPET_H

#include <vector>

#include <QFrame>

#include "types.h"

#include "QtCodeArea.h"

#include "CodeSnippetParams.h"

class QBoxLayout;
class QPushButton;
class QtCodeFile;
class QtCodeNavigator;
class SourceLocationFile;

class QtCodeSnippet
	: public QFrame
{
	Q_OBJECT

public:
	static QtCodeSnippet* merged(
		QtCodeSnippet* a, QtCodeSnippet* b, QtCodeNavigator* navigator, QtCodeFile* file);

	QtCodeSnippet(const CodeSnippetParams& params, QtCodeNavigator* navigator, QtCodeFile* file);
	virtual ~QtCodeSnippet();

	QtCodeFile* getFile() const;
	QtCodeArea* getArea() const;

	uint getStartLineNumber() const;
	uint getEndLineNumber() const;

	int lineNumberDigits() const;

	void updateCodeSnippet(const CodeSnippetParams& params);

	void updateLineNumberAreaWidthForDigits(int digits);
	void updateContent();

	void setIsActiveFile(bool isActiveFile);

	uint getLineNumberForLocationId(Id locationId) const;
	std::pair<uint, uint> getLineNumbersForLocationId(Id locationId) const;

	Id getFirstActiveLocationId(Id tokenId) const;
	QRectF getLineRectForLineNumber(uint lineNumber) const;

	std::string getCode() const;

	void findScreenMatches(const std::wstring& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches);

	std::vector<Id> getLocationIdsForTokenIds(const std::set<Id>& tokenIds) const;

	void ensureLocationIdVisible(Id locationId, bool animated);

private slots:
	void clickedTitle();
	void clickedFooter();

private:
	QPushButton* createScopeLine(QBoxLayout* layout);
	void updateDots();

	QtCodeNavigator* m_navigator;
	QtCodeFile* m_file;

	Id m_titleId;
	std::wstring m_titleString;

	Id m_footerId;
	std::wstring m_footerString;

	std::vector<QPushButton*> m_dots;

	QPushButton* m_title;
	QPushButton* m_footer;
	QtCodeArea* m_codeArea;
};

#endif // QT_CODE_SNIPPET_H
