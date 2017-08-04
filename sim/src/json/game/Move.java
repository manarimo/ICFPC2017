package json.game;

import org.codehaus.jackson.annotate.JsonIgnore;

public class Move {
    public Claim claim;
    public Pass pass;

    private Move(final Claim claim, final Pass pass) {
        this.claim = claim;
        this.pass = pass;
    }

    public static Move of(final Claim claim) {
        return new Move(claim, null);
    }

    public static Move of(final Pass pass) {
        return new Move(null, pass);
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
    }
}
