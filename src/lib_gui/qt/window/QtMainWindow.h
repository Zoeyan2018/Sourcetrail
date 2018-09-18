#ifndef QT_MAIN_WINDOW_H
#define QT_MAIN_WINDOW_H

#include <memory>
#include <utility>
#include <vector>

#include <QMainWindow>

#include "SearchMatch.h"
#include "LicenseChecker.h"
#include "QtWindowsTaskbarButton.h"
#include "QtWindowStack.h"

class Bookmark;
class QDockWidget;
class View;

class QtViewToggle
	: public QWidget
{
	Q_OBJECT

public:
	QtViewToggle(View* view, QWidget *parent = nullptr);

public slots:
	void toggledByAction();
	void toggledByUI();

private:
	View* m_view;
};


class MouseReleaseFilter
	: public QObject
{
	Q_OBJECT

public:
	MouseReleaseFilter(QObject* parent);

protected:
	bool eventFilter(QObject* obj, QEvent* event);

private:
	size_t m_backButton;
	size_t m_forwardButton;
};


class QtMainWindow
	: public QMainWindow
{
	Q_OBJECT

public:
	QtMainWindow();
	~QtMainWindow();

	void addView(View* view);
	void removeView(View* view);

	void showView(View* view);
	void hideView(View* view);

	View* findFloatingView(const std::string& name) const;

	void loadLayout();
	void saveLayout();

	void loadDockWidgetLayout();
	void loadWindow(bool showStartWindow);

	void forceEnterLicense(LicenseChecker::LicenseState state);

	void updateHistoryMenu(const std::vector<std::shared_ptr<MessageBase>>& historyMenuItems);
	void updateBookmarksMenu(const std::vector<std::shared_ptr<Bookmark>>& bookmarks);

	void setContentEnabled(bool enabled);
	void refreshStyle();

	void setWindowsTaskbarProgress(float progress);
	void hideWindowsTaskbarProgress();

signals:
	void showScreenSearch();
	void hideScreenSearch();

protected:
	virtual void showEvent(QShowEvent* event) override;
	virtual void keyPressEvent(QKeyEvent* event) override;
	virtual void contextMenuEvent(QContextMenuEvent* event) override;
	virtual void closeEvent(QCloseEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;

public slots:
	void about();
	void openSettings();
	void showBugtracker();
	void showDocumentation();
	void showKeyboardShortcuts();
	void showErrorHelpMessage();
	void showEula(bool forceAccept = false);
	void acceptedEula();
	void showLicenses();
	void enterLicense();
	void enteredLicense();

	void showDataFolder();
	void showLogFolder();

	void showStartScreen();
	void hideStartScreen();

	void newProject();
	void newProjectFromCDB(const FilePath& filePath);
	void openProject();
	void editProject();

	void find();
	void findFulltext();
	void findOnScreen();
	void codeReferencePrevious();
	void codeReferenceNext();
	void codeLocalReferencePrevious();
	void codeLocalReferenceNext();
	void overview();

	void closeWindow();
	void refresh();
	void forceRefresh();

	void undo();
	void redo();
	void zoomIn();
	void zoomOut();
	void resetZoom();

	void resetWindowLayout();

	void openRecentProject();
	void updateRecentProjectMenu();

	void toggleView(View* view, bool fromMenu);

private slots:
	void toggleShowDockWidgetTitleBars();

	void showBookmarkCreator();
	void showBookmarkBrowser();

	void openHistoryAction();
	void activateBookmarkAction();

private:
	struct DockWidget
	{
		QDockWidget* widget;
		View* view;
		QAction* action;
		QtViewToggle* toggle;
	};

    void setupEditMenu();
	void setupProjectMenu();
	void setupViewMenu();
	void setupHistoryMenu();
	void setupBookmarksMenu();
	void setupHelpMenu();

	DockWidget* getDockWidgetForView(View* view);

	void setShowDockWidgetTitleBars(bool showTitleBars);

	template<typename T>
		T* createWindow();

	std::vector<DockWidget> m_dockWidgets;

	bool m_loaded;

	QMenu* m_viewMenu;
	QAction* m_viewSeparator;

	QMenu* m_historyMenu;
	std::vector<std::shared_ptr<MessageBase>> m_history;

	QMenu* m_bookmarksMenu;
	std::vector<std::shared_ptr<Bookmark>> m_bookmarks;

	QAction** m_recentProjectAction;
	QAction* m_showTitleBarsAction;

	bool m_showDockWidgetTitleBars;

	QtWindowStack m_windowStack;

	QtWindowsTaskbarButton m_windowsTaskbarButton;
};

#endif // QT_MAIN_WINDOW_H
