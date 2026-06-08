#ifndef DYNAMIC_ARRAY_HPP
#define DYNAMIC_ARRAY_HPP

#include <cstdlib>
#include <new>
#include "splashkit.h"

class array_invalid_index{};
class array_allocation_error{};

template<typename T>
class dynamic_array{
    int current_size;
    int current_capacity;
    T *data;

    // Find the smallest power of two that can hold the required number of elements.
    int min_capacity(int required_size){
        if(required_size <= 0){
            return 0;
        }

        int result = 1;
        while(result < required_size){
            result *= 2;
        }

        return result;
    }
    // Destruct each constructed element.
    void destroy_elements(){
        for(int i = 0; i < current_size; i++){
            data[i].~T();
        }
    }
    // Allocate a new memory block, copy existing elements into it, then release the old block.
    void resize(int new_capacity){
        if(new_capacity == 0){
            free(data);
            data = nullptr;
            current_capacity = 0;
            return;
        }

        T *new_data = (T *)malloc(sizeof(T) * new_capacity);
        if(new_data == nullptr){
            throw array_allocation_error();
        }

        int constructed = 0;
        try{
            for(int i = 0; i < current_size; i++){
                new (&new_data[i]) T(data[i]);
                constructed++;
            }
        }
        catch(...){
            for(int i = 0; i < constructed; i++){
                new_data[i].~T();
            }
            free(new_data);
            throw;
        }

        destroy_elements();
        free(data);

        data = new_data;
        current_capacity = new_capacity;
    }

    void ensure_capacity(){
        if(current_size < current_capacity){
            return;
        }

        if(current_capacity == 0){
            resize(1);
        }
        else{
            resize(current_capacity * 2);
        }
    }

    void shrink_if_needed(){
        int new_capacity = min_capacity(current_size);

        if(new_capacity != current_capacity){
            resize(new_capacity);
        }
    }

    public:
        dynamic_array(){
            current_size = 0;
            current_capacity = 0;
            data = nullptr;
        }

        //深拷贝
        dynamic_array(const dynamic_array &other){
            current_size = 0;
            current_capacity = 0;
            data = nullptr;

            if(other.current_capacity == 0){
                return;
            }

            T *new_data = (T *)malloc(sizeof(T) * other.current_capacity);
            if(new_data == nullptr){
                throw array_allocation_error();
            }

            int constructed = 0;
            try{
                for(int i = 0; i < other.current_size; i++){
                    new (&new_data[i]) T(other.data[i]);
                    constructed++;
                }
            }
            catch(...){
                for(int i = 0; i < constructed; i++){
                    new_data[i].~T();
                }
                free(new_data);
                throw;
            }

            data = new_data;
            current_size = other.current_size;
            current_capacity = other.current_capacity;
        }

        /*先复制一份 b，叫 temp
        把 a 和 temp 的内部数据交换
        a 现在拥有 b 的复制内容
        temp 现在拥有 a 的旧内容
        函数结束，temp 析构，释放 a 的旧内容*/
        dynamic_array& operator=(const dynamic_array &other){
            if(this == &other){
                return *this;
            }

            dynamic_array temp(other);

            //a.current_size 和 temp.current_size 交换
            int old_size = current_size;
            current_size = temp.current_size;
            temp.current_size = old_size;

            //交换 capacity
            int old_capacity = current_capacity;
            current_capacity = temp.current_capacity;
            temp.current_capacity = old_capacity;

            //交换 data
            T *old_data = data;
            data = temp.data;
            temp.data = old_data;

            return *this;
        }

        ~dynamic_array(){
            destroy_elements();
            free(data);
        }

        int length() const{
            return current_size;
        }

        int size() const{
            return current_size;
        }

        int capacity() const{
            return current_capacity;
        }

        T& get(int index){
            if(index < 0 || index >= current_size){
                throw array_invalid_index();
            }

            return data[index];
        }

        const T& get(int index) const{
            if(index < 0 || index >= current_size){
                throw array_invalid_index();
            }

            return data[index];
        }

        T& operator[](int index){
            return get(index);
        }

        const T& operator[](int index) const{
            return get(index);
        }

        void add(const T &value){
            ensure_capacity();

            // Construct a T object at the next available memory position and initialize it with value.
            new (&data[current_size]) T(value);
            current_size++;
        }

        void remove(int index){
            if(index < 0 || index >= current_size){
                throw array_invalid_index();
            }

            for(int i = index + 1; i < current_size; i++){
                data[i - 1] = data[i];
            }
            // Destruct the last constructed element.
            data[current_size - 1].~T();
            current_size--;

            shrink_if_needed();
        }

        void fill(int times, const T &value){
            for(int i = 0; i < times; i++){
                add(value);
            }
        }
};

#endif
