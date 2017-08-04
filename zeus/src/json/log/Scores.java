package json.log;

import json.game.Map;
import json.game.Move;
import json.game.Score;

import java.util.List;

public class Scores {
    public Map map;
    public List<Move> moves;
    public List<Score> scores;

    public Scores(final Map map, final List<Move> moves, final List<Score> scores) {
        this.map = map;
        this.moves = moves;
        this.scores = scores;
    }
}
