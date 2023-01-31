/*
 * CSVRow.h
 *
 *  Created on: 2018/08/16
 *      Author: ikki
 *  https://stackoverflow.com/questions/1120140/how-can-i-read-and-parse-csv-files-in-c
 */

#ifndef CSVROW_H
#define CSVROW_H

#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class CSVRow
{
public:
    CSVRow();
    ~CSVRow();
    std::string const& operator[](std::size_t index) const;
    std::size_t size() const;
    void readNextRow(std::istream& str);

private:
    std::vector<std::string>    m_data;
};

#endif
