package json.game;

import org.codehaus.jackson.annotate.JsonIgnore;

public class Site {
    public int id;
    public double x;
    public double y;

    @JsonIgnore
    public int getId() {
        return id;
    }
}
