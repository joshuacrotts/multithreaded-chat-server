import javax.swing.*;
import java.awt.*;

/**
 *
 */
public class ClientPane extends JPanel {

    /**
     *
     */
    private final ConnectedUsersPane CONNECTED_USERS_PANE;

    /**
     *
     */
    private final ClientFriendListPane CLIENT_FRIEND_LIST_PANE;

    /**
     *
     */
    private final ClientRoomListPane CLIENT_ROOM_LIST_PANE;

    /**
     *
     */
    private final ClientInputField INPUT_FIELD;

    /**
     *
     */
    private final ClientMessageField MESSAGE_FIELD;

    public ClientPane() {
        this.INPUT_FIELD = new ClientInputField();
        this.MESSAGE_FIELD = new ClientMessageField();
        this.CLIENT_FRIEND_LIST_PANE = new ClientFriendListPane();
        this.CLIENT_ROOM_LIST_PANE = new ClientRoomListPane();
        this.CONNECTED_USERS_PANE = new ConnectedUsersPane();
        super.setLayout(new BorderLayout());

        // Set the layout for the super-parent (Split between main chat view and side panel).
        JSplitPane messageBoxAndRightPane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT);
        messageBoxAndRightPane.setResizeWeight(0.7);
        messageBoxAndRightPane.setEnabled(false);
        messageBoxAndRightPane.setDividerSize(1);

        // Now set the layout for the parent of the room and field panes.
        JSplitPane rightPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT);
        rightPane.setResizeWeight(0.5);
        rightPane.setEnabled(false);
        rightPane.setDividerSize(1);

        // Now set the layout for the rooms and friends list.
        JSplitPane roomsFriendsPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT);
        roomsFriendsPane.setResizeWeight(0.5);
        roomsFriendsPane.setEnabled(false);
        roomsFriendsPane.setDividerSize(1);

        // Add the two subchildren to the roomsFriendsPane.
        roomsFriendsPane.add(this.CLIENT_ROOM_LIST_PANE);
        roomsFriendsPane.add(this.CLIENT_FRIEND_LIST_PANE);

        // Add the two children to the rhs parent.
        rightPane.add(this.CONNECTED_USERS_PANE);
        rightPane.add(roomsFriendsPane);

        // Add the two children to the super-parent.
        messageBoxAndRightPane.add(this.MESSAGE_FIELD.getScrollPane());
        messageBoxAndRightPane.add(rightPane);

        // Add the superParent to the center of the BorderLayout.
        super.add(messageBoxAndRightPane, BorderLayout.CENTER);
        super.add(this.INPUT_FIELD, BorderLayout.SOUTH);
    }

    public ConnectedUsersPane getConnectedUsersPane() {
        return this.CONNECTED_USERS_PANE;
    }

    public ClientInputField getInputField() {
        return this.INPUT_FIELD;
    }

    public ClientMessageField getMessageField() {
        return this.MESSAGE_FIELD;
    }
}
