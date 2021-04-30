import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

import javax.swing.JTextField;

public class WriteThread implements Runnable {

    /**
     * 
     */
    private Thread thread;

    /**
     * 
     */
    private Socket s;

    /**
     * 
     */
    private PrintWriter writefp;

    /**
     * 
     */
    private ClientInputField inputField;

    public WriteThread(Socket s, ClientInputField inputField) {
        this.s = s;
        this.inputField = inputField;
        
        // Create the stdin and writer file pointers.
        // The write fp corresponds to the read on the server.
        try {
            this.writefp = new PrintWriter(s.getOutputStream(), true);
        } catch (IOException ex) {
            ex.printStackTrace();
        }

        this.thread = new Thread(this);
        this.thread.start();
    }

    @Override
    public void run() {
        while (!this.s.isClosed()) {
            String line = "";
            // Block until we receive some input.
            try {
                line = (String) this.inputField.getBlockingQueue().take();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            this.writefp.write(line + "\n");
            this.writefp.flush();
            this.inputField.setReady(false);
        }
    }
    
    /**
     * 
     */
    public void close() {
        this.writefp.write("leave\nleave\n");
        this.writefp.flush();
        try {
            this.s.close();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
}
