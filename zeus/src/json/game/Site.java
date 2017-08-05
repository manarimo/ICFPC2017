package json.game;

import org.codehaus.jackson.annotate.JsonIgnore;

public class Site {
    public int id;
    public double x;
    public double y;

    public Site(final int id, final double x, final double y) {
        this.id = id;
        this.x = x;
        this.y = y;
    }

    @JsonIgnore
    public int getId() {
        return id;
    }
}
