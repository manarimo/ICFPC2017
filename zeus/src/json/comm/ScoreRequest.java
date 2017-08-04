package json.comm;

import json.game.Move;
import json.game.Score;
import org.codehaus.jackson.JsonNode;

import java.util.List;

public class ScoreRequest {
    public Stop stop;
    public JsonNode state;

    public ScoreRequest(final Stop stop, final JsonNode state) {
        this.stop = stop;
        this.state = state;
    }

    public static class Stop {
        public List<Move> moves;
        public List<Score> scores;

        public Stop(final List<Move> moves, final List<Score> scores) {
            this.moves = moves;
            this.scores = scores;
        }
    }
}
