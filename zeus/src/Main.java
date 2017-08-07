import game.GameServer;
import json.game.Map;
import json.game.Settings;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.GnuParser;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;
import org.codehaus.jackson.map.DeserializationConfig;
import org.codehaus.jackson.map.ObjectMapper;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class Main {
    public static void main(final String[] rawArgs) throws IOException, ParseException {
        // args[0]: map file path
        // args[1]: # players
        // args[2..]: ai execs
        final GnuParser parser = new GnuParser();
        final CommandLine parse = parser.parse(new ZeusOptions(), rawArgs);
        final String[] args = parse.getArgs();
        if (args.length < 2) {
            System.err.println("zeus [map file] [# players] (options) ai1 ai2 ...");
            return;
        }
        final String mapFilePath = args[0];
        final int numPlayer = Integer.valueOf(args[1]);
        if (args.length < numPlayer + 2) {
            System.err.println("zeus [map file] [# players] (options) ai1 ai2 ...");
            return;
        }

        for (int i = 0; i < args.length; i++) {
            System.err.println(String.format("args[%d] = %s", i, args[i]));
        }

        final List<String> ais = new ArrayList<>();
        for (int i = 0; i < numPlayer; i++) {
            ais.add(args[i+2]);
        }

        final Settings settings = new Settings();
        if (parse.hasOption("x1")) {
            settings.futures = true;
        }
        if (parse.hasOption("x2")) {
            settings.splurges = true;
        }
        if (parse.hasOption("x3")) {
            settings.options = true;
        }

        final ObjectMapper objectMapper = new ObjectMapper();
        objectMapper.configure(DeserializationConfig.Feature.FAIL_ON_UNKNOWN_PROPERTIES,false);

        final Map map = objectMapper.readValue(new File(mapFilePath), Map.class);
        final GameServer gameServer = new GameServer(map, ais, settings);
        gameServer.run();
    }

    public static class ZeusOptions extends Options {
        public ZeusOptions() {
            addOption("x1", false, "Futures 1.0");
            addOption("x2", false, "Splurges 1.0");
            addOption("x3", false, "Options 1.0");
        }
    }
}
