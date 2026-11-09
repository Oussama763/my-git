#pragma once

#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

struct Config
{

};

class GitRepo {
private:
	fs::path worktree_;
	fs::path gitdir_;
	Config config_;

	void load_config(bool force);

public:
	GitRepo(const fs::path& path, bool force = false);
	static GitRepo create(const fs::path& path);
	static std::optional<GitRepo> find(const fs::path& path = fs::current_path(), bool required = true);
	fs::path path(const fs::path& subpath) const;
	fs::path file(const fs::path& subpath, bool mkdirs = false) const;
	fs::path dir(const fs::path& subpath, bool mkdirs = false) const;
	
	const fs::path& worktree() const {return worktree_;}
	const fs::path& gitdir() const {return gitdir_;}
	const Config& config() const {return config_;}

};

