//
// Copyright (c) 2000 by Tech Soft America, LLC.
// The information contained herein is confidential and proprietary to
// Tech Soft America, LLC., and considered a trade secret as defined under
// civil and criminal statutes.  Tech Soft America shall pursue its civil
// and criminal remedies in the event of unauthorized use or misappropriation
// of its trade secrets.  Use of this information by anyone other than
// authorized employees of Tech Soft America, LLC. is granted only under a
// written non-disclosure agreement, expressly prescribing the scope and
// manner of such use.
//
// This header file is a completely self-contained class that defines all of the
// member functions that it needs inside the declaration.  It is just a utility class
// to stuff unaligned data values into bytes.
//

#ifndef _B_BYTE_STREAM_H_
#define _B_BYTE_STREAM_H_


#ifndef STATUS_ERROR
#  define STATUS_ERROR 0
#endif
#ifndef STATUS_NORMAL
#  define STATUS_NORMAL 1
#endif




class BByteStream {
  private:
    int byteplace;	    
    int bitplace;	    
    int bitsperval;	    
    int allocated;	    
    unsigned char *data;    
    unsigned short mask;    
  public:
    BByteStream(int size, unsigned char *cptr, int bpv) {
        if( bpv == 9 || bpv == 10 || bpv == 12 ) {
            allocated = size;
            data = cptr; 
            bitsperval = bpv;
            byteplace = 0;
            bitplace = 16 - bitsperval; 
            mask = (unsigned short)((1<<bitsperval)-1);
        }
        else {
            
            data = 0;
        }
    };
    BByteStream() { data = NULL; };
    
    void put(unsigned short value) alter {
        data[byteplace] |= (value >> (8 - bitplace));
        data[byteplace+1] |= (value << bitplace);
        bitplace -= bitsperval - 8;
        if( bitplace < 0 ) {
            byteplace += 2; 
            bitplace += 8;
        }
        else
            byteplace++; 
    };	    
    
    void get(unsigned short alter &value) {
        value = (unsigned short)((data[byteplace] << 8) | (data[byteplace + 1]));
        value = (unsigned short)((value >> bitplace) & mask );
        bitplace -= bitsperval - 8;
        if( bitplace < 0 ) {
            byteplace += 2; 
            bitplace += 8;
        }
        else
            byteplace++; 
    }; 

}; 




class BVarStream {
  private:
    unsigned int *m_data;   
    int m_allocated;        
    int m_used;             
    int m_bit;              
    int m_rused;            
    int m_rbit;             
    int m_can_reallocate;   
    int m_status;           
    unsigned int m_mask[33];  
    unsigned int m_range[33]; 


    
    void Reset() {
        int i;
        m_data = 0;
        m_allocated = 0;
        m_used = 0;
        m_bit = 0;
        m_rused = 0;
        m_rbit = 0;
        m_can_reallocate = 0;
        m_status = STATUS_NORMAL;
        m_mask[0] = 0;
        m_range[0] = 0;
        for( i = 1 ; i <= 32 ; i++ ) {
            m_mask[i] = ((unsigned int)(-1)) >> (32-i);
            m_range[i] = (0x1 << (i-1)) - 1;
        }
    };

    
    #ifdef STREAM_BIGENDIAN
        #ifndef SWAP32
            #define SWAP32(i) (\
                (((i) >> 24) & 0x000000ff) |\
                (((i) & 0x00ff0000) >> 8) |\
                (((i) & 0x0000ff00) << 8) |\
                ((i) << 24) \
                )
            #endif
    void SwapBytes( ) 
    {
        int i;
        for( i = 0 ; i < m_allocated ; i++ )
            m_data[i] = SWAP32( m_data[i] );
    };
#   else
    inline void SwapBytes() {};
