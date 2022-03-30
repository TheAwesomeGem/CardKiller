// Copyright (C) Topaz Centuallas - All Rights Reserved
// Unauthorized copying of this file, via any medium is strictly prohibited
// Proprietary and confidential
// Written by Topaz Centuallas <topazcentuallas@gmail.com>, 3/30/22, 6:43 PM
//

#include <iostream>
#include <memory>
#include <set>
#include <vector>
#include <unordered_map>


template<typename T>
T clamp(T value, T min, T max)
{
    if (value < min)
    {
        value = min;
    }

    if (value > max)
    {
        value = max;
    }

    return value;
}

template<class InputIt, class OutputIt, class Pred, class Fct>
void transform_if(InputIt first, InputIt last, OutputIt dest, Pred pred, Fct transform)
{
    while (first != last)
    {
        if (pred(*first))
        {
            *dest++ = transform(*first);
        }

        ++first;
    }
}

enum class StateType
{
    MaxHealth,
    Health
};

struct StateContainer
{
    std::unordered_map<StateType, std::pair<std::string, float>> states;
};

struct Action
{
    virtual ~Action() = default;

    virtual bool CanApplyTo(const StateContainer& target) const = 0;

    virtual void Apply(StateContainer& target) = 0;
};

struct Card
{
    void AddAction(std::unique_ptr<Action> action)
    {
        actions.push_back(std::move(action));
    }

    std::vector<std::unique_ptr<Action>> actions;
    StateContainer stateContainer;
};

struct DamageAction : Action
{
    explicit DamageAction(float damage_)
            : damage{damage_}
    {
        printf("Initialized DamageAction.\n");
    }

    ~DamageAction() override
    {
        printf("DeInitialized DamageAction.\n");
    }

    bool CanApplyTo(const StateContainer& other) const override
    {
        return true;
    }

    void Apply(StateContainer& target) override
    {
        printf("Executing DamageAction.\n");
    }

    float damage;
};

struct HealAction : Action
{
    explicit HealAction(float heal_)
            : heal{heal_}
    {
        printf("Initialized HealAction.\n");
    }

    ~HealAction() override
    {
        printf("DeInitialized HealAction.\n");
    }

    bool CanApplyTo(const StateContainer& other) const override
    {
        return true;
    }

    void Apply(StateContainer& target) override
    {
        float newHealth = target.states[StateType::Health].second + heal;

        target.states[StateType::Health].second = clamp(newHealth, 0.0F, target.states[StateType::MaxHealth].second);

        printf("Executing HealAction to %d targets.\n");
    }

    float heal;
};

static std::unique_ptr<Action> CreateDamageAction()
{
    return std::make_unique<DamageAction>(20);
}

static std::unique_ptr<Action> CreateHealAction()
{
    return std::make_unique<HealAction>(30);
}

void ApplyAction(Card& source, int actionIndex, Card& target)
{
    auto& action = source.actions[actionIndex];
    action->Apply(target.stateContainer);
}

int main()
{
    {
        Card card;
        card.stateContainer.states[StateType::MaxHealth] = std::make_pair("Max Health", 100.0F);
        card.stateContainer.states[StateType::Health] = std::make_pair("Health", 20.0F);
        card.AddAction(CreateHealAction());
        card.AddAction(CreateDamageAction());

        ApplyAction(card, 0, card);
        printf("Health is now %f.\n", card.stateContainer.states[StateType::Health].second);
    }

    std::cin.get();

    return 0;
}
