package json.game;

import org.codehaus.jackson.annotate.JsonIgnore;

import java.util.ArrayList;
import java.util.List;

public class Move {
    public Claim claim;
    public Pass pass;
    public Splurge splurge;
    public Option option;

    private Move(final Claim claim, final Pass pass, final Splurge splurge, final Option option) {
        this.claim = claim;
        this.pass = pass;
        this.splurge = splurge;
        this.option = option;
    }

    public static Move of(final Claim claim) {
        return new Move(claim, null, null, null);
    }

    public static Move of(final Pass pass) {
        return new Move(null, pass, null, null);
    }

    public static Move of(final Splurge splurge) {
        return new Move(null, null, splurge, null);
    }

    public static Move of(final Option option) {
        return new Move(null, null, null, option);
    }

    public static class Claim {
        public int punter;
        public int source;
        public int target;

        @JsonIgnore
        public River toRiver() {
            return new River(source, target);
        }
    }

    public static class Pass {
        public int punter;

        public Pass() {}

        public Pass(final int punter) {
            this.punter = punter;
        }
    }

    public static class Splurge {
        public int punter;
        public List<Integer> route;

        @JsonIgnore
        public List<River> toRivers() {
            final List<River> rivers = new ArrayList<>();
            for (int i = 1; i < route.size(); i++) {
                rivers.add(new River(route.get(i - 1), route.get(i)));
            }
            return rivers;
        }
    }

    public static class Option {
        public int punter;
        public int source;
        public int target;

        @JsonIgnore
        public River toRiver() {
            return new River(source, target);
        }
    }
}
