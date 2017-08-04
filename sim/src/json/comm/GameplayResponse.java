package json.comm;

import json.game.Move;
import org.codehaus.jackson.JsonNode;
import org.codehaus.jackson.annotate.JsonIgnore;

public class GameplayResponse {
    public Move.Claim claim;
    public Move.Pass pass;
    public JsonNode state;

    @JsonIgnore
    public Move toMove() {
        if (claim != null) {
            return Move.of(claim);
        } else {
            return Move.of(pass);
        }
    }
}
