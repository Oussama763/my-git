#pragma once 
#include "repository.hpp"
#include <memory>
#include <vector>
class Data
{
private:
    std::vector<uint8_t> bytes;
public:
    Data() = default;
    explicit Data(std::vector<uint8_t> b): bytes(std::move(b)) {}
    const std::vector<uint8_t>& getBytes() { return bytes; }

    
};
enum class ObjectType
{
    BLOB, 
    COMMIT, 
    TAG, 
    TREE,
};
class GitObject 
{
public:
    virtual ~GitObject() = 0;
    GitObject() = default;
    GitObject(const Data* data = nullptr); 
    virtual void serialize(const GitRepo* git_repo) = 0; 
    virtual void deserialize(const Data* data) = 0;

};


class GitBlob : public GitObject
{
private:
    Data blob_data;
public:
    GitBlob() = default;
    explicit GitBlob(const Data* data);

    void serialize(const GitRepo* git_repo) override;
    void deserialize(const Data* data) override;
};


class GitTree : public GitObject
{
private:
    Data tree_data; 
public:
    GitTree() = default;
    explicit GitTree(const Data* data);

    void serialize(const GitRepo* git_repo) override;
    void deserialize(const Data* data) override;
    
};

class GitCommit : public GitObject
{
private:
    Data commit_data; 
public:
    GitCommit() = default;
    explicit GitCommit(const Data* data);

    void serialize(const GitRepo* git_repo) override;
    void deserialize(const Data* data) override;


};

class GitTag : public GitObject
{
private:
    Data tag_data;
public:
    GitTag() = default;
    explicit GitTag(const Data* data);

    void serialize(const GitRepo* git_repo) override;
    void deserialize(const Data* data) override;


};