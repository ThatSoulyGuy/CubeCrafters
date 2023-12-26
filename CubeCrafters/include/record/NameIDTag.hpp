#ifndef NAMEIDTAG_HPP
#define NAMEIDTAG_HPP

#include <string>
#include <memory>
#include "record/Hash.hpp"

class NameIDTag
{

public:

    std::string name;
    std::string description;
    std::string hash;
    void* reference;

    ~NameIDTag()
    {
        name = "";
        description = "";
        hash = "";
    }

    static std::shared_ptr<NameIDTag> Register(const std::string& name, const std::string& description, void* reference)
    {
        std::shared_ptr<NameIDTag> out(new NameIDTag());

        out->name = name;
        out->description = description;
        out->hash = Hash::GenerateMD5Hash(name);
        out->reference = reference;

        return out;
    }

    static std::shared_ptr<NameIDTag> Register(const std::string& name, void* reference)
    {
        std::shared_ptr<NameIDTag> out(new NameIDTag());

        out->name = name;
        out->description = "<any>";
        out->hash = Hash::GenerateMD5Hash(name);
        out->reference = reference;

        return out;
    }

    static std::shared_ptr<NameIDTag> Register(void* reference)
    {
        std::shared_ptr<NameIDTag> out(new NameIDTag());

        out->name = "<any>";
        out->description = "<any>";
        out->hash = Hash::GenerateMD5Hash(std::to_string(rand()));
        out->reference = reference;

        return out;
    }
};

#endif // !NAMEIDTAG_HPP