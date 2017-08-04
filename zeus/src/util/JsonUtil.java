package util;

import org.codehaus.jackson.map.ObjectMapper;

import java.io.*;
import java.util.Scanner;

public class JsonUtil {
    private static final ObjectMapper objectMapper = new ObjectMapper();

    public static void write(final OutputStream outputStream, final Object object) throws IOException {
        final OutputStreamWriter writer = new OutputStreamWriter(outputStream);
        final String json = objectMapper.writeValueAsString(object);
        writer.write(String.valueOf(json.length()));
        writer.write(":");
        writer.write(json);
        writer.flush();
    }

    public static <T> T read(final InputStream inputStream, final Class<T> classT) throws IOException {
        final Scanner scanner = new Scanner(inputStream);
        final String input = scanner.nextLine();
        final String json = input.substring(input.indexOf(":") + 1);
        return objectMapper.readValue(json, classT);
    }
}
