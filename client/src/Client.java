import javax.swing.*;
import java.io.IOException;
import java.net.Socket;

public class Client {

    /**
     * IP address of the server.
     */
    private static final String IP = "143.198.113.42";
    
    /**
     * Port for the server.
     */
    private static final int PORT = 20158;
    
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
     * Socket created for the client to connect to.
     */
    private Socket socket;
    
    public Client(ClientFrame frame) {
        try {
            this.socket = new Socket(Client.IP, Client.PORT);
        } catch (IOException e) {
            // TODO Auto-generated catch block
            JOptionPane.showMessageDialog(frame, "Connection refused.", "Error", JOptionPane.ERROR_MESSAGE);
            e.printStackTrace();
        }
        
        this.FRAME = frame;
        this.READ_THREAD = new ReadThread(this.socket, this.FRAME.getClientPanel().getMessageField());
        this.WRITE_THREAD = new WriteThread(this.socket, this.FRAME.getClientPanel().getInputField());
    }
    
    public void sendCloseRequest() {
        this.WRITE_THREAD.close();
    }
}

