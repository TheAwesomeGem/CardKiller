// Copyright (C) Topaz Centuallas - All Rights Reserved
// Unauthorized copying of this file, via any medium is strictly prohibited
// Proprietary and confidential
// Written by Topaz Centuallas <topazcentuallas@gmail.com>, 3/30/22, 6:43 PM
//

#include <iostream>
#include <vector>
#include <unordered_map>
#include <valarray>


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
    Health
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
    Damage
};

struct Action
{
    EffectType effect;
    EffectOperator op;
    float value;
};

struct Move
{
    // TODO: Add requirements/conditions to apply certain action
    // TODO: Add limits to certain actions. Like a max and a min (EffectOperator+Value)
    std::vector<Action> actions;
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
    std::vector<Move> moves; // TODO: Add requirements/constraints to performing certain moves
    std::unordered_map<StateType, State> states;
};

void ApplyAction(const Action& action, Card& target);

void ApplyMove(const Card& source, int moveIndex, Card& target)
{
    // TODO: Add a way to restrict certain moves to certain targets

    const Move& move = source.moves[moveIndex];

    for (const Action& action: move.actions)
    {
        ApplyAction(action, target);
    }
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

int main()
{
    {
        Card card;
        card.states.emplace(StateType::Health, State{0.0F, 100.0F, 20.F});

        Move& healMove = card.moves.emplace_back();
        Action& healAction = healMove.actions.emplace_back();
        healAction.value = 20.F;
        healAction.op = EffectOperator::Additive;
        healAction.effect = EffectType::Heal;

        Move& damageMove = card.moves.emplace_back();
        Action& damageAction = damageMove.actions.emplace_back();
        damageAction.value = 20.F;
        damageAction.op = EffectOperator::Additive;
        damageAction.effect = EffectType::Damage;

        ApplyMove(card, 0, card);
        printf("Health is now %f.\n", card.states.at(StateType::Health).current);

        ApplyMove(card, 1, card);
        ApplyMove(card, 1, card);
        printf("Health is now %f.\n", card.states.at(StateType::Health).current);
    }

    std::cin.get();

    return 0;
}
