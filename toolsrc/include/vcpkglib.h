#pragma once

#include "StatusParagraphs.h"
#include "vcpkg_paths.h"
#include "ImmutableSortedVector.h"

namespace vcpkg
{
    StatusParagraphs database_load_check(const vcpkg_paths& paths);

    void write_update(const vcpkg_paths& paths, const StatusParagraph& p);

    struct StatusParagraph_and_associated_files
    {
        StatusParagraph pgh;
        ImmutableSortedVector<std::string> files;
    };

    std::vector<StatusParagraph_and_associated_files> get_installed_files(const vcpkg_paths& paths, const StatusParagraphs& status_db);


    class CMakeCommandBuilder
    {
    public:
        static CMakeCommandBuilder start(const fs::path& cmake_exe, const fs::path& cmake_script);

        CMakeCommandBuilder add_variable(const std::wstring& varname, const std::string& varvalue);
        CMakeCommandBuilder add_variable(const std::wstring& varname, const std::wstring& varvalue);
        CMakeCommandBuilder add_path(const std::wstring& varname, const fs::path& path);

        std::wstring build();

    private:
        CMakeCommandBuilder(const fs::path& cmake_exe, const fs::path& cmake_script);

        fs::path cmake_exe;
        fs::path cmake_script;
        std::map<std::wstring, std::wstring> pass_variables;
    };

} // namespace vcpkg
