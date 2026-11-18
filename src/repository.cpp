#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
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



namespace
{
    // Binary mode so Windows does not turn '\n' into "\r\n".
    void write_text_file(const fs::path& p, const std::string& text)
    {
        std::ofstream out(p, std::ios::binary);
        if (!out)
        {
            throw std::runtime_error("Cannot write file: " + p.string());
        }
        out.write(text.data(), static_cast<std::streamsize>(text.size()));
    }
}

GitRepo GitRepo::create(const fs::path& path)
{
    // force = true: the .git folder does not exist yet.
    GitRepo repo(path, true);

    if (fs::exists(repo.worktree_))
    {
        if (!fs::is_directory(repo.worktree_))
        {
            throw std::runtime_error("Not a directory: " + path.string());
        }
        // exists() first, so we never list a folder that is missing.
        if (fs::exists(repo.gitdir_) &&
            fs::directory_iterator(repo.gitdir_) != fs::directory_iterator{})
        {
            throw std::runtime_error(".git is not empty: " + repo.gitdir_.string());
        }
    }
    else
    {
        fs::create_directories(repo.worktree_);
    }

    repo.dir("branches", true);
    repo.dir("objects", true);
    repo.dir("refs/tags", true);
    repo.dir("refs/heads", true);

    write_text_file(repo.file("description", true),
                    "Unnamed repository; edit this file 'description' to name the repository.\n");
    write_text_file(repo.file("HEAD", true), "ref: refs/heads/master\n");
    write_text_file(repo.file("config", true),
                    "[core]\n"
                    "\trepositoryformatversion = 0\n"
                    "\tfilemode = false\n"
                    "\tbare = false\n");

    return repo;
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