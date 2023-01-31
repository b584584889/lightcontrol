//
//  MyException.h
//  vilscenter
//
//  Created by ikki on 2019/4/25.
//  Copyright © 2019 SMIMS Corp. All rights reserved.
//

#ifndef MyException_h
#define MyException_h

#include <iostream>
#include <string>

#include "../logger/log.h"

using namespace std;

class MyException : public exception
{
public:
    MyException(const string& msg) : m_msg(msg)
    {
        vilslog::printf("MyException::MyException - set m_msg to:%s\n", m_msg.c_str());
    }

    MyException(bool value)
    {
        vilslog::printf("MyException::MyException - value FAILE\n");
    }

    ~MyException()
    {
        vilslog::printf("MyException::~MyException\n");
    }
    
    virtual const char* what() const throw ()
    {
        vilslog::printf("MyException - what\n");
        return m_msg.c_str();
    }
        
    const string m_msg;
};

#endif /* MyException_h */
