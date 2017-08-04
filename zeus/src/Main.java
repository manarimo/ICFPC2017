import game.GameServer;
import json.game.Map;
import org.codehaus.jackson.map.DeserializationConfig;
import org.codehaus.jackson.map.ObjectMapper;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class Main {
    public static void main(final String[] args) throws IOException {
        // args[0]: map file path
        // args[1]: # players
        // args[2..]: ai execs
        if (args.length < 2) {
            System.err.println("sim [map file] [# players] ai1 ai2 ...");
            return;
        }
        final String mapFilePath = args[0];
        final int numPlayer = Integer.valueOf(args[1]);
        if (args.length < numPlayer + 2) {
            System.err.println("sim [map file] [# players] ai1 ai2 ...");
            return;
        }
        final List<String> ais = new ArrayList<>();
        for (int i = 0; i < numPlayer; i++) {
            ais.add(args[i+2]);
        }

        final ObjectMapper objectMapper = new ObjectMapper();
        objectMapper.configure(DeserializationConfig.Feature.FAIL_ON_UNKNOWN_PROPERTIES,false);

        final Map map = objectMapper.readValue(new File(args[0]), Map.class);
        final GameServer gameServer = new GameServer(map, ais);
        gameServer.run();
    }
}
