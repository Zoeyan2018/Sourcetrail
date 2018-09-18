#ifndef CODE_CONTROLLER_H
#define CODE_CONTROLLER_H

#include <map>
#include <string>

#include "FilePath.h"
#include "MessageListener.h"
#include "MessageActivateLegend.h"
#include "MessageCodeShowDefinition.h"
#include "MessageActivateErrors.h"
#include "MessageErrorCountClear.h"
#include "MessageShowError.h"
#include "MessageActivateAll.h"
#include "MessageActivateFullTextSearch.h"
#include "MessageActivateLocalSymbols.h"
#include "MessageActivateTokens.h"
#include "MessageActivateTrailEdge.h"
#include "MessageChangeFileView.h"
#include "MessageDeactivateEdge.h"
#include "MessageFlushUpdates.h"
#include "MessageFocusIn.h"
#include "MessageFocusOut.h"
#include "MessageScrollCode.h"
#include "MessageScrollToLine.h"
#include "MessageShowScope.h"
#include "types.h"

#include "SnippetMerger.h"
#include "Controller.h"
#include "CodeView.h"

class StorageAccess;
class SourceLocation;
class SourceLocationCollection;
class SourceLocationFile;

class CodeController
	: public Controller
	, public MessageListener<MessageActivateAll>
	, public MessageListener<MessageActivateErrors>
	, public MessageListener<MessageActivateFullTextSearch>
	, public MessageListener<MessageActivateLegend>
	, public MessageListener<MessageActivateLocalSymbols>
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageActivateTrailEdge>
	, public MessageListener<MessageChangeFileView>
	, public MessageListener<MessageCodeShowDefinition>
	, public MessageListener<MessageDeactivateEdge>
	, public MessageListener<MessageErrorCountClear>
	, public MessageListener<MessageFlushUpdates>
	, public MessageListener<MessageFocusIn>
	, public MessageListener<MessageFocusOut>
	, public MessageListener<MessageScrollCode>
	, public MessageListener<MessageScrollToLine>
	, public MessageListener<MessageShowError>
	, public MessageListener<MessageShowScope>
{
public:
	CodeController(StorageAccess* storageAccess);
	~CodeController();

private:
	static const uint s_lineRadius;

	virtual void handleMessage(MessageActivateAll* message);
	virtual void handleMessage(MessageActivateErrors* message);
	virtual void handleMessage(MessageActivateFullTextSearch* message);
	virtual void handleMessage(MessageActivateLegend* message);
	virtual void handleMessage(MessageActivateLocalSymbols* message);
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageActivateTrailEdge* message);
	virtual void handleMessage(MessageChangeFileView* message);
	virtual void handleMessage(MessageCodeShowDefinition* message);
	virtual void handleMessage(MessageDeactivateEdge* message);
	virtual void handleMessage(MessageErrorCountClear* message);
	virtual void handleMessage(MessageFlushUpdates* message);
	virtual void handleMessage(MessageFocusIn* message);
	virtual void handleMessage(MessageFocusOut* message);
	virtual void handleMessage(MessageScrollCode* message);
	virtual void handleMessage(MessageScrollToLine* message);
	virtual void handleMessage(MessageShowError* message);
	virtual void handleMessage(MessageShowScope* message);

	CodeView* getView() const;

	virtual void clear();

	std::vector<CodeSnippetParams> getSnippetsForFileWithState(const FilePath& filePath, CodeView::FileState state) const;
	std::vector<CodeSnippetParams> getSnippetsForActiveSourceLocations(
		const SourceLocationCollection* collection, Id declarationId) const;
	std::vector<CodeSnippetParams> getSnippetsForCollection(std::shared_ptr<SourceLocationCollection> collection) const;
	std::vector<CodeSnippetParams> getSnippetsForFile(std::shared_ptr<SourceLocationFile> file) const;

	std::shared_ptr<SnippetMerger> buildMergerHierarchy(
		const SourceLocation* location, const SourceLocationFile* scopeLocations, SnippetMerger& fileScopedMerger,
		std::map<int, std::shared_ptr<SnippetMerger>>& mergers) const;
	const SourceLocation* getSourceLocationOfParentScope(
		size_t lineNumber, const SourceLocationFile* scopeLocations) const;

	std::vector<std::string> getProjectDescription(SourceLocationFile* locationFile) const;

	void expandVisibleSnippets(std::vector<CodeSnippetParams>* snippets, bool useSingleFileCache) const;
	void addAllSourceLocations(std::vector<CodeSnippetParams>* snippets) const;
	void addModificationTimes(std::vector<CodeSnippetParams>* snippets) const;

	void saveOrRestoreViewMode(MessageBase* message);

	void showCodeSnippets(
		std::vector<CodeSnippetParams> snippets, const CodeView::CodeParams params, bool addSourceLocations = true);

	StorageAccess* m_storageAccess;
	mutable std::shared_ptr<SourceLocationCollection> m_collection;

	std::map<Id, bool> m_messageIdToViewModeMap;
};

#endif // CODE_CONTROLLER_H
