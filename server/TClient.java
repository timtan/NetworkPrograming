import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.net.Socket;

import com.google.protobuf.ByteString;
import com.trend.Packet;


public class TClient {
    public static final String host = "localhost";
    private Socket clientSocket;
    
//    public TClient() {
//        
//    }
    
    public void connect() throws Exception{
        BufferedInputStream in = null;
        BufferedOutputStream out = null;
        
        try {
            clientSocket = new Socket(host, TServer.SERVER_PORT);
            in = new BufferedInputStream(clientSocket.getInputStream());
            out = new BufferedOutputStream(clientSocket.getOutputStream());
            System.out.println("Connect to server on port " + clientSocket.getPort());
            
            Packet.Header.Builder headBuilder = Packet.Header.newBuilder();
            headBuilder.setFileName("test.txt");
            headBuilder.setFileSize(10);
            headBuilder.setDigest("xxxxx");
            headBuilder.build().writeDelimitedTo(out);
            out.flush();
            System.out.println("Send header message...");
                        
            Packet.Ack ack = Packet.Ack.parseDelimitedFrom(in);
            if (ack.getSuccess() && ack.getType() == Packet.Ack.AckType.HEADER) {
                System.out.println(String.format("Receive ack(%s) success from server", ack.getType().toString()));
                ByteString test = ByteString.EMPTY;
                Packet.Block.Builder blockBuilder = Packet.Block.newBuilder();
                blockBuilder.setSeqNum(0);
                blockBuilder.setDigest("xxxx");
                blockBuilder.setSize(0);
                blockBuilder.setContent(ByteString.EMPTY);
                blockBuilder.setEof(true);
                blockBuilder.build().writeDelimitedTo(out);
                out.flush();
                
                ack = Packet.Ack.parseDelimitedFrom(in);
                System.out.println(String.format("Receive ack(%s) %s from server", ack.getType(), ack.getSuccess()? "success" : "failed"));
                
//                ack = Packet.Ack.parseDelimitedFrom(in);
//                System.out.println(String.format("Receive ack(%s) success from server", ack.getType()));
            }          
        }
        catch (Exception e) {
            e.printStackTrace();
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
