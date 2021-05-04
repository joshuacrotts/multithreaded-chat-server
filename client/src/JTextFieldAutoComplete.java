import javax.swing.*;
import java.util.List;

public class JTextFieldAutoComplete extends JPopupMenu {

    /**
     *
     */
    private final JTextField textfield;

    /**
     *
     */
    private final List<String> suggestions;

    public JTextFieldAutoComplete(JTextField textfield, List<String> suggestions) {
        this.textfield = textfield;
        this.suggestions = suggestions;
    }

    /**
     *
     * @param curr
     */
    public void updateSuggestions(String curr) {
        // Remove all of the suggestions no matter what.
        super.removeAll();
        super.setVisible(!curr.isEmpty());
        if (!curr.isEmpty()) {
            for (String suggestion : this.suggestions) {
                if (suggestion.startsWith(curr)) {
                    super.add(new JMenuItem(suggestion));
                }
            }

            // Revalidate and update the drop-down to correctly position
            // the menuitems.
            super.revalidate();
            super.repaint();
            super.pack();
            int x = this.textfield.getLocationOnScreen().x;
            int y = this.textfield.getLocationOnScreen().y + this.textfield.getHeight();
            super.setLocation(x, y);
        }
    }
}