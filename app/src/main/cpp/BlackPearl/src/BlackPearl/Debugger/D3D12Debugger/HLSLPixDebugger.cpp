#include "pch.h"
#ifdef GE_PLATFORM_WINDOWS
#include <filesystem>
#include <shlobj.h>
#include "BlackPearl/Core.h"
#include "HLSLPixDebugger.h"

namespace BlackPearl {

	std::wstring HLSLPixDebugger::GetLatestWinPixGpuCapturerPath_Cpp17()
	{
        LPWSTR programFilesPath = nullptr;
        SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, NULL, &programFilesPath);

        std::filesystem::path pixInstallationPath = programFilesPath;
        pixInstallationPath /= "Microsoft PIX";

        std::wstring newestVersionFound;

        for (auto const& directory_entry : std::filesystem::directory_iterator(pixInstallationPath))
        {
            if (directory_entry.is_directory())
            {
                if (newestVersionFound.empty() || newestVersionFound < directory_entry.path().filename().c_str())
                {
                    newestVersionFound = directory_entry.path().filename().c_str();
                }
            }
        }

        if (newestVersionFound.empty())
        {
            // TODO: Error, no PIX installation found
            GE_CORE_ERROR("no PIX installation found");
        }

        return pixInstallationPath / newestVersionFound / L"WinPixGpuCapturer.dll";
	}
}

#endif