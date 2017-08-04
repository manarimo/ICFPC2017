package json.comm;

import json.game.Move;
import org.codehaus.jackson.JsonNode;

import java.util.List;

public class GameplayRequest {
    public Moves move;
    public JsonNode state;

    public GameplayRequest(final Moves moves, final JsonNode state) {
        this.move = moves;
        this.state = state;
    }

    public static class Moves {
        public List<Move> moves;

        public Moves(final List<Move> moves) {
            this.moves = moves;
        }
    }
}
