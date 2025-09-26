#ifndef USERDATA_H
#define USERDATA_H
#include <QString>

class userdata
{
public:
    userdata();
    userdata(int id,QString name,QString pass,QString pos,QString pl):id(id),username(name),password(pass),position(pos),plus(pl){}
    int id;
    QString username;
    QString password;
    QString position;
    QString plus;
    static void copy_ass_to_head(userdata * head,userdata * ass){
        head->id = ass->id;
        head->username = ass->username;
        head->password = ass->password;
        head->position = ass->position;
        head->plus = ass->plus;
    }
};

#endif // USERDATA_H
