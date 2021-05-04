import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 *
 */
public class ReadThread implements Runnable {

    /**
     *
     */
    private static final Pattern REGEX = Pattern.compile
            ("(?<msgType>-?\\d+),(?<textFlag>-?\\d+),(?<color>-?\\d+),(?<msg>.*)", Pattern.MULTILINE);

    /**
     *
     */
    private final Client CLIENT;

    /**
     *
     */
    private final ClientMessageField MESSAGE_FIELD;

    /**
     *
     */
    private final ConnectedUsersPane CONNECTED_USERS_PANE;

    /**
     *
     */
    private final ClientRoomListPane ROOM_LIST_PANE;

    /**
     *
     */
    private final ClientFriendListPane FRIEND_LIST_PANE;

    /**
     *
     */
    private final Thread THREAD;

    /**
     *
     */
    private BufferedReader readfp;

    public ReadThread(Client client, ClientPane clientPane) {
        this.CLIENT = client;
        this.MESSAGE_FIELD = clientPane.getMessageField();
        this.CONNECTED_USERS_PANE = clientPane.getConnectedUsersPane();
        this.ROOM_LIST_PANE = clientPane.getRoomListPane();
        this.FRIEND_LIST_PANE = clientPane.getFriendListPane();
        try {
            this.readfp = new BufferedReader(
                    new InputStreamReader(this.CLIENT.getSocket().getInputStream()));
        } catch (IOException e) {
            e.printStackTrace();
        }
        this.THREAD = new Thread(this);
        this.THREAD.start();
    }

    @Override
    public void run() {
        while (!this.CLIENT.getSocket().isClosed()) {
            String line = "";
            try {
                // Block while there's no data. Implicit block by BufferedReader.
                while ((line = this.readfp.readLine()) != null) {
                    this.parseLine(line);
                }
            } catch (IOException ex) {
                if (ex.getMessage().equals("Socket closed")) {
                    System.out.println("Client disconnected.");
                } else {
                    ex.printStackTrace();
                }
            }
        }
    }

    /**
     * @param line
     */
    private void parseLine(String line) {
        Matcher data = REGEX.matcher(line);
        int msgType = 0;
        int textFlag = 0;
        int colorFlag = 0;
        String message = "";
        if (data.find()) {
            msgType = Integer.parseInt(data.group("msgType"));
            textFlag = Integer.parseInt(data.group("textFlag"));
            colorFlag = Integer.parseInt(data.group("color"));
            message = data.group("msg");
        }

        // If the msg type is 0, that means it's a message to the client's "backend"
        // rather than something that needs to be displayed.
        if (msgType < 3) {
            // 0 is the initial connection flag. We have to tokenize the string.
            if (msgType == 0) {
                String[] users = message.split(",");
                for (String client : users) {
                    this.CONNECTED_USERS_PANE.addClient(client);
                }
            }
            // 1 means a new user connected.
            else if (msgType == 1) {
                String client = message.split("connected.")[0];
                this.CONNECTED_USERS_PANE.addClient(client);
            }
            // 2 means a user disconnected.
            else if (msgType == 2) {
                System.out.println("Someone else disconnected!");
            }
        } else {
            this.MESSAGE_FIELD.appendString(message, textFlag, colorFlag);
        }
    }

    public ClientMessageField getMessageField() {
        return this.MESSAGE_FIELD;
    }
}
