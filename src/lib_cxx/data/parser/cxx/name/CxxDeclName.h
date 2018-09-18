#ifndef CXX_DECL_NAME_H
#define CXX_DECL_NAME_H

#include <memory>
#include <string>
#include <vector>

#include "NameHierarchy.h"
#include "CxxName.h"

class CxxDeclName: public CxxName
{
public:
	// uncomment this constructor if required, but try to use the one using move constructors for the members
	//CxxDeclName(
	//	const std::wstring& name,
	//	const std::vector<std::wstring>& templateParameterNames
	//);

	CxxDeclName(
		std::wstring&& name,
		std::vector<std::wstring>&& templateParameterNames
	);

	// uncomment this constructor if required, but try to use the one using move constructors for the members
	//CxxDeclName(
	//	const std::wstring& name,
	//	const std::vector<std::wstring>& templateParameterNames,
	//	std::shared_ptr<CxxName> parent
	//);

	CxxDeclName(
		std::wstring&& name,
		std::vector<std::wstring>&& templateParameterNames,
		std::shared_ptr<CxxName> parent
	);

	virtual NameHierarchy toNameHierarchy() const;

	std::wstring getName() const;
	std::vector<std::wstring> getTemplateParameterNames() const;

private:
	std::wstring m_name;
	std::vector<std::wstring> m_templateParameterNames;
};

#endif // CXX_DECL_NAME_H
