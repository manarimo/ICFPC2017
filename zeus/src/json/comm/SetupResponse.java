package json.comm;

import json.game.Future;
import org.codehaus.jackson.JsonNode;

import java.util.List;

public class SetupResponse {
    public int ready;
    public JsonNode state;
    public List<Future> futures;
}
