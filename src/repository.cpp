#include <filesystem>
#include <stdexcept>
#include "repository.hpp"

GitRepo::GitRepo(const fs::path& path, bool force)
    : worktree_{ path }, gitdir_{ worktree_ / ".git" }
{
    if (!(force || fs::is_directory(gitdir_)))
    {
        throw std::runtime_error("Not a Git repo! : " + path.string());
    }
    //read .git/config (INI file)
}


fs::path GitRepo::path(const fs::path& subpath) const
{
    return gitdir_ / subpath;
}

fs::path GitRepo::file(const fs::path& subpath, bool mkdirs) const
{
    //Path under .git/ for a file; create its parent dirs if mkdir.
    if (!dir(subpath.parent_path(), mkdirs).empty())
    {
        return path(subpath);
    }
    return {};
}

fs::path GitRepo::dir(const fs::path& subpath, bool mkdirs) const
{
    //Path under .git/ gor a directory; create it if mkdir
    fs::path p = GitRepo::path(subpath);
    if (fs::exists(p))
    {
        if (fs::is_directory(p))
        {
            return p;
        }
        throw std::runtime_error("Not a directory!: " + p.string());
    }
    if (mkdirs)
    {
        fs::create_directories(p);
        return p;
    }
    return {};
}



std::optional<GitRepo> GitRepo::find(const fs::path& path, bool required)
{
    fs::path p = fs::canonical(path);
    while (true)
    {
        if (fs::is_directory(p / ".git"))
            return {p};
        if (p == p.parent_path())
        {
            if (required)
            {
                throw std::runtime_error("No .git directory");
            }
            return std::nullopt;
        }
        p = p.parent_path();

    }
}