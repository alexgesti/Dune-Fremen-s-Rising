using System;
using RagnarEngine;
public enum State
{
    NONE,
    ABILITY_1,
    ABILITY_2,
    ABILITY_3,
    ABILITY_4,
    POSTCAST,
    DEATH
}

public class Characters
{
    // Basic Character info
    public string name;
    public string prefabPath;
    public State state;
    public int hitPoints;

    // Position
    public Vector3 pos;

    // Abilities
    public Abilities[] abilities;
}