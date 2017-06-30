/**
 * Created by curi on 17-6-29.
 */
public class H264Decoder {

    static{
        System.loadLibrary("H264Decoder");
    }

    private long Decoder;

    public H264Decoder(){
        Decoder = nativeH264init();
    }

    public byte[] decode(byte[] frm,int len){
        return nativeH264decode(Decoder,frm,len);

    }

    private native long nativeH264init();

    private native byte[] nativeH264decode(long thiz,byte[] frm,int len);


}
