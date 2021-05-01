import javax.swing.*;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

/**
 *
 */
public class ClientInputField extends JTextField {

    /**
     * "Mutex" to lock the write thread when the input isn't ready.
     */
    public final BlockingQueue<String> INPUT_QUEUE;

    public ClientInputField() {
        this.INPUT_QUEUE = new LinkedBlockingQueue<>();
        this.addEnterKeyListener();
    }

    private void addEnterKeyListener() {
        super.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER) {
                    INPUT_QUEUE.offer(getText());
                    setText("");
                }
            }
        });
    }

    public BlockingQueue getBlockingQueue() {
        return this.INPUT_QUEUE;
    }
}
