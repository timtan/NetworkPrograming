import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.net.Socket;

import com.trend.Packet;


public class TClient {
    public static final String host = "localhost";
    private Socket clientSocket;
    
//    public TClient() {
//        
//    }
    
    public void connect() throws Exception{
        BufferedInputStream in = null;
        try {
            clientSocket = new Socket(host, TServer.SERVER_PORT);
            in = new BufferedInputStream(clientSocket.getInputStream());
            //BufferedOutputStream out = new BufferedOutputStream(clientSocket.getOutputStream());
            System.out.println("Connect to server on port " + clientSocket.getPort());
            
            Packet.Ack ack = Packet.Ack.parseFrom(in);
            System.out.println("Receive ack from server is "+ack.getSuccess());            
        }
        catch (Exception e) {
            
        }
        finally {
            if (in != null) in.close();
            //if (clientSocket != null) clientSocket.close();
        }
    }
    
    
    public static void main(String[] args) {
        try {
            new TClient().connect();
        }
        catch (Exception e) {
            System.out.println("Fatal:"+e.getMessage());
        }
    }
}
