#pragma once 
#include <memory>
#include <string>
#include <string_view>


class GitObject 
{
public:
    virtual ~GitObject() = default;
    virtual std::string serialize() const = 0;
    virtual void deserialize(std::string_view data) = 0;
    virtual std::string_view fmt() const = 0;
};


class GitBlob : public GitObject
{
private:
    std::string blob_data;
public:

    std::string serialize() const override;
    void deserialize(std::string_view data) override;
    std::string_view fmt() const override { return "blob"; }

};

/*
class GitTree : public GitObject
{
private:
public:
    std::string serialize() const override;
    void deserialize(std::string_view data) override;


};

class GitCommit : public GitObject
{
private:
public:
    std::string serialize() const override;
    void deserialize(std::string_view data) override;

};

class GitTag : public GitObject
{
private:
public:
    std::string serialize() const override;
    void deserialize(std::string_view data) override;

};
*/