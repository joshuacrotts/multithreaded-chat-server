import javax.swing.*;
import java.awt.*;

/**
 *
 */
public class ClientPanel extends JPanel {

    /**
     *
     */
    private final ClientInputField INPUT_FIELD;

    /**
     *
     */
    private final ClientMessageField MESSAGE_FIELD;

    public ClientPanel() {
        this.INPUT_FIELD = new ClientInputField();
        this.MESSAGE_FIELD = new ClientMessageField();
        super.setLayout(new BorderLayout());
        super.add(this.INPUT_FIELD, BorderLayout.SOUTH);
        super.add(this.MESSAGE_FIELD.getScrollPane(), BorderLayout.CENTER);
    }

    public ClientInputField getInputField() {
        return this.INPUT_FIELD;
    }

    public ClientMessageField getMessageField() {
        return this.MESSAGE_FIELD;
    }
}
