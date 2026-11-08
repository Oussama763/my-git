#pragma once

#include <string>
#include <optional>
#include <map>


class GitRepo;

namespace gitima {

enum class RefType {
	Direct,
	Symbolic
};

struct Reference {
	std::string name;
	RefType type;
	std::string target;
};

class RefManger {
public:
	static std::optional<std::string> resolve(GitRepo& repo, const std::string& ref_name);
	static std::optional<Reference> read(GitRepo& repo, const std::string ref_name);
	static void write(GitRepo& repo, const std::string& ref_name, const std::string& value, bool symbolic = false);
	static std::map<std::string, std::string> list_all(GitRepo& repo, const std::string& prefix = "refs");
};
}
