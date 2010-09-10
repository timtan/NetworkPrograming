import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.LinkedList;
import java.util.List;

import com.trend.Packet;


public class RequestProcessor implements Runnable {
    private static List<Socket> pool = new LinkedList<Socket>();
    private File localFile = null;
    private String filename ="";
    private long filesize = 0;
    private BufferedOutputStream fout = null;
    
    public static void processRequest(Socket request) {
        synchronized(pool) {
            pool.add(pool.size(), request);
            pool.notifyAll();
        }
    }
    
    private void writeResponse(boolean success, Packet.Ack.AckType type, BufferedOutputStream conOut) throws IOException {
        Packet.Ack.Builder ackBuilder = Packet.Ack.newBuilder();
        ackBuilder.setType(type);
        ackBuilder.setSuccess(success);
        Packet.Ack response = ackBuilder.build();
        response.writeDelimitedTo(conOut);
        conOut.flush();        
    }
    
    private Packet.Block getFileBlock(BufferedInputStream in ) throws IOException {
        Packet.Block.Builder blockBuilder = Packet.Block.newBuilder();
        blockBuilder.mergeDelimitedFrom(in);
        Packet.Block block = blockBuilder.build();
        
        return block;
    }
    
    
    @Override
    public void run() {
        while (true) {
            Socket connection;
            synchronized(pool) {
                while (pool.isEmpty()) {
                    try {
                        pool.wait();                        
                    }
                    catch (InterruptedException e) {                        
                    }
                }
                
                connection = pool.remove(0);
                System.out.println("Accept a client from "+connection.getInetAddress().toString());
            }                
            
            try {
                BufferedInputStream in = new BufferedInputStream(connection.getInputStream());
                BufferedOutputStream out = new BufferedOutputStream(connection.getOutputStream());
                
                if (fout == null) {
                    Packet.Header.Builder headerBuilder = Packet.Header.newBuilder();
                    headerBuilder.mergeDelimitedFrom(in);
                    Packet.Header header = headerBuilder.build();                    
                    fout = new BufferedOutputStream(new FileOutputStream("/tmp/"+header.getFileName()));
                    filesize = header.getFileSize();
                    writeResponse(true, Packet.Ack.AckType.HEADER, out);
                    System.out.println(String.format("Get a new Header:%s, size:%d",header.getFileName(), header.getFileSize()));                   
                }
                                
                if (fout != null) {
                    while (true) {
                        Packet.Block block = getFileBlock(in);
                        if (block.getEof()) break;
                        
                        byte[] content = block.getContent().toByteArray(); 
                        fout.write(content, 0, block.getSize());
                        
                        writeResponse(true, Packet.Ack.AckType.BLOCK, out);
                    }                    
                }
                
                writeResponse(true, Packet.Ack.AckType.EOF, out);
            }
            catch (IOException e) {
                // Not to handle
                System.out.println(e.getMessage());
            }
            finally {
                try {
                    if (connection != null) connection.close();
                    if (fout != null) {
                        fout.flush();
                        fout.close();
                        fout = null;
                    }
                }
                catch (Exception e) {
                    // Todo:
                }
            }
            
        } // end while
    } 

}
