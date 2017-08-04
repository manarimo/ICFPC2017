package game;

import json.comm.GameplayRequest;
import json.comm.GameplayResponse;
import json.comm.SetupRequest;
import json.comm.SetupResponse;
import json.game.Map;
import json.game.Move;
import json.game.River;
import json.game.Site;
import json.log.Score;
import org.codehaus.jackson.JsonNode;
import org.codehaus.jackson.map.ObjectMapper;
import util.JsonUtil;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.*;
import java.util.stream.Collectors;

public class GameServer {
    private static final ObjectMapper objectMapper = new ObjectMapper();

    private final Map map;
    private final java.util.Map<Integer, Set<River>> claimedRivers;
    private final Set<River> remainingRivers;
    private final List<String> ais;
    private final List<JsonNode> states;
    private final List<Move> history;

    // siteId of mine -> siteId -> distance
    private final java.util.Map<Integer, java.util.Map<Integer, Integer>> distances;

    public GameServer(final Map map, final List<String> ais) throws IOException {
        this.map = map;
        claimedRivers = new HashMap<>();
        for (int i = 0; i < ais.size(); i++) {
            claimedRivers.put(i, new HashSet<>());
        }
        remainingRivers = new HashSet<>(map.rivers);
        this.ais = ais;
        states = new ArrayList<>();
        for (int i = 0; i < ais.size(); i++) {
            states.add(null);
        }
        history = new ArrayList<>();
        distances = new HashMap<>();
        for (final Integer mineSiteId : map.mines) {
            final java.util.Map<Integer, Integer> dist = new HashMap<>();
            distances.put(mineSiteId, dist);
            final Set<Integer> rests = new HashSet<>(map.sites.stream().map(Site::getId).collect(Collectors.toList()));
            final Queue<Integer> q = new LinkedList<>();
            q.add(mineSiteId);
            rests.remove(mineSiteId);
            q.add(null);
            int d = 0;
            while (q.size() > 1) {
                final Integer id = q.remove();
                if (id == null) {
                    d++;
                    q.add(null);
                    continue;
                }
                dist.put(id, d);
                for (final River river : map.rivers) {
                    if (river.target == id && rests.contains(river.source)) {
                        q.add(river.source);
                        rests.remove(river.source);
                    } else if (river.source == id && rests.contains(river.target)) {
                        q.add(river.target);
                        rests.remove(river.target);
                    }
                }
            }
        }
        System.err.println("Server initialized.");
        System.out.println(objectMapper.writeValueAsString(map));
    }

    public void run() throws IOException {
        for (int i = 0; i < ais.size(); i++) {
            System.err.println(String.format("Seting up AI #%d...", i));
            final SetupRequest request = new SetupRequest(i, ais.size(), map);

            final Process exec = Runtime.getRuntime().exec(ais.get(i).split(" "));
            final OutputStream outputStream = exec.getOutputStream();
            final InputStream inputStream = exec.getInputStream();
            JsonUtil.write(outputStream, request);
            outputStream.close();

            try {
                final SetupResponse response = JsonUtil.read(inputStream, SetupResponse.class);
                states.set(i, response.state);
                System.err.println("OK");
            } catch (final Exception e) {
                System.err.println("ERROR");
                final InputStream errorStream = exec.getErrorStream();
                final Scanner scanner = new Scanner(errorStream);
                while (scanner.hasNextLine()) {
                    System.err.println(scanner.nextLine());
                }
            }
        }
        for (int i = 0; i < map.rivers.size(); i++) {
            final int punterId = i % ais.size();
            System.err.println(String.format("Turn #%d AI=%d", i, punterId));
            final List<Move> moves = new ArrayList<>();
            for (int j = Math.max(0, i - ais.size()); j < i; j++) {
                moves.add(history.get(j));
            }
            final GameplayRequest request = new GameplayRequest(new GameplayRequest.Moves(moves), states.get(punterId));

            final Process exec = Runtime.getRuntime().exec(ais.get(punterId).split(" "));
            final InputStream inputStream = exec.getInputStream();
            final OutputStream outputStream = exec.getOutputStream();
            JsonUtil.write(outputStream, request);
            outputStream.close();


            try {
                final GameplayResponse response = JsonUtil.read(inputStream, GameplayResponse.class);
                handle(response.toMove());
                states.set(punterId, response.state);
                history.add(response.toMove());
                System.out.println(objectMapper.writeValueAsString(response));
                System.err.println("OK");
            } catch (final Exception e) {
                System.err.println("ERROR");
                final InputStream errorStream = exec.getErrorStream();
                final Scanner scanner = new Scanner(errorStream);
                while (scanner.hasNextLine()) {
                    System.err.println(scanner.nextLine());
                }
            }
        }
        score();
    }

    private void handle(final Move move) {
        if (move.claim == null) {
            return;
        }
        final Move.Claim claim = move.claim;
        River river = claim.toRiver();
        if (!remainingRivers.contains(river)) {
            //todo warning
            return;
        }
        remainingRivers.remove(river);
        claimedRivers.get(claim.punter).add(river);
    }

    private void score() throws IOException {
        final List<Score.AIScore> aiScores = new ArrayList<>();
        for (int i = 0; i < ais.size(); i++) {
            int score = 0;
            for (final Integer mineSiteId : map.mines) {
                final Set<Integer> rests = new HashSet<>(map.sites.stream().map(Site::getId).collect(Collectors.toList()));
                final Queue<Integer> q = new LinkedList<>();
                q.add(mineSiteId);
                rests.remove(mineSiteId);
                while (!q.isEmpty()) {
                    final Integer id = q.remove();
                    score += distances.get(mineSiteId).get(id);
                    for (final River river : map.rivers) {
                        if (river.target == id && claimedRivers.get(i).contains(river) && rests.contains(river.source)) {
                            q.add(river.source);
                            rests.remove(river.source);
                        } else if (river.source == id && claimedRivers.get(i).contains(river) && rests.contains(river.target)) {
                            q.add(river.target);
                            rests.remove(river.target);
                        }
                    }
                }
            }
            aiScores.add(new Score.AIScore(i, score));
        }
        System.out.println(objectMapper.writeValueAsString(new Score(aiScores)));
    }
}
