import javax.swing.*;
import java.awt.*;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Vector;
import java.util.concurrent.BlockingDeque;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingDeque;
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
    private final Vector<String> HISTORY;

    /**
     *
     */
    private JTextFieldAutoComplete autocomplete;

    /**
     *
     */
    private int historyIndex = -1;

    public ClientInputField(ClientFrame frame) {
        this.INPUT_QUEUE = new LinkedBlockingQueue<>();
        this.HISTORY = new Vector<>();
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
                // If they press enter, then submit the input to the queue.
                if (e.getKeyCode() == KeyEvent.VK_ENTER) {
                    INPUT_QUEUE.offer(getText());
                    if (INPUT_QUEUE.peek() != null) {
                        HISTORY.add(INPUT_QUEUE.peek());
                        historyIndex = HISTORY.size() - 1;
                    }
                    setText("");
                }
                // If they press up, scroll through the previous inputs.
                else if (e.getKeyCode() == KeyEvent.VK_UP && (historyIndex - 1) >= 0) {
                    setText(HISTORY.get(historyIndex));
                    historyIndex--;
                }
                // If they press the down button, scroll through the NEXT inputs, given that they exist.
                else if (e.getKeyCode() == KeyEvent.VK_DOWN && (historyIndex + 1) < HISTORY.size()) {
                    setText(HISTORY.get(++historyIndex));
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
