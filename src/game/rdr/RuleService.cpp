#include "RuleService.hpp"

namespace YimMenu
{
    void RuleService::add_rule(joaat_t hash, BOOL value)
    {
        auto it = m_rules.find(hash);
        if(it != m_rules.end())
            it->second = value;
        else
            m_rules.emplace(hash, value);
    }

    void RuleService::get_rule(joaat_t hash, BOOL* value)
    {
        auto it = m_rules.find(hash);
        if(it != m_rules.end())
            *value = it->second;
    }

    void RuleService::remove_rule(joaat_t hash)
    {
        auto it = m_rules.find(hash);
        if(it != m_rules.end())
            m_rules.erase(it);
    }

    AwardService::AwardService()
    {
        g_award_service = this;
    }

    AwardService::~AwardService()
    {
        g_award_service = nullptr;
    }
}