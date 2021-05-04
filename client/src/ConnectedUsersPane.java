import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.border.TitledBorder;
import java.awt.*;
import java.util.TreeSet;

public class ConnectedUsersPane extends JPanel {

    /**
     *
     */
    private static final String PANE_TITLE = "Connected Users";

    /**
     *
     */
    private final JPanel CONNECTED_USERS_PANEL;

    /**
     *
     */
    private final JLabel TITLE_LABEL;

    /**
     *
     */
    private final TreeSet<JComparableLabel> CONNECTED_CLIENTS;

    public ConnectedUsersPane() {
        super.setLayout(new BorderLayout());
        super.setOpaque(true);

        // Create and set the layout for the connected users panel.
        this.CONNECTED_USERS_PANEL = new JPanel();
        this.CONNECTED_USERS_PANEL.setLayout(new BoxLayout(this.CONNECTED_USERS_PANEL, BoxLayout.PAGE_AXIS));
        this.TITLE_LABEL = new JLabel(PANE_TITLE);
        this.TITLE_LABEL.setHorizontalAlignment(JLabel.CENTER);
        this.CONNECTED_CLIENTS = new TreeSet<>();

        // Create the scroll pane.
        JScrollPane scrollPane = new JScrollPane(this.CONNECTED_USERS_PANEL);
        super.add(this.TITLE_LABEL, BorderLayout.NORTH);
        super.add(scrollPane, BorderLayout.CENTER);
    }

    /**
     *
     * @param client
     */
    public void addClient(String client) {
        JComparableLabel label = new JComparableLabel(client);
        this.CONNECTED_USERS_PANEL.add(label);
        this.CONNECTED_USERS_PANEL.revalidate();
        this.CONNECTED_USERS_PANEL.repaint();
    }

    /**
     * Not working yet...
     * @param client
     */
    public void removeClient(String client) {
        for (JComparableLabel label : this.CONNECTED_CLIENTS) {
            if (client.equals(label.getText())) {
                this.CONNECTED_CLIENTS.remove(label);
                break;
            }
        }
    }

    /**
     *
     */
    private static class JComparableLabel extends JLabel implements Comparable<JComparableLabel> {
        public JComparableLabel(String label) {
            super(label);
        }

        @Override
        public int compareTo(JComparableLabel other) {
            return this.getText().compareTo(other.getText());
        }
    }
}
