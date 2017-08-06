package game;

import json.comm.*;
import json.game.*;
import json.game.Map;
import json.log.Scores;
import json.log.State;
import org.codehaus.jackson.JsonNode;
import org.codehaus.jackson.map.ObjectMapper;
import util.JsonUtil;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.*;
import java.util.concurrent.TimeUnit;
import java.util.stream.Collectors;

public class GameServer {
    private static final ObjectMapper objectMapper = new ObjectMapper();

    private final Map map;
    private final java.util.Map<Integer, Set<River>> claimedRivers;
    private final Set<River> remainingRivers;
    private final List<String> ais;
    private final List<JsonNode> states;
    private final List<Move> history;
    private final List<Integer> scores;
    private final Settings settings;
    private final List<List<Future>> futures;
    private final List<String> names;
    private final List<Boolean> zombie;
    private final List<Integer> skipping;

    // siteId of mine -> siteId -> distance
    private final java.util.Map<Integer, java.util.Map<Integer, Integer>> distances;

    public GameServer(final Map map, final List<String> ais, final Settings settings) throws IOException {
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
        scores = new ArrayList<>();
        this.settings = settings;
        futures = new ArrayList<>();
        for (int i = 0; i < ais.size(); i++) {
            futures.add(new ArrayList<>());
        }
        names = new ArrayList<>();
        for (int i = 0; i < ais.size(); i++) {
            names.add("noname");
        }
        zombie = new ArrayList<>();
        for (int i = 0; i < ais.size(); i++) {
            zombie.add(false);
        }
        skipping = new ArrayList<>();
        for (int i = 0; i < ais.size(); i++) {
            skipping.add(0);
        }
        System.err.println("Server initialized.");
    }

    public void run() throws IOException {
        // 1. Setup
        for (int i = 0; i < ais.size(); i++) {
            System.err.println(String.format("Setting up AI #%d...", i));
            final SetupRequest request = new SetupRequest(i, ais.size(), map, settings);

            final Process exec = Runtime.getRuntime().exec(ais.get(i));
            handshake(i, exec);
            final OutputStream outputStream = exec.getOutputStream();
            final InputStream inputStream = exec.getInputStream();
            JsonUtil.write(outputStream, request);
            outputStream.close();
            setTimeout(exec, 10);

            try {
                final SetupResponse response = JsonUtil.read(inputStream, SetupResponse.class);
                states.set(i, response.state);
                if (settings.futures && response.futures != null) {
                    futures.get(i).addAll(validateFutures(response.futures));
                }
                System.err.println("OK");
            } catch (final Exception e) {
                System.err.println("ERROR");
                zombie.set(i, true);
                final InputStream errorStream = exec.getErrorStream();
                final Scanner scanner = new Scanner(errorStream);
                while (scanner.hasNextLine()) {
                    System.err.println(scanner.nextLine());
                }
            }
        }

        // 2. Gameplay
        for (int i = 0; i < map.rivers.size(); i++) {
            final int punterId = i % ais.size();
            System.err.println(String.format("Turn #%d AI=%d", i, punterId));
            final List<Move> moves = new ArrayList<>();
            for (int j = Math.max(0, i - ais.size()); j < i; j++) {
                moves.add(history.get(j));
            }

            Move move = Move.of(new Move.Pass(punterId));
            if (!zombie.get(punterId)) {
                final GameplayRequest request = new GameplayRequest(new GameplayRequest.Moves(moves), states.get(punterId));

                final Process exec = Runtime.getRuntime().exec(ais.get(punterId));
                handshake(punterId, exec);
                final InputStream inputStream = exec.getInputStream();
                final OutputStream outputStream = exec.getOutputStream();
                JsonUtil.write(outputStream, request);
                outputStream.close();
                setTimeout(exec, 1);

                try {
                    final GameplayResponse response = JsonUtil.read(inputStream, GameplayResponse.class);
                    move = response.toMove();
                    states.set(punterId, response.state);
                } catch (final Exception e) {
                    System.err.println("ERROR");
                    zombie.set(punterId, true);
                    final InputStream errorStream = exec.getErrorStream();
                    final Scanner scanner = new Scanner(errorStream);
                    while (scanner.hasNextLine()) {
                        System.err.println(scanner.nextLine());
                    }
                }
            }
            handle(move, punterId);
            int score = score(punterId);
            scores.add(score);
            System.err.println("OK");
            System.err.println(punterId + " " + score);
        }

        // 3. Scoring
        stop();
    }

    private List<Future> validateFutures(final List<Future> futures) {
        if (futures == null) {
            return Collections.emptyList();
        }
        final java.util.Map<Integer, Integer> futureMap = new HashMap<>();
        for (final Future future : futures) {
            if (map.mines.contains(future.source) && !map.mines.contains(future.target)) {
                futureMap.put(future.source, future.target);
            }
        }
        return futureMap.entrySet().stream()
                .map(entry -> new Future(entry.getKey(), entry.getValue()))
                .collect(Collectors.toList());
    }

    private void pass(final int punterId, final String message) {
        history.add(Move.of(new Move.Pass(punterId)));
        System.err.println(message);
        skipping.set(punterId, skipping.get(punterId) + 1);
    }

