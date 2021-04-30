import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import javax.swing.JTextField;

public class ClientInputField extends JTextField {
    
    /**
     * Mutex to lock the write thread when the input isn't ready.
     */
    public BlockingQueue<String> inputQueue = new LinkedBlockingQueue<>();
    
    public ClientInputField() {
        this.addEnterKeyListener();
    }
    
    private void addEnterKeyListener() {
        super.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER) {
                    inputQueue.offer(getText());
                    setText("");
                }
            }
        });
    }

    public BlockingQueue getBlockingQueue() {
        return this.inputQueue;
    }
}
