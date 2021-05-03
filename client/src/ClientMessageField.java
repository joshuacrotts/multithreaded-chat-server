import javax.swing.*;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.text.BadLocationException;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyledDocument;
import java.awt.*;
import java.util.Arrays;

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
    public void appendString(String s) {
        try {
            StyledDocument doc = this.MESSAGE_FIELD.getStyledDocument();
            ClientTextAttributes attributes = new ClientTextAttributes(s, this.MESSAGE_FIELD);
            doc.insertString(doc.getLength(), s, attributes.getStyle());
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

    /**
     *
     */
    private class ClientTextAttributes {

        /**
         *
         */
        private static final int ITALIC_FLAG = 1;

        /**
         *
         */
        private static final int BOLD_FLAG = 2;

        /**
         *
         */
        private final Style STYLE;

        public ClientTextAttributes(String rawString, JTextPane messageField) {
            String[] data = rawString.split("\\d,\\d\n\n.*");
            System.out.println(Arrays.toString(data));
            this.STYLE = messageField.addStyle("", null);
            int textFlag = 0;
            int colorFlag = 0;

            // If we have more than one arg, then we know there's a color and style element set.
            if (data.length > 1) {
                textFlag = Integer.parseInt(data[0]);
                colorFlag = Integer.parseInt(data[1]);

                if ((textFlag & ITALIC_FLAG) != 0) {
                    StyleConstants.setItalic(this.STYLE, true);
                }
                if ((textFlag & BOLD_FLAG) != 0) {
                    StyleConstants.setBold(this.STYLE, true);
                }

                StyleConstants.setForeground(this.STYLE, this.extractColor(colorFlag));
            }
        }

        private Color extractColor(int color) {
            return new Color(color >> 24 & 0xff, color >> 16 & 0xff, color >> 8 & 0xff, color & 0xff);
        }

        public Style getStyle() {
            return this.STYLE;
        }
    }
}
