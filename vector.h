#ifndef vector_h
#define vector_h
#include<iostream>
#include<stdexcept>
#include<memory>
#include<initializer_list>
#include<algorithm>

using namespace std;

template<typename T>
class Vector{
public:
    class ConstIterator;
    class Iterator;
    using size_type = std::size_t;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = Vector::Iterator;
    using const_iterator = Vector::ConstIterator;
    
private:
    std::size_t sz;
    std::size_t max_sz;
    pointer values;
    static constexpr size_t min_sz{5};
    
public:
    Vector() : sz{0}, max_sz{min_sz}, values{ new value_type[max_sz] } {
    } //default
    
    Vector(size_t n) : sz{0}, max_sz{min_sz}, values{ new value_type[max_sz] } {
        reserve(n);
    } //parameterised 1
    
    Vector(std::initializer_list<value_type> l) : Vector(l.size()) {
        for(const auto& v : l)
            values[sz++] = v;
    } //parameterised 2
  
      Vector(const Vector& r) : Vector(r.sz)
    {
        this->sz = r.size();
        this->max_sz = r.capacity();
        this->values = new value_type[r.capacity()];
        for (std::size_t i{ 0 }; i < r.size(); i++)
            this->values[i] = r[i];
    } //copy
  
    ~Vector(){
        delete[] values;
    } //destructor
    
    Vector& operator=(Vector vec){
        std::swap(vec.sz, sz);
        std::swap(vec.max_sz, max_sz);
        std::swap(vec.values, values);
        return *this;
    }
    
    size_t size() const {
        return this->sz;
    }
    
    bool empty() const{
        if(this->sz == 0) return true;
        return false;
    }
    
    void clear() {
        sz = 0;
    }
    
  void reserve(size_t n){
        if (max_sz >= n || 0 >= n) return;
        allocate(n);
    }
    
    void allocate(std::size_t n) {
        pointer next {new value_type[n]};
        for (size_t i{0}; i < sz; ++i){
            next[i] = values[i];
        }
        delete[] values;
        values = next;
        max_sz = n;
    }
    
    void shrink_to_fit() {
        if (max_sz == sz) return;
        if (sz == 0) {
            values = nullptr;
            max_sz = 0;
            delete[] values;
        }
        reserve(sz);
    }
    
   void push_back(const_reference x) {
        if(sz >= max_sz)
            reserve(max_sz * 2 +1);
        values[sz++] = x;
    }
    
    void pop_back() {
        if (this->sz == 0) throw std::runtime_error("Vector muss nicht leer sein!");
        else --sz;
    }
    
    reference operator[](std::size_t index) {
        if(index > sz) throw out_of_range("Index ausser Reichweite");
        return values[index];
    }
    
    const_reference operator[](std::size_t index) const{
        if(index > sz) throw out_of_range("Index ausser Reichweite");
        return values[index];
    }
    
    size_t capacity() const{
        return this->max_sz;
    }
    
    iterator begin(){
        return values;
    }
    
    iterator end(){
        return values + sz;
    }
    const_iterator begin() const{
        return values;
    }
    
    const_iterator end() const{
        return values + sz;
    }
    
    std::ostream& print(std::ostream& o) const{
        o << '[';
        bool first{true};
        for (size_t i{0}; i<sz; i++){
            if (first)
                first = false;
            else o << ", ";
            o << values[i];
        }
        o << ']';
        return o;
    }
    
    class Iterator{
    public:
        using value_type = Vector::value_type;
        using reference = Vector::reference;
        using pointer = Vector::pointer;
        using difference_type = Vector::difference_type;
        using iterator_category = std::forward_iterator_tag;
    private:
        pointer ptr;
    public:
        Iterator(pointer ptr = nullptr) : ptr{ptr} {
            
        }
        
        reference operator*(){
            return *ptr;
        }
        
        pointer operator->(){
            return this->ptr;
        }
        
        bool operator==(const const_iterator& it) const{
            if(it == *this) return true;
            return false;
        }
        
        bool operator!=(const const_iterator& it) const{
            if(it != *this) return true;
            else return false;
        }
        
        iterator& operator++() {
            ++ptr;
            return *this;
        }
        
        iterator operator++(int){
            return ptr++;
        }
        
        operator const_iterator() const{
            return ConstIterator{ptr};
        }
    };
    
    class ConstIterator{
    public:
        using value_type = Vector::value_type;
        using reference = Vector::const_reference;
        using pointer = Vector::const_pointer;
        using difference_type = Vector::difference_type;
        using iterator_category = std::forward_iterator_tag;
    private:
        pointer ptr;
    public:
        ConstIterator(pointer ptr = nullptr) : ptr{ptr} {
            
        }
        reference operator*(){
            return *ptr;
        }
        
        pointer operator->(){
            return ptr;
        }
        
        bool operator==(const const_iterator& it) const{
            if(ptr == it.ptr) return true;
            else return false;
        }
        
        bool operator!=(const const_iterator& it) const{
            if(ptr != it.ptr) return true;
            else return false;
        }
        
        const_iterator& operator++(){
            ++ptr;
            return *this;
        }
        
        //const_iterator operator++(int){
        //   return ptr++;
        //}
        
        const_iterator operator++(int){
            ConstIterator help(ptr);
            ptr += 1;
            return help;
        }
        
        friend Vector::difference_type operator-(const Vector::ConstIterator& lop, const Vector::ConstIterator& rop) {
            return lop.ptr-rop.ptr;
        }
    };
    
    iterator insert(const_iterator pos, const_reference val) {
        auto diff = pos - begin();
        if (diff<0 || static_cast<size_type>(diff)>sz)
            throw std::runtime_error("Iterator out of bounds");
        size_type current{static_cast<size_type>(diff)};
        if (sz>=max_sz)
            reserve (max_sz*2+1); //Achtung Sonderfall , wenn keine Mindestgroesse definiert ist
        for (auto i{sz}; i-->current;)
            values[i+1] = values[i];
        values[current] = val;
        ++sz ;
        return iterator{values+current};
    }
    
    iterator erase(const_iterator pos) {
        auto diff = pos-begin ();
        if (diff<0 || static_cast<size_type>(diff)>=sz)
            throw std::runtime_error("Iterator out of bounds");
        size_type current{static_cast<size_type>(diff)};
        for (auto i{current}; i<sz-1; ++i)
            values [i]=values [i +1];
        --sz;
        return iterator{values+current};
    }
};

template<typename T>
inline std::ostream& operator<<(std::ostream& o, const Vector<T> &v){
    return v.print(o);
}
#endif  /* vector_h */


