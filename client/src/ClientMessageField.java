import javax.swing.*;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.text.BadLocationException;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyledDocument;
import java.awt.*;

/**
 *
 */
public class ClientMessageField {

    /**
     *
     */
    private final JScrollPane SCROLL_PANE;

    /**
     *
     */
    private final JTextPane MESSAGE_FIELD;

    public ClientMessageField() {
        this.MESSAGE_FIELD = new JTextPane();
        this.SCROLL_PANE = new JScrollPane(this.MESSAGE_FIELD, JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED,
                JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        this.addTextChangeEvent();
    }

    /**
     * @param s
     */
    public void appendString(String s, Object attributes) {
        try {
            StyledDocument doc = this.MESSAGE_FIELD.getStyledDocument();
            Style style = this.MESSAGE_FIELD.addStyle("", null);
            // Just for some silly testing.
            int r = (int) (Math.random() * 256);
            int g = (int) (Math.random() * 256);
            int b = (int) (Math.random() * 256);
            StyleConstants.setItalic(style, true);
            StyleConstants.setForeground(style, new Color(r, g, b));
            doc.insertString(doc.getLength(), s, style);
        } catch (BadLocationException exc) {
            exc.printStackTrace();
        }
    }

    /**
     *
     */
    public void clearField() {
        this.MESSAGE_FIELD.setText("");
    }

    /**
     *
     */
    private void addTextChangeEvent() {
        this.MESSAGE_FIELD.getStyledDocument().addDocumentListener(new DocumentListener() {
            @Override
            public void insertUpdate(DocumentEvent e) {
                MESSAGE_FIELD.setCaretPosition(MESSAGE_FIELD.getText().length());
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
        return this.SCROLL_PANE;
    }

    public JEditorPane getMessageField() {
        return this.MESSAGE_FIELD;
    }
}
