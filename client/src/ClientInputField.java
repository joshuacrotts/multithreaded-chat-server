import javax.swing.*;
import java.awt.*;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

/**
 *
 */
public class ClientInputField extends JTextField {

    /**
     *
     */
    private static final List<String> COMMANDS = Arrays.asList("/clear", "/help", "/leave", "/login", "/msg");

    /**
     * "Mutex" to lock the write thread when the input isn't ready.
     */
    private final BlockingQueue<String> INPUT_QUEUE;

    /**
     *
     */
    private JTextFieldAutoComplete autocomplete;

    public ClientInputField(ClientFrame frame) {
        this.INPUT_QUEUE = new LinkedBlockingQueue<>();
        this.addEnterKeyListener();
        this.autocomplete = new JTextFieldAutoComplete(this, COMMANDS);
        this.setComponentPopupMenu(this.autocomplete);
    }

    /**
     *
     */
    private void addEnterKeyListener() {
        //
        this.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER) {
                    INPUT_QUEUE.offer(getText());
                    setText("");
                }
            }

            @Override
            public void keyReleased(KeyEvent e) {
                autocomplete.updateSuggestions(getText());
            }
        });
    }

    public BlockingQueue getBlockingQueue() {
        return this.INPUT_QUEUE;
    }
}
