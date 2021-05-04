import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.border.TitledBorder;
import java.awt.*;
import java.util.TreeSet;

public class ClientRoomListPane extends JPanel {

    /**
     *
     */
    private static final String PANE_TITLE = "Available Rooms";

    /**
     *
     */
    private final JScrollPane CONNECTED_USERS_SCROLL_PANE;

    /**
     *
     */
    private final JLabel TITLE_LABEL;

    public ClientRoomListPane() {
        this.CONNECTED_USERS_SCROLL_PANE = new JScrollPane();
        this.CONNECTED_USERS_SCROLL_PANE.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        this.CONNECTED_USERS_SCROLL_PANE.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
        this.TITLE_LABEL = new JLabel(PANE_TITLE);
        this.TITLE_LABEL.setHorizontalAlignment(JLabel.CENTER);

        super.setLayout(new BorderLayout());
        super.setOpaque(true);
        super.add(this.TITLE_LABEL, BorderLayout.NORTH);
        super.add(this.CONNECTED_USERS_SCROLL_PANE, BorderLayout.CENTER);
    }
}
