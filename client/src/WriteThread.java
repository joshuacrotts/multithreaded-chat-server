import java.io.IOException;
import java.io.PrintWriter;

/**
 *
 */
public class WriteThread implements Runnable {

    /**
     *
     */
    private final Client CLIENT;

    /**
     *
     */
    private final Thread THREAD;

    /**
     *
     */
    private final ClientInputField INPUT_FIELD;

    /**
     *
     */
    private PrintWriter writefp;

    public WriteThread(Client client, ClientInputField inputField) {
        this.CLIENT = client;
        this.INPUT_FIELD = inputField;

        // Create the stdin and writer file pointers.
        // The write fp corresponds to the read on the server.
        try {
            this.writefp = new PrintWriter(this.CLIENT.getSocket().getOutputStream(), true);
        } catch (IOException ex) {
            ex.printStackTrace();
        }

        this.THREAD = new Thread(this);
        this.THREAD.start();
    }

    @Override
    public void run() {
        while (!this.CLIENT.getSocket().isClosed()) {
            String line = "";
            // Block until we receive some input.
            try {
                line = (String) this.INPUT_FIELD.getBlockingQueue().take();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            // Parse the input to see if we are going to use a local command or not.
            if ((line = this.CLIENT.parseCommand(line)) != null) {
                this.writefp.write(line + "\n");
                this.writefp.flush();
            }
        }
    }

    /**
     *
     */
    public void close() {
        this.writefp.write("leave\nleave\n");
        this.writefp.flush();
        try {
            this.CLIENT.getSocket().close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
