package json.log;

import json.game.Move;

public class State {
    public Move move;
    public int score;

    public State(final Move move, final int score) {
        this.move = move;
        this.score = score;
    }
}
