#ifndef _dodpixel_h
#define _dodpixel_h

class DOD_Pixel {
  public:
    int id;
    int x;
    int y;
    DOD_Pixel() {};
    DOD_Pixel(int a, int b, int c) : id(a), x(b), y(c) {}
};

#endif