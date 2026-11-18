#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include "repository.hpp"

class RepoTest : public ::testing::Test
{
protected:
    fs::path root;

    void SetUp() override
    {
        root = fs::temp_directory_path() /
               ("my_git_test_" + std::string(::testing::UnitTest::GetInstance()->current_test_info()->name()));
        fs::remove_all(root);
        fs::create_directories(root);
    }

    void TearDown() override
    {
        fs::remove_all(root);
    }

    void make_git_dir()
    {
        fs::create_directories(root / ".git");
    }
};

TEST_F(RepoTest, ConstructorThrowsWithoutGitDir)
{
    EXPECT_THROW(GitRepo repo(root), std::runtime_error);
}

TEST_F(RepoTest, ConstructorForceAcceptsMissingGitDir)
{
    EXPECT_NO_THROW(GitRepo repo(root, true));
}

TEST_F(RepoTest, ConstructorSetsWorktreeAndGitdir)
{
    make_git_dir();
    GitRepo repo(root);
    EXPECT_EQ(repo.worktree(), root);
    EXPECT_EQ(repo.gitdir(), root / ".git");
}

TEST_F(RepoTest, PathJoinsUnderGitdirWithoutCreating)
{
    make_git_dir();
    GitRepo repo(root);
    EXPECT_EQ(repo.path("refs/heads/master"), root / ".git" / "refs/heads/master");
    EXPECT_FALSE(fs::exists(root / ".git" / "refs"));
}

TEST_F(RepoTest, DirReturnsEmptyWhenMissing)
{
    make_git_dir();
    GitRepo repo(root);
    EXPECT_TRUE(repo.dir("objects").empty());
    EXPECT_FALSE(fs::exists(root / ".git" / "objects"));
}

TEST_F(RepoTest, DirCreatesWhenAsked)
{
    make_git_dir();
    GitRepo repo(root);
    fs::path p = repo.dir("refs/heads", true);
    EXPECT_EQ(p, root / ".git" / "refs/heads");
    EXPECT_TRUE(fs::is_directory(p));
}

TEST_F(RepoTest, DirReturnsExistingDirectory)
{
    make_git_dir();
    fs::create_directories(root / ".git" / "objects");
    GitRepo repo(root);
    EXPECT_EQ(repo.dir("objects"), root / ".git" / "objects");
}

TEST_F(RepoTest, DirThrowsWhenPathIsAFile)
{
    make_git_dir();
    std::ofstream(root / ".git" / "HEAD") << "ref: refs/heads/master\n";
    GitRepo repo(root);
    EXPECT_THROW(repo.dir("HEAD"), std::runtime_error);
    EXPECT_THROW(repo.dir("HEAD", true), std::runtime_error);
}

TEST_F(RepoTest, FileAtTopLevelOfGitdir)
{
    make_git_dir();
    GitRepo repo(root);
    EXPECT_EQ(repo.file("HEAD"), root / ".git" / "HEAD");
}

TEST_F(RepoTest, FileReturnsEmptyWhenParentMissing)
{
    make_git_dir();
    GitRepo repo(root);
    EXPECT_TRUE(repo.file("objects/ab/cdef").empty());
    EXPECT_FALSE(fs::exists(root / ".git" / "objects"));
}

TEST_F(RepoTest, FileCreatesParentButNotFile)
{
    make_git_dir();
    GitRepo repo(root);
    fs::path p = repo.file("objects/ab/cdef", true);
    EXPECT_EQ(p, root / ".git" / "objects/ab/cdef");
    EXPECT_TRUE(fs::is_directory(root / ".git" / "objects" / "ab"));
    EXPECT_FALSE(fs::exists(p));
}

TEST_F(RepoTest, FindAtRepoRoot)
{
    make_git_dir();
    auto repo = GitRepo::find(root);
    ASSERT_TRUE(repo.has_value());
    EXPECT_EQ(repo->worktree(), fs::canonical(root));
}

TEST_F(RepoTest, FindFromNestedSubdirectory)
{
    make_git_dir();
    fs::create_directories(root / "src" / "deep" / "folder");
    auto repo = GitRepo::find(root / "src" / "deep" / "folder");
    ASSERT_TRUE(repo.has_value());
    EXPECT_EQ(repo->worktree(), fs::canonical(root));
}

TEST_F(RepoTest, FindNotRequiredReturnsNulloptOutsideRepo)
{
    auto repo = GitRepo::find(root, false);
    EXPECT_FALSE(repo.has_value());
}

TEST_F(RepoTest, FindRequiredThrowsOutsideRepo)
{
    EXPECT_THROW(GitRepo::find(root, true), std::runtime_error);
}


// ---- GitRepo::create ------------------------------------------------------

namespace
{
    std::string slurp(const fs::path& p)
    {
        std::ifstream in(p, std::ios::binary);
        return {std::istreambuf_iterator<char>(in), {}};
    }
}

TEST_F(RepoTest, CreateBuildsTheGitLayout)
{
    GitRepo repo = GitRepo::create(root / "proj");
    EXPECT_TRUE(fs::is_directory(root / "proj" / ".git" / "branches"));
    EXPECT_TRUE(fs::is_directory(root / "proj" / ".git" / "objects"));
    EXPECT_TRUE(fs::is_directory(root / "proj" / ".git" / "refs" / "tags"));
    EXPECT_TRUE(fs::is_directory(root / "proj" / ".git" / "refs" / "heads"));
    EXPECT_EQ(repo.gitdir(), root / "proj" / ".git");
}

TEST_F(RepoTest, CreateWritesHeadAndConfigWithUnixNewlines)
{
    GitRepo::create(root / "proj");
    EXPECT_EQ(slurp(root / "proj" / ".git" / "HEAD"), "ref: refs/heads/master\n");
    const std::string config = slurp(root / "proj" / ".git" / "config");
    EXPECT_NE(config.find("repositoryformatversion = 0"), std::string::npos);
    EXPECT_EQ(config.find('\r'), std::string::npos);
}

TEST_F(RepoTest, CreateWorksInAnExistingEmptyFolder)
{
    EXPECT_NO_THROW(GitRepo::create(root));
    EXPECT_TRUE(fs::is_directory(root / ".git" / "objects"));
}

TEST_F(RepoTest, CreateThrowsWhenGitDirIsNotEmpty)
{
    GitRepo::create(root);
    EXPECT_THROW(GitRepo::create(root), std::runtime_error);
}

TEST_F(RepoTest, CreateThrowsWhenPathIsAFile)
{
    std::ofstream(root / "afile") << "x";
    EXPECT_THROW(GitRepo::create(root / "afile"), std::runtime_error);
}

TEST_F(RepoTest, CreatedRepoIsFoundByFind)
{
    GitRepo::create(root / "proj");
    fs::create_directories(root / "proj" / "a" / "b");
    auto found = GitRepo::find(root / "proj" / "a" / "b");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(fs::weakly_canonical(found->worktree()), fs::weakly_canonical(root / "proj"));
}
