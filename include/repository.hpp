#pragma once

#include <filesystem>
#include <string>
#include <optional>

namespace fs = std::filesystem;

struct Config
{

};

class GitRepo {
private:
	fs::path worktree;
	fs::path gitdir;
	Config config;

	void load_config(bool force);

public:
	GitRepo(const fs::path& path, bool force = false);
	static GitRepo create(const fs::path& path);
	static std::optional<GitRepo> find(const fs::path& path = fs::current_path(), bool required = true);
	fs::path path(const fs::path& subpath) const;
	fs::path file(const fs::path& subpath, bool mkdirs = false) const;
	fs::path dir(const fs::path& subpath, bool mkdirs = false) const;
	
	const fs::path& worktree() const {return worktree;}
	const fs::path& gitdir() const {return gitdir;}
	const Config& config() const {return config;}

};

