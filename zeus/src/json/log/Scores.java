package json.log;

import json.game.Map;

import java.util.List;

public class Scores {
    public Map map;
    public int numPunter;
    public List<State> history;

    public Scores(final Map map, final int numPunter, final List<State> history) {
        this.map = map;
        this.numPunter = numPunter;
        this.history = history;
    }
}
