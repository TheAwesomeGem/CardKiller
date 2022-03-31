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
    Energy,
    Bleeding
};

enum class EffectOperator
{
    Specified,
    Additive,
    Multiplicative,
    Exponential
};

struct Action
{
    Action(StateType stateType_, EffectOperator op_, float value_)
            : stateType{stateType_}, op{op_}, value{value_}
    {

    }

    StateType stateType;
    EffectOperator op;
    float value;
};

struct Requirement
{
    Requirement(StateType _stateType, float min_, float max_)
            : stateType{_stateType}, min{min_}, max{max_}
    {

    }

    StateType stateType;
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
    std::vector<Requirement> requirements;
    int16_t maxUses; // TODO: Maybe this can be part of the requirements?
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

void ApplyEffect(EffectOperator op, float modifier, State& value);

void ApplyMove(Card& source, size_t moveIndex, Card& target)
{
    // TODO: Add a way to restrict certain moves to certain targets
    Move& move = source.moves[moveIndex];

    if (move.maxUses < 1)
    {
        printf("You cannot use this move anymore.\n");

        return;
    }

    for (const Requirement& requirement: move.requirements)
    {
        float current = target.states.at(requirement.stateType).current;

        if (current < requirement.min || current > requirement.max)
        {
            printf("You do not meet the requirement to be able to do this move.\n");

            return;
        }
    }

    for (const Action& action: move.actions)
    {
        ApplyEffect(action.op, action.value, target.states.at(action.stateType));
    }

    --move.maxUses;
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

    CardBuilder& AddRequirement(size_t moveIndex, StateType stateType, float min, float max)
    {
        card.moves[moveIndex].requirements.emplace_back(Requirement{stateType, min, max});

        return *this;
    }

    CardBuilder& AddActions(size_t moveIndex, StateType stateType, EffectOperator op, float value)
    {
        card.moves[moveIndex].actions.emplace_back(stateType, op, value);

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
    // TODO: Support bleeding

    {
        Card card = CardBuilder{}
                .AddState(StateType::Health, 0.0F, 100.0F, 20.0F)
                .AddState(StateType::Energy, 0.0F, 100.0F, 40.0F)
                .AddMove(2)
                .AddMove(INTEGER_16_MAX)
                .AddRequirement(0, StateType::Energy, 11.0F, FLOAT_MAX)
                .AddRequirement(1, StateType::Energy, 20.0F, FLOAT_MAX)
                .AddActions(0, StateType::Health, EffectOperator::Additive, +20.0F)
                .AddActions(0, StateType::Energy, EffectOperator::Additive, -10.0F)
                .AddActions(1, StateType::Health, EffectOperator::Additive, -30.0F)
                .AddActions(1, StateType::Energy, EffectOperator::Additive, -20.0F)
                .Use();

        ApplyMove(card, 0, card);
        printf("Health is now %f.\n", card.states.at(StateType::Health).current);
        printf("Energy is now %f.\n", card.states.at(StateType::Energy).current);

        ApplyMove(card, 1, card);
        printf("Health is now %f.\n", card.states.at(StateType::Health).current);
        printf("Energy is now %f.\n", card.states.at(StateType::Energy).current);

        ApplyMove(card, 0, card);
        printf("Health is now %f.\n", card.states.at(StateType::Health).current);
        printf("Energy is now %f.\n", card.states.at(StateType::Energy).current);
    }

    std::cin.get();

    return 0;
}
