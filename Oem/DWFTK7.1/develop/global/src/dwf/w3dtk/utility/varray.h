
#ifndef VARRAY_H
#define VARRAY_H

#include <stdlib.h>
#include <string.h>

#ifdef NEED_BOOL_TYPE
#undef NEED_BOOL_TYPE
# ifdef true
#   undef true
# endif
# ifdef false
#   undef false
# endif
    typedef int bool;

    const bool true = !0;
    const bool false = 0;
#endif

 
#ifdef __cplusplus
extern "C++" {

template <class T> class VArray {
	
public:
	
	VArray(unsigned long size=1) {
		m_Size = size;
		m_Data = new T[m_Size];
		m_Count = 0;
	};
	
	~VArray() {
		delete[] m_Data;
	}; 
	
 	inline unsigned long Count() const { return m_Count; }
 	inline unsigned long Size() const { return m_Size; }

	T& operator[](unsigned long i){
		EnsureSize(i+1);
		if(i >= m_Count) m_Count=i+1;
		return m_Data[i];
	};

	void Append(T const & item) {
		EnsureSize(m_Count+1);
		m_Data[m_Count++] = item;
	};

	void AppendArray(T const *item_array, unsigned long n_items) {
		if(!n_items) return;
		EnsureSize(n_items+m_Count);
		memcpy(&m_Data[m_Count], item_array, n_items*sizeof(T));
		m_Count+=n_items;
	};

 	T & GetData(unsigned long i) {
		return m_Data[i];
	};

 	T & Pop() {
		return m_Data[--m_Count];
	};

	void InsertAt(T const & item, unsigned long i) {

		if(i >= m_Count) {
			EnsureSize(i+1);
			m_Count=i+1;
		}else{
			EnsureSize(m_Count+1);
			memmove(&m_Data[i+1], &m_Data[i], (m_Count-i)*sizeof(T)); 
			m_Count++;
		}
		m_Data[i] = item;
	};

	bool RemoveAt(unsigned long i, T * old_item=0) {

		if(i>=m_Count) return false;
		m_Count--;
		if(old_item) *old_item = m_Data[i];
		if(i!=m_Count) memmove(&m_Data[i], &m_Data[i+1], (m_Count-i)*sizeof(T)); 
		return true;
	};

	bool RemoveAt(unsigned long i, T & old_item) {
		return RemoveAt(i,&old_item);
	};

 	void SetCount(unsigned long count) { 
		EnsureSize(count);
		m_Count=count;
	};


	bool ReplaceAt(T const & item, unsigned long i, T * old_item=0) {

		if(i>=m_Count){
			EnsureSize(i+1);
			m_Count=i+1;
			m_Data[i]=item;
			return false;
		}
		if(old_item) *old_item = m_Data[i];
		m_Data[i]=item;
		return true;
	};

	bool ReplaceAt(T const & item, unsigned long i, T & old_item) {
		return ReplaceAt(item,i,&old_item);
	};

	void MapFunction(void(*function)(T, void*), void * user_data) const{
		unsigned long i;
		for(i=0;i<m_Count;i++)
			(*function)(m_Data[i],user_data);
	};

	void MapFunction(void(*function)(T&, void*), void * user_data) {
		unsigned long i;
		for(i=0;i<m_Count;i++)
			(*function)(m_Data[i],user_data);
	};

	void MapFunction(void(*function)(T const &, void*), void * user_data) const{
		unsigned long i;
		for(i=0;i<m_Count;i++)
			(*function)(m_Data[i],user_data);
	};

	void TrimSize() {

		m_Size=m_Count;
		if(!m_Size) m_Size++;
		T * new_data = new T[m_Size];
		if(m_Count) memcpy(new_data,m_Data,m_Count*sizeof(T));
		delete[] m_Data;
		m_Data=new_data;
	};

	void EnsureSize(unsigned long size) {

		if(size<=m_Size) return;

		m_Size = (unsigned long) (size * 1.5);
		T * new_data = new T[m_Size];
		memcpy(new_data,m_Data,m_Count*sizeof(T));
		delete[] m_Data;
		m_Data=new_data;
	};
	
private:
	
	unsigned long m_Count;   
	unsigned long m_Size;
	T *m_Data;
	
}; 

} /* extern "C++" */
#endif

#endif 



