#ifndef __SERVED_H__
#define __SERVED_H__


namespace macdetect {
  class Served {
  private:
    int m_nSocketFD;
    
  protected:
  public:
    Served(int nSocketFD);
    ~Served();
  };
}


#endif /* __SERVED_H__ */
