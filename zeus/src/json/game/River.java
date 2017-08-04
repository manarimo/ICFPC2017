package json.game;

import java.util.Objects;

public class River {
    public int source;
    public int target;

    public River() {}

    public River(final int source, final int target) {
        this.source = source;
        this.target = target;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        River river = (River) o;
        return source == river.source &&
                target == river.target;
    }

    @Override
    public int hashCode() {
        return Objects.hash(source, target);
    }
}
