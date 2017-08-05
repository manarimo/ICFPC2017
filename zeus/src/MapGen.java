import json.game.Map;
import json.game.River;
import json.game.Site;
import org.codehaus.jackson.map.ObjectMapper;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Random;

public class MapGen {
    public static void main(String[] args) throws IOException {
        final int n = 15;
        final double pRiver = 0.7;
        final double pMine = 0.1;
        final Random random = new Random();
        final Map map = new Map();
        map.sites = new ArrayList<>();
        map.mines = new ArrayList<>();
        map.rivers = new ArrayList<>();
        for (int j = 0; j < n; j++) {
            for (int i = 0; i < n; i++) {
                map.sites.add(new Site(i + j * n, i, j));
                if (i > 0 && random.nextDouble() < pRiver) {
                    map.rivers.add(new River(i + j * n, i - 1 + j * n));
                }
                if (j > 0 && random.nextDouble() < pRiver) {
                    map.rivers.add(new River(i + j * n, i + (j - 1) * n));
                }
                if (i > 0 && j > 0 && random.nextDouble() < pRiver) {
                    map.rivers.add(new River(i + j * n, i - 1 + (j - 1) * n));
                }
                if (random.nextDouble() < pMine) {
                    map.mines.add(i + j * n);
                }
            }
        }
        System.out.println(new ObjectMapper().writeValueAsString(map));
    }
}
