#pragma once
#include <unordered_map>
#include "util/Joaat.hpp"

namespace YimMenu
{
    class RuleService
    {
    private:
    std::unordered_map<joaat_t, BOOL> m_rules{};
    
    public:
    void add_rule(joaat_t hash, BOOL value);

    void get_rule(joaat_t hash, BOOL* value);

    void remove_rule(joaat_t hash);
    };

    class AwardService : public RuleService
    {
        public:
        AwardService();
        ~AwardService();
    };

    inline AwardService* g_award_service;
}