#   endif

    
    void Reallocate( )
    {

        if( m_can_reallocate ) {
            unsigned int *temp;
            m_allocated *= 2;
            temp = new unsigned int[ m_allocated ];
            if( temp == NULL ) {
                
                m_status = STATUS_ERROR;
                m_used = 0; 
            }
            else {
                memcpy( temp, m_data, (m_used+1) * sizeof( unsigned int ) );
                delete [] m_data;
                m_data = temp;
            }
        }
        else {
            
            m_status = STATUS_ERROR;
            m_used = 0;
        }
    };

    
    void Put2( int numbits, int val ) 
    {
        if( m_bit + numbits <= 32 ) {
            m_data[m_used] |= val << (32 - m_bit - numbits);
            m_bit += numbits;
        }
        else {
            int shift = (numbits + m_bit - 32);

            if( m_used + 1 >= m_allocated )
                Reallocate( );
            m_data[m_used++] |= val >> shift;
            m_data[m_used] = val << (32-shift);
            m_bit += numbits - 32;
        }
    };

    
    void Get2( int numbits, int *val ) 
    {
        if( m_rbit + numbits <= 32 ) {
            *val = (m_data[m_rused] >> (32 - m_rbit - numbits)) & m_mask[numbits];
            m_rbit += numbits;
        }
        else {
            int shift = (numbits + m_rbit - 32);
            *val = (m_data[m_rused++] << shift) & m_mask[numbits];
            *val |= m_data[m_rused] >> (32-shift);
            m_rbit += numbits - 32;
        }
    };


  public:

    BVarStream() {
        m_can_reallocate = 0;
        m_data = NULL;
    };

    ~BVarStream( )
    {
        if( m_data != NULL ) {
            if( m_can_reallocate )
                delete [] m_data;
            m_data = NULL;
        }
    };

    
    void InitWrite( int size, void *pointer ) {
        Reset();
        m_allocated = size/4;
        m_data = (unsigned int *) pointer;
        m_data[0] = 0;
        m_can_reallocate = 1;
    };

    
    void InitRead( int size, const void *pointer )
    {
        Reset();
        m_allocated = size/4;
        m_data = (unsigned int *) pointer;
        m_can_reallocate = 0;
    };

    
    int Put( int *numbits_array, int val )
    {
        int temprange = 0, i;

        i = 0;
        for(;;){
            temprange = m_range[ numbits_array[i] ];
            if( val < -temprange || val > temprange ) {
                
                Put2( numbits_array[i], m_mask[ numbits_array[i] ] );
                i++;
            }
            else {
                
                Put2( numbits_array[i], val+temprange );
                break;
            }
        }
        return m_status;
    };

    
    int Get( int *numbits_array ) 
    {
        int i = 0;
        int val;

        for(;;) {
            Get2( numbits_array[i], &val );
            if( val == (int)m_mask[ numbits_array[i] ] )
                i++;
            else
                break;
        }
        val -= m_range[ numbits_array[i] ];
        return val;
    };

}; 





class BPack {
  private:
    unsigned int *m_data; 
    int m_allocated;	
    int m_used;		
    int m_bit;		
    int m_rused;		
    int m_rbit;		
    int m_can_reallocate; 
    int m_status;		
    unsigned int m_mask[33]; 
    unsigned int m_range[33]; 


    
    void Reset() {
        int i;
        m_data = 0;
        m_allocated = 0;
        m_used = 0;
        m_bit = 0;
        m_rused = 0;
        m_rbit = 0;
        m_can_reallocate = 0;
        m_status = STATUS_NORMAL;
        m_mask[0] = 0;
        m_range[0] = 0;
        for( i = 1 ; i <= 32 ; i++ ) {
            m_mask[i] = ((unsigned int)(-1)) >> (32-i);
            m_range[i] = (0x1 << (i-1)) - 1;
        }
    };

    
    void Reallocate( )
    {

        if( m_can_reallocate ) {
            unsigned int *temp;
            m_allocated *= 2;
            temp = new unsigned int[ m_allocated ];
            if( temp == NULL ) {
                
                m_status = STATUS_ERROR;
                m_used = 0; 
            }
            else {
                memcpy( temp, m_data, (m_used+1) * sizeof( unsigned int ) );
                delete [] m_data;
                m_data = temp;
            }
        }
        else {
            
            m_status = STATUS_ERROR;
            m_used = 0;
        }
    };



  public:

    BPack() { 
        m_can_reallocate = 0;
        m_data = NULL;
    };

    ~BPack( ) {
        if( m_data != NULL ) {
            if( m_can_reallocate )
                delete [] m_data;
            m_data = NULL;
        }
    };

    
    void InitWrite( int size, void *pointer ) {
        Reset();
        m_allocated = size/4;
        m_data = (unsigned int *) pointer;
        if (m_data) m_data[0] = 0;
        m_can_reallocate = 0;
    };

    
    void InitRead( int size, const void *pointer )
    {
        Reset();
        m_allocated = size/4;
        m_data = (unsigned int *) pointer;
        m_can_reallocate = 0;
    };

    void Put( int numbits, int val ) 
    {
        if( m_bit + numbits <= 32 ) {
            m_data[m_used] |= val << (32 - m_bit - numbits);
            m_bit += numbits;
        }
        else {
            int shift = (numbits + m_bit - 32);

            if( m_used + 1 >= m_allocated )
                Reallocate( );
            m_data[m_used++] |= val >> shift;
            m_data[m_used] = val << (32-shift);
            m_bit += numbits - 32;
        }
    };

    int Get( int numbits ) 
    {
        int return_val;

        if( m_rbit + numbits <= 32 ) {
            return_val = (m_data[m_rused] >> (32 - m_rbit - numbits)) & m_mask[numbits];
            m_rbit += numbits;
        }
        else {
            int shift = (numbits + m_rbit - 32);
            return_val = (m_data[m_rused++] << shift) & m_mask[numbits];
            return_val |= m_data[m_rused] >> (32-shift);
            m_rbit += numbits - 32;
        }
        return return_val;
    };

    int NumBytes() const { return (m_used+(m_bit?1:0)) * 4; };
    void SetCanReallocate(int val) alter { m_can_reallocate = val; };
    int GetStatus() const { return m_status; };

    
    #ifdef STREAM_BIGENDIAN
	void SwapBytes( ) 
	{
	    int i;
	    for( i = 0 ; i < m_allocated ; i++ )
		m_data[i] = SWAP32( m_data[i] );
	};
    #else
	inline void SwapBytes() {};
    #endif

}; 


#endif 