    private void handle(final Move move, final int punterId) throws IOException {
        if (move.claim == null) {
            pass(punterId, null);
            return;
        }
        if (move.splurge != null) {
            if (!settings.splurges) {
                pass(punterId, "Splurges が有効になっていません。");
            }
            if (move.splurge.route.size() - 2 > skipping.get(punterId)) {
                pass(punterId, String.format("有給申請 %d日 残有給: %d日", move.splurge.route.size() - 2, skipping.get(punterId)));
                return;
            }
            for (final River river : move.splurge.toRivers()) {
                if (!containsRiver(river)) {
                    pass(punterId, "それ、先月までなんですよ。");
                    return;
                }
            }
            for (final River river : move.splurge.toRivers()) {
                River realRiver = removeRiver(river);
                claimedRivers.get(move.splurge.punter).add(realRiver);
                history.add(move);
                skipping.set(punterId, 0);
            }
        }
        final Move.Claim claim = move.claim;
        River river = claim.toRiver();
        if (!remainingRivers.contains(river)) {
            pass(punterId, "それ、取られてますよ。");
            return;
        }
        remainingRivers.remove(river);
        claimedRivers.get(claim.punter).add(river);
        history.add(move);
        skipping.set(punterId, 0);
    }

    private boolean containsRiver(final River river) {
        return remainingRivers.contains(river) || remainingRivers.contains(river.reverse());
    }

    private River removeRiver(final River river) {
        if (remainingRivers.contains(river)) {
            remainingRivers.remove(river);
            return river;
        } else {
            remainingRivers.remove(river.reverse());
            return null;
        }
    }

    private Integer findFutureTarget(final int punterId, final int mineSiteId) {
        for (final Future future : futures.get(punterId)) {
            if (future.source == mineSiteId) {
                return future.target;
            }
        }
        return null;
    }

    private int score(final int punterId) {
        int score = 0;
        for (final Integer mineSiteId : map.mines) {
            Integer futureTarget = null;
            if (settings.futures) {
                futureTarget = findFutureTarget(punterId, mineSiteId);
            }
            final Set<Integer> rests = new HashSet<>(map.sites.stream().map(Site::getId).collect(Collectors.toList()));
            final Queue<Integer> q = new LinkedList<>();
            q.add(mineSiteId);
            rests.remove(mineSiteId);
            while (!q.isEmpty()) {
                final Integer id = q.remove();
                final int d = distances.get(mineSiteId).get(id);
                if (id.equals(futureTarget)) {
                    score += d * d * d;
                }
                score += d * d;
                for (final River river : map.rivers) {
                    if (river.target == id && claimedRivers.get(punterId).contains(river) && rests.contains(river.source)) {
                        q.add(river.source);
                        rests.remove(river.source);
                    } else if (river.source == id && claimedRivers.get(punterId).contains(river) && rests.contains(river.target)) {
                        q.add(river.target);
                        rests.remove(river.target);
                    }
                }
            }
        }
        return score;
    }

    private void stop() throws IOException {
        final List<Score> pScores = new ArrayList<>();
        for (int i = 0; i < ais.size(); i++) {
            int score = score(i);
            pScores.add(new Score(i, score));
        }
        for (int i = 0; i < ais.size(); i++) {
            final ScoreRequest.Stop stop = new ScoreRequest.Stop(history, pScores);
            final ScoreRequest request = new ScoreRequest(stop, states.get(i));

            final Process exec = Runtime.getRuntime().exec(ais.get(i));
            handshake(i, exec);
            final OutputStream outputStream = exec.getOutputStream();
            JsonUtil.write(outputStream, request);
            outputStream.close();
        }

        final List<State> states = new ArrayList<>();
        for (int i = 0; i < history.size(); i++) {
            states.add(new State(history.get(i), scores.get(i)));
        }
        System.out.println(objectMapper.writeValueAsString(new Scores(map, settings, ais.size(), futures, states, pScores, names)));
    }

    private void handshake(final int punterId, final Process exec) throws IOException {
        final OutputStream outputStream = exec.getOutputStream();
        final InputStream inputStream = exec.getInputStream();

        try {
            final HandshakeRequest request = JsonUtil.read(inputStream, HandshakeRequest.class);
            names.set(punterId, request.me);
            final HandshakeResponse response = new HandshakeResponse(request.me);
            JsonUtil.write(outputStream, response);
        } catch (final Exception e) {
            System.err.println("ERROR");
            final InputStream errorStream = exec.getErrorStream();
            final Scanner scanner = new Scanner(errorStream);
            while (scanner.hasNextLine()) {
                System.err.println(scanner.nextLine());
            }
        }
    }

    private void setTimeout(final Process exec, final int waitSecond) {
        new Thread(() -> {
            try {
                long l = System.currentTimeMillis();
                exec.waitFor(waitSecond, TimeUnit.SECONDS);
                if (exec.isAlive()) {
                    System.err.println("Time out!!!");
                    exec.destroy();
                    throw new RuntimeException("Time out");
                } else {
                    long l2 = System.currentTimeMillis();
                    System.err.println("time: " + (l2-l));
                }
            } catch (InterruptedException e) {
                System.err.println("err");
            }
        }).start();
    }
}
