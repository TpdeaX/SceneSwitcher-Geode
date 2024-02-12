#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;


static bool nodeIsHovered(CCNode* node, CCPoint const& gpos) {
    auto pos = node->getParent()->convertToWorldSpace(node->getPosition());
    auto size = node->getScaledContentSize();

    auto rect = CCRect{
        pos.x - size.width / 2,
        pos.y - size.height / 2,
        size.width,
        size.height
    };

    return rect.containsPoint(gpos);
}

static bool operator!=(CCPoint const& p1, CCPoint const& p2) {
    return p1.x != p2.x || p1.y != p2.y;
}

static std::vector<std::string> stringSplit(std::string const& orig, std::string const& split) {
    std::vector<std::string> res;

    if (orig.size()) {
        std::string s = orig;

        size_t pos = 0;

        while ((pos = s.find(split)) != std::string::npos) {
            res.push_back(s.substr(0, pos));
            s.erase(0, pos + split.length());
        }

        res.push_back(s);
    }

    return res;
}

static std::string stringToLower(std::string const& orig) {
    std::string res = orig;
    std::transform(res.begin(), res.end(), res.begin(), [](unsigned char c) { return std::tolower(c); });
    return res;
}

static std::string stringReplace(std::string const& orig, std::string const& subs, std::string const& reps) {
    std::string::size_type n = 0;
    std::string res = orig;

    while ((n = res.find(subs, n)) != std::string::npos) {
        res.replace(n, subs.size(), reps);
        n += reps.size();
    }

    return res;
}

static bool stringContains(std::string const& orig, std::string const& subs) {
    return orig.find(subs) != std::string::npos;
}

static std::string readFileString(const std::string& _path) {
    std::ifstream in(_path, std::ios::in | std::ios::binary);
    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize((const size_t)in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    return """";
}

template <class R, class T>
R& from(T base, intptr_t offset) {
    return *reinterpret_cast<R*>(reinterpret_cast<uintptr_t>(base) + offset);
}