import java.awt.BorderLayout;

import javax.swing.JPanel;

public class ClientPanel extends JPanel {

    /**
     * 
     */
    private ClientInputField inputField;
    
    /**
     * 
     */
    private ClientMessageField messageField;
    
    public ClientPanel() {
        this.inputField = new ClientInputField();
        this.messageField = new ClientMessageField();
        super.setLayout(new BorderLayout());
        super.add(this.inputField, BorderLayout.SOUTH);
        super.add(this.messageField.getScrollPane(), BorderLayout.CENTER);
    }
    
    public ClientInputField getInputField() {
        return this.inputField;
    }
    
    public ClientMessageField getMessageField() {
        return this.messageField;
    }
}
