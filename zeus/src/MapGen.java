import json.game.Map;
import json.game.River;
import json.game.Site;
import org.codehaus.jackson.map.ObjectMapper;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Random;

public class MapGen {
    public static void randomCornerMine() throws IOException {
        final int n = 15;
        final double pRiver = 0.7;
        final double pMine = 0.1;
        final Random random = new Random();
        final Map map = new Map();
        map.sites = new ArrayList<>();
        map.mines = new ArrayList<>();
        map.rivers = new ArrayList<>();
        for (int j = 0; j < n / 2; j++) {
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
                if ((i == 0 || i == n - 1) && (j == 0 || j == n - 1)) {
                    map.mines.add(i + j * n);
                }
            }
        }
        System.out.println(new ObjectMapper().writeValueAsString(map));
    }

    public static void archipelago() throws IOException {
        final Map map = new Map();
        map.sites = new ArrayList<>();
        map.mines = new ArrayList<>();
        map.rivers = new ArrayList<>();

        int id = 0;
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                map.mines.add(id);
                map.rivers.add(new River(id, (id + 1) % 4));
                map.sites.add(new Site(id++, i * 5, j * 5));
            }
        }

        for (int i = 0; i < 6; ++i) {
            if (i == 0) map.mines.add(id);
            else map.rivers.add(new River(id - 1, id));
            map.sites.add(new Site(id++, 10 + i * 5, 0));
        }

        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (j == 0 || j == 3) map.mines.add(id);
                if (j != 0) map.rivers.add(new River(id - 1, id));
                map.sites.add(new Site(id++, 10 * i + 5 * j, 10 + 10 * i));
            }
        }
        System.out.println(new ObjectMapper().writeValueAsString(map));
    }

    public static void houki() throws IOException {
        final int eLen = 100;
        final int keNum = 50;
        final int keLen = 3;

        final Map map = new Map();
        map.sites = new ArrayList<>();
        map.mines = new ArrayList<>();
        map.rivers = new ArrayList<>();

        int id = 0;
        for (int i = 0; i < keNum; ++i) {
            for (int j = 0; j < keLen; ++j) {
                map.sites.add(new Site(id, -10 + i * 5, (j - 5) * keNum));
                if (j == 0) {
                    map.mines.add(id);
                }
                if (j != keLen - 1) {
                    map.rivers.add(new River(id, id + 1));
                } else {
                    map.rivers.add(new River(id, keLen * keNum));
                }
                id++;
            }
        }

        map.sites.add(new Site(id++, 0, 0));
        for (int i = 0; i < eLen; ++i) {
            map.rivers.add(new River(id - 1, id));
            map.sites.add(new Site(id++, i * 5, 0));
        }
        System.out.println(new ObjectMapper().writeValueAsString(map));
    }

    public static void rand() throws IOException {
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

    public static void main(String[] args) throws IOException {
        randomCornerMine();
    }
}
