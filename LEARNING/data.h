#ifndef DATA_H
#define DATA_H
#include <QRect>
namespace leaves{
    class data
    {
    public:
        data(int ID,const QRect & RECT,int FONTSIZE,double OPACITY):id(ID),rect(RECT),fontSize(FONTSIZE),opacity(OPACITY){};
        data(){};
        int id;
        QRect rect;
        int fontSize;
        double opacity;
        static bool Compare(data * a,data * b) {return a->id < b->id;}
    };
}



#endif // DATA_H
