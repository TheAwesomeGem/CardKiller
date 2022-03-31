// Copyright (C) Topaz Centuallas - All Rights Reserved
// Unauthorized copying of this file, via any medium is strictly prohibited
// Proprietary and confidential
// Written by Topaz Centuallas <topazcentuallas@gmail.com>, 3/30/22, 6:43 PM
//

#include <iostream>
#include <vector>
#include <unordered_map>
#include <valarray>


static constexpr const float FLOAT_MAX = std::numeric_limits<float>::max();
static constexpr const float FLOAT_MIN = std::numeric_limits<float>::min();
static constexpr const int16_t INTEGER_16_MAX = std::numeric_limits<int16_t>::max();
static constexpr const int16_t INTEGER_16_MIN = std::numeric_limits<int16_t>::min();

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
    Health,
    Energy
};

enum class EffectOperator
{
    Specified,
    Additive,
    Multiplicative,
    Exponential
};

enum class EffectType
{
    Heal,
    Damage,
    EnergyUse
};

enum class RequirementType
{
    Energy,
    Health,
    Range
};

struct Action
{
    Action(EffectType effect_, EffectOperator op_, float value_)
            : effect{effect_}, op{op_}, value{value_}
    {

    }

    EffectType effect;
    EffectOperator op;
    float value;
};

struct RequirementData
{
    RequirementData(float min_, float max_)
            : min{min_}, max{max_}
    {

    }

    float min;
    float max;
};

struct Move
{
    explicit Move(int16_t maxUses_)
            : maxUses{maxUses_}
    {

    }

    std::vector<Action> actions;
    std::unordered_map<RequirementType, RequirementData> requirements;
    int16_t maxUses;
};

struct State
{
    State(float min_, float max_, float current_)
            : min{min_}, max{max_}, current{current_}
    {

    }

    float min;
    float max;
    float current;
};

struct Card
{
    std::vector<Move> moves;
    std::unordered_map<StateType, State> states;
};

void ApplyAction(const Action& action, Card& target);

void ApplyMove(Card& source, size_t moveIndex, Card& target)
{
    // TODO: Add a way to restrict certain moves to certain targets
    Move& move = source.moves[moveIndex];

    if (move.maxUses < 1)
    {
        printf("You cannot use this move anymore.\n");

        return;
    }

    for (const Action& action: move.actions)
    {
        ApplyAction(action, target);
    }

    --move.maxUses;
}

void ApplyEffect(EffectOperator op, float modifier, State& value);

void ApplyAction(const Action& action, Card& target)
{
    switch (action.effect)
    {
        case EffectType::Heal:
        {
            ApplyEffect(action.op, action.value, target.states.at(StateType::Health));

            break;
        }
        case EffectType::Damage:
        {
            ApplyEffect(action.op, -action.value, target.states.at(StateType::Health));

            break;
        }
        case EffectType::EnergyUse:
        {
            ApplyEffect(action.op, -action.value, target.states.at(StateType::Energy));

            break;
        }
    }
}

void ApplyEffect(EffectOperator op, float modifier, State& state)
{
    switch (op)
    {
        case EffectOperator::Specified:
        {
            state.current = clamp(modifier, state.min, state.max);

            break;
        }

        case EffectOperator::Additive:
        {
            state.current = clamp(state.current + modifier, state.min, state.max);

            break;
        }
        case EffectOperator::Multiplicative:
        {
            state.current = clamp(state.current * modifier, state.min, state.max);

            break;
        }
        case EffectOperator::Exponential:
        {
            state.current = clamp(std::pow(state.current, modifier), state.min, state.max);

            break;
        }
    }
}

struct CardBuilder
{
    CardBuilder()
            : card{}
    {

    }

    Card Use()
    {
        return std::move(card);
    }

    CardBuilder& AddState(StateType stateType, float min, float max, float current)
    {
        card.states.emplace(stateType, State{min, max, current});

        return *this;
    }

    CardBuilder& AddRequirement(size_t moveIndex, RequirementType requirementType, float min, float max)
    {
        card.moves[moveIndex].requirements.emplace(requirementType, RequirementData{min, max});

        return *this;
    }

    CardBuilder& AddActions(size_t moveIndex, EffectType effectType, EffectOperator op, float value)
    {
        card.moves[moveIndex].actions.emplace_back(effectType, op, value);

        return *this;
    }

    CardBuilder& AddMove(int maxUses)
    {
        Move& move = card.moves.emplace_back(maxUses);

        return *this;
    }

private:
    Card card;
};

int main()
{
    {
        Card card = CardBuilder{}
                .AddState(StateType::Health, 0.0F, 100.0F, 20.0F)
                .AddState(StateType::Energy, 0.0F, 100.0F, 40.0F)
                .AddMove(2)
                .AddMove(INTEGER_16_MAX)
                .AddRequirement(0, RequirementType::Energy, 10.0F, FLOAT_MAX)
                .AddRequirement(1, RequirementType::Energy, 20.0F, FLOAT_MAX)
                .AddActions(0, EffectType::Heal, EffectOperator::Additive, 20.0F)
                .AddActions(0, EffectType::EnergyUse, EffectOperator::Additive, 10.0F)
                .AddActions(1, EffectType::Damage, EffectOperator::Additive, 30.0F)
                .AddActions(1, EffectType::EnergyUse, EffectOperator::Additive, 20.0F)
                .Use();

        ApplyMove(card, 0, card);
        printf("Health is now %f.\n", card.states.at(StateType::Health).current);
        printf("Energy is now %f.\n", card.states.at(StateType::Energy).current);

        ApplyMove(card, 1, card);
        printf("Health is now %f.\n", card.states.at(StateType::Health).current);
        printf("Energy is now %f.\n", card.states.at(StateType::Energy).current);
    }

    std::cin.get();

    return 0;
}
