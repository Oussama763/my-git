#pragma once

#include <filesystem>
#include <string>
#include <map>
#include <optional>

namespace fs = std::filesystem

namespace my-git {
struct Config {
	std::map<std::string, std::map<std::string, std::string>> sections;
	int repo_format_version{0};
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
	static std::optional<GitRepo> find(const fs::path& path = fs::current_path());
	fs::path repo_path(const fs::path& subpath) const;
	fs::paath repo_file(const fs::path& subpath, bool mkdirs = false) const;
	fs::paath repo_dir(const fs::path& subpath, bool mkdirs = false) const;
	
	const fs::path& worktree() const {return worktree;}
	const fs::path& gitdir() const {return gitdir;}
	const Config& config() const {return config;}

};

}
