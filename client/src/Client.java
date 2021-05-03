import javax.swing.*;
import java.io.IOException;
import java.net.Socket;

/**
 *
 */
public class Client {

    /**
     * IP address of the server.
     */
    private static final String IP = "localhost";

    /**
     * Port for the server.
     */
    private static final int PORT = 8080;

    /**
     * JFrame UI for the client.
     */
    private final ClientFrame FRAME;

    /**
     * Read file pointer/thread handler. This thread corresponds
     * to the "write" on the server. This means that any data written
     * to the output file ptr on the server is read by the client.
     */
    private final ReadThread READ_THREAD;

    /**
     * Write file pointer/thread handler. This thread corresponds to
     * the "read" on the server. This means that any data that goes to
     * the input file handle is written through this thread in the client.
     */
    private final WriteThread WRITE_THREAD;

    /**
     *
     */
    private final CommandParser COMMAND_PARSER;

    /**
     * Socket created for the client to connect to.
     */
    private Socket socket;

    public Client(ClientFrame frame) {
        try {
            this.socket = new Socket(Client.IP, Client.PORT);
        } catch (IOException e) {
            JOptionPane.showMessageDialog(frame, e.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
            System.exit(1);
        }

        this.FRAME = frame;
        this.READ_THREAD = new ReadThread(this, this.FRAME.getClientPanel().getMessageField());
        this.WRITE_THREAD = new WriteThread(this, this.FRAME.getClientPanel().getInputField());
        this.COMMAND_PARSER = new CommandParser(this);
    }

    public void sendCloseRequest() {
        this.WRITE_THREAD.close();
    }

    public Socket getSocket() {
        return this.socket;
    }

    public WriteThread getWriteThread() {
        return this.WRITE_THREAD;
    }

    public ReadThread getReadThread() {
        return this.READ_THREAD;
    }

    public String parseCommand(String command) {
        return this.COMMAND_PARSER.parseCommand(command);
    }
}

