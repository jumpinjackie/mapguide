
class RS_ByteData;



class AGGImageIO
{
public:

    static bool Save(const RS_String& filename, const RS_String& format, 
                     unsigned int* src, int src_width, int src_height,
                     int dst_width, int dst_height);

    static RS_ByteData* Save(const RS_String& format, 
                      unsigned int* src, int src_width, int src_height,
                      int dst_width, int dst_height);


    static void Combine(const RS_String& src1, const RS_String& src2, const RS_String& dst);

    static unsigned int* DecodePNG(const unsigned char* src, size_t len, int& width, int& height);
    static unsigned int* DecodeJPEG(const unsigned char* src, size_t len, int& width, int& height);
    static unsigned int* DecodeImage(const RS_String& filename, int& width, int& height);

private:

    static unsigned char* ReadFile(const RS_String& fname, size_t& len);
    static void UnmultiplyAlpha(unsigned int* argb, int len);
};