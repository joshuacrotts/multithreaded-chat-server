import java.awt.Dimension;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.JFrame;
import javax.swing.JOptionPane;

public class ClientFrame extends JFrame {

    /**
     * 
     */
    private final ClientPanel CLIENT_PANEL;

    /**
     * 
     */
    private final Client CLIENT;

    public ClientFrame() {
        this.CLIENT_PANEL = new ClientPanel();
        this.CLIENT = new Client(this);
        this.addWindowCloseListener();
        super.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
        super.setTitle("Multithreaded Chat Server");
        super.setVisible(true);
        super.setMinimumSize(new Dimension(640, 480));
        super.setLocationRelativeTo(null);
        super.add(this.CLIENT_PANEL);
        super.pack();
    }

    /**
     * 
     */
    public void addWindowCloseListener() {
        super.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent evt) {
                int confirm = JOptionPane.showOptionDialog(null,
                        "Are you sure you want to close the application?",
                        "Exit Confirmation", JOptionPane.YES_NO_OPTION,
                        JOptionPane.QUESTION_MESSAGE, null, null, null);
                if (confirm == 0) {
                    CLIENT.sendCloseRequest();
                    System.exit(0);
                }
            }
        });
    }

    public ClientPanel getClientPanel() {
        return this.CLIENT_PANEL;
    }
}
