package json.comm;

import json.game.Map;
import json.game.Settings;

public class SetupRequest {
    public int punter;
    public int punters;
    public Map map;
    public Settings settings;

    public SetupRequest(final int punter, final int punters, final Map map, final Settings settings) {
        this.punter = punter;
        this.punters = punters;
        this.map = map;
        this.settings = settings;
    }
}
