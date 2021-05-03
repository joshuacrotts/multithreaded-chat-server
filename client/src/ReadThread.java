import javax.swing.text.Style;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Arrays;
import java.util.StringTokenizer;

/**
 *
 */
public class ReadThread implements Runnable {

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
    private final ClientMessageField MESSAGE_FIELD;

    /**
     *
     */
    private BufferedReader readfp;

    public ReadThread(Client client, ClientMessageField messageField) {
        this.CLIENT = client;
        this.MESSAGE_FIELD = messageField;
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
                    System.out.println("Line: " + line);
                    this.MESSAGE_FIELD.appendString(line);
                }
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }
    }

    public ClientMessageField getMessageField() {
        return this.MESSAGE_FIELD;
    }
}
