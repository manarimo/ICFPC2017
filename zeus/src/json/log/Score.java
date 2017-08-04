package json.log;

import java.util.List;

public class Score {
    public List<AIScore> scores;

    public Score(final List<AIScore> scores) {
        this.scores = scores;
    }

    public static class AIScore {
        public int punter;
        public int score;

        public AIScore(final int punter, final int score) {
            this.punter = punter;
            this.score = score;
        }
    }
}
