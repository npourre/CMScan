template<class T>
class RNGWrapper { 
  public:
    static void set(T* object, double (T::*func)());
    static double rng();
  private:
    static T* m_obj;
    static double (T::*m_func)();
};

template<class T> T* RNGWrapper<T>::m_obj;

template<class T> double (T::*RNGWrapper<T>::m_func)();

template<class T> void RNGWrapper<T>::set(T* object, double (T::*func)()) {
  m_obj = object; m_func = func;
}

template<class T> double RNGWrapper<T>::rng() {return (m_obj->*m_func)(); }
