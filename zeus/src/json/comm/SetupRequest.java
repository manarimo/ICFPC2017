package json.comm;

import json.game.Map;

public class SetupRequest {
    public int punter;
    public int punters;
    public Map map;

    public SetupRequest(final int punter, final int punters, final Map map) {
        this.punter = punter;
        this.punters = punters;
        this.map = map;
    }
}
