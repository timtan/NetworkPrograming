import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class TServer {
    
    public static final int SERVER_PORT = 2010;
    public static final int SERVER_TIMEOUT = 600000;
    private final int MAX_NUM_THREAD = 5;
    private ServerSocket srvSocket;
    
    
    private void bindServicePort(int tryCount) throws Exception{
        for(int i = 0; i < tryCount; i++) {
            try {
                srvSocket = new ServerSocket(SERVER_PORT);
                srvSocket.setSoTimeout(SERVER_TIMEOUT);
                System.out.println("Listening for connections on port " + srvSocket.getLocalPort() + " ...");
                return;
            }
            catch(Exception e) {
                System.out.println(e.getMessage());
            }
        }
        throw new Exception(String.format("unable to bind service port(%d)", SERVER_PORT));
    }
    
    
    public TServer() throws Exception{
        bindServicePort(5);
        
        for (int i = 0; i < MAX_NUM_THREAD; i++) {
            Thread t = new Thread(new RequestProcessor());
            t.start();
        }
    }
    
    public void waitRequest() throws IOException {
        while (true) {
            Socket clientSocket = null;                      
            clientSocket = srvSocket.accept();
            RequestProcessor.processRequest(clientSocket);
        }
    }
    /**
     * @param args
     */
    public static void main(String[] args) {
        try {
            new TServer().waitRequest();
        }
        catch (Exception e) {
            System.out.println(String.format("Fatal:%s", e.getMessage()));
        }
        System.out.println("Server is terminated");
    }
}
