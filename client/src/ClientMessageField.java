import javax.swing.*;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.text.BadLocationException;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyledDocument;
import java.awt.*;
import java.util.Arrays;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

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
        this.MESSAGE_FIELD.setEditable(false);
        this.addTextChangeEvent();
    }

    /**
     * @param s
     */
    public void appendString(String s) {
        try {
            StyledDocument doc = this.MESSAGE_FIELD.getStyledDocument();
            ClientTextAttributes attributes = new ClientTextAttributes(s, this.MESSAGE_FIELD);
            doc.insertString(doc.getLength(), attributes.MESSAGE + "\n", attributes.STYLE);
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
                MESSAGE_FIELD.setCaretPosition(MESSAGE_FIELD.getStyledDocument().getLength());
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
    private static class ClientTextAttributes {

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
        private static final Pattern REGEX = Pattern.compile
                ("(?<msgType>-?\\d+),(?<textFlag>-?\\d+),(?<color>-?\\d+),(?<msg>.*)", Pattern.MULTILINE);

        /**
         *
         */
        private final Style STYLE;

        /**
         *
         */
        private final String MESSAGE;

        public ClientTextAttributes(String rawString, JTextPane messageField) {
            Matcher data = REGEX.matcher(rawString);
            this.STYLE = messageField.addStyle("", null);
            int msgType = 0;
            int textFlag = 0;
            int colorFlag = 0;

            // If we have more than one arg, then we know there's a color and style element set.
            if (data.find()) {
                msgType = Integer.parseInt(data.group("msgType"));
                textFlag = Integer.parseInt(data.group("textFlag"));
                colorFlag = Integer.parseInt(data.group("color"));
                this.MESSAGE = data.group("msg");

                if ((textFlag & ITALIC_FLAG) != 0) {
                    StyleConstants.setItalic(this.STYLE, true);
                }
                if ((textFlag & BOLD_FLAG) != 0) {
                    StyleConstants.setBold(this.STYLE, true);
                }

                StyleConstants.setForeground(this.STYLE, this.extractColor(colorFlag));
            } else {
                this.MESSAGE = rawString;
            }
        }

        private Color extractColor(int color) {
            return new Color(color >> 24 & 0xff, color >> 16 & 0xff, color >> 8 & 0xff, color & 0xff);
        }
    }
}
