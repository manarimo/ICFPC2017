package json.log;

import json.game.Future;
import json.game.Map;
import json.game.Settings;

import java.util.List;

public class Scores {
    public Map map;
    public Settings settings;
    public int numPunter;
    public List<List<Future>> futures;
    public List<State> history;

    public Scores(final Map map, final Settings settings, final int numPunter, final List<List<Future>> futures, final List<State> history) {
        this.map = map;
        this.settings = settings;
        this.numPunter = numPunter;
        this.futures = futures;
        this.history = history;
    }
}
