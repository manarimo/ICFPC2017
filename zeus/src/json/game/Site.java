package json.game;

import org.codehaus.jackson.annotate.JsonIgnore;

public class Site {
    public int id;

    @JsonIgnore
    public int getId() {
        return id;
    }
}
