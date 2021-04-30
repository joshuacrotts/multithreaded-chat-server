import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.Socket;

import javax.swing.JTextArea;

public class ReadThread implements Runnable {

    /**
     * 
     */
    private Thread thread;
    
    /**
     * 
     */
    private BufferedReader readfp;
    
    /**
     * 
     */
    private Socket s;

    /**
     * 
     */
    private ClientMessageField messageField;

    public ReadThread(Socket s, ClientMessageField messageField) {
        this.s = s;
        this.messageField = messageField;
        try {
            this.readfp = new BufferedReader(
                    new InputStreamReader(s.getInputStream()));
        } catch (IOException e) {
            e.printStackTrace();
        }
        this.thread = new Thread(this);
        this.thread.start();
    }

    @Override
    public void run() {
        while (!this.s.isClosed()) {
            String line = "";
            try {
                // Block while there's no data. Implicit block
                // by BufferedReader.
                while ((line = this.readfp.readLine()) != null) {
                    this.messageField.appendString(line + "\n");
                }
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }
    }
}
