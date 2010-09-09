import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import com.trend.Packet;
import com.trend.Packet.Header;

public class TServer {
    
    public static final int SERVER_PORT = 2010;
    public static final int SERVER_TIMEOUT = 30000;
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
    }
    
    public void waitRequest() throws IOException {
        while (true) {
            Socket clientSocket = null;
            BufferedInputStream in = null;
            BufferedOutputStream out = null;
            
            clientSocket = srvSocket.accept();
            
            try {
                in = new BufferedInputStream(clientSocket.getInputStream());
                out = new BufferedOutputStream(clientSocket.getOutputStream());
                
                Packet.Header.Builder headerBuilder = Packet.Header.newBuilder();
                headerBuilder.mergeDelimitedFrom(in);
                Packet.Header header = headerBuilder.build();
                System.out.println(String.format("Get a new Header:%s, size:%d",header.getFileName(), header.getFileSize()));
                                
                Packet.Ack.Builder ackBuilder = Packet.Ack.newBuilder();
                ackBuilder.setType(Packet.Ack.AckType.HEADER);
                ackBuilder.setSuccess(true);
                out.write(ackBuilder.build().toByteArray());
                out.flush();
                clientSocket.close();
                System.out.println("Accept a client from "+clientSocket.getLocalAddress().toString());
                //Header header = Header.parseFrom(in);                                
            }
            catch (IOException e) {
                // Not to handle
                System.out.println(e.getMessage());
            }
            finally {
                try {
                    if (in != null) in.close();
                    if (out != null) out.close();
                    if (clientSocket != null) clientSocket.close();                
                }
                catch (Exception e) {
                    // Todo:
                }
            }            
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
