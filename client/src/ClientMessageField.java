import javax.swing.JEditorPane;
import javax.swing.JScrollPane;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;

public class ClientMessageField {
    
    /**
     * 
     */
    private JScrollPane scrollPane;
    
    /**
     * 
     */
    private JEditorPane messageField;
    
    public ClientMessageField() {
        this.messageField = new JEditorPane();
        this.scrollPane = new JScrollPane(this.messageField, JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED,
                JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        this.addTextChangeEvent();
    }

    /**
     *
     * @param s
     */
    public void appendString(String s) {
        try {
           Document doc = this.messageField.getDocument();
           doc.insertString(doc.getLength(), s, null);
        } catch(BadLocationException exc) {
           exc.printStackTrace();
        }
    }

    /**
     *
     */
    public void clearField() {
        this.messageField.setText("");
    }
    
    /**
     * 
     */
    private void addTextChangeEvent() {
        this.messageField.getDocument().addDocumentListener(new DocumentListener() {
            @Override
            public void insertUpdate(DocumentEvent e) {
                messageField.setCaretPosition(messageField.getText().length());
            }
            
            @Override
            public void changedUpdate(DocumentEvent e) {
            }

            @Override
            public void removeUpdate(DocumentEvent e) {
            }
        });
    }
    
    public JScrollPane getScrollPane() {
        return this.scrollPane;
    }
    
    public JEditorPane getMessageField() {
        return this.messageField;
    }
}
