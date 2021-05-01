/**
 *
 */
public class CommandParser {

    /**
     *
     */
    private final Client CLIENT;

    public CommandParser(Client client) {
        this.CLIENT = client;
    }

    /**
     * @param command
     * @return
     */
    public String parseCommand(String command) {
        if (command.contains("clear")) {
            parseClearCommand();
            return null;
        }

        return command;
    }

    /**
     *
     */
    private void parseClearCommand() {
        ReadThread readThread = this.CLIENT.getReadThread();
        readThread.getMessageField().clearField();
    }
}
