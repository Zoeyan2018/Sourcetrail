#ifndef CXX_VS_10_TO_14_HEADER_PATH_DETECTOR_H
#define CXX_VS_10_TO_14_HEADER_PATH_DETECTOR_H

#include "PathDetector.h"
#include "ApplicationArchitectureType.h"

class CxxVs10To14HeaderPathDetector : public PathDetector
{
public:
	enum VisualStudioType
	{
		VISUAL_STUDIO_2010,
		VISUAL_STUDIO_2012,
		VISUAL_STUDIO_2013,
		VISUAL_STUDIO_2015
	};

	CxxVs10To14HeaderPathDetector(VisualStudioType type, bool isExpress, ApplicationArchitectureType architecture);
	std::vector<FilePath> getPaths() const override;

private:
	static int visualStudioTypeToVersion(const VisualStudioType t);
	static std::string visualStudioTypeToString(const VisualStudioType t);

	FilePath getVsInstallPathUsingRegistry() const;

	const int m_version;
	const bool m_isExpress;
	const ApplicationArchitectureType m_architecture;
};

#endif // CXX_VS_10_TO_14_HEADER_PATH_DETECTOR_H